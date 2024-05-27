cmake_minimum_required(VERSION 2.8)
project( ArUco_world )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} include )
add_executable( DisplayImage 
    ArUco_world.cpp 
    ./src/DataLoader.cpp
    ./src/natural_sort_test.cpp
    ./include/DataLoader.h
    ./include/natural_sort.hpp
    ./include/LabelTool.h
    ./src/LabelTool.cpp
)
target_link_libraries( DisplayImage ${OpenCV_LIBS} )