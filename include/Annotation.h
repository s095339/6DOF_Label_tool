#ifndef _ANNOTATION_H_  
#define _ANNOTATION_H_

//STL library
#include <iostream>
#include <map>
//Opencv Library
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>

//***************************//
//   Box3d                   //
//***************************//


enum class VertertexType
{   
                 
    FrontTopRight,      // 0
    FrontTopLeft,       // 1
    FrontBottomLeft,    // 2
    FrontBottomRight,   // 3
    RearTopRight,       // 4
    RearTopLeft,        // 5
    RearBottomLeft,     // 6
    RearBottomRight,    // 7
    Center,//8
    FrontCenter, //9
    TopCenter//10

};

class Box3d{

private:
    cv::Point3f position;
    cv::Point3f rotation;    
    cv::Vec3f size;
    std::vector<cv::Point3f> vertices; //11 vertice
public:
    Box3d(  cv::Point3f position = cv::Point3f(0.2,0.1,0.2), 
            cv::Point3f rotation = cv::Point3f(0,0,0), 
            cv::Vec3f   size = cv::Vec3f(0.2,0.2,0.2)
    ); //generate a Box
    std::vector<cv::Point3f> get_vertex();// return the vertices

    void configure_box(cv::Point3f, cv::Point3f , cv::Vec3f);
    //change the size, position, roatation of the box object 
};


//***************************//
//  Annotation               //
//***************************//


class Annotation{
private:
    std::vector<Box3d> Box_list;
public:
    Annotation();
    void box_spawn();
    void box_remove(int box_id);
    void box_clean();
    void configure_box(
        int box_id, 
        cv::Point3f position, 
        cv::Point3f rotation, 
        cv::Vec3f size
    );
    Box3d& get_box(int box_id);
    void generate_Annotation();
    int box_number();
};


#endif