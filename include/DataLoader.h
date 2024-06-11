#ifndef _DATALOADER_H_  

#define _DATALOADER_H_

//class declaration
#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>

#include <vector>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

class DataLoader{
private:
    std::string datapath;
    std::vector<std::string> Image_path_list;
    cv::Mat Camera_intrinsic;
    std::map<std::string, double> instrinsic_para;
    cv::Mat dist;
    void _natsort();

public:
    DataLoader(std::string);
    
    std::string get_item(int idx);
    std::string operator[](int idx);

    //setter & getter
    void set_Camera_intrinsic(cv::Mat);
    void set_Camera_intrinsic(std::map<std::string, double>);
    cv::Mat get_Camera_intrinsic();
    std::map<std::string, double> get_camera_intrinsic_para();
    void set_Camera_dist(cv::Mat);
    cv::Mat get_Camera_dist();
    //不是那麼重要的
    size_t length();
};


#endif