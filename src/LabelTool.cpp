#include "DataLoader.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "LabelTool.h"
#include <opencv2/aruco.hpp>
#include <map>
//non-menber function



//menber function

//************************//
// ImageData              //
//************************//
ImageData::ImageData(std::string image_path, cv::Mat* const intr_ptr, cv::Mat* const dist_ptr)
:image_path{image_path}, keep_in_mem{0}
{
    this->intrinsic_ptr = intr_ptr;
    this->dist_ptr = dist_ptr;

}



void ImageData::get_extrinsic(int keep_in_mem)
{
    //this->image_path
    //this->intrinsic
    //this->dist
    //std::cout<<"opencv version:"<<CV_VERSION;  // opencv 4.7.0
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
    /*
    cv::Mat outputImage = image.clone();
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    cv::imshow("display", outputImage);
    cv::waitKey(0);
    */

    //TODO: get extrinsic from multiple aruco markers

    if(this->keep_in_mem){
        this->image = image.clone();
        this->keep_in_mem = 1;
    }
    //

}

cv::Mat ImageData::get_image(){
    if(this->keep_in_mem)
        return this->image;
    else
        return cv::imread(this->image_path, cv::IMREAD_COLOR);
    
}

//************************//
// LabelTool              //
//************************//

LabelTool::LabelTool(DataLoader& dataloader)
:dataloader{dataloader}, intrinsic{dataloader.get_Camera_intrinsic()}, dist{dataloader.get_Camera_dist()}
{

}
void LabelTool::build_data_list(int keep_in_mem){
    // add all image 
    for(int i=0; i<dataloader.length(); i+=10){
        this->data_list.push_back(ImageData(dataloader[i], &intrinsic, &dist));
    }
    // get extrinsic
    for(auto &imgdat : this->data_list){
        imgdat.get_extrinsic(keep_in_mem);
    }
    
}

cv::Mat LabelTool::get_imgdat(int idx){
    return this->data_list.at(idx).get_image();
}