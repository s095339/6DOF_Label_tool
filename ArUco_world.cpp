#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>

#include <vector>
#include <map>


//my include
#include "DataLoader.h"
#include "LabelTool.h"

namespace fs = std::filesystem;
//using namespace cv;
using namespace std;



vector<string> get_data_list(string img_path){

    vector<string> img_path_list = {};

    cout << "read image ... " << endl;
    for (const auto & entry : fs::directory_iterator(img_path)){
        //std::cout << entry.path() << std::endl;
        string entrypath = string(entry.path());
        std::size_t found = entrypath.rfind(".");
        if(string(entry.path()).substr(found) != ".png") continue;
        img_path_list.push_back(entry.path());
    
    }
    //for debug======================
    
    cv::Mat image;
    for(const auto & img_path : img_path_list){
        image = cv::imread( img_path, cv::IMREAD_COLOR );
        cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
        cv::imshow("Display Image", image);
        cv::waitKey(0);
    }
    
    //=================================
    return img_path_list;

}

map<string, double> intrinsic_para = {
    {"fx", 908.78692627},
    {"fy", 907.3694458},
    {"cx", 638.07818604},
    {"cy", 358.32138062}
};

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
    

    //read image from dataloader=================
    /*
    for(int i=0; i<dataloader.length(); i+=10){
        ImageData imgdat(dataloader[i], &intrinsic, &dist);
        imgdat.get_extrinsic();
    }
    */

    LabelTool labeltool(dataloader);
    labeltool.build_data_list();
    cv::imshow("output", labeltool.get_imgdat(20));
    cv::waitKey(0);


    return 0;
}