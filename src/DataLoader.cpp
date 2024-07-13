#include "DataLoader.h"
#include "natural_sort.hpp"
#include <sys/stat.h>
//constructor initialization list

using namespace std;
namespace fs = std::filesystem;

//******/
//int aruco_dict;
//std::map<std::string, double> intrinsic_params;
//std::map<int, cv::Point3f> refMarkerArray;
// Map to convert dictionary name to OpenCV constant
std::map<std::string, int> aruco_dict_map = {
    {"DICT_4X4_50", cv::aruco::DICT_4X4_50},
    {"DICT_4X4_100", cv::aruco::DICT_4X4_100},
    {"DICT_4X4_250", cv::aruco::DICT_4X4_250},
    {"DICT_4X4_1000", cv::aruco::DICT_4X4_1000},
    {"DICT_5X5_50", cv::aruco::DICT_5X5_50},
    {"DICT_5X5_100", cv::aruco::DICT_5X5_100},
    {"DICT_5X5_250", cv::aruco::DICT_5X5_250},
    {"DICT_5X5_1000", cv::aruco::DICT_5X5_1000},
    {"DICT_6X6_50", cv::aruco::DICT_6X6_50},
    {"DICT_6X6_100", cv::aruco::DICT_6X6_100},
    {"DICT_6X6_250", cv::aruco::DICT_6X6_250},
    {"DICT_6X6_1000", cv::aruco::DICT_6X6_1000},
    {"DICT_7X7_50", cv::aruco::DICT_7X7_50},
    {"DICT_7X7_100", cv::aruco::DICT_7X7_100},
    {"DICT_7X7_250", cv::aruco::DICT_7X7_250},
    {"DICT_7X7_1000", cv::aruco::DICT_7X7_1000},
    {"DICT_ARUCO_ORIGINAL", cv::aruco::DICT_ARUCO_ORIGINAL},
    {"DICT_APRILTAG_16h5", cv::aruco::DICT_APRILTAG_16h5},
    {"DICT_APRILTAG_25h9", cv::aruco::DICT_APRILTAG_25h9},
    {"DICT_APRILTAG_36h10", cv::aruco::DICT_APRILTAG_36h10},
    {"DICT_APRILTAG_36h11", cv::aruco::DICT_APRILTAG_36h11}
};



