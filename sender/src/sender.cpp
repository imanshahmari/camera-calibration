#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include <fstream>

#include "cluon-complete.hpp"
#include "messages.hpp"

#define INPUT_FILES_INTRINSIC "../../data/intrinsic-input/sample-camera/*.jpg"
#define INPUT_FILES_EXTRINSIC "../../data/extrinsic-input/*.jpg"

bool VERBOSE = false;

struct IntrinsicParameters {
    cv::Mat cameraMatrix;
    cv::Mat distortionCoefficients;
};

IntrinsicParameters getIntrinsicParameters() {

    // TODO using this for subpix accuracy also so float accuracy?
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);

    int CHECKERBOARD[2]{6, 9};

    std::vector<std::vector<cv::Point3f>> objpoints;
    std::vector<std::vector<cv::Point2f>> imgpoints;

    std::vector<cv::Point3f> objp;
    for (int i{0}; i < CHECKERBOARD[1]; i++) {
        for (int j{0}; j < CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j, i, 0));
    }

    std::vector<cv::String> images;
    cv::glob(INPUT_FILES_INTRINSIC, images);

    for (const cv::String &fname : images) {
        cv::Mat img = cv::imread(fname);
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Point2f> corners;
        bool ret = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corners);

        if (ret) {
            if(VERBOSE){
                std::cout << fname << std::endl;
            }
            objpoints.push_back(objp);
            imgpoints.push_back(corners);

            if (VERBOSE) {
                cv::drawChessboardCorners(img, cv::Size(9, 6), corners, ret);
                cv::resize(img, img, cv::Size(1152, 864));
                cv::imshow("img", img);
                cv::waitKey(0);
                cv::destroyAllWindows();
            }
        }
    }

    cv::Mat img2 = cv::imread(images[0]);
    cv::Mat mtx,dist,rvecs, tvecs;
    float ret = cv::calibrateCamera(objpoints, imgpoints, img2.size(), mtx, dist, rvecs, tvecs, 0, criteria);
    std::cout << "RMSE:" << ret << std::endl;

    if (VERBOSE) {
        std::cout << mtx.row(0) << std::endl;
        std::cout << mtx.row(1) << std::endl;
        std::cout << mtx.row(2) << std::endl;
        std::cout << dist.row(0) << std::endl;
    }

    // Create an instance of the IntrinsicParameters struct and assign the matrices
    IntrinsicParameters result;
    result.cameraMatrix = mtx.clone(); // Assuming mtx is the camera matrix
    result.distortionCoefficients = dist.clone(); // Assuming dist is the distortion coefficients

    return result;
}

void sendDataoverOD4(cluon::OD4Session &od4,cv::Mat &mtx, cv::Mat &dist, int id){
    calValues msg;
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

    // cluon::OD4Session od4(132, [](cluon::data::Envelope &&envelope) noexcept {});

    IntrinsicParameters parameters1;
    parameters1 = getIntrinsicParameters();


    cv::Mat mtx = parameters1.cameraMatrix;

    // sendDataoverOD4(od4, mtx, dist, 1);

    return 0;
}