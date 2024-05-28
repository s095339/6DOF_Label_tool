#ifndef _LABELTOOL_H_  

#define _LABELTOOL_H_

#include "DataLoader.h"

//STL library
#include <iostream>
#include <map>
//Opencv Library
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>


class ImageData{
private:
    std::string image_path;

    //the camera pose of this image
    cv::Vec3f tvecs; 
    cv::Vec3f rvecs;

    //the camera parameter of this image
    cv::Mat* intrinsic_ptr;
    cv::Mat* dist_ptr;
    
    //image read from opnecv
    cv::Mat image;

    
    int keep_in_mem;

    int _estimateCameraPose(
        std::map<int, cv::Point3f> refMarkerArray,
        std::vector<int> markerIds, 
        std::vector<std::vector<cv::Point2f>> corners
        );
public:                             
    //constructor                                                
    ImageData(std::string, cv::Mat* const, cv::Mat* const);
    //ImageData(ImageData && source); 
    //menber function
    int get_extrinsic(
        std::map<int, cv::Point3f>, 
        int keep_in_mem = 0
        );
    cv::Mat get_image();
};


class LabelTool{
private:
    DataLoader dataloader;
    cv::Mat intrinsic;
    cv::Mat dist;
    std::vector<ImageData> data_list;

    std::map<int, cv::Point3f> refMarkerArray;
    void _set_coordinate_ref(std::map<int, cv::Point3f>);
public:
    LabelTool(DataLoader&); //Constructor
    void build_data_list(std::map<int, cv::Point3f>, int keep_in_mem = 0);
    
    ImageData get_imgdat(int);


};








#endif