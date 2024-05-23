#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>

#include <vector>


namespace fs = std::filesystem;
//using namespace cv;
using namespace std;




int main(int argc, char** argv )
{
    
    // get  all image

    std::string path = "../data";
    
    vector<string> img_path_list = {};

    cout << "read image ... " << endl;
    for (const auto & entry : fs::directory_iterator(path))
        //std::cout << entry.path() << std::endl;
        img_path_list.push_back(entry.path());


    
    string img_path = "";
    cv::Mat image;
    for(const auto & img_path : img_path_list){
        image = cv::imread( img_path, cv::IMREAD_COLOR );
        cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
        cv::imshow("Display Image", image);
        cv::waitKey(0);
    }
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