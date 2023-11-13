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


#define INPUT_FILES "../data/calibration_input/*.jpg"
#define OUTPUT_FILE "../data/calibration_output/calibresult.jpg"
#define SAMPLE_FILE "../data/calibration_input/snapshot_2021_03_22_15_27_51.jpg"
#define DRAW false



void getCalibrationMatrix(cv::Mat& mtx, cv::Mat& dist) {

    int CHECKERBOARD[2]{6,9};

    // Termination criteria
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);


    std::vector<std::vector<cv::Point3f>> objpoints;  // 3D points in real world space
    std::vector<std::vector<cv::Point2f>> imgpoints;  // 2D points in image plane

    std::vector<cv::Point3f> objp;  
    for(int i{0}; i<CHECKERBOARD[1]; i++){
        for(int j{0}; j<CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j,i,0));
    }


    std::vector<std::string> images;
    cv::glob(INPUT_FILES, images);

    for (const std::string& fname : images) {
        cv::Mat img = cv::imread(fname);
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        // Find the chessboard corners
        std::vector<cv::Point2f> corners;
        bool ret = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corners);

        if (ret) {
            std::cout << fname << std::endl;
            objpoints.push_back(objp);
            imgpoints.push_back(corners);

            if (DRAW) {
                // draw and display the corners
                cv::drawChessboardCorners(img, cv::Size(9, 6), corners, ret);
                cv::resize(img, img, cv::Size(1152, 864));
                cv::imshow("img", img);
                cv::waitKey(0);
                cv::destroyAllWindows();
            }
        }
    }

    cv::Mat img2 = cv::imread(SAMPLE_FILE);
    cv::Mat gray2;
    cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

    cv::Mat rvecs, tvecs;
    bool ret = cv::calibrateCamera(objpoints, imgpoints, gray2.size(), mtx, dist, rvecs, tvecs);

    if (ret) {
        cv::Mat dst;
        cv::undistort(img2, dst, mtx, dist);
        cv::imwrite(OUTPUT_FILE, dst);
    }

    // std::ofstream file("graphics/calibration_output/calibration_matrix.csv");
    // if (file.is_open()) {
    //     file << mtx.row(0) << std::endl;
    //     file << mtx.row(1) << std::endl;
    //     file << mtx.row(2) << std::endl;
    //     file << dist.row(0) << std::endl;
    //     file.close();
    // }

    std::cout << mtx.row(0) << std::endl;
    std::cout << mtx.row(1) << std::endl;
    std::cout << mtx.row(2) << std::endl;
    std::cout << dist.row(0) << std::endl;
}


void sendDataoverOD4(cluon::OD4Session &od4,cv::Mat &mtx, cv::Mat &dist){
    calValues msg;
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

int main() {
    cluon::OD4Session od4(132, [](cluon::data::Envelope &&envelope) noexcept {});

    cv::Mat mtx, dist;
    getCalibrationMatrix(mtx, dist);

    while (true){
        sendDataoverOD4(od4,mtx,dist);
    }
    
    return 0;
}