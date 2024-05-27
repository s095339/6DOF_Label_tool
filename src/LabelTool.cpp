#include "DataLoader.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "LabelTool.h"
#include <opencv2/aruco.hpp>
//non-menber function



//menber function

ImageData::ImageData(std::string image_path, cv::Mat* intrinsic, cv::Mat* dist)
:image_path{image_path}, intrinsic{intrinsic}, dist{dist}
{



}

void ImageData::get_extrinsic()
{
    //this->image_path
    //this->intrinsic
    //this->dist
    std::cout<<"opencv version:"<<CV_VERSION;
    //read image from path
    cv::Mat image = cv::imread(this->image_path, cv::IMREAD_COLOR);
    cv::Mat img_gray;
    cv::cvtColor(image, img_gray, cv::COLOR_BGR2GRAY);

    //python
    //aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_100)
    //parameters = aruco.DetectorParameters_create()

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);
    detector.detectMarkers(image, markerCorners, markerIds, rejectedCandidates);

    cv::Mat outputImage = image.clone();
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    cv::imshow("display", outputImage);
    cv::waitKey(0);

}