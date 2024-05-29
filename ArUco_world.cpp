#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>
#include <filesystem>
#include <string>

#include <vector>
#include <map>
#include <tuple>

//my include
#include "DataLoader.h"
#include "LabelTool.h"

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

void test_annotation(LabelTool& labeltool){

    /*  test OK 
    for(int i=0; i<labeltool.get_data_length(); i++){
        cv::imshow("test", labeltool.imshow_with_label(i));
        cv::waitKey(0);
    }
    */
    labeltool.get_anno().box_spawn(); //generate first box
    cv::imshow("test", labeltool.imshow_with_label(20));
    cv::waitKey(0);

    labeltool.get_anno().box_spawn(); //generate second box and configure
    labeltool.get_anno().configure_box(
        1, 
        cv::Point3f(0.1,0,0.1), 
        cv::Point3f(0,0.1,0),
        cv::Vec3f(0,0,0)
        );

    cv::imshow("test", labeltool.imshow_with_label(20));
    cv::waitKey(0);

    labeltool.get_anno().box_remove(0);
    cv::imshow("test", labeltool.imshow_with_label(20));
    cv::waitKey(0);

}
int main(int argc, char** argv )
{
    

    //************************************//
    // Get Data and store in a dataloader //
    //************************************//
    // get  all image

    std::string path = "../data/label1_bag";

    //read all image out
    //vector<string> img_path_list =  get_data_list(path);
    DataLoader dataloader(path);
    //for(int i=0; i<dataloader.length(); i++){
    //    cout<<dataloader[i]<<endl;
    //}
    cv::Mat intrinsic = (cv::Mat_<double>(3,3) << intrinsic_para["fx"], 0.0, intrinsic_para["cx"], 0.0, intrinsic_para["fy"] ,\
                                                        intrinsic_para["cy"], 0.0, 0.0, 1.0);
    cv::Mat dist = (cv::Mat_<double>(1,5) << 0,0,0,0,0);
    

    dataloader.set_Camera_intrinsic(intrinsic);
    dataloader.set_Camera_dist(dist);


    
    /*
    cout << "Camera instricsic matrix:" << endl;
    cout <<  dataloader.get_Camera_intrinsic() << endl;
    */
    //cout << intrinsic.size << endl;
    

    //Test ImageData=================
    /*
    for(int i=0; i<dataloader.length(); i+=10){
        ImageData imgdat(dataloader[i], &intrinsic, &dist);
        imgdat.get_extrinsic();
    }
    */

    //************************************//
    // Labeltool Object                   //
    //************************************//

    LabelTool labeltool(dataloader);
    labeltool.build_data_list(refMarkerArray);
    //labeltool.get_imgdat(20);
    test_annotation(labeltool);
    //labeltool.set_coordinate_ref();

    //Test Labeltool and Dataloader
    //cv::imshow("output", labeltool.get_imgdat(20));
    //cv::waitKey(0);


    return 0;
}