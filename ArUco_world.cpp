#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>
#include <filesystem>
#include <vector>
#include <map>
#include <tuple>



//my include
#include "DataLoader.h"
#include "LabelTool.h"
#include "Annotation.h"

namespace fs = std::filesystem;
//using namespace cv;
using namespace std;

map<string, double> intrinsic_para = {
    {"fx", 908.78692627},
    {"fy", 907.3694458},
    {"cx", 638.07818604},
    {"cy", 358.32138062}
};

map<int, cv::Point3f> refMarkerArray = {
    //===============================
        {20, cv::Vec3f(0.0, 0.0, 0.0) },
        {21, cv::Vec3f(0.5, 0.0, 0.0) },
        {22, cv::Vec3f(0.5, 0.0, 0.7) },
        {23, cv::Vec3f(0.0, 0.0, 0.7) },
    //=={==, cv::Vec3f(=============) }===========
        {25, cv::Vec3f(0.0, 0.0, 0.1) },
        {26, cv::Vec3f(0.0, 0.0, 0.2) },
        {27, cv::Vec3f(0.0, 0.0, 0.3) },
        {28, cv::Vec3f(0.0, 0.0, 0.4) },
        {29, cv::Vec3f(0.0, 0.0, 0.5) },
        {30, cv::Vec3f(0.0, 0.0, 0.6) },
    //=={==, cv::Vec3f(=============) }============
        {14, cv::Vec3f(0.1, 0.0, 0.0) },
        {24, cv::Vec3f(0.2, 0.0, 0.0) },
        {19, cv::Vec3f(0.3, 0.0, 0.0) },
    //=={=== cv::Vec3f(=============) }============
        {7,  cv::Vec3f(0.5, 0.0, 0.1) },
        {8,  cv::Vec3f(0.5, 0.0, 0.2) },
        {9,  cv::Vec3f(0.5, 0.0, 0.3) },
        {10, cv::Vec3f(0.5, 0.0, 0.4) },
        {11, cv::Vec3f(0.5, 0.0, 0.5) },
        {12, cv::Vec3f(0.5, 0.0, 0.6) },
    //=={=== cv::Vec3f(=============) }=============
        {15, cv::Vec3f(0.1, 0.0, 0.7) },
        {16, cv::Vec3f(0.2, 0.0, 0.7) },
        {17, cv::Vec3f(0.3, 0.0, 0.7) },
        {18, cv::Vec3f(0.4, 0.0, 0.7) },
};