//***Member Function***/
DataLoader::DataLoader(std::string datapath)
:datapath{datapath}
{
    //vector<string> img_path_list = {};
    std::string filePath = datapath +"/world_camera_setting.json";
    //read json//
    json jsonData;
    

    try{
        std::ifstream file(filePath);
        file >> jsonData;
    }catch (const std::exception& e){
        std::cout<< "[ERROR] world_camera_setting.json not found!"<<std::endl;
        return;
    }
    IsDepth = jsonData["depth"];
    std::string color_image_path = datapath + "/color";
    std::string depth_image_path = datapath + "/depth";
    std::cout << "reading color data from "<< color_image_path << std::endl;
    if(IsDepth)std::cout << "reading depth data from "<< depth_image_path << std::endl;

    if(IsDepth){
        struct stat sb;
        if (stat(depth_image_path.c_str(), &sb) != 0){
            std::cout << "depth dir doesn't exist" << std::endl;
            return;
        }
    }
    
    
    cout << "read image ... " << endl;
    std::vector<std::string> image_path_list_temp;
    for (const auto & entry : fs::directory_iterator(color_image_path)){
        std::cout << "read " << entry.path() << std::endl;
        string entrypath = string(entry.path());
        std::size_t found = entrypath.rfind("_color.");
        if(IsDepth && string(entry.path()).substr(found) != "_color.png"){
            std::cout << "[delete] " << entry.path() << std::endl;
            continue;
        }
        else if(!IsDepth && string(entry.path()).substr(found) != ".png"){
            continue;
        }

       image_path_list_temp.push_back(entry.path());
    
    }
    

    //world camera setting
    //std::map<std::string, double> intrinsic_params;
    //std::map<int, cv::Point3f> refMarkerArray;
    

    try {

       
        
        // Read the aruco_dict
        std::string dict_name = jsonData["aruco_dict"];
        if (aruco_dict_map.find(dict_name) != aruco_dict_map.end()) {
            aruco_dict = aruco_dict_map[dict_name];
        } else {
            throw std::runtime_error("Unsupported ArUco dictionary type: " + dict_name);
        }

        // Read intrinsic parameters
        for (const auto& item : jsonData["_intrinsic_para"]) {
            for (const auto& [key, value] : item.items()) {
                intrinsic_params[key] = value;
            }
        }

        // Read reference marker array
        for (const auto& item : jsonData["_refMarkerArray"]) {
            for (const auto& [key, value] : item.items()) {
                int marker_id = std::stoi(key);
                cv::Point3f point(value[0], value[1], value[2]);
                refMarkerArray[marker_id] = point;
            }
        }
        

        // Print the data to verify
        std::cout << "Aruco Dictionary: " << aruco_dict << std::endl;
        std::cout << "Intrinsic Parameters:" << std::endl;
        for (const auto& [key, value] : intrinsic_params) {
            std::cout << "  " << key << ": " << value << std::endl;
        }

        std::cout << "Reference Marker Array:" << std::endl;
        for (const auto& [key, value] : refMarkerArray) {
            std::cout << "  Marker " << key << ": (" << value.x << ", " << value.y << ", " << value.z << ")" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return ;
    }

    //this->Image_path_list 
    this->_set_Camera_intrinsic();
    this->_natsort(image_path_list_temp);
    if(IsDepth){
        std::cout << "check: "<< std::endl;
        for(const auto& color_file_path: image_path_list_temp){
            fs::path full_path(color_file_path);

            // Extract the filename from the path
            std::string color_filename = full_path.filename().string();
            std::string depth_file_name;
            auto pos = color_filename.find("_color.png");
            if (pos != std::string::npos) {
                // Replace "_color.png" with "_depth.png"
                depth_file_name = color_filename.substr(0, pos) + "_depth.png";
                
            } else {
                std::cerr << "The specified filename does not contain '_color.png'" << std::endl;
                continue;
            }


            // Check if the corresponding depth file exists
            //std::cout << "check: " << depth_file_name << std::endl;
        
            fs::path depth_file_path = fs::path(depth_image_path) / depth_file_name;
            if (fs::exists(depth_file_path)) {
                std::cout << "read color: " << color_file_path<< "\nread depth:"<< depth_file_name << std::endl;
                this->Image_path_list.push_back(color_file_path);
                this->depth_path_list.push_back(depth_file_path);
            }
        }
    }else{
        std::cout << "check: "<< std::endl;
        for(const auto& color_file_path: image_path_list_temp){
                this->Image_path_list.push_back(color_file_path);
        }
    }


    
}


void DataLoader::_set_Camera_intrinsic(){
    this->Camera_intrinsic = (cv::Mat_<double>(3,3) << this->intrinsic_params["fx"], 0.0, this->intrinsic_params["cx"], 0.0, this->intrinsic_params["fy"] ,\
                                                        this->intrinsic_params["cy"], 0.0, 0.0, 1.0);
}

cv::Mat DataLoader::get_Camera_intrinsic(){
    return this->Camera_intrinsic;
}
map<string, double> DataLoader::get_camera_intrinsic_para(){
    return this->intrinsic_params;
}
void DataLoader::set_Camera_dist(cv::Mat dist){
    this->dist = dist;
}

cv::Mat DataLoader::get_Camera_dist(){
    return this->dist;
}

std::map<int, cv::Point3f> DataLoader::get_refMarkerArray(){

    return this->refMarkerArray;
}
int DataLoader::get_aruco_dict(){
    return this->aruco_dict;
}

void DataLoader::_natsort(){

    SI::natural::sort(this->Image_path_list);
    if(IsDepth) SI::natural::sort(this->depth_path_list);
}
void DataLoader::_natsort(std::vector<std::string>& image_list){
    SI::natural::sort(image_list);
}

std::string DataLoader::get_depth(int idx){
    return this->depth_path_list.at(idx);
}



std::string  DataLoader::operator[](int idx){
    return this->Image_path_list.at(idx);
}


size_t DataLoader::length(){
    return this->Image_path_list.size();
}