#include "DataLoader.h"
#include <iostream>
#include "LabelTool.h"
#include "Annotation.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <tuple>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <fstream>


namespace fs = std::filesystem;


//non-menber function
void rotatePlane(cv::Point3f& point, double degree) {
    double rad = degree * M_PI / 180.0;
    double cosTheta = cos(rad);
    double sinTheta = sin(rad);

    double x = point.x * cosTheta - point.z * sinTheta;
    double z = point.x * sinTheta + point.z * cosTheta;
    point.x = x;
    point.z = z;
}
//menber function

//************************//
// ImageData              //
//************************//
ImageData::ImageData(std::string image_path,std::map<std::string, double>intrinsic_para,  cv::Mat* const intr_ptr, cv::Mat* const dist_ptr)
:image_path{image_path}, keep_in_mem{0}
{
    this->intrinsic_para = intrinsic_para;
    this->intrinsic_ptr = intr_ptr;
    this->dist_ptr = dist_ptr;
    this->is_depth = false;
}

ImageData::ImageData(std::string image_path,std::string depth_path,std::map<std::string, double>intrinsic_para,  cv::Mat* const intr_ptr, cv::Mat* const dist_ptr)
:image_path{image_path},depth_path{depth_path}, keep_in_mem{0}
{
    this->intrinsic_para = intrinsic_para;
    this->intrinsic_ptr = intr_ptr;
    this->dist_ptr = dist_ptr;
    this->is_depth = true;
}
cv::Mat ImageData::get_image(){
    if(this->keep_in_mem)
        return this->image.clone();
    else
        return cv::imread(this->image_path, cv::IMREAD_COLOR);
    
}

void ImageData::set_intrinsic_para(std::map<std::string, double> intr_para){
    this->intrinsic_para = intr_para;
}

