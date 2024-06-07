#include "Annotation.h"

#include <iostream>
#include <map>
//Opencv Library
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>

//***************************//
//   Box3d                   //
//***************************//
Box3d::Box3d(
    int cls, cv::Point3f position, cv::Point3f rotation, cv::Vec3f size
)
{
    this->cls = cls;
    this->position = position;
    this->rotation = rotation;
    this->size = size;

    //width, height, depth = self.size3d//
    float width = this->size[0];
    float height = this->size[1];
    float depth = this->size[2];



    //    # By default just use the normal OpenCV coordinate system
    //    if (self.coord_system is None):
    //        cx, cy, cz = self.center_location
    float cx = this-> position.x;
    float cy = -(this-> position.y);
    float cz = this-> position.z;

    float right, left, top, bottom, front, rear;
    //X axis point to the right
    right = cx + width / 2.0;
    left = cx - width / 2.0;
    //Y axis point upward(注意這邊是反過來的)
    top = cy - height / 2.0;
    bottom = cy + height / 2.0;
    //Z axis point forward
    front = cz + depth / 2.0;
    rear = cz - depth / 2.0;

    this->vertices = {
        cv::Point3f(left, bottom, rear),
        cv::Point3f(left, bottom, front),
        cv::Point3f(left, top, rear),
        cv::Point3f(left, top, front),
        cv::Point3f(right, bottom, rear),
        cv::Point3f(right, bottom, front),
        cv::Point3f(right, top, rear),
        cv::Point3f(right, top, front),

        //center
        cv::Point3f(cx, cy, cz),
        // front center
        cv::Point3f(cx, cy, front),
        // top center
        cv::Point3f(cx, top, cz),
        
        //in front of front center
        cv::Point3f(cx, cy, front+0.2),
        //above top center
        cv::Point3f(cx, top-0.2, cz)

    };
}




void Box3d::configure_box(
    cv::Point3f position, cv::Vec3f rotation, cv::Vec3f size
)
{   

    this->position += position;
    this->rotation += rotation;
    this->size += size;

    //width, height, depth = self.size3d//
    float width = this->size[0];
    float height = this->size[1];
    float depth = this->size[2];



    //    # By default just use the normal OpenCV coordinate system
    //    if (self.coord_system is None):
    //        cx, cy, cz = self.center_location
    float cx = this-> position.x;
    float cy = -(this-> position.y);
    float cz = this-> position.z;

    float right, left, top, bottom, front, rear;
    //X axis point to the right
    right = cx + width / 2.0;
    left = cx - width / 2.0;
    //Y axis point upward(注意這邊是反過來的)
    top = cy - height / 2.0;
    bottom = cy + height / 2.0;
    //Z axis point forward
    front = cz + depth / 2.0;
    rear = cz - depth / 2.0;

    this->vertices = {
        cv::Point3f(left, bottom, rear),
        cv::Point3f(left, bottom, front),
        cv::Point3f(left, top, rear),
        cv::Point3f(left, top, front),
        cv::Point3f(right, bottom, rear),
        cv::Point3f(right, bottom, front),
        cv::Point3f(right, top, rear),
        cv::Point3f(right, top, front),

        //center
        cv::Point3f(cx, cy, cz),
        // front center
        cv::Point3f(cx, cy, front),
        // top center
        cv::Point3f(cx, top, cz),
        
        //in front of front center
        cv::Point3f(cx, cy, front+0.2),
        //above top center
        cv::Point3f(cx, top-0.2, cz)

    };

    cv::Point3f center = this->vertices[8];
    std::vector<cv::Point3f> rotatedVertices;
    cv::Mat rotationMatrix;
    cv::Rodrigues(this->rotation, rotationMatrix);

    
    for (const auto& vertex : this->vertices) {
        cv::Point3f translatedVertex = vertex - center;//center
        cv::Mat vertexMat = (cv::Mat_<float>(3, 1) << translatedVertex.x, translatedVertex.y, translatedVertex.z);
        cv::Mat rotatedVertexMat = rotationMatrix * vertexMat;
        rotatedVertices.push_back(cv::Point3f(rotatedVertexMat) + center);
    }
    this->vertices = rotatedVertices;

}

std::vector<cv::Point3f> Box3d::get_vertex(){
    return this->vertices;
}

cv::Point3f Box3d::get_position(){
    return this->position;
}

cv::Point3f Box3d::get_size(){
    return this->size;
}

cv::Point3f Box3d::get_rotation(){
    return this->rotation;
}
int Box3d::get_cls(){
    return this->cls;
}
//***************************//
//  Annotation               //
//***************************//

Annotation::Annotation(){
    std::cout<<"[info] Annotation object generated" << std::endl;
}

void Annotation::box_spawn(int cls){
    Box3d box(cls);
    this->Box_list.push_back(box);
}

void Annotation::box_remove(int box_id){
    // remove a single element
    if(this->Box_list.empty())
        std::cout << "[error] no box in the annotation" << std::endl;
    else
        this->Box_list.erase(this->Box_list.begin() + box_id);
}

void Annotation::box_clean(){
    this->Box_list.clear();
}

void Annotation::configure_box(
        int box_id, 
        cv::Point3f position = cv::Point3f(0,0,0), 
        cv::Vec3f rotation = cv::Vec3f(0,0,0), 
        cv::Vec3f size = cv::Vec3f(0,0,0)
        )
{
    if(box_id >= this->Box_list.size() || box_id < 0){
        std::cout << "box id out of range" << std::endl;
    }
    else{
        Box3d& box = this->Box_list.at(box_id);
        box.configure_box(position, rotation, size);
    }
}

Box3d& Annotation::get_box(int box_id){
    return this->Box_list.at(box_id);
}

int Annotation::box_number(){
    return this->Box_list.size();
}

//void Annotation::generate_Annotation(){
//    std::cout<<"[todo] generate annotation json file " << std::endl;
//}