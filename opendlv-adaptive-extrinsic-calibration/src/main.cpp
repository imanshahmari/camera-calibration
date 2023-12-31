#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <set>
#include <stdexcept>



#include "cluon-complete.hpp"
#include "messages.hpp"


struct IntrinsicParameters {
    cv::Mat cameraMatrix;
    cv::Mat distortionCoefficients;
    int cameraId;
};

struct ExtrinsicParameters {
    cv::Mat rotationMatrix;
    cv::Mat translationVector;
};


struct CommandLineOptions {
    std::string dir1;           
    std::string dir2;
    std::string writeDest;
    float threshold;
    int k;
    bool verbose;
    bool sift;
    bool send;
    int od4;

    CommandLineOptions() : threshold(0.35), k(3), verbose(false), sift(false), send(false),od4(132){}
};



CommandLineOptions parseCommandLine(int argc, char *argv[]) {
    CommandLineOptions options;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose") {
            options.verbose = true;
        }else if (arg == "--send") {
            options.send = true;
        }else if (arg == "--sift") {
            options.sift = true;
        }else if (arg == "-dir1") {
            options.dir1 = argv[++i];
        }else if (arg == "-dir2") {
            options.dir2 = argv[++i];
        }else if (arg == "-threshold") {
            options.threshold = std::stof(argv[++i]);
        }else if (arg == "-k") {
            options.k = std::stoi(argv[++i]);
        }else if (arg == "-write") {
            options.writeDest = argv[++i];
        }else if (arg == "-od4") {
            options.od4 = std::stoi(argv[++i]);
        }
    }

    return options;
}


void writeYAML(const std::string& filename, const ExtrinsicParameters& parameters) {
    YAML::Node node;

    std::vector<double> rData;
    cv::Mat RFlattened = parameters.rotationMatrix.reshape(1, 1);
    rData.assign(RFlattened.begin<double>(), RFlattened.end<double>());
    node["rotation_matrix"] = rData;

    std::vector<double> tData;
    cv::Mat TFlattened = parameters.translationVector.reshape(1, 1);
    tData.assign(TFlattened.begin<double>(), TFlattened.end<double>());
    node["translation_vector_normalized"] = tData;

    try {
        std::ofstream fout(filename);
        fout << node;
        fout.close();
    } catch (const std::exception& e) {
        std::cerr << "Error writing YAML file: " << e.what() << std::endl;
    }
}


std::string detectSingleImageType(const std::string& dirPath) {
    std::set<std::string> allowedExtensions = {".jpg", ".png", ".bmp"};
    std::set<std::string> foundImageExtensions;

    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();

            if (allowedExtensions.find(extension) != allowedExtensions.end()) {
                foundImageExtensions.insert(extension);
            } else if (extension == ".yml") {
                // Do nothing
            } else {
                throw std::invalid_argument("Unsupported file type found: " + extension);
            }
        }
    }

    if (foundImageExtensions.size() == 1) {
        return *foundImageExtensions.begin();
    } else {
        throw std::invalid_argument("Multiple image types or no supported types detected.");
    }
}


std::vector<std::pair<std::string, std::string>> pairImages(const std::string& folderPath1, const std::string& folderPath2) {
    
    std::string imageType = detectSingleImageType(folderPath1);

    std::vector<cv::String> leftFiles, rightFiles;
    cv::glob(folderPath1 + "/*" + imageType, leftFiles, false);
    cv::glob(folderPath2 + "/*" + imageType, rightFiles, false);

    std::sort(leftFiles.begin(), leftFiles.end());
    std::sort(rightFiles.begin(), rightFiles.end());

    std::vector<std::pair<std::string, std::string>> pairedImages;

    for (size_t i = 0; i < leftFiles.size() && i < rightFiles.size(); ++i) {
        pairedImages.push_back(std::make_pair(leftFiles[i], rightFiles[i]));
    }

    return pairedImages;
}



IntrinsicParameters readYAML(const std::string& dirPath) {
    IntrinsicParameters result;

    std::vector<cv::String> yamlFiles;
    cv::glob(dirPath + "/*.yml", yamlFiles, false);

    if (yamlFiles.empty()) {
        std::cerr << "No YAML files found in the specified directory." << std::endl;
        return result;
    }

    std::string filename = yamlFiles[0];

    try {
        YAML::Node yamlNode = YAML::LoadFile(filename);

        result.cameraId = yamlNode["camera_id"].as<int>();

        YAML::Node distortionNode = yamlNode["distortion_coefficients"];
        cv::Mat distortionCoefficients(1, distortionNode.size(), CV_64F);
        for (std::size_t i = 0; i < distortionNode.size(); ++i) {
            distortionCoefficients.at<double>(i) = distortionNode[i].as<double>();
        }
        result.distortionCoefficients = distortionCoefficients.clone();

        YAML::Node matrixNode = yamlNode["camera_matrix"];
        cv::Mat cameraMatrix(3, 3, CV_64F);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cameraMatrix.at<double>(i, j) = matrixNode[i * 3 + j].as<double>();
            }
        }
        result.cameraMatrix = cameraMatrix.clone();

    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML file: " << e.what() << std::endl;
    }

    return result;
}


