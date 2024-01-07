#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <fstream>
#include <future>
#include <filesystem>


#include "cluon-complete.hpp"
#include "messages.hpp"



bool VERBOSE = false;


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
    cv::Size conv;
    cv::Size checkerBoard;
    int od4;
    bool verbose;
    bool send;
    CommandLineOptions() : conv(11, 11), checkerBoard(4, 7), od4(132), verbose(false),send(false) {}
};


CommandLineOptions parseCommandLine(int argc, char *argv[]) {
    CommandLineOptions options;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose") {
            options.verbose = true;
        }else if (arg == "-checkersize") {
            options.checkerBoard.width = std::stoi(argv[++i]);
            options.checkerBoard.height = std::stoi(argv[++i]);
            std::cout << "Checkerboard size set to: " << options.checkerBoard.width << "x" 
            << options.checkerBoard.height << std::endl;
        }else if (arg == "-dir1") {
            options.dir1 = argv[++i];
        }else if (arg == "-dir2") {
            options.dir2 = argv[++i];
        }else if (arg == "-od4") {
            options.od4 = std::stoi(argv[++i]);
        }else if (arg == "-conv") {
            options.conv.width = std::stoi(argv[++i]);
            options.conv.height = std::stoi(argv[++i]);             
        }else if (arg == "-write") {
            options.writeDest = argv[++i];
        }else if (arg == "--send") {
            options.send = true;
        }
    }
    
    return options;
}


void writeYAML(const std::string& filename, ExtrinsicParameters& parameters) {
    YAML::Node node;

    std::vector<double> rData;
    cv::Mat RFlattened = parameters.rotationMatrix.reshape(1, 1);
    rData.assign(RFlattened.begin<double>(), RFlattened.end<double>());
    node["rotation_matrix"] = rData;

    std::vector<double> tData;
    cv::Mat TFlattened = parameters.translationVector.reshape(1, 1);
    tData.assign(TFlattened.begin<double>(), TFlattened.end<double>());
    node["translation_vector"] = tData;

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


ExtrinsicParameters getExtrinsicParameters(CommandLineOptions args){

    std::vector<std::pair<std::string, std::string>> pairedImages = pairImages(args.dir1, args.dir2);

    IntrinsicParameters intrinsics1 = readYAML(args.dir1);
    IntrinsicParameters intrinsics2 = readYAML(args.dir2);



    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 1000, 0.0001);

    std::vector<std::vector<cv::Point3f>> objPoints;
    std::vector<std::vector<cv::Point2f>> imgPointsLeft;
    std::vector<std::vector<cv::Point2f>> imgPointsRight;

    std::vector<cv::Point3f> objp;
    for (int i{0}; i < args.checkerBoard.height; i++) {
        for (int j{0}; j < args.checkerBoard.width; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    cv::Mat imgLeft,imgLeftGray,imgRight,imgRightGray;



    for (const auto& el : pairedImages) {


        imgLeft = cv::imread(el.first);
        imgRight = cv::imread(el.second);

        cv::cvtColor(imgLeft, imgLeftGray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(imgRight, imgRightGray, cv::COLOR_BGR2GRAY);


        std::vector<cv::Point2f> cornersLeftImg,cornersRightImg;

        bool retLeft = cv::findChessboardCorners(imgLeftGray, args.checkerBoard, cornersLeftImg);   
        bool retRight = cv::findChessboardCorners(imgRightGray, args.checkerBoard, cornersRightImg);

        if (retLeft && retRight) {

            cv::cornerSubPix(imgLeftGray, cornersLeftImg, args.conv, cv::Size(-1, -1), criteria);
            cv::cornerSubPix(imgRightGray, cornersRightImg, args.conv, cv::Size(-1, -1), criteria);

            objPoints.push_back(objp);

            imgPointsLeft.push_back(cornersLeftImg);
            imgPointsRight.push_back(cornersRightImg);

            if (args.verbose) {

                cv::drawChessboardCorners(imgLeft, args.checkerBoard, cornersLeftImg, retLeft);
                cv::drawChessboardCorners(imgRight, args.checkerBoard, cornersRightImg, retLeft);

                cv::resize(imgLeft, imgLeft, cv::Size(800, 800));
                cv::resize(imgRight, imgRight, cv::Size(800, 800));

                cv::Mat compositeImage;
                cv::hconcat(imgLeft, imgRight, compositeImage);

                cv::imshow("Left and Right Images", compositeImage);
                cv::waitKey(0);
                cv::destroyAllWindows();
            }
        }
    }


    int flag = cv::CALIB_FIX_INTRINSIC;
    cv::Mat R, T, E, F,rvecs, tvecs, perViewErrors;


    double ret = cv::stereoCalibrate(
        objPoints, imgPointsLeft, imgPointsRight,
        intrinsics1.cameraMatrix, intrinsics1.distortionCoefficients,
        intrinsics2.cameraMatrix, intrinsics2.distortionCoefficients,
        imgLeft.size(), R, T, E, F, rvecs, tvecs, perViewErrors, flag, criteria
        );



    std::cout << "Parameters from camera " << intrinsics2.cameraId << " to " << intrinsics1.cameraId << std::endl;
    std::cout << "Rotation Matrix R:\n" << R << "\nTranslation Vector T:\n" << T << std::endl;
    std::cout << "RMSE for stereo calibration:" << ret << std::endl;

    ExtrinsicParameters result;
    result.rotationMatrix = R;
    result.translationVector = T;

    return result;
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


void writeYAML(const std::string& filename, const ExtrinsicParameters& parameters) {
    YAML::Node node;

    std::vector<double> rData;
    cv::Mat RFlattened = parameters.rotationMatrix.reshape(1, 1);
    rData.assign(RFlattened.begin<double>(), RFlattened.end<double>());
    node["rotation_matrix"] = rData;

    std::vector<double> tData;
    cv::Mat TFlattened = parameters.translationVector.reshape(1, 1);
    tData.assign(TFlattened.begin<double>(), TFlattened.end<double>());
    node["translation_vector"] = tData;

    try {
        std::ofstream fout(filename);
        fout << node;
        fout.close();
    } catch (const std::exception& e) {
        std::cerr << "Error writing YAML file: " << e.what() << std::endl;
    }
}



int main(int argc, char *argv[]) {

    CommandLineOptions args = parseCommandLine(argc, argv);

    if (args.verbose) {
        std::cout << "Verbose mode enabled." << std::endl;
    }

    ExtrinsicParameters exParameters = getExtrinsicParameters(args);

    if (args.send){
        cluon::OD4Session od4(args.od4, [](cluon::data::Envelope &&envelope) noexcept {});
        sendDataoverOD4(od4, exParameters);
    }

    if (!args.writeDest.empty()){
        writeYAML(args.writeDest, exParameters);
    }

    return 0;
}