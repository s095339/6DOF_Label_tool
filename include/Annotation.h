#ifndef _ANNOTATION_H_  
#define _ANNOTATION_H_

//STL library
#include <iostream>
#include <map>
//Opencv Library
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>

#include "nlohmann/json.hpp"
#include <cmath>
using json = nlohmann::json;
//***************************//
//   Box3d                   //
//***************************//

enum class cls{
    foup,
    grab_point1,
    grab_point2
};
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
    int cls; //class 
    cv::Point3f position;
    cv::Vec3f rotation;    
    cv::Vec3f size;
    std::vector<cv::Point3f> vertices; //11 vertice
    std::vector<cv::Point3f> boxVertices;

    
public:
    Box3d(  int cls,
            cv::Point3f position = cv::Point3f(0.2,0.1,0.2), 
            cv::Point3f rotation = cv::Point3f(0,0,0), 
            cv::Vec3f   size = cv::Vec3f(0.2,0.2,0.2)
    ); //generate a Box
    std::vector<cv::Point3f> get_vertex();// return the vertices
    int get_cls();
    cv::Point3f get_position();
    void rotate_world(double);
    cv::Point3f get_size();
    cv::Point3f get_rotation();
    void configure_box(cv::Point3f, cv::Vec3f , cv::Vec3f);
    void rotate_box(cv::Vec3f);
    json box_to_json(int );
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
    void box_spawn(
        int cls,
        cv::Point3f position = cv::Point3f(0.2,0.1,0.2), 
        cv::Point3f rotation = cv::Point3f(0,0,0), 
        cv::Vec3f   size = cv::Vec3f(0.2,0.2,0.2)
    );
    void box_remove(int box_id);
    void box_clean();
    void configure_box(
        int box_id, 
        cv::Point3f position, 
        cv::Vec3f rotation, 
        cv::Vec3f size
    );
    void rotate_box(
        int box_id,
        cv::Vec3f rotation
    );
    Box3d& get_box(int box_id);
    //void generate_Annotation();
    int box_number();
    double LoadJson(const std::string& filename);
    void dumpToJson(const std::string& filename = "../boxes.json", double world_rotation = 0);
};




#endif