#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>


#define SAMPLE_FILE1 "../../data/extrinsic-input/example-2/left.jpg"
#define SAMPLE_FILE2 "../../data/extrinsic-input/example-2/right.jpg"


// #define SAMPLE_FILE1 "../../data/intrinsic-input/example-3/test/IMG_20231205_112306.jpg"
// #define SAMPLE_FILE2 "../../data/intrinsic-input/example-3/test/IMG_20231205_112347.jpg"



// void computeRotationAndTranslation(const std::vector<cv::Point2f>& points1,
//                                    const std::vector<cv::Point2f>& points2,
//                                    const cv::Mat& cameraMatrix1,
//                                    const cv::Mat& distCoeffs1,
//                                    const cv::Mat& cameraMatrix2,
//                                    const cv::Mat& distCoeffs2) {
//     cv::Mat E, R, t, mask;
//     cv::recoverPose(points1, points2, cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, E, R, t, mask);

//     std::cout << "Rotation Matrix:\n" << R << std::endl;
//     std::cout << "Translation Vector:\n" << t << std::endl;
// }


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

    const double ratio_thresh = 0.90;

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

    cv::Mat distCoeffs = (cv::Mat_<double>(1, 5) << 0.2075004141750106, -1.736070924719815, 0.003009760259382691, 0.001162220896554918, 4.326962781013687);

    cv::Mat K = (cv::Mat_<double>(3, 3) << 3598.928931086823, 0, 1091.063450487599,
                                           0, 3574.483843858951, 2323.706136831262,
                                           0, 0, 1);
    
    cv::Mat essential_matrix = K.t() * fundamental_matrix * K;

    std::vector<cv::Point2f> points1, points2;
    for (const auto& match : good_matches) {
        points1.push_back(keypoints1[match.queryIdx].pt);
        points2.push_back(keypoints2[match.trainIdx].pt);
    }

    cv::Mat E, R, t, mask;
    
    cv::recoverPose(points1, points2,K,distCoeffs,K,distCoeffs,E, R, t,cv::RANSAC,0.999,1.0, mask);



    essential_matrix /= cv::norm(essential_matrix, cv::NORM_L2);
    E /= cv::norm(E, cv::NORM_L2);

    // Display the results
    std::cout << "Essential Matrix1:\n" << essential_matrix << std::endl;
    std::cout << "Essential Matrix2:\n" << E << std::endl << std::endl;
    // std::cout << "Relative Rotation (R):\n" << R << std::endl << std::endl;
    // std::cout << "Relative Translation (t):\n" << t << std::endl;


    return 0;
}