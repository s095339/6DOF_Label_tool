cmake_minimum_required(VERSION 3.25)
project( ArUco_world )
find_package( OpenCV REQUIRED )
find_package(wxWidgets REQUIRED COMPONENTS net core base)


if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()


include_directories( 
    ${OpenCV_INCLUDE_DIRS} 
    include 
    gui/include
    )



add_executable( DisplayImage 
    ArUco_world.cpp 
    main.cpp
    ./src/DataLoader.cpp
    ./src/natural_sort_test.cpp
    ./include/DataLoader.h
    ./include/natural_sort.hpp
    ./include/LabelTool.h
    ./src/LabelTool.cpp
    ./include/Annotation.h
    ./src/Annotation.cpp

    #GUI
    ./gui/include/simple.h
    ./gui/src/simple.cpp
    ./gui/include/Communicate.h
    ./gui/include/Panel.h
    ./gui/src/Panel.cpp
    ./gui/src/Communicate.cpp

    ./gui/include/menu.h
    ./gui/src/menu.cpp

    ./gui/include/toolbar.h
    ./gui/src/toolbar.cpp

    ./gui/include/Sizer.h
    ./gui/src/Sizer.cpp

    ./gui/include/border.h
    ./gui/src/border.cpp

    ./gui/include/align.h
    ./gui/src/align.cpp

    ./gui/include/gotoclass.h
    ./gui/src/gotoclass.cpp

    ./gui/include/DataReader.h
    ./gui/src/DataReader.cpp

    ./gui/include/Annotator.h
    ./gui/src/Annotator.cpp
)
target_link_libraries( DisplayImage ${wxWidgets_LIBRARIES} ${OpenCV_LIBS} )



include(CMakePrintHelpers)
cmake_print_variables(wxWidgets)