#include "DataLoader.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "LabelTool.h"
#include "Annotation.h"
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

int ImageData::calculate_extrinsic(std::map<int, cv::Point3f>refMarkerArray, int keep_in_mem)
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
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
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
    int flag = this->_estimateCameraPose(refMarkerArray, markerIds, markerCorners);
    
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
:dataloader{dataloader}, intrinsic{dataloader.get_Camera_intrinsic()}, dist{dataloader.get_Camera_dist()}, intrinsic_para{dataloader.get_camera_intrinsic_para()}
{
    this->anno = Annotation();

}
void LabelTool::build_data_list(std::map<int, cv::Point3f> refMarkerArray, int interval,  int keep_in_mem){
    this->_set_coordinate_ref(refMarkerArray);
    // add all image 
    std::cout << "build data list ... " << std::endl;
    int deleted = 0;
    int count = 0;
    for(int i=0; i<dataloader.length(); i+=interval){
        ImageData imgdat(dataloader[i],this->intrinsic_para, &intrinsic, &dist);// &(this->intrinsic) &(this->dist)
        std::cout << "read image "<< count++ <<" : " <<dataloader[i] << std::endl;
        int flag = imgdat.calculate_extrinsic(refMarkerArray, keep_in_mem);
        if(flag == 1) this->data_list.push_back(imgdat);
        else deleted++;
    }

    std::cout << "[info] stored " << this->data_list.size() << 
        " images in LabelTool, "<< deleted << " images deleted"
        << std::endl;
    // get extrinsic
    //for(auto &imgdat : this->data_list){
    //    imgdat.calculate_extrinsic(this->refMarkerArray, keep_in_mem);
    //}
    
}


ImageData LabelTool::get_imgdat(int idx){
    return this->data_list.at(idx);
}

void LabelTool::_set_coordinate_ref(std::map<int, cv::Point3f> refMarkerArray){
    this->refMarkerArray = refMarkerArray;
}

Annotation& LabelTool::get_anno(){
    return this->anno;
}

int LabelTool::get_data_length(){
    return this->data_list.size();
}
cv::Mat LabelTool::imshow_with_label(int idx, int show_selected_box_direction){
                                    // image id   //the id of the box 
    // get image data
    ImageData imgdat = this->get_imgdat(idx);

    //get image
    cv::Mat img = imgdat.get_image();

    //get image extrinsic
    cv::Vec3f tvecs, rvecs;
    tvecs = std::get<0>(imgdat.get_extrinsic());
    rvecs = std::get<1>(imgdat.get_extrinsic());

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
            if(idx<9)cv::putText(
                img, std::to_string(idx++),pt+cv::Point2f(2,2), 1, 2, cv::Scalar(0,255,255),2
                );

            cv::circle(img, pt, 2, cv::Scalar(0,128,128));
        }

        cv::line(img,pts_camera[0],pts_camera[1], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[0],pts_camera[4], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[5],pts_camera[1], cv::Scalar(255,0,0),2);
        cv::line(img,pts_camera[4],pts_camera[5], cv::Scalar(255,0,0),2);

        cv::line(img,pts_camera[2],pts_camera[6], cv::Scalar(0,255,0),2);
        cv::line(img,pts_camera[3],pts_camera[7], cv::Scalar(0,255,0),2);
        cv::line(img,pts_camera[2],pts_camera[3], cv::Scalar(0,255,0),2);
        cv::line(img,pts_camera[6],pts_camera[7], cv::Scalar(0,255,0),2);

        cv::line(img,pts_camera[0],pts_camera[2], cv::Scalar(0,0,255),2);
        cv::line(img,pts_camera[3],pts_camera[1], cv::Scalar(0,0,255),2);
        cv::line(img,pts_camera[7],pts_camera[5], cv::Scalar(0,0,255),2);
        cv::line(img,pts_camera[6],pts_camera[4], cv::Scalar(0,0,255),2);

        if(i == show_selected_box_direction){
            cv::arrowedLine(img,pts_camera[8],pts_camera[9], cv::Scalar(255,0,0),3,7);
            cv::arrowedLine(img,pts_camera[8],pts_camera[10], cv::Scalar(0,255,0),3,7);
            cv::arrowedLine(img,pts_camera[8],pts_camera[11], cv::Scalar(0,0,255),3,7);
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
    this->data_list.erase(this->data_list.begin()+idx);

}

void LabelTool::dump_dataset_json(fs::path path){
    int image_idx = 0;
    for(ImageData & imgdat: this->data_list){ //每一個image data 會存一個json
        
        
    
        cv::Mat img = imgdat.get_image();
        cv::Vec3f tvecs, rvecs;
        tvecs = std::get<0>(imgdat.get_extrinsic());
        rvecs = std::get<1>(imgdat.get_extrinsic());
        //std::cout << "tvec rvec"<< std::endl;

        
        json j; 
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
            

            for(int i=0; i<9; i++){
                int vertex_id = (i+8)%9;
                //因為在objectron的label 中心點放在第一個 而我們的label中心點放第九個 要配合obejctron的label
                
                obj["keypoints_3d"].push_back({vertices[vertex_id].x, vertices[vertex_id].y, vertices[vertex_id].z});
                obj["projected_cuboid"].push_back({
                    int(pts_camera[vertex_id].x), 
                    int(pts_camera[vertex_id].y)
                    });
            }
            //std::cout << "push to array<< std"<<::std::endl;
            
            obj["location"]= {vertices[8].x, vertices[8].y, vertices[8].z};

            
            //obj["quaternion_xyzw"] = 
            obj["scale"]= {box.get_size().x, box.get_size().y, box.get_size().z};

            j["objects"].push_back(obj);

        }

        //store data
        std::ostringstream oss;
        oss << std::setw(5) << std::setfill('0') << image_idx;
        std::string dataname = oss.str();

        fs::path json_path = path / (dataname + ".json");
        fs::path img_path = path / (dataname + ".png");
        //cv::imwrite(img_path, img);

        std::ofstream file(json_path);

        if (file.is_open()) {
            file << j.dump(4); // Pretty print with 4 spaces
            file.close();
            cv::imwrite(img_path, img);
        } 
        std::cout << "Save image and Json, filename = " << dataname <<::std::endl;
        image_idx++;
    }
}