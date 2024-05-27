#ifndef _LABELTOOL_H_  

#define _LABELTOOL_H_

#include "DataLoader.h"
#include <iostream>
#include <opencv2/opencv.hpp>


class ImageData{
private:
    std::string image_path;
    cv::Mat tvec;
    cv::Mat rvec;
    cv::Mat* intrinsic;
    cv::Mat* dist;
    
public:                                                                             
    ImageData(std::string, cv::Mat*, cv::Mat*);
    void get_extrinsic();
};


class LabelTool{
private:
    DataLoader dataloader;
    cv::Mat intrinsic;
    cv::Mat dist;
public:
    LabelTool();
    



};








#endif