int ImageData::calculate_extrinsic(std::map<int, cv::Point3f>refMarkerArray,int aruco_dict,  int keep_in_mem)
{   
    /*
    Parameter
    -----------------------------------------
    refMarkerArray: aruco marker 3d position map
    keep_in_mem: store image in memory(keep_in_mem=1) or not (keep_in_men=0)

    Return
    ------------------------------------------
    int flag: return 1 : pnp run successfully
              otherwise: pnp can't calculate CameraPose
    
    */

    //this->image_path
    //this->intrinsic
    //this->dist
    //std::cout<<"opencv version:"<<CV_VERSION;  // opencv 4.7.0
    //read image from path
    cv::Mat image = cv::imread(this->image_path, cv::IMREAD_COLOR);
    
    cv::Mat img_gray;
    cv::cvtColor(image, img_gray, cv::COLOR_BGR2GRAY);
    this->width = image.size[1];
    this->height = image.size[0];;
    //python
    //aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_100)
    //parameters = aruco.DetectorParameters_create()

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(aruco_dict);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);
    detector.detectMarkers(image, markerCorners, markerIds, rejectedCandidates);
    /*
    cv::Mat outputImage = image.clone();
    cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    cv::imshow("display", outputImage);
    cv::waitKey(0);
    */

    

    if(this->keep_in_mem){
        this->image = image.clone();
        this->keep_in_mem = 1;
    }
    
    //TODO: get extrinsic from multiple aruco markers
    int flag;
    try{
        flag = this->_estimateCameraPose(refMarkerArray, markerIds, markerCorners);
    }catch(const std::exception& e){
        std::cout << e.what() << std::endl;
        flag = 0;
    }    
    return flag;
}
int ImageData::_estimateCameraPose(
    std::map<int, cv::Point3f> refMarkerArray,
    std::vector<int> markerIds, 
    std::vector<std::vector<cv::Point2f>> corners
    )
{
    /*
    Parameter
    --------------------------
    refMarkerArray: aruco marker 3d position map
    markerIds:  detected markerIds
    corners: detected marker's corners

    Return
    --------------------------
    int flag: return 1 : pnp run successfully
              otherwise: pnp can't calculate CameraPose
    */
    //marker_count = len(refMarkerArray)
    //#if marker_count<4: #標誌板少於四個
    //#    raise RuntimeError('at least 3 pair of points required when invoking solvePnP')
    int marker_count = refMarkerArray.size();
    
    if(marker_count<4)
        throw std::runtime_error("At least 3 pair of points required when invoking solvePnP");

    //
    //corners=corners; ids=markerIDs
    //print('ids:\n')
    //print(ids)
    //print('corners:\n')
    //print(corners)

    
    //objectPoints=[]
    //imagePoints=[]
    
    std::vector<cv::Point3f> objectPoints;
    std::vector<cv::Point2f> imagePoints;
    //#檢查是否探測到了所有預期的基準marker
    //if len(ids) !=0: #檢測到了marker,存儲marker的世界坐標到objectPoints，構建對應的圖像平面坐標列表 imagePoints
    //    print('------detected ref markers----')
    //    for i in range(len(ids)): #遍歷探測到的marker ID,
    //        if ids[i][0] in refMarkerArray: #如果是參考點的標志，提取基準點的圖像座標，用於構建solvePnP()的輸入
    //
    //            print('id:\n ' + str(ids[i][0]))
    //            print('cornors: \n '+ str(corners[i][0]))
    //            objectPoints.append(refMarkerArray[ ids[i][0] ])
    //            imagePoints.append(corners[i][0][0].tolist()) #提取marker的左上點
    //    objectPoints=np.float32(objectPoints)
    //    imagePoints=np.float32(imagePoints)
    //        
    //    print('------------------------------\n')
    //    print('objectPoints:\n'+str(objectPoints))
    //    print('imagePoints:\n'+str(imagePoints))
    //    pass
    //else:
    //    return False, None, None
    std::vector<int> idx_list;
    for(auto const& [key, val] : refMarkerArray) idx_list.push_back(key);
    //std::cout<< idx_list << std::endl;
    
    if(markerIds.size()!=0){
        //std::cout<< "----------detect ref markers------------" << std::endl;
        for(int i=0; i<markerIds.size(); i++){
            bool found = (std::find(idx_list.begin(), idx_list.end(), markerIds[i]) != idx_list.end());
            if(found){
                //std::cout<< markerIds[i] <<":"<< corners[i][0] << std::endl;
                objectPoints.push_back(refMarkerArray[markerIds[i]]);
                imagePoints.push_back(corners[i][0]);
            }
        }
        //std::cout<< objectPoints << std::endl;
    }

    //
    //#如果檢測到的基準參考點大於3個，可以解算相機的姿態啦
    //if len(objectPoints)>=4:
    //    #至少需要4個點
    //    retval, rvec, tvec = cv2.solvePnP(objectPoints, imagePoints, cameraMtx, dist)
    //    #rMatrix,jacobian = cv2.Rodrigues(rvec)
    //    return True, rvec, tvec
    //else:
    //    return False, None, None
    //

    /*
    Copy from opencv documentation
    objectPoints:	Array of object points in the object coordinate space, Nx3 1-channel or 1xN/Nx1 3-channel, where N is the number of points. vector<Point3d> can be also passed here.
    imagePoints:	Array of corresponding image points, Nx2 1-channel or 1xN/Nx1 2-channel, where N is the number of points. vector<Point2d> can be also passed here.
    */
    cv::Vec3f rvecs, tvecs;
    int flag;
    if(objectPoints.size()>=4){
        //std::cout << "pnp" << std::endl;
        flag = cv::solvePnP(objectPoints, imagePoints, *(this->intrinsic_ptr), *(this->dist_ptr), rvecs, tvecs);
        //std::cout << "yoyo" << std::endl;
    }

    /*
    printf("flag = %d\n", flag);
    std::cout<< "rvecs: "<< rvecs << std::endl;
    std::cout<< "tvecs: "<< tvecs << std::endl;
    */

    this->rvecs = rvecs;
    this->tvecs = tvecs;

    return flag;
}   


std::tuple<cv::Vec3f, cv::Vec3f> ImageData::get_extrinsic(){
    return  std::tuple<cv::Vec3f, cv::Vec3f>(this->tvecs, this->rvecs);
}

