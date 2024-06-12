#ifndef _DATALOADER_H_  

#define _DATALOADER_H_

//class declaration
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <filesystem>
#include <string>

#include <vector>
#include <map>
#include <filesystem>

#include <fstream>
#include "nlohmann/json.hpp"
namespace fs = std::filesystem;
using json = nlohmann::json;
class DataLoader{
private:
    std::string datapath;
    std::vector<std::string> Image_path_list;
    cv::Mat Camera_intrinsic;
    std::map<std::string, double> intrinsic_params;
    cv::Mat dist;
    void _natsort();
    //world_camera setting
    int aruco_dict;
    std::map<int, cv::Point3f> refMarkerArray;
    //std::map<std::string, double> intrinsic_params;

    void _set_Camera_intrinsic();
public:
    DataLoader(std::string);
    
    std::string get_item(int idx);
    std::string operator[](int idx);
    
    //setter & getter
    
    

    cv::Mat get_Camera_intrinsic();
    std::map<std::string, double> get_camera_intrinsic_para();
    void set_Camera_dist(cv::Mat);
    cv::Mat get_Camera_dist();
    std::map<int, cv::Point3f> get_refMarkerArray();
    int get_aruco_dict();
    //不是那麼重要的
    size_t length();
};


#endif