void performSIFTMatching(std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2,const cv::Mat& img1, const cv::Mat& img2,
    std::vector<cv::DMatch>& good_matches, CommandLineOptions options) {
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    cv::Mat descriptors1, descriptors2;

    sift->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    sift->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    cv::FlannBasedMatcher matcher;
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors1, descriptors2, knn_matches, options.k);

    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < options.threshold * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    if(options.verbose){
        cv::Mat img_matches;
        cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);

        cv::Size screenSize(1280, 720);
        cv::resize(img_matches, img_matches, screenSize);

        cv::imshow("SIFT Matches", img_matches);
        cv::waitKey(0);

    }
}

void performORBMatching(std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2,const cv::Mat& img1, const cv::Mat& img2,
    std::vector<cv::DMatch>& good_matches, CommandLineOptions options) {

    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    cv::Mat descriptors1, descriptors2;

    orb->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    orb->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors1, descriptors2, knn_matches, options.k);

    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < options.threshold * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    if(options.verbose){
        cv::Mat img_matches;
        cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);

        cv::Size screenSize(1280, 720);
        cv::resize(img_matches, img_matches, screenSize);

        cv::imshow("ORB Matches", img_matches);
        cv::waitKey(0);
    }
}


void sendDataoverOD4(cluon::OD4Session &od4,ExtrinsicParameters parameters){
    calValues msg;

    cv::Mat R = parameters.rotationMatrix;
    cv::Mat T = parameters.translationVector;

    std::vector<double> rotation;
    std::vector<double> translation;


    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            rotation.push_back(R.row(i).at<double>(j));
        }
        translation.push_back(T.row(0).at<double>(i));
    }

    uint32_t rotationDataLen = rotation.size() * sizeof(double);
    uint32_t translationDataLen = translation.size() * sizeof(double);
    
    std::string rStr(rotationDataLen, ' ');
    std::string tStr(translationDataLen, ' ');
    

    ::memcpy(rStr.data(), rotation.data(), rotationDataLen);
    ::memcpy(tStr.data(), translation.data(), translationDataLen);

    msg.r(rStr);
    msg.t(tStr);

    od4.send(msg);
}


int main(int argc, char *argv[]) {

    CommandLineOptions args = parseCommandLine(argc, argv);

    std::vector<std::pair<std::string, std::string>> pairedImages = pairImages(args.dir1, args.dir2);

    std::vector<cv::Point2f> allPoints1, allPoints2;
    std::vector<cv::DMatch> allMatches;

    for (auto el: pairedImages){
        cv::Mat img1 = cv::imread(el.first, cv::IMREAD_GRAYSCALE);
        cv::Mat img2 = cv::imread(el.second, cv::IMREAD_GRAYSCALE);

        std::vector<cv::KeyPoint> keypoints1, keypoints2;
        std::vector<cv::DMatch> good_matches;

        if (args.sift){
            performSIFTMatching(keypoints1,keypoints2,img1, img2, good_matches,args);
        }
        else{
            performORBMatching(keypoints1,keypoints2,img1, img2, good_matches,args); 
        }


        for (const auto& keypoint : keypoints1) {
            allPoints1.push_back(keypoint.pt);
        }

        for (const auto& keypoint : keypoints2) {
            allPoints2.push_back(keypoint.pt);
        }

        allMatches.insert(allMatches.end(), good_matches.begin(), good_matches.end());
    }

    IntrinsicParameters intrinsics1 = readYAML(args.dir1);
    IntrinsicParameters intrinsics2 = readYAML(args.dir2);

    cv::Mat dist1 = intrinsics1.distortionCoefficients;
    cv::Mat dist2 = intrinsics2.distortionCoefficients;
    cv::Mat K1 = intrinsics1.cameraMatrix;
    cv::Mat K2 = intrinsics2.cameraMatrix;

    std::vector<cv::Point2f> points1, points2;
    for (const auto& match : allMatches) {
        points1.push_back(allPoints1[match.queryIdx]);
        points2.push_back(allPoints2[match.trainIdx]);
    }


    cv::Mat E, R, t, mask;

    cv::recoverPose(points1, points2,K1,dist2,K2,dist2,E, R, t,cv::RANSAC,0.999,1.0, mask);

    std::cout << "Relative Rotation (R):\n" << R << std::endl << std::endl;
    std::cout << "Relative Translation (t):\n" << t << std::endl;


    cluon::OD4Session od4(args.od4, [](cluon::data::Envelope &&envelope) noexcept {});

    ExtrinsicParameters exParameters;
    exParameters.rotationMatrix = R;
    exParameters.translationVector = t;

    if (args.send){
        sendDataoverOD4(od4, exParameters);
    }

    if (!args.writeDest.empty()){
        writeYAML(args.writeDest, exParameters);
    }

    return 0;
}