std::string ImageData::get_image_path(){
    return this->image_path;
}
std::string ImageData::get_depth_path(){
    return this->depth_path;
}
json ImageData::get_image_json() {
    //std::cout << "goint to reat" << std::endl;
    //std::cout << "it's ok" << std::endl;        
    return {
                {"rvec", {rvecs[0], rvecs[1], rvecs[2]}},
                {"tvec", {tvecs[0], tvecs[1], tvecs[2]}},
                {"height", height},
                {"width", width},
                {"intrinsics", {
                    {"cx", intrinsic_para.at("cx")},
                    {"cy", intrinsic_para.at("cy")},
                    {"fx", intrinsic_para.at("fx")},
                    {"fy", intrinsic_para.at("fy")}
                }}
            };
}
//************************//
// LabelTool              //
//************************//

LabelTool::LabelTool(DataLoader& dataloader)
:
dataloader{dataloader}, 
intrinsic_para{dataloader.get_camera_intrinsic_para()},
intrinsic{dataloader.get_Camera_intrinsic()},
dist{dataloader.get_Camera_dist()},
aruco_dict{dataloader.get_aruco_dict()},
refMarkerArray{dataloader.get_refMarkerArray()},
anno{Annotation()},
is_depth{dataloader.is_depth()}
{
    this->world_rotate_degree = 0.0;
}

void LabelTool::build_data_list(int interval,  int keep_in_mem){

    
    // add all image 
    std::cout << "build data list ... " << std::endl;
    if(this->is_depth) std::cout << "depth data "<<std::endl;
    int deleted = 0;
    int count = 0;
    for(int i=0; i<dataloader.length(); i+=interval){
        if(this->is_depth) {
            //std::cout << "read image:  "<< dataloader[i] <<"and " <<  dataloader.get_depth(i)<<std::endl;
            ImageData imgdat(dataloader[i], dataloader.get_depth(i),this->intrinsic_para, &(this->intrinsic), &(this->dist));// &(this->intrinsic) &(this->dist)
            std::cout << "read color image "<< count <<" : " <<dataloader[i] << std::endl;
            std::cout << "read depth image "<< count <<" : " <<dataloader.get_depth(i) << std::endl;
            count++;
            int flag = imgdat.calculate_extrinsic(refMarkerArray, this->aruco_dict, keep_in_mem);
            if(flag == 1) this->data_list.push_back(imgdat);
            else {
                try {
                    if (fs::remove(imgdat.get_image_path()) && fs::remove(imgdat.get_depth_path())) {
                        std::cout << "File " << imgdat.get_image_path() << " and " << imgdat.get_depth_path() << " deleted successfully.\n";
                    } else {
                        std::cout << "File " << imgdat.get_image_path() << " and " << imgdat.get_depth_path()<< " not found.\n";
                    }
                } catch (const fs::filesystem_error& e) {
                    std::cerr << "Error deleting file: " << e.what() << '\n';
                }
                deleted++;
            }
        }
        else{ 
            ImageData imgdat(dataloader[i],this->intrinsic_para, &(this->intrinsic), &(this->dist));// &(this->intrinsic) &(this->dist)
            std::cout << "read image "<< count++ <<" : " <<dataloader[i] << std::endl;
            int flag = imgdat.calculate_extrinsic(refMarkerArray, this->aruco_dict, keep_in_mem);
            if(flag == 1) this->data_list.push_back(imgdat);
            else {
                try {
                    if (fs::remove(imgdat.get_image_path())) {
                        std::cout << "File " << imgdat.get_image_path() << " deleted successfully.\n";
                    } else {
                        std::cout << "File " << imgdat.get_image_path() << " not found.\n";
                    }
                } catch (const fs::filesystem_error& e) {
                    std::cerr << "Error deleting file: " << e.what() << '\n';
                }
                deleted++;
            }
        }
        
    }

    std::cout << "[info] stored " << this->data_list.size() << 
        " images in LabelTool, "<< deleted << " images deleted"
        << std::endl;
    // get extrinsic
    //for(auto &imgdat : this->data_list){
    //    imgdat.calculate_extrinsic(this->refMarkerArray, keep_in_mem);
    //}
    
}

