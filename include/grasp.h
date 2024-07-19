#ifndef _GRASP_H_  
#define _GRASP_H_

//STL library
#include <iostream>
#include <map>
//Opencv Library
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>

#include "nlohmann/json.hpp"
#include <cmath>

using json = nlohmann::json;
namespace fs = std::filesystem;

const float grasp_rect_h = 0.1;
const float grasp_rect_w = 0.08;


class Grasp
{
private:
    int cls; //class 
    cv::Point3f position;
    cv::Vec3f rotation;    
    float width;
    std::vector<cv::Point3f> vertices; //7 vertice
public:    
    Grasp(
        int cls,
        cv::Point3f position = cv::Point3f(0.2,0.1,0.2), 
        cv::Point3f rotation = cv::Point3f(0,0,0), 
        float width = 0.04
    ){

        this->cls = cls;
        this->position = position;
        this->rotation = rotation;
        this->width = width;
        
        build_vertices();
        this->vertices = rotate(this->vertices, this->rotation);

    };
    void build_vertices(){
        float cx = this-> position.x;
        float cy = this-> position.y;
        float cz = this-> position.z;

        float top, bottom,right,left;
        float gripper_top, gripper_bottom;
        //note that it is right hand rule
        
        //x axis point to the left
        left = cx; // grasp center is on the left
        right = cx - grasp_rect_w;
        //y point upperward
        top = cy + grasp_rect_h / 2;
        bottom = cy - grasp_rect_h / 2;
        //gripper
        gripper_top = cy + this->width / 2;
        gripper_bottom = cy - this->width / 2;
        
        this->vertices = {
            cv::Point3f(cx, cy, cz), //grasp center point
            cv::Point3f(left, top, cz), // top left
            cv::Point3f(left, bottom, cz), // bottom left
            cv::Point3f(right, top, cz), // top right
            cv::Point3f(right, bottom, cz), // bottom right

            cv::Point3f(left, gripper_top, cz), //top_gripper  5
            cv::Point3f(left, gripper_bottom, cz), //buttom_gripper  6
            cv::Point3f((right+left)/2, gripper_top, cz), //top_gripper  7
            cv::Point3f((right+left)/2, gripper_bottom, cz), //buttom_gripper  8

            cv::Point3f((right+left)/2, cy, cz), // ct of rect 9
            cv::Point3f(right, cy, cz), // right_center 10
        };
    }
    std::vector<cv::Point3f> rotate( std::vector<cv::Point3f> v , cv::Vec3f rot){
        cv::Point3f center = v[0];
        std::vector<cv::Point3f> rotatedVertices;
        cv::Mat rotationMatrix_x;
        cv::Mat rotationMatrix_y;
        cv::Mat rotationMatrix_z;
        

        cv::Vec3f rotation_x = {rot[0], 0.0f, 0.0f};
        cv::Vec3f rotation_y = {0.0f, rot[1], 0.0f};
        cv::Vec3f rotation_z = {0.0f, 0.0f, rot[2]};

        cv::Rodrigues(rotation_x, rotationMatrix_x);
        cv::Rodrigues(rotation_y, rotationMatrix_y);
        cv::Rodrigues(rotation_z, rotationMatrix_z);

        cv::Mat rotationMatrix = rotationMatrix_z * rotationMatrix_y * rotationMatrix_x;
        
        for (const auto& vertex : v) {
            cv::Point3f translatedVertex = vertex - center;//center
            cv::Mat vertexMat = (cv::Mat_<float>(3, 1) << translatedVertex.x, translatedVertex.y, translatedVertex.z);
            //rotate
            // Apply the first rotation (around x-axis)
            cv::Mat rotatedVertexMatX = rotationMatrix_z * vertexMat;

            // Apply the second rotation (around y-axis) in the updated local frame
            cv::Mat rotatedVertexMatXY = rotationMatrix_y * rotatedVertexMatX;

            // Apply the third rotation (around z-axis) in the updated local frame
            cv::Mat rotatedVertexMatXYZ = rotationMatrix_x * rotatedVertexMatXY;


            
            // Translate the vertex back to its original center
            cv::Point3f rotatedVertex = cv::Point3f(rotatedVertexMatXYZ) + center;
            //======
            rotatedVertices.push_back(rotatedVertex);
        }
        
        return rotatedVertices;


   

    }
    
    std::vector<cv::Point3f> get_vertex(){
        return vertices;
    };// return the vertices
    int get_cls(){
        return cls;
    };
    cv::Point3f get_position(){
        return position;
    };
    void rotate_world(double degree){

        double rad = degree * M_PI / 180.0;
        double cosTheta = cos(rad);
        double sinTheta = sin(rad);

        double x,z;
        x = this->position.x * cosTheta - this->position.z * sinTheta;
        z = this->position.x * sinTheta + this->position.z * cosTheta;
        this->position.x = x;
        this->position.z = z;

        for(auto & vertex : this->vertices){ 
            x = vertex.x * cosTheta - vertex.z * sinTheta;
            z = vertex.x * sinTheta + vertex.z * cosTheta;
            vertex.x = x;
            vertex.z = z;
        }
    };
    float get_width(){
        return width;
    };
    cv::Point3f get_rotation(){
        return rotation;
    };
    void configure_grasp(
        cv::Point3f position, cv::Vec3f rotation_in, float width
    )
    {
        this->position += position;
        this->rotation += rotation_in;
        this->width += width;

        
        if (this->width<0.01)this->width=0.01;
        
        build_vertices();
        this->vertices = rotate(this->vertices, this->rotation);
    }
    
    json grasp_to_json(int idx){
        
        return{
            {"grasp_id", idx},
            {"class_id", this->cls},
            {"position", {this->position.x, this->position.y, this->position.z}},
            {"rotation", {this->rotation[0], this->rotation[1], this->rotation[2]}},
            {"width", this->width}
        };
    }



};


#endif