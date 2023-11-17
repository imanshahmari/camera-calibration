#define SAMPLE_FILE1 "../data/middle_left.png"
#define SAMPLE_FILE2 "../data/middle_right.png"


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>


int main() {
    const double ratio_thresh = 0.15; // Adjust this threshold as needed

    cv::Mat img1 = cv::imread(SAMPLE_FILE1, cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread(SAMPLE_FILE2, cv::IMREAD_GRAYSCALE);

    if (img1.empty() || img2.empty()) {
        std::cerr << "Error loading images!" << std::endl;
        return -1;
    }

    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    sift->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    sift->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    // Match descriptors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors1, descriptors2, knn_matches, 3);

    // Apply ratio test
    std::vector<cv::DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    // Draw matches on a new image
    cv::Mat img_matches;
    cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);

    // Resize the image to fit on the screen
    cv::Size screenSize(1280, 720); // Adjust the size as needed
    cv::resize(img_matches, img_matches, screenSize);

    // Display the matches
    cv::imshow("SIFT Matches", img_matches);
    cv::waitKey(0);

    return 0;
}