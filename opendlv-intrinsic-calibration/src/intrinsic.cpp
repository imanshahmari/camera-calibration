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

bool VERBOSE = false;
bool FASTCHECK = false;


struct IntrinsicParameters {
    cv::Mat cameraMatrix;
    cv::Mat distortionCoefficients;
};

struct CommandLineOptions {
    std::string imagesDir;
    std::string outputYAML;
    cv::Size checkerBoard;
    int cameraId;
};

CommandLineOptions parseCommandLine(int argc, char *argv[]) {
    CommandLineOptions options;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--verbose") {
            VERBOSE = true;
        } else if (arg == "--fastcheck") {
            FASTCHECK = true;
        } else if (arg == "-imagesdir") {
            options.imagesDir = argv[++i];
        } else if (arg == "-outputyaml") {
            options.outputYAML = argv[++i];
        } else if (arg == "-checkersize") {
            if (i + 2 < argc) {
                try {
                    options.checkerBoard.width = std::stoi(argv[++i]);
                    options.checkerBoard.height = std::stoi(argv[++i]);
                } catch (const std::exception& e) {
                    throw std::invalid_argument("Error: Invalid checkerboard size format. Please provide width and height as positive integers using -checkersize followed by width and height.");
                }

                if (options.checkerBoard.width <= 0 || options.checkerBoard.height <= 0) {
                    std::cout << "Note: Width and height represent how many times a change in color happens "
                                 "going from white to black or vice versa in each direction (horizontally and vertically), "
                                 "not the number of squares." << std::endl;
                    throw std::invalid_argument("Error: Invalid checkerboard size format. Please provide positive integers as width and height.");
                }

                std::cout << "Checkerboard size set to: " << options.checkerBoard.width << "x" << options.checkerBoard.height << std::endl;
            } else {
                throw std::invalid_argument("Error: Insufficient arguments for -checkersize. Please provide width and height as positive integers.");
            }
        } else if (arg == "-cameraid") {
            if (i + 1 < argc) {
                try {
                    options.cameraId = std::stoi(argv[++i]);
                } catch (const std::exception& e) {
                    throw std::invalid_argument("Error: Invalid camera ID format. Please provide a valid integer using -cameraid followed by the camera ID.");
                }
            } else {
                throw std::invalid_argument("Error: Insufficient arguments for -cameraid. Please provide a valid integer as the camera ID.");
            }
        }
    }

    if (options.imagesDir.empty()) {
        throw std::invalid_argument("Error: Images directory must be specified. Pass in -imagesdir followed by the path to the images directory.");
    }

    if (options.outputYAML.empty()) {
        throw std::invalid_argument("Error: Output YAML file must be specified. Pass in -outputyaml followed by the filename and path to where the output file should be stored.");
    }

    return options;
}



bool isFileImage(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of(".");
    if (dotPos != std::string::npos) {
        std::string extension = filePath.substr(dotPos + 1);
        return (extension == "bmp" || extension == "png" || extension == "jpg");
    }
    return false;
}


void writeYAML(const std::string& filename, const IntrinsicParameters& parameters, const int& cameraId) {
    YAML::Node node;

    std::vector<double> cameraMatrixData;
    cv::Mat cameraMatrixFlattened = parameters.cameraMatrix.reshape(1, 1);
    cameraMatrixData.assign(cameraMatrixFlattened.begin<double>(), cameraMatrixFlattened.end<double>());

    std::vector<double> distortionCoefficientsData;
    cv::Mat distortionCoefficientsFlattened = parameters.distortionCoefficients.reshape(1, 1);
    distortionCoefficientsData.assign(distortionCoefficientsFlattened.begin<double>(), distortionCoefficientsFlattened.end<double>());

    node["camera_id"] = cameraId;
    node["camera_matrix"] = cameraMatrixData;
    node["distortion_coefficients"] = distortionCoefficientsData;

    try {
        std::ofstream fout(filename);
        fout << node;
        fout.close();
    } catch (const std::exception& e) {
        std::cerr << "Error writing YAML file: " << e.what() << std::endl;
    }
    std::cout << "File written to:" << filename << std::endl;
}



IntrinsicParameters getIntrinsicParameters(std::string filePath,cv::Size checkerBoard) {

    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.0001);

    std::vector<std::vector<cv::Point3f>> objPoints;
    std::vector<std::vector<cv::Point2f>> imgPoints;

    std::vector<cv::Point3f> objp;
    for (int i{0}; i < checkerBoard.height; i++) {
        for (int j{0}; j < checkerBoard.width; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    std::vector<cv::String> images;
    cv::glob(filePath, images);

    for (const cv::String &fname : images) {
        if (isFileImage(fname)) {
            cv::Mat img = cv::imread(fname);
            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

            std::vector<cv::Point2f> corners;

            bool ret = false;

            if(FASTCHECK){
                ret = cv::findChessboardCorners(gray, checkerBoard, corners,
                    cv::CALIB_CB_FAST_CHECK);
            }else{
                ret = cv::findChessboardCorners(gray, checkerBoard, corners);
            }

            if (ret) {
                if(VERBOSE){
                    std::cout << fname << std::endl;
                }

                cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), criteria);

                objPoints.push_back(objp);
                imgPoints.push_back(corners);

                if (VERBOSE) {
                    cv::drawChessboardCorners(img, checkerBoard, corners, ret);
                    cv::resize(img, img, cv::Size(1152, 864));
                    cv::imshow("img", img);
                    cv::waitKey(0);
                    cv::destroyAllWindows();
                }
            }
        }
    }

    cv::Mat sampleImg = cv::imread(images[0]);
    cv::Mat mtx,dist,rvecs, tvecs;

    float ret = cv::calibrateCamera(objPoints, imgPoints, sampleImg.size(), mtx, dist, rvecs, tvecs, 0, criteria);
    std::cout << "RMSE for single camera calibration:" << ret << std::endl;
    std::cout << mtx.row(0) << std::endl;
    std::cout << mtx.row(1) << std::endl;
    std::cout << mtx.row(2) << std::endl;
    std::cout << dist.row(0) << std::endl;


    IntrinsicParameters result;
    result.cameraMatrix = mtx.clone(); 
    result.distortionCoefficients = dist.clone();

    return result;
}


int main(int argc, char *argv[]) {

    CommandLineOptions args = parseCommandLine(argc, argv);

    if (VERBOSE) {
        std::cout << "Verbose mode enabled." << std::endl;
    }

    if (FASTCHECK) {
        std::cout << "Fast check enabled." << std::endl;
    }

    IntrinsicParameters parameters;
    parameters = getIntrinsicParameters(args.imagesDir,args.checkerBoard);

    writeYAML(args.outputYAML, parameters,args.cameraId);

    return 0;
}