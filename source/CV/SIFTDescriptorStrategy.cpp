#include "SIFTDescriptorStrategy.h"

#include <QImage>
#include <opencv2/xfeatures2d.hpp>

std::vector<std::pair<QPoint, QPoint>> SIFTDescriptorStrategy::GetDescriptors(
    QImage const& img1, QImage const& img2, QRect const& domain) {
  cv::Mat const image_mat1{img1.height(), img1.width(), CV_8UC3,
                           (cv::Scalar*)img1.scanLine(0)};
  auto mask1 = cv::Mat::ones(img1.width(), img1.height(), CV_8U);

  cv::Mat const image_mat2{img2.height(), img2.width(), CV_8UC3,
                           (cv::Scalar*)img2.scanLine(0)};
  auto mask2 = cv::Mat::ones(img2.width(), img2.height(), CV_8U);

  auto sift_detector = cv::SIFT::create(10);

  std::vector<cv::KeyPoint> keypoints1, keypoints2;
  cv::Mat descriptors1, descriptors2;
  sift_detector->detectAndCompute(image_mat1, mask1, keypoints1, descriptors1);
  sift_detector->detectAndCompute(image_mat2, mask2, keypoints2, descriptors2);

  auto matcher =
      cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
  std::vector<std::vector<cv::DMatch>> knn_matches;
  matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

  float const ratio_thresh = 0.7f;

  std::vector<std::pair<QPoint, QPoint>> res;

  for (auto const& matches : knn_matches) {
    if (matches[0].distance < ratio_thresh * matches[1].distance) {
      QPointF point1{keypoints1[matches[0].queryIdx].pt.x,
                     keypoints1[matches[0].queryIdx].pt.y};
      QPointF point2{keypoints1[matches[0].trainIdx].pt.x,
                     keypoints1[matches[0].trainIdx].pt.y};

      res.emplace_back(std::make_pair(point1.toPoint(), point2.toPoint()));
    }
  }

  return res;
}
