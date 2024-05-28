#ifndef _LABELTOOL_H_  

#define _LABELTOOL_H_

#include "DataLoader.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>

class ImageData{
private:
    std::string image_path;
    cv::Mat tvec;
    cv::Mat rvec;
    cv::Mat* intrinsic_ptr;
    cv::Mat* dist_ptr;
    
    cv::Mat image;
    int keep_in_mem;
public:                             
    //constructor                                                
    ImageData(std::string, cv::Mat* const, cv::Mat* const);
    //ImageData(ImageData && source); 
    //menber function
    void get_extrinsic(int keep_in_mem);
    cv::Mat get_image();
};


class LabelTool{
private:
    DataLoader dataloader;
    cv::Mat intrinsic;
    cv::Mat dist;
    std::vector<ImageData> data_list;
public:
    LabelTool(DataLoader&); //Constructor
    void build_data_list(int keep_in_mem = 0);
    cv::Mat get_imgdat(int);


};








#endif