void LabelTool::world_rotation(double degree){

    this->world_rotate_degree = this->world_rotate_degree + degree;
    if(this->world_rotate_degree >= 360) this->world_rotate_degree-=360;
    //for(const auto& [key, value] : refMarkerArray){
    //        std::cout << "  " << key << ": " << value << std::endl;
    //}
    //轉世界
    //std::cout << " after rotation "<< std::endl;
    for(auto& [key, point] : this->refMarkerArray) {
        rotatePlane(point, degree);
    }

    //for(const auto& [key, value] : refMarkerArray){
    //        std::cout << "  " << key << ": " << value << std::endl;
    //    }
    //轉圖片
    for(auto & imgdat:this->data_list){
        imgdat.calculate_extrinsic(refMarkerArray, this->aruco_dict);//re-calculate
    }
    //轉箱子
    for(int i=0; i<this->anno.box_number(); i++ ){
        Box3d& box = this->anno.get_box(i);
        //box.rotate_world(degree);
    }


}

ImageData LabelTool::get_imgdat(int idx){
    return this->data_list.at(idx);
}



Annotation& LabelTool::get_anno(){
    return this->anno;
}

int LabelTool::get_data_length(){
    return this->data_list.size();
}
cv::Mat LabelTool::imshow_with_label(
    int idx, 
    int show_selected_box_direction,
    int show_selected_paired_grasp,
    int show_selected_paired_id
    ){
                                    // image id   //the id of the box 
    //
    cv::Point3f origin(0.0,0.0,0.0);
    cv::Point3f xaxis(0.15,0.0,0.0);
    cv::Point3f yaxis(0.0,0.15,0.0);
    cv::Point3f zaxis(0.0,0.0,0.15);
    std::vector<cv::Point3f> axis_point = {   
        origin,
        xaxis,
        yaxis,
        zaxis
    };

   
    // get image data
    ImageData imgdat = this->get_imgdat(idx);

    //get image
    cv::Mat img = imgdat.get_image();

    //get image extrinsic
    cv::Vec3f tvecs, rvecs;
    tvecs = std::get<0>(imgdat.get_extrinsic());
    rvecs = std::get<1>(imgdat.get_extrinsic());
    //print origin axis
    std::vector<cv::Point2f> pts_camera_axis; 
    cv::projectPoints(axis_point,rvecs, tvecs, this->intrinsic, this->dist, pts_camera_axis);
    cv::arrowedLine(img,pts_camera_axis[0],pts_camera_axis[3], cv::Scalar(255,0,0),2);
    cv::arrowedLine(img,pts_camera_axis[0],pts_camera_axis[2], cv::Scalar(0,255,0),2);
    cv::arrowedLine(img,pts_camera_axis[0],pts_camera_axis[1], cv::Scalar(0,0,255),2);
    cv::putText(img, "x", pts_camera_axis[1], 1, 2,cv::Scalar(0,0,255),1);
    cv::putText(img, "y", pts_camera_axis[2], 1, 2,cv::Scalar(0,255,0),1);
    cv::putText(img, "z", pts_camera_axis[3], 1, 2,cv::Scalar(255,0,0),1);
    // get annotation data (box information)
    for(int i=0 ; i<this->anno.box_number(); i++)//iterate all box 
    {
        Box3d box = this->anno.get_box(i);
        std::vector<cv::Point3f> vertices = box.get_vertex();

        //transformation box coordinate from world to image plane  
        std::vector<cv::Point2f> pts_camera; 
        cv::projectPoints(vertices,rvecs, tvecs, this->intrinsic, this->dist, pts_camera);

        
        //visualize box 
        idx = 1;
        for(auto& pt : pts_camera){
            //if(idx<9)cv::putText(
            //    img, std::to_string(idx++),pt+cv::Point2f(2,2), 1, 2, cv::Scalar(0,255,255),2
            //    );

            //cv::circle(img, pt, 2, cv::Scalar(0,128,128));
        }

        cv::line(img,pts_camera[0],pts_camera[1], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[0],pts_camera[4], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[5],pts_camera[1], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[4],pts_camera[5], cv::Scalar(255,0,0),2);

        cv::line(img,pts_camera[2],pts_camera[6], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[3],pts_camera[7], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[2],pts_camera[3], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[6],pts_camera[7], cv::Scalar(255,0,0),2);

        cv::line(img,pts_camera[0],pts_camera[2], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[3],pts_camera[1], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[7],pts_camera[5], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[6],pts_camera[4], cv::Scalar(255,0,0),2);

        if(i == show_selected_box_direction){
            cv::arrowedLine(img,pts_camera[8],pts_camera[9], cv::Scalar(255,0,0),3,7);
            cv::arrowedLine(img,pts_camera[8],pts_camera[10], cv::Scalar(0,255,0),3,7);
            cv::arrowedLine(img,pts_camera[8],pts_camera[11], cv::Scalar(0,0,255),3,7);
        }


        // print grasp
        

        for(int j=0; j< box.paired_grasp_number(); j++){
            cv::Scalar color(0, 255, 0);
            
            if(j == show_selected_paired_grasp){
                std::cout << " paired grasp color "<< std::endl;
                color = cv::Scalar(0,0,255);
            }

            std::vector<cv::Point3f> vertices_grasp0 = std::get<0>(box.get_paired_grasp(j)).get_vertex(); 
            std::vector<cv::Point2f> pts_camera_grasp0; 
            cv::projectPoints(vertices_grasp0,rvecs, tvecs, this->intrinsic, this->dist, pts_camera_grasp0);

            for(int kk=0; kk<5; kk++){
                auto pt = pts_camera_grasp0[kk];

                cv::circle(img, pt, 2, cv::Scalar(0,128,128));
            }


            cv::line(img,pts_camera_grasp0[5],pts_camera_grasp0[7], color,2);
            cv::line(img,pts_camera_grasp0[6],pts_camera_grasp0[8], color,2);
            cv::line(img,pts_camera_grasp0[7],pts_camera_grasp0[8], color,2);
            if(i == show_selected_box_direction && j == show_selected_paired_grasp && show_selected_paired_id == 0){
                std::cout << " paired grasp id 0 "<< std::endl;

                cv::line(img,pts_camera_grasp0[1],pts_camera_grasp0[2], cv::Scalar(200,200,0),2);
                cv::line(img,pts_camera_grasp0[4],pts_camera_grasp0[2], cv::Scalar(200,200,0),2);
                cv::line(img,pts_camera_grasp0[3],pts_camera_grasp0[4], cv::Scalar(200,200,0),2);
                cv::line(img,pts_camera_grasp0[1],pts_camera_grasp0[3], cv::Scalar(200,200,0),2);
            }


            std::vector<cv::Point3f> vertices_grasp1 = std::get<1>(box.get_paired_grasp(j)).get_vertex(); 
            std::vector<cv::Point2f> pts_camera_grasp1; 
            cv::projectPoints(vertices_grasp1,rvecs, tvecs, this->intrinsic, this->dist, pts_camera_grasp1);

            for(int kk=0; kk<5; kk++){
                auto pt = pts_camera_grasp1[kk];

                cv::circle(img, pt, 2, cv::Scalar(0,128,128));
            }
            
            cv::line(img,pts_camera_grasp1[5],pts_camera_grasp1[7], color,2);
            cv::line(img,pts_camera_grasp1[6],pts_camera_grasp1[8], color,2);
            cv::line(img,pts_camera_grasp1[7],pts_camera_grasp1[8], color,2);
            if(i == show_selected_box_direction && j == show_selected_paired_grasp && show_selected_paired_id == 1){
                cv::line(img,pts_camera_grasp1[1],pts_camera_grasp1[2], cv::Scalar(200,200,0),2);
                cv::line(img,pts_camera_grasp1[4],pts_camera_grasp1[2], cv::Scalar(200,200,0),2);
                cv::line(img,pts_camera_grasp1[3],pts_camera_grasp1[4], cv::Scalar(200,200,0),2);
                cv::line(img,pts_camera_grasp1[1],pts_camera_grasp1[3], cv::Scalar(200,200,0),2);
            }
        }
    }

    
    //cv::imshow("Stream", img);
    //cv::waitKey(0);
    return img;
}

