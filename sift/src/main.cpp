#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>


#define SAMPLE_FILE1 "../data/middle_left.png"
#define SAMPLE_FILE2 "../data/middle_right.png"



void performORBMatching(const cv::Mat& img1, const cv::Mat& img2, const double ratio_thresh,
                        std::vector<cv::DMatch>& good_matches) {
    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    orb->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    orb->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors1, descriptors2, knn_matches, 2);

    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    cv::Mat img_matches;
    cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);

    cv::Size screenSize(1280, 720);
    cv::resize(img_matches, img_matches, screenSize);

    cv::imshow("ORB Matches", img_matches);
    cv::waitKey(0);
}



void performSIFTMatching(const cv::Mat& img1, const cv::Mat& img2, const double ratio_thresh,
                         std::vector<cv::DMatch>& good_matches) {
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    sift->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    sift->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    cv::FlannBasedMatcher matcher;
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors1, descriptors2, knn_matches, 3);

    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    cv::Mat img_matches;
    cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);

    cv::Size screenSize(1280, 720);
    cv::resize(img_matches, img_matches, screenSize);

    cv::imshow("SIFT Matches", img_matches);
    cv::waitKey(0);
}

cv::Mat computeFundamentalMatrix(const std::vector<cv::KeyPoint>& keypoints1,
                                 const std::vector<cv::KeyPoint>& keypoints2,
                                 const std::vector<cv::DMatch>& good_matches) {
    std::vector<cv::Point2f> points1, points2;
    for (const auto& match : good_matches) {
        points1.push_back(keypoints1[match.queryIdx].pt);
        points2.push_back(keypoints2[match.trainIdx].pt);
    }

    return cv::findFundamentalMat(points1, points2, cv::FM_RANSAC);
}


int main() {
    const double ratio_thresh = 0.6;

    cv::Mat img1 = cv::imread(SAMPLE_FILE1, cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread(SAMPLE_FILE2, cv::IMREAD_GRAYSCALE);

    if (img1.empty() || img2.empty()) {
        std::cerr << "Error loading images!" << std::endl;
        return -1;
    }

    std::vector<cv::DMatch> good_matches;

    performORBMatching(img1, img2, ratio_thresh, good_matches);

    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    orb->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    orb->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    cv::Mat fundamental_matrix = computeFundamentalMatrix(keypoints1, keypoints2, good_matches);

    std::cout << "Fundamental Matrix:\n" << fundamental_matrix << std::endl;

    return 0;
}
