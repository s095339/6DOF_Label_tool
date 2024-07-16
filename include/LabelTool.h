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
#include "nlohmann/json.hpp"

class ImageData{
private:
    std::string image_path;

    //depth
    bool is_depth;
    std::string depth_path;
    
    //the camera pose of this image
    cv::Vec3f tvecs; 
    cv::Vec3f rvecs;

    //the camera parameter of this image
    std::map<std::string, double> intrinsic_para;
    cv::Mat* intrinsic_ptr;
    cv::Mat* dist_ptr;
    
    //image read from opnecv
    cv::Mat image;
    int keep_in_mem;

    //h,w
    int height;
    int width;
    
    //world_camera setting
    //int aruco_dict;
    //std::map<int, cv::Point3f> refMarkerArray;
    //std::map<std::string, double> intrinsic_params;

    int _estimateCameraPose(
        std::map<int, cv::Point3f> refMarkerArray,
        std::vector<int> markerIds, 
        std::vector<std::vector<cv::Point2f>> corners
        );

public:                             
    //constructor                                                  
    ImageData(std::string,std::map<std::string, double>, cv::Mat* const, cv::Mat* const);//only color
    ImageData(std::string,std::string,std::map<std::string, double>, cv::Mat* const, cv::Mat* const);// color and depth
    //ImageData(ImageData && source); 
    //menber function
    void set_intrinsic_para(std::map<std::string, double>);//for dumping dataset to json
    void set_aruco_dict(int);
    void refMarkerArray(std::map<int, cv::Point3f>);
     
    int calculate_extrinsic(
        std::map<int, cv::Point3f>, 
        int,
        int keep_in_mem = 0
        );
    std::tuple<cv::Vec3f, cv::Vec3f> get_extrinsic();
    
    cv::Mat get_image();
    std::string get_image_path();
    std::string get_depth_path();
    json get_image_json() ;
};


class LabelTool{
private:
    DataLoader dataloader;
    std::map<std::string, double> intrinsic_para;
    cv::Mat intrinsic;
    cv::Mat dist;
    std::vector<ImageData> data_list;

    //Annotation object
    Annotation anno;
    double world_rotate_degree;// rotation world to avoid gimbal lock

    //world camera setting
    std::map<int, cv::Point3f> refMarkerArray;
    int aruco_dict;
    //depth 
    bool is_depth;
    bool _get_pnp_pose(std::vector<cv::Point2f>&, cv::Vec3f, cv::Vec3f&, cv::Vec3f&);


public:
    LabelTool(DataLoader&); //Constructor
    void build_data_list(int interval = 1, int keep_in_mem = 0);
    void world_rotation(double degree = 0);
    double get_world_rotation(){ return this->world_rotate_degree; };

    ImageData get_imgdat(int);
    Annotation& get_anno();
    
    cv::Mat imshow_with_label(
        int idx, 
        int show_selected_box_direction = -1, 
        int show_selected_grasp = -1,
        int show_selected_paired_id = -1
         ); //for test

    int get_data_length();// get the number of imagedata
    void generate_annotation();

    void remove_imgdat(int);
    void dump_dataset_json();

    void dump_annotaion_json(const std::string& filename,  double world_rotation );
    int load_annotation_json(const std::string& filename);
};







#endif