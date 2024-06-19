cmake_minimum_required(VERSION 3.29.3)
project(ArUco_world)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)

# Include wxWidgets
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    include
    gui/include
    include/nlohmann  # Add the path to the nlohmann directory
)

# Add the executable
add_executable(DisplayImage
    main.cpp
    src/DataLoader.cpp
    src/natural_sort_test.cpp
    include/DataLoader.h
    include/natural_sort.hpp
    include/LabelTool.h
    src/LabelTool.cpp
    include/Annotation.h
    include/nlohmann/json.hpp
    src/Annotation.cpp
    gui/include/DataReader.h
    gui/src/DataReader.cpp
    gui/include/Annotator.h
    gui/src/Annotator.cpp
)

# Link libraries
target_link_libraries(DisplayImage PRIVATE ${wxWidgets_LIBRARIES} ${OpenCV_LIBS})

# Print wxWidgets variables for debugging
include(CMakePrintHelpers)
cmake_print_variables(wxWidgets)