#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>

#include <vector>
#include <map>

namespace fs = std::filesystem;
//using namespace cv;
using namespace std;



vector<string> get_data_list(string img_path){

    vector<string> img_path_list = {};

    cout << "read image ... " << endl;
    for (const auto & entry : fs::directory_iterator(img_path))
        //std::cout << entry.path() << std::endl;
        img_path_list.push_back(entry.path());


    

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
    
    // get  all image

    std::string path = "../data";

    //read all image out
    vector<string> img_path_list =  get_data_list(path);

    cv::Mat intrinsic = (cv::Mat_<double>(3,3) << intrinsic_para["fx"], 0.0, intrinsic_para["cx"], 0.0, intrinsic_para["fy"] ,\
                                                        intrinsic_para["cy"], 0.0, 0.0, 1.0);
    cv::Mat dist = (cv::Mat_<double>(1,5) << 0,0,0,0,0);
    

    
    cout << "Camera instricsic matrix:" << endl;
    cout << dist << endl;
    
    //cout << intrinsic.size << endl;
    



    
   
    
    /*
    if ( argc != 2 )
    {
        cout << "usage: DisplayImage.out <Image_Path>\n" << endl;
        return -1;
    }
 
    Mat image;
    image = imread( argv[1], IMREAD_COLOR );
 
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);
    */
    return 0;
}