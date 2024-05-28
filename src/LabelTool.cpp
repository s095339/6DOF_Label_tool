#include "DataLoader.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "LabelTool.h"
#include <opencv2/aruco.hpp>
#include <map>
#include <stdexcept>
#include <algorithm>
//non-menber function



//menber function

//************************//
// ImageData              //
//************************//
ImageData::ImageData(std::string image_path, cv::Mat* const intr_ptr, cv::Mat* const dist_ptr)
:image_path{image_path}, keep_in_mem{0}
{
    this->intrinsic_ptr = intr_ptr;
    this->dist_ptr = dist_ptr;

}

cv::Mat ImageData::get_image(){
    if(this->keep_in_mem)
        return this->image;
    else
        return cv::imread(this->image_path, cv::IMREAD_COLOR);
    
}



int ImageData::get_extrinsic(std::map<int, cv::Point3f>refMarkerArray, int keep_in_mem)
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
        flag = cv::solvePnP(objectPoints, imagePoints, *(this->intrinsic_ptr), *(this->dist_ptr), rvecs, tvecs);
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

//************************//
// LabelTool              //
//************************//

LabelTool::LabelTool(DataLoader& dataloader)
:dataloader{dataloader}, intrinsic{dataloader.get_Camera_intrinsic()}, dist{dataloader.get_Camera_dist()}
{

}
void LabelTool::build_data_list(std::map<int, cv::Point3f> refMarkerArray, int keep_in_mem){
    this->_set_coordinate_ref(refMarkerArray);
    // add all image 

    int deleted = 0;
    for(int i=0; i<dataloader.length(); i+=10){
        std::cout << "read image: " <<dataloader[i] << std::endl;
        ImageData imgdat(dataloader[i], &intrinsic, &dist);
        int flag = imgdat.get_extrinsic(refMarkerArray);
        if(flag == 1) this->data_list.push_back(imgdat);
        else deleted++;
    }

    std::cout << "[info] stored " << this->data_list.size() << 
        " images in LabelTool, "<< deleted << " images deleted"
        << std::endl;
    // get extrinsic
    //for(auto &imgdat : this->data_list){
    //    imgdat.get_extrinsic(this->refMarkerArray, keep_in_mem);
    //}
    
}

ImageData LabelTool::get_imgdat(int idx){
    return this->data_list.at(idx);
}

void LabelTool::_set_coordinate_ref(std::map<int, cv::Point3f> refMarkerArray){
    this->refMarkerArray = refMarkerArray;
}