void LabelTool::generate_annotation(){

    std::cout << "[todo] Generate annotation" << std::endl;
}

void LabelTool::remove_imgdat(int idx){
    //this->Box_list.erase(this->Box_list.begin() + box_id);
    ImageData& imgdat = this->data_list.at(idx);
    std::string color_path = imgdat.get_image_path();
    std::string depth_path;
    
    try {
        // Check if the file exists before attempting to delete it
        if (fs::exists(color_path)) {
            // Remove the file
            fs::remove(color_path);
            std::cout << "File deleted successfully: " << color_path << std::endl;
        } else {
            std::cerr << "File does not exist: " << color_path << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }


    if(this->is_depth){
        depth_path = imgdat.get_depth_path();
        try {
        // Check if the file exists before attempting to delete it
            if (fs::exists(depth_path)) {
                // Remove the file
                fs::remove(depth_path);
                std::cout << "File deleted successfully: " << depth_path << std::endl;
            } else {
                std::cerr << "File does not exist: " << depth_path << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } 
    
    


    this->data_list.erase(this->data_list.begin()+idx);

}

bool LabelTool::_get_pnp_pose(std::vector<cv::Point2f>& points_2d, cv::Vec3f size,  cv::Vec3f&rvec , cv::Vec3f&tvec){
    float width = size[0];   // Extract width from size
    float height = size[1];  // Extract height from size
    float depth = size[2];   // Extract depth from size

    // Calculate vertices in left-handed coordinates
    float cx = 0;
    float cy = 0;
    float cz = 0;

    float left = cx + width / 2.0;
    float right = cx - width / 2.0;
    float top = cy + height / 2.0;
    float bottom = cy - height / 2.0;
    float front = cz + depth / 2.0;
    float rear = cz - depth / 2.0;

    std::vector<cv::Point3f> objectPoints = {
        cv::Point3f(left, bottom, rear),
        cv::Point3f(left, bottom, front),
        cv::Point3f(left, top, rear),
        cv::Point3f(left, top, front),
        cv::Point3f(right, bottom, rear),
        cv::Point3f(right, bottom, front),
        cv::Point3f(right, top, rear),
        cv::Point3f(right, top, front),
    };

    return cv::solvePnP(objectPoints, points_2d, this->intrinsic, this->dist, rvec, tvec);
     
}
void LabelTool::dump_dataset_json(){

    fs::path path(dataloader.get_datapath());
    fs::path anno_path = path / "annotation";

    try {
        // Check if the directory exists
        if (fs::exists(anno_path)) {
            // Remove the directory and all its contents
            fs::remove_all(anno_path);
            std::cout << "Deleted existing directory and its contents: " << anno_path << std::endl;
        }

        // Create the new directory
        fs::create_directory(anno_path);
        std::cout << "Created directory: " << anno_path << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    


    int image_idx = 0;
    for(ImageData & imgdat: this->data_list){ //每一個image data 會存一個json
        
        
    
        cv::Mat img = imgdat.get_image();
        cv::Vec3f tvecs, rvecs;
        tvecs = std::get<0>(imgdat.get_extrinsic());
        rvecs = std::get<1>(imgdat.get_extrinsic());
        //std::cout << "tvec rvec"<< std::endl;

        
        json j; 

        std::string color_path = imgdat.get_image_path();
        fs::path full_color_path(color_path);
        std::string result_color;
        // Find the position of "color" in the path
        auto pos = full_color_path.string().find("color/");
        if (pos != std::string::npos) {
            // Extract the subpath starting from "color/"
            fs::path sub_path = full_color_path.string().substr(pos);

            // Convert sub_path to std::string
           result_color = sub_path.string();

        } else {
            std::cerr << "The specified path does not contain 'color/'" << std::endl;
            continue;
        }


        std::string result_depth;
        if(is_depth){
            std::string depth_path = imgdat.get_depth_path();
            fs::path full_depth_path = fs::path(depth_path);
            pos = full_depth_path.string().find("depth/");
            if (pos != std::string::npos) {
            // Extract the subpath starting from "color/"
                fs::path sub_path = full_depth_path.string().substr(pos);

                // Convert sub_path to std::string
                result_depth = sub_path.string();

            } else {
                std::cerr << "The specified path does not contain 'color/'" << std::endl;
                continue;
            }
        }



        
        j["color_data_path"] = result_color;
        if(is_depth){
            j["isDepth"] = true;
            j["depth_data_path"] = result_depth;
        }else{
            j["isDepth"] = false;
        }
        j["camera_data"] = imgdat.get_image_json();
        // std::cout << "get cam data OK"<< std::endl;
        j["objects"] = json::array();
        //std::cout << "json::array() OK"<< std::endl;
        for(int box_id = 0; box_id < this->anno.box_number(); box_id ++ ){ //每一個box
            Box3d& box = this->anno.get_box(box_id);
            std::vector<cv::Point3f> vertices = box.get_vertex();
            //transformation box coordinate from world to image plane   
            std::vector<cv::Point2f> pts_camera; 
            cv::projectPoints(vertices,rvecs, tvecs, this->intrinsic, this->dist, pts_camera);
            json obj;

            //std::cout << "create obj"<< std::endl;
            obj["class"] = box.get_cls();

            obj["keypoints_3d"]= json::array();
            obj["location"]= json::array();
            obj["projected_cuboid"]= json::array();
            //obj["quaternion_xyzw"] = "";//TODO
            obj["scale"]= json::array();
            
            std::vector<cv::Point2f> pnp_box_2d; // 算box的PNP
            
            

            
            int box_2d_point_order[] = {8,0,1,2,3,4,5,6,7};//8是中心點
            int box_3d_point_order[] = {8,0,1,2,3,4,5,6,7};
            for(int i=0; i<9; i++){
                int vertex_2d_id = box_2d_point_order[i];
                int vertex_3d_id = box_3d_point_order[i];
                //解算PNP時不用到中心點
                if(i>0){
                    pnp_box_2d.push_back(pts_camera[vertex_2d_id]);
                }
                
                obj["keypoints_3d"].push_back({vertices[vertex_3d_id].x, vertices[vertex_3d_id].y, vertices[vertex_3d_id].z});
                obj["projected_cuboid"].push_back({
                    int(pts_camera[vertex_2d_id].x), 
                    int(pts_camera[vertex_2d_id].y)
                    });
            }
            //std::cout << "push to array<< std"<<::std::endl;
            
            obj["location"]= {vertices[8].x, vertices[8].y, vertices[8].z};

            
            //obj["quaternion_xyzw"] = 
            obj["scale"]= {box.get_size().x, box.get_size().y, box.get_size().z};
            cv::Vec3f rvec; 
            cv::Vec3f tvec;
            this->_get_pnp_pose(pnp_box_2d, box.get_size(),rvec, tvec);
            obj["rvec"] = {rvec[0], rvec[1], rvec[2]};
            obj["tvec"] = {tvec[0], tvec[1], tvec[2]};

            j["objects"].push_back(obj);


        }

        //store data
        //std::ostringstream oss;
        //oss << std::setw(5) << std::setfill('0') << image_idx;
        
        fs::path temp = imgdat.get_image_path();
        std::string dataname = temp.filename().string();
        pos = dataname.find("_color.png");
        std::string jsonname;
        if (pos != std::string::npos) {
            // Extract the part before "_color.png"
            jsonname = dataname.substr(0, pos);
        } else {
            std::cerr << "The specified filename does not contain '_color.png'" << std::endl;
        }

        fs::path json_path = anno_path / (jsonname + ".json");
        //fs::path img_path = anno_path / (dataname + ".png");
        //cv::imwrite(img_path, img);

        std::ofstream file(json_path);

        if (file.is_open()) {
            file << j.dump(4); // Pretty print with 4 spaces
            file.close();
            //cv::imwrite(img_path, img);
        } 
        std::cout << "Save Json, filename = " << json_path <<::std::endl;
        image_idx++;
    }
}

void LabelTool::dump_annotaion_json(const std::string& filename,  double world_rotation ){
    anno.dumpToJson(filename, this->world_rotate_degree);
}
int LabelTool::load_annotation_json(const std::string& filename){

    double world_rot = anno.LoadJson(filename);

    std::cout << "World Rotation(degree) : " << world_rot << std::endl;
    if(world_rot<0){
        return 1;
    }else{
    //this->world_rotate_degree = world_rot;
    world_rotation(world_rot);
        return 0;
    }
}