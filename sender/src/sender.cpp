#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <future>


#include "cluon-complete.hpp"
#include "messages.hpp"

#define INPUT_FILES_INTRINSIC "../../data/intrinsic-input/example-1/*.jpg"
#define INPUT_IMAGE_LEFT "../../data/extrinsic-input/example-1/snapshot_2021_03_22_15_27_45.jpg"
#define INPUT_IMAGE_RIGHT "../../data/extrinsic-input/example-1/snapshot_2021_03_22_15_27_51.jpg"

bool VERBOSE = false;

struct IntrinsicParameters {
    cv::Mat cameraMatrix;
    cv::Mat distortionCoefficients;
};

struct ExtrintsicParameters {
    cv::Mat rotationMatrix;
    cv::Mat translationMatrix;
};

IntrinsicParameters getIntrinsicParameters() {

    // TODO using this for subpix accuracy also (float accuracy)?
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    cv::Size checkerboard = cv::Size(6, 9);

    std::vector<std::vector<cv::Point3f>> objPoints;
    std::vector<std::vector<cv::Point2f>> imgPoints;

    std::vector<cv::Point3f> objp;
    for (int i{0}; i < checkerboard.height; i++) {
        for (int j{0}; j < checkerboard.width; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    std::vector<cv::String> images;
    cv::glob(INPUT_FILES_INTRINSIC, images);

    for (const cv::String &fname : images) {
        cv::Mat img = cv::imread(fname);
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> corners;
        bool ret = cv::findChessboardCorners(gray, checkerboard, corners,
            cv::CALIB_CB_FAST_CHECK);

        if (ret) {
            if(VERBOSE){
                std::cout << fname << std::endl;
            }

            objPoints.push_back(objp);
            imgPoints.push_back(corners);

            if (VERBOSE) {
                cv::drawChessboardCorners(img, checkerboard, corners, ret);
                cv::resize(img, img, cv::Size(1152, 864));
                cv::imshow("img", img);
                cv::waitKey(0);
                cv::destroyAllWindows();
            }
        }
    }

    cv::Mat sampleImg = cv::imread(images[0]);
    cv::Mat mtx,dist,rvecs, tvecs;

    float ret = cv::calibrateCamera(objPoints, imgPoints, sampleImg.size(), mtx, dist, rvecs, tvecs, 0, criteria);
    std::cout << "RMSE for single camera calibration:" << ret << std::endl;

    if (VERBOSE) {
        std::cout << mtx.row(0) << std::endl;
        std::cout << mtx.row(1) << std::endl;
        std::cout << mtx.row(2) << std::endl;
        std::cout << dist.row(0) << std::endl;
    }

    IntrinsicParameters result;
    result.cameraMatrix = mtx.clone(); 
    result.distortionCoefficients = dist.clone();

    return result;
}


void getExtrinsicParameters(IntrinsicParameters parametersLeft, IntrinsicParameters parametersRight){

    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.0001);
    cv::Size checkerboard = cv::Size(6, 9);

    std::vector<std::vector<cv::Point3f>> objPoints;
    std::vector<std::vector<cv::Point2f>> imgPointsLeft;
    std::vector<std::vector<cv::Point2f>> imgPointsRight;

    std::vector<cv::Point3f> objp;
    for (int i{0}; i < checkerboard.height; i++) {
        for (int j{0}; j < checkerboard.width; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    //TODO should have more frames for left and right camera (they should be synced)
    cv::Mat imgLeft,imgLeftGray,imgRight,imgRightGray;

    imgLeft = cv::imread(INPUT_IMAGE_LEFT);
    imgRight = cv::imread(INPUT_IMAGE_RIGHT);

    cv::cvtColor(imgLeft, imgLeftGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imgRight, imgRightGray, cv::COLOR_BGR2GRAY);


    std::vector<cv::Point2f> cornersLeftImg,cornersRightImg;

    bool retLeft = cv::findChessboardCorners(imgLeftGray, checkerboard, cornersLeftImg);   
    bool retRight = cv::findChessboardCorners(imgRightGray, checkerboard, cornersRightImg);


    if (retLeft && retRight) {

        objPoints.push_back(objp);

        imgPointsLeft.push_back(cornersLeftImg);
        imgPointsRight.push_back(cornersRightImg);

        // cv::drawChessboardCorners(img, checkerboard, corners, ret);
        // cv::resize(img, img, cv::Size(1152, 864));
        // cv::imshow("img", img);
        // cv::waitKey(0);
        // cv::destroyAllWindows();
    }

    int flag = cv::CALIB_FIX_INTRINSIC;
    cv::Mat R, T, E, F,rvecs, tvecs, perViewErrors;

    double ret = cv::stereoCalibrate(
        objPoints, imgPointsLeft, imgPointsRight,
        parametersLeft.cameraMatrix, parametersLeft.distortionCoefficients,
        parametersRight.cameraMatrix, parametersRight.distortionCoefficients,
        imgLeft.size(), R, T, E, F, rvecs, tvecs, perViewErrors, flag, criteria
        );

    std::cout << "RMSE for stereo calibration:" << ret << std::endl;
    if(VERBOSE){
        std::cout << "Rotation Matrix R:\n" << R << "\nTranslation Matrix T:\n" << T << std::endl;
    }
}



void sendDataoverOD4(cluon::OD4Session &od4,IntrinsicParameters parameters, int id){
    calValues msg;

    cv::Mat mtx = parameters.cameraMatrix;
    cv::Mat dist = parameters.distortionCoefficients;

    msg.cameraId(id);
    msg.m00(mtx.row(0).at<double>(0));
    msg.m01(mtx.row(0).at<double>(1));
    msg.m02(mtx.row(0).at<double>(2));
    msg.m10(mtx.row(1).at<double>(0));
    msg.m11(mtx.row(1).at<double>(1));
    msg.m12(mtx.row(1).at<double>(2));
    msg.m20(mtx.row(2).at<double>(0));
    msg.m21(mtx.row(2).at<double>(1));
    msg.m22(mtx.row(2).at<double>(2));
    msg.d0(dist.row(0).at<double>(0));
    msg.d1(dist.row(0).at<double>(1));
    msg.d2(dist.row(0).at<double>(2));
    msg.d3(dist.row(0).at<double>(3));
    msg.d4(dist.row(0).at<double>(4));
    od4.send(msg);
}

int main(int argc, char *argv[]) {

    if (argc == 2 && std::string(argv[1]) == "--verbose") {
        VERBOSE = true;
    }

    cluon::OD4Session od4(132, [](cluon::data::Envelope &&envelope) noexcept {});

    IntrinsicParameters parameters1;
    parameters1 = getIntrinsicParameters();
    sendDataoverOD4(od4, parameters1, 1);

    getExtrinsicParameters(parameters1,parameters1);

    return 0;
}