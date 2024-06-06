#include "DataLoader.h"
#include "natural_sort.hpp"
//constructor initialization list

using namespace std;
namespace fs = std::filesystem;

DataLoader::DataLoader(std::string datapath)
:datapath{datapath}
{
    //vector<string> img_path_list = {};

    cout << "read image ... " << endl;
    for (const auto & entry : fs::directory_iterator(this->datapath)){
        //std::cout << "read " << entry.path() << std::endl;
        string entrypath = string(entry.path());
        std::size_t found = entrypath.rfind(".");
        if(string(entry.path()).substr(found) != ".png") continue;
        this->Image_path_list.push_back(entry.path());
    
    }
    //for debug======================
    /*
    cv::Mat image;
    for(const auto & img_path : img_path_list){
        image = cv::imread( img_path, cv::IMREAD_COLOR );
        cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
        cv::imshow("Display Image", image);
        cv::waitKey(0);
    }
    */

    //this->Image_path_list 
    this->_natsort();
}

void DataLoader::set_Camera_intrinsic(cv::Mat intr){
    this->Camera_intrinsic = intr;
}

cv::Mat DataLoader::get_Camera_intrinsic(){
    return this->Camera_intrinsic;
}

void DataLoader::set_Camera_dist(cv::Mat dist){
    this->dist = dist;
}

cv::Mat DataLoader::get_Camera_dist(){
    return this->dist;
}



void DataLoader::_natsort(){

    SI::natural::sort(this->Image_path_list);

}

std::string DataLoader::get_item(int idx){
    return this->Image_path_list.at(idx);
}

std::string  DataLoader::operator[](int idx){
    return this->Image_path_list.at(idx);
}


size_t DataLoader::length(){
    return this->Image_path_list.size();
}