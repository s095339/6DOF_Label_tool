#include "Annotation.h"
#include <fstream>
#include <iostream>
#include <map>
//Opencv Library
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>

//json
namespace fs = std::filesystem;

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
        // right center
        cv::Point3f(right, cy, cz),

        //in front of front center
        cv::Point3f(cx, cy, front+0.15),
        //above top center
        cv::Point3f(cx, top-0.15, cz),
        //rhs of right center
        cv::Point3f(right+0.15, cy, cz)
    };

    cv::Point3f center = this->vertices[8];
    std::vector<cv::Point3f> rotatedVertices;
    cv::Mat rotationMatrix_x;
    cv::Mat rotationMatrix_y;
    cv::Mat rotationMatrix_z;
    

    cv::Vec3f rotation_x = {this->rotation[0], 0.0f, 0.0f};
    cv::Vec3f rotation_y = {0.0f, this->rotation[1], 0.0f};
    cv::Vec3f rotation_z = {0.0f, 0.0f, this->rotation[2]};

    cv::Rodrigues(rotation_x, rotationMatrix_x);
    cv::Rodrigues(rotation_y, rotationMatrix_y);
    cv::Rodrigues(rotation_z, rotationMatrix_z);

    cv::Mat rotationMatrix = rotationMatrix_z * rotationMatrix_y * rotationMatrix_x;
    
    for (const auto& vertex : this->vertices) {
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
    


    this->vertices = rotatedVertices;



}



