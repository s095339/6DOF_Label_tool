#ifndef _LABELTOOL_H_  

#define _LABELTOOL_H_

#include "DataLoader.h"
#include "Annotation.h"
//STL library
#include <iostream>
#include <map>
#include <tuple>
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
    int calculate_extrinsic(
        std::map<int, cv::Point3f>, 
        int keep_in_mem = 0
        );
    std::tuple<cv::Vec3f, cv::Vec3f> get_extrinsic();

    cv::Mat get_image();
    std::string get_image_path();
};


class LabelTool{
private:
    DataLoader dataloader;
    cv::Mat intrinsic;
    cv::Mat dist;
    std::vector<ImageData> data_list;

    std::map<int, cv::Point3f> refMarkerArray;

     //Annotation object
    Annotation anno;


    void _set_coordinate_ref(std::map<int, cv::Point3f>);
public:
    LabelTool(DataLoader&); //Constructor
    void build_data_list(std::map<int, cv::Point3f>, int interval = 1, int keep_in_mem = 0);
    

    ImageData get_imgdat(int);
    Annotation& get_anno();
    
    cv::Mat imshow_with_label(int idx, int show_selected_box_direction = -1); //for test

    int get_data_length();// get the number of imagedata
    void generate_annotation();

    void remove_imgdat(int);
};








#endif