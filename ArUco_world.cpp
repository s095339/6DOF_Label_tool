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

/* python 
re) MarkerArray={   \
    #===============================
        20:[0.0, 0.0, 0.0],\
        21:[0.5, 0.0, 0.0],\
        22:[0.5, 0.0, 0.7],\
        23:[0.0, 0.0, 0.7],\
    #===============================
        25:[0.0, 0.0, 0.1],\
        26:[0.0, 0.0, 0.2],\
        27:[0.0, 0.0, 0.3],\
        28:[0.0, 0.0, 0.4],\
        29:[0.0, 0.0, 0.5],\
        30:[0.0, 0.0, 0.6],\
    #================================
        14:[0.1, 0.0, 0.0],\
        24:[0.2, 0.0, 0.0],\
        19:[0.3, 0.0, 0.0],\
    #================================
        7:[0.5, 0.0, 0.1],\
        8:[0.5, 0.0, 0.2],\
        9:[0.5, 0.0, 0.3],\
        10:[0.5, 0.0, 0.4],\
        11:[0.5, 0.0, 0.5],\
        12:[0.5, 0.0, 0.6],\
    #=================================
        15:[0.1, 0.0, 0.7],\
        16:[0.2, 0.0, 0.7],\
        17:[0.3, 0.0, 0.7],\
        18:[0.4, 0.0, 0.7],\
    }

*/


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
    
    
    labeltool.get_imgdat(20);

    //labeltool.set_coordinate_ref();

    //Test Labeltool and Dataloader
    //cv::imshow("output", labeltool.get_imgdat(20));
    //cv::waitKey(0);


    return 0;
}