void Box3d::rotate_box(cv::Vec3f rotation){
    this->rotation += rotation;
    int rotate_dir = -1;
    for(int i = 0; i<3; i++){
        if(rotation[i] != 0){ 
            rotate_dir = i;
            break;
        }
    }

    if(rotate_dir == -1)return;

    cv::Point3f center = this->vertices[8];
    std::vector<cv::Point3f> rotatedVertices;
    cv::Mat rotationMatrix_x;
    cv::Mat rotationMatrix_y;
    cv::Mat rotationMatrix_z;
    

    cv::Vec3f rotation_x = {this->rotation[0], 0.0f, 0.0f};
    cv::Vec3f rotation_y = {0.0f, this->rotation[1], 0.0f};
    cv::Vec3f rotation_z = {0.0f, 0.0f, this->rotation[2]};

    cv::Rodrigues(rotation_x, rotationMatrix_x);
    cv::Rodrigues(rotation_y, rotationMatrix_y);
    cv::Rodrigues(rotation_z, rotationMatrix_z);
    std::vector<cv::Mat> rotateMat  {rotationMatrix_x, rotationMatrix_y, rotationMatrix_z};

    //cv::Vec3f rotation_vec = {0.0f, 0.0f, 0.0f};
    //rotation_vec[rotate_dir] = rotation_in[rotate_dir];
    //cv::Mat rotationMatrix;

    //cv::Rodrigues(rotation_vec, rotationMatrix);
  

    //cv::Mat rotationMatrix = rotationMatrix_z * rotationMatrix_y * rotationMatrix_x;
    
    for (const auto& vertex : this->vertices) {
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
    
    this->vertices = rotatedVertices;

    //debug
    cv::Point3f rot = this->rotation;
    std::cout << "rotation rx: " << rot.x/M_PI << "π ,ry: "<< rot.y/M_PI <<"π ,rz: " << rot.z/M_PI <<"π "<< std::endl;
}
void Box3d::configure_box(
    cv::Point3f position, cv::Vec3f rotation_in, cv::Vec3f size
)
{   

    this->position += position;
    this->rotation += rotation_in;
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
        // right center
        cv::Point3f(right, cy, cz),

        //in front of front center
        cv::Point3f(cx, cy, front+0.15),
        //above top center
        cv::Point3f(cx, top-0.15, cz),
        //rhs of right center
        cv::Point3f(right+0.15, cy, cz)
    };

    cv::Point3f center = this->vertices[8];
    std::vector<cv::Point3f> rotatedVertices;
    cv::Mat rotationMatrix_x;
    cv::Mat rotationMatrix_y;
    cv::Mat rotationMatrix_z;
    

    cv::Vec3f rotation_x = {this->rotation[0], 0.0f, 0.0f};
    cv::Vec3f rotation_y = {0.0f, this->rotation[1], 0.0f};
    cv::Vec3f rotation_z = {0.0f, 0.0f, this->rotation[2]};

    cv::Rodrigues(rotation_x, rotationMatrix_x);
    cv::Rodrigues(rotation_y, rotationMatrix_y);
    cv::Rodrigues(rotation_z, rotationMatrix_z);

    cv::Mat rotationMatrix = rotationMatrix_z * rotationMatrix_y * rotationMatrix_x;
    
    for (const auto& vertex : this->vertices) {
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
    


    this->vertices = rotatedVertices;

    //debug
    cv::Point3f rot = this->rotation;
    std::cout << "rotation rx: " << rot.x/M_PI << "π ,ry: "<< rot.y/M_PI <<"π ,rz: " << rot.z/M_PI <<"π "<< std::endl;
}

std::vector<cv::Point3f> Box3d::get_vertex(){
    return this->vertices;
}

cv::Point3f Box3d::get_position(){
    return this->position;
}

void Box3d::rotate_world(double degree){

    
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

json Box3d::box_to_json(int idx){
    return {
        {"box_id", idx},
        {"class_id", this->cls},
        {"position", {this->position.x, this->position.y, this->position.z}},
        {"rotation", {this->rotation[0], this->rotation[1], this->rotation[2]}},
        {"size", {this->size[0], this->size[1], this->size[2]}}
    };

}
//***************************//
//  Annotation               //
//***************************//

Annotation::Annotation(){
    std::cout<<"[info] Annotation object generated" << std::endl;
}

void Annotation::box_spawn(
    int cls,
    cv::Point3f position, 
    cv::Point3f rotation, 
    cv::Vec3f size
    )
{
    Box3d box(cls,position,rotation,size);
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
void Annotation::rotate_box(
    int box_id,
    cv::Vec3f rotation
){
    if(box_id >= this->Box_list.size() || box_id < 0){
        std::cout << "box id out of range" << std::endl;
    }
    else{
        Box3d& box = this->Box_list.at(box_id);
        box.rotate_box(rotation);
    }

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
double Annotation::LoadJson(const std::string& filename){
    if (!fs::exists(filename)) {
        std::cerr << "File " << filename << " does not exist!" << std::endl;
        return -1;
    }

    // Read JSON from file
    std::ifstream file(filename);
    json j;
    file >> j;
    file.close();
    std::cout << "load json OK" << std::endl;
    for (const auto& item : j["box_list"]) {
        int cls = item["class_id"];
        cv::Point3f position(item["position"][0], item["position"][1], item["position"][2]);
        cv::Point3f rotation(item["rotation"][0], item["rotation"][1], item["rotation"][2]);
        cv::Vec3f size(item["size"][0], item["size"][1], item["size"][2]);
        
        this->box_spawn(cls, position, rotation, size);
    }
    try{
        return j["world_rotation"];
    }catch(const std::exception& e){
        return 0;
    }

   
}
void Annotation::dumpToJson(const std::string& filename,  double world_rotation ){
    json j;
    
    std::string fullfilename = filename + "/box.json";
    
    std::cout <<"ENTER OK" << std::endl;
    if (fs::exists(fullfilename)) {
        
        // Delete the existing file
        if (!fs::remove(fullfilename)) {
            std::cerr << "Failed to delete existing file " << fullfilename << std::endl;
            return; // Exit without saving the file
        }
        std::cout << "Deleted existing file " << fullfilename << std::endl;
    }

    std::ofstream file(fullfilename);

    j["name"] = "Global Annotation";
    j["world_rotation"] = world_rotation;
    //std::cout <<"name OK" << std::endl;

    j["box_list"] = json::array();

    for(int i=0; i<this->box_number(); i++){
        j["box_list"].push_back(
            this->get_box(i).box_to_json(i)
            );
    }
    std::cout <<"boxlist OK" << std::endl;

    if (file.is_open()) {
        file << j.dump(4); // Pretty print with 4 spaces
        file.close();
        std::cout << "JSON data saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file " << filename << " for writing" << std::endl;
    }



    
}
//void Annotation::generate_Annotation(){
//    std::cout<<"[todo] generate annotation json file " << std::endl;
//}