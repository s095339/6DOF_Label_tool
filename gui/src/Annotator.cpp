#include <wx/wx.h>
#include <wx/sizer.h>
#include "Annotator.h"
#include <wx/stattext.h>
/////////////////////////////
#include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/matx.hpp>
#include <filesystem>
///////////////////////////////
#include <opencv2/aruco.hpp>
#include "DataLoader.h"
#include "LabelTool.h"
#include "Annotation.h"
#include <filesystem>

namespace fs = std::filesystem;


using namespace std;


Annotator::Annotator(const wxString& title,  wxBitmapType format, string dir_path)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800,400))
{   

    cv::namedWindow("Stream", cv::WINDOW_NORMAL);
    cv::resizeWindow("Stream", 1280, 720);
    wxInitAllImageHandlers();
    // build LabelTool===============================
    this->dataloader = new DataLoader(dir_path);
    this->labeltool = new LabelTool(*dataloader);
    int interval = 1;
    labeltool->build_data_list(interval);

    box_id = 0;
    //box_count = 0;
    //max_img = labeltool->get_data_length();
    image_id = 0;
    //arrangement===============================================
    wxPanel *paintPanel = new wxPanel(this, -1);
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hbox0 = new wxBoxSizer(wxHORIZONTAL);// put box and choose box
    wxBoxSizer *vbox_cls = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *vbox_spawn = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *vbox_remove = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *vbox_select = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *vbox_copy = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hbox0_5  = new wxBoxSizer(wxHORIZONTAL);
    
    wxBoxSizer* vbox_stride = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_rotation = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_size = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);// configure box
    wxBoxSizer *vbox_xyz = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *vbox_whd = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *vbox_rxyz = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hbox_xyz = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox_whd = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox_rxyz = new wxBoxSizer(wxHORIZONTAL);
    
    wxBoxSizer* vbox_x = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_y = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_z = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_w = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_h = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_d = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_rx = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_ry = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox_rz = new wxBoxSizer(wxVERTICAL);
   
    //grasp
    wxBoxSizer* vbox_grasp = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hbox_grasp_cls = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox_grasp_setting = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox_grasp_configuration = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* vbox_grasp_location = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* hbox_grasp_location_x = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* hbox_grasp_location_y = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* hbox_grasp_location_z = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* vbox_grasp_rotation = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* hbox_grasp_rotation_x = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* hbox_grasp_rotation_y = new wxBoxSizer(wxHORIZONTAL);
            wxBoxSizer* hbox_grasp_rotation_z = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* vbox_grasp_width = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* hbox_grasp_width = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);// choose the image
    
    wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);

    //* set bounding box [hbox0]
    cls_select = new wxTextCtrl(paintPanel, ID_CLS_SELECT, "", wxDefaultPosition, wxDefaultSize);
    box_spawn  = new wxButton(paintPanel, ID_BOX_SPAWN, wxT(" BoxSpawn "), wxDefaultPosition);
    box_remove = new wxButton(paintPanel, ID_BOX_REMOVE, wxT(" BoxRemove "), wxDefaultPosition);
    box_select = new wxComboBox(paintPanel, ID_COMBLEBOX, wxT("-no box-"));
    box_copy = new wxButton(paintPanel, ID_BOX_COPY, wxT("BoxCopy"));

    //* stride
    stride_val = new wxStaticText(paintPanel, wxID_ANY,  
                            wxString::Format(wxT("position stride\n unit:(m) \n value:%f"), stride));
    size_val = new wxStaticText(paintPanel, wxID_ANY, 
                            wxString::Format(wxT("size stride\n unit:(m) \n value:%f"), size_diff));
    rotation_val = new wxStaticText(paintPanel, wxID_ANY, 
                            wxString::Format(wxT("rotation stride\n unit:(degree) \n value:%f"), rotate));

    stride_text = new wxTextCtrl(paintPanel, wxID_ANY, "0.05" );
    size_text = new wxTextCtrl(paintPanel, wxID_ANY, "0.05" );
    rotation_text = new wxTextCtrl(paintPanel, wxID_ANY, "5" );
    //* world setting
    rotate_world = new wxButton(paintPanel, ID_ROTATE_WORLD, wxT("rotate world"));
    world_degree = new wxStaticText(paintPanel, wxID_ANY, "world degree = 0");
    //* Configure bounding box [hbox1]
    {// position
        //x
        box_x_plus = new wxButton(paintPanel, ID_X_PLUS, wxT("x+"));
        box_x_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_x_minus = new wxButton(paintPanel, ID_X_MINUS, wxT("x-"));

        //y
        box_y_plus = new wxButton(paintPanel, ID_Y_PLUS, wxT("y+"));
        box_y_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_y_minus = new wxButton(paintPanel, ID_Y_MINUS, wxT("y-"));

        //z
        box_z_plus = new wxButton(paintPanel, ID_Z_PLUS, wxT("z+"));
        box_z_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_z_minus = new wxButton(paintPanel, ID_Z_MINUS, wxT("z-"));
    }
    {// size    
        //x
        box_w_plus = new wxButton(paintPanel, ID_W_PLUS, wxT("w+"));
        box_w_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_w_minus = new wxButton(paintPanel, ID_W_MINUS, wxT("w-"));

        //y
        box_h_plus = new wxButton(paintPanel, ID_H_PLUS, wxT("h+"));
        box_h_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_h_minus = new wxButton(paintPanel, ID_H_MINUS, wxT("h-"));

        //z
        box_d_plus = new wxButton(paintPanel, ID_D_PLUS, wxT("d+"));
        box_d_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_d_minus = new wxButton(paintPanel, ID_D_MINUS, wxT("d-"));

    }
    {// rotation
        //x
        box_rx_plus = new wxButton(paintPanel, ID_RX_PLUS, wxT("rx+"));
        box_rx_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_rx_minus = new wxButton(paintPanel, ID_RX_MINUS, wxT("rx-"));

        //y
        box_ry_plus = new wxButton(paintPanel, ID_RY_PLUS, wxT("ry+"));
        box_ry_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_ry_minus = new wxButton(paintPanel, ID_RY_MINUS, wxT("ry-"));

        //z
        box_rz_plus = new wxButton(paintPanel, ID_RZ_PLUS, wxT("rz+"));
        box_rz_val = new wxStaticText(paintPanel, wxID_ANY, wxT(""));
        box_rz_minus = new wxButton(paintPanel, ID_RZ_MINUS, wxT("rz-"));
    }
    
    //* set grasp
    cls_grasp_select0 = new wxTextCtrl(paintPanel, wxID_ANY, "" ,wxDefaultPosition, wxDefaultSize);
    cls_grasp_select1 = new wxTextCtrl(paintPanel, wxID_ANY, "" ,wxDefaultPosition, wxDefaultSize);

    single_grasp_spawn = new wxButton(paintPanel, ID_SINGLE_GRASP_SPAWM, wxT("s-graspspawn "));
    paired_grasp_spawn = new wxButton(paintPanel, ID_PAIRED_GRASP_SPAWM, wxT("p-graspspawn "));
    paired_grasp_remove = new wxButton(paintPanel, ID_PAIRED_GRASP_REMOVE, wxT("grasp remove "));
    paired_grasp_select = new wxComboBox(paintPanel, ID_PAIRED_GRASP_SELECT, wxT("-no grasp-"));
    paired_index_select = new wxComboBox(paintPanel, ID_PAIRED_INDEX_SELECT, wxT("-no paired id-"));
    paired_grasp_copy = new wxButton(paintPanel, ID_PAIRED_GRASP_COPY, wxT("grasp copy "));

    paired_index_select->Append(wxT("paired_id: 0"));
    paired_index_select->Append(wxT("paired_id: 1"));
    paired_index_select->Append(wxT("paired_id: 2"));

    //* configure grasp
    // grasp position
    grasp_stride_text = new wxTextCtrl(paintPanel, wxID_ANY, "" ,wxDefaultPosition, wxDefaultSize);
    grasp_position_label = new wxStaticText(paintPanel, wxID_ANY, "Grasp Position");
    //x
    grasp_x_plus = new wxButton(paintPanel, ID_GRASP_X_PLUS, wxT("x+"));
    grasp_x_val = new wxStaticText(paintPanel, wxID_ANY, "x = none");
    grasp_x_minus = new wxButton(paintPanel, ID_GRASP_X_MINUS, wxT("x-"));
    //y
    grasp_y_plus = new wxButton(paintPanel, ID_GRASP_Y_PLUS, wxT("y+ "));
    grasp_y_val = new wxStaticText(paintPanel, wxID_ANY, "y = none");
    grasp_y_minus = new wxButton(paintPanel, ID_GRASP_Y_MINUS, wxT("y-"));
    //z
    grasp_z_plus = new wxButton(paintPanel, ID_GRASP_Z_PLUS, wxT("z+"));
    grasp_z_val = new wxStaticText(paintPanel, wxID_ANY, "z = none");
    grasp_z_minus = new wxButton(paintPanel, ID_GRASP_Z_MINUS, wxT("z-"));
    // grasp rotation
    grasp_rotate_text = new wxTextCtrl(paintPanel, wxID_ANY, "" ,wxDefaultPosition, wxDefaultSize);
    grasp_rotation_label = new wxStaticText(paintPanel, wxID_ANY, "Grasp Rotation");
    //rx
    grasp_rx_plus = new wxButton(paintPanel, ID_GRASP_RX_PLUS, wxT("rx+"));
    grasp_rx_val = new wxStaticText(paintPanel, wxID_ANY, "rx = none");
    grasp_rx_minus = new wxButton(paintPanel, ID_GRASP_RX_MINUS, wxT("rx-"));
    //ry
    grasp_ry_plus = new wxButton(paintPanel, ID_GRASP_RY_PLUS, wxT("ry+ "));
    grasp_ry_val = new wxStaticText(paintPanel, wxID_ANY, "ry = none");
    grasp_ry_minus = new wxButton(paintPanel, ID_GRASP_RY_MINUS, wxT("ry-"));
    //rz
    grasp_rz_plus = new wxButton(paintPanel, ID_GRASP_RZ_PLUS, wxT("rz+"));
    grasp_rz_val = new wxStaticText(paintPanel, wxID_ANY, "rz = none");
    grasp_rz_minus = new wxButton(paintPanel, ID_GRASP_RZ_MINUS, wxT("rz-"));
    //grasp width
    grasp_width_label = new wxStaticText(paintPanel, wxID_ANY, "Grasp Width");

    grasp_w_plus = new wxButton(paintPanel, ID_GRASP_W_PLUS, wxT("w+"));
    grasp_w_val = new wxStaticText(paintPanel, wxID_ANY, "w = none");
    grasp_w_minus = new wxButton(paintPanel, ID_GRASP_W_MINUS, wxT("w-"));
    //* choose image [hbox2]
  
     
    img_id_text = new wxStaticText(paintPanel, -1, wxT("-------"),wxDefaultPosition,wxSize(20,-1));
    previous_image = new wxButton(paintPanel, ID_PRE_IMG, wxT("<-"));
    next_image = new wxButton(paintPanel, ID_NEXT_IMG, wxT("->"));
    previous_10_image = new wxButton(paintPanel, ID_PRE10_IMG, wxT("10<-"));
    next_10_image = new wxButton(paintPanel, ID_NEXT10_IMG, wxT("->10"));
    remove_image = new wxButton(paintPanel, ID_REMOVE_IMG, wxT("Remove"));

    //* json
    save_json = new wxButton(paintPanel, ID_SAVE_JSON, wxT("SaveJson"));
    load_json = new wxButton(paintPanel, ID_LOAD_JSON, wxT("LoadJson"));
    dumpDataset = new wxButton(paintPanel, ID_DUMP_DATASET, wxT("Dump Dataset"));


    //* set box
    vbox_cls->Add(img_id_text);
    vbox_spawn->Add(box_spawn);
    vbox_remove->Add(box_remove);
    vbox_select->Add(box_select);
    vbox_copy->Add(box_copy);
    hbox0->Add(vbox_cls, 1, wxEXPAND);
    hbox0->Add(vbox_spawn, 1, wxEXPAND);
    hbox0->Add(vbox_remove, 1, wxEXPAND);
    hbox0->Add(vbox_copy, 1, wxEXPAND);
    hbox0->Add(vbox_select, 1, wxEXPAND);
    
    vbox->Add(hbox0, 1, wxEXPAND);
    
    hbox0_5->Add(rotate_world);
    hbox0_5->Add(world_degree);
    vbox->Add(hbox0_5, 1, wxEXPAND);
    //* stride
    //vbox_stride->Add(stride_val);
    //vbox_stride->Add(stride_text);
    //vbox_size->Add(size_val);
    //vbox_size->Add(size_text);
    //vbox_rotation->Add(rotation_val);
    //vbox_rotation->Add(rotation_text);
    //* configurate box
    //position
    vbox_x->Add(box_x_plus);
    vbox_x->Add(box_x_val);
    vbox_x->Add(box_x_minus);
     
    vbox_y->Add(box_y_plus);
    vbox_y->Add(box_y_val);
    vbox_y->Add(box_y_minus);

    vbox_z->Add(box_z_plus);
    vbox_z->Add(box_z_val);
    vbox_z->Add(box_z_minus);
    //size
    vbox_w->Add(box_w_plus);
    vbox_w->Add(box_w_val);
    vbox_w->Add(box_w_minus);

    vbox_h->Add(box_h_plus);
    vbox_h->Add(box_h_val);
    vbox_h->Add(box_h_minus);

    vbox_d->Add(box_d_plus);
    vbox_d->Add(box_d_val);
    vbox_d->Add(box_d_minus);
    //rotation
    vbox_rx->Add(box_rx_plus);
    vbox_rx->Add(box_rx_val);
    vbox_rx->Add(box_rx_minus);

    vbox_ry->Add(box_ry_plus);
    vbox_ry->Add(box_ry_val);
    vbox_ry->Add(box_ry_minus);

    vbox_rz->Add(box_rz_plus);
    vbox_rz->Add(box_rz_val);
    vbox_rz->Add(box_rz_minus);

    hbox_xyz->Add(vbox_x);
    hbox_xyz->Add(vbox_y);
    hbox_xyz->Add(vbox_z);

    hbox_whd->Add(vbox_w);
    hbox_whd->Add(vbox_h);
    hbox_whd->Add(vbox_d);
    
    hbox_rxyz->Add(vbox_rx);
    hbox_rxyz->Add(vbox_ry);
    hbox_rxyz->Add(vbox_rz);

    
    vbox_xyz->Add(stride_val,0,wxALIGN_CENTER);
    vbox_xyz->Add(stride_text,0,wxALIGN_CENTER);
    vbox_xyz->Add(hbox_xyz,0,wxALIGN_CENTER);
    
    vbox_whd->Add(size_val,0,wxALIGN_CENTER);
    vbox_whd->Add(size_text,0,wxALIGN_CENTER);
    vbox_whd->Add(hbox_whd,0,wxALIGN_CENTER);

    vbox_rxyz->Add(rotation_val,0,wxALIGN_CENTER);
    vbox_rxyz->Add(rotation_text,0,wxALIGN_CENTER);
    vbox_rxyz->Add(hbox_rxyz,0,wxALIGN_CENTER);

    hbox1->Add(vbox_xyz, 1, wxEXPAND);
    hbox1->Add(vbox_whd, 1, wxEXPAND);
    hbox1->Add(vbox_rxyz, 1, wxEXPAND);
    

    vbox->Add(hbox1 , 1, wxEXPAND);

    //grasp======================================================================
    //wxBoxSizer* vbox_grasp = new wxBoxSizer(wxVERTICAL);
    //wxBoxSizer *hbox_grasp_cls = new wxBoxSizer(wxHORIZONTAL);
    //wxBoxSizer *hbox_grasp_setting = new wxBoxSizer(wxHORIZONTAL);
    hbox_grasp_cls->Add(cls_grasp_select0,0,wxALIGN_CENTER);
    hbox_grasp_cls->Add(cls_grasp_select1,0,wxALIGN_CENTER);

    hbox_grasp_setting->Add(single_grasp_spawn,0,wxALIGN_CENTER);
    hbox_grasp_setting->Add(paired_grasp_spawn,0,wxALIGN_CENTER);
    hbox_grasp_setting->Add(paired_grasp_remove,0,wxALIGN_CENTER);
    hbox_grasp_setting->Add(paired_grasp_select,0,wxALIGN_CENTER);
    hbox_grasp_setting->Add(paired_index_select,0,wxALIGN_CENTER);
    hbox_grasp_setting->Add(paired_grasp_copy,0,wxALIGN_CENTER);
    ////grasp location////
    hbox_grasp_location_x->Add(grasp_x_plus,0,wxALIGN_CENTER);
    hbox_grasp_location_x->Add(grasp_x_minus,0,wxALIGN_CENTER);
    hbox_grasp_location_x->Add(grasp_x_val,0,wxALIGN_CENTER);

    hbox_grasp_location_y->Add(grasp_y_plus,0,wxALIGN_CENTER);
    hbox_grasp_location_y->Add(grasp_y_minus,0,wxALIGN_CENTER);
    hbox_grasp_location_y->Add(grasp_y_val,0,wxALIGN_CENTER);

    hbox_grasp_location_z->Add(grasp_z_plus,0,wxALIGN_CENTER);
    hbox_grasp_location_z->Add(grasp_z_minus,0,wxALIGN_CENTER);
    hbox_grasp_location_z->Add(grasp_z_val,0,wxALIGN_CENTER);

    vbox_grasp_location->Add(grasp_position_label);
    vbox_grasp_location->Add(grasp_stride_text);
    vbox_grasp_location->Add(hbox_grasp_location_x);
    vbox_grasp_location->Add(hbox_grasp_location_y);
    vbox_grasp_location->Add(hbox_grasp_location_z);
    ////grasp rotation////
    hbox_grasp_rotation_x->Add(grasp_rx_plus,0,wxALIGN_CENTER);
    hbox_grasp_rotation_x->Add(grasp_rx_minus,0,wxALIGN_CENTER);
    hbox_grasp_rotation_x->Add(grasp_rx_val,0,wxALIGN_CENTER);

    hbox_grasp_rotation_y->Add(grasp_ry_plus,0,wxALIGN_CENTER);
    hbox_grasp_rotation_y->Add(grasp_ry_minus,0,wxALIGN_CENTER);
    hbox_grasp_rotation_y->Add(grasp_ry_val,0,wxALIGN_CENTER);

    hbox_grasp_rotation_z->Add(grasp_rz_plus,0,wxALIGN_CENTER);
    hbox_grasp_rotation_z->Add(grasp_rz_minus,0,wxALIGN_CENTER);
    hbox_grasp_rotation_z->Add(grasp_rz_val,0,wxALIGN_CENTER);

    vbox_grasp_rotation->Add(grasp_rotation_label);
    vbox_grasp_rotation->Add(grasp_rotate_text);
    vbox_grasp_rotation->Add(hbox_grasp_rotation_x);
    vbox_grasp_rotation->Add(hbox_grasp_rotation_y);
    vbox_grasp_rotation->Add(hbox_grasp_rotation_z);
    ////grasp width////
    hbox_grasp_width->Add(grasp_w_plus,0,wxALIGN_CENTER);
    hbox_grasp_width->Add(grasp_w_minus,0,wxALIGN_CENTER);
    hbox_grasp_width->Add(grasp_w_val,0,wxALIGN_CENTER);

    vbox_grasp_width->Add(grasp_width_label);
    vbox_grasp_width->Add(hbox_grasp_width);

    hbox_grasp_configuration->Add(vbox_grasp_location, 1, wxEXPAND);
    hbox_grasp_configuration->Add(vbox_grasp_rotation, 1, wxEXPAND);
    hbox_grasp_configuration->Add(vbox_grasp_width, 1, wxEXPAND);

    
    vbox_grasp->Add(hbox_grasp_cls);
    vbox_grasp->Add(hbox_grasp_setting);
    vbox_grasp->Add(hbox_grasp_configuration);

    vbox->Add(vbox_grasp , 1, wxEXPAND);
    //==================================================================================

    hbox2->Add(previous_10_image);
    hbox2->Add(previous_image);
    hbox2->Add(img_id_text);
    hbox2->Add(next_image);
    hbox2->Add(next_10_image);
    hbox2->Add(remove_image);

    vbox->Add(hbox2, 0 ,  wxEXPAND, 0);

    hbox3->Add(save_json);
    hbox3->Add(load_json);
    hbox3->Add(dumpDataset);
    vbox->Add(hbox3,0,wxEXPAND,0);

    paintPanel->SetSizer(vbox);
    //*boxing setting
    Connect(ID_BOX_SPAWN, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnBoxSpawn));
    Connect(ID_BOX_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnBoxRemove));
    box_select->Connect(ID_COMBLEBOX, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(Annotator::OnComboBoxSelect), NULL, this);
    Connect(ID_BOX_COPY, wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(Annotator::OnBoxCopy));
    //* world setting
    Connect(ID_ROTATE_WORLD, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Annotator::OnWorldRotate));
    //* stride
    Connect(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED,wxCommandEventHandler(Annotator::OnTextUpdate)); 
    //* box configuration
    {
    //position
    Connect(ID_X_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_X_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_Y_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_Y_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_Z_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_Z_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    //size
    Connect(ID_W_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_W_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_H_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_H_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_D_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_D_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    //rotation
    Connect(ID_RX_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_RX_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_RY_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_RY_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_RZ_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    Connect(ID_RZ_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnBoxConfigure));
    }
    // *grasp
    //grasp setting
    {
    Connect(ID_SINGLE_GRASP_SPAWM, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnSingleGraspSpawn));
    Connect(ID_PAIRED_GRASP_SPAWM, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnPairedGraspSpawn));
    Connect(ID_PAIRED_GRASP_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnPairedGraspRemove));
    paired_grasp_select->Connect(ID_PAIRED_GRASP_SELECT, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(Annotator::OnPairedGraspSelect), NULL, this);
    paired_index_select->Connect(ID_PAIRED_INDEX_SELECT, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(Annotator::OnPairedIdSelect), NULL, this);
     Connect(ID_PAIRED_GRASP_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnPairedGraspCopy));
    }
    //grasp configuration
    
    { //position
    Connect(ID_GRASP_X_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_X_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_Y_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_Y_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_Z_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_Z_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    }

    { //rotation
    Connect(ID_GRASP_RX_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_RX_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_RY_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_RY_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_RZ_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_RZ_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    }
    {//width
    Connect(ID_GRASP_W_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    Connect(ID_GRASP_W_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnGraspConfigure));
    }
    
    //*image choose
    Connect(ID_PRE_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnPreviousClick));
    Connect(ID_NEXT_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnNextClick));
    Connect(ID_PRE10_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnPrevious10Click));
    Connect(ID_NEXT10_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnNext10Click));
    
    Connect(ID_REMOVE_IMG, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRemoveClick));

    //*json
    Connect(ID_SAVE_JSON, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnSaveJson));
    
    Connect(ID_LOAD_JSON, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnLoadJson));
    Connect(ID_DUMP_DATASET, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnDumpDataset));
    ShowImage();
}
//* Set box *//
void Annotator::OnBoxSpawn(wxCommandEvent & WXUNUSED(event)){
    try
    {
        int cls = stoi(cls_select->GetValue().ToStdString());
        if (cls <0){
            wxLogError("Number should >=  0");
            return;
        }
        //box_count ++;
        //box_id  = box_count -1; //set to latest box    
        labeltool->get_anno().box_spawn(cls);
        cout << "Box Spawn" << endl;
        
        box_select->Append(wxString::Format("box id: %d cls: %d", labeltool->get_anno().box_number()-1,cls ));

        ShowImage();

        return; 
    }
    catch(const std::exception& e)
    {
        wxLogError("Please input Number");
        return;
    }
}
void Annotator::OnBoxRemove(wxCommandEvent & WXUNUSED(event)){
    box_id = box_select->GetSelection();
    if (box_id != wxNOT_FOUND)
    {
        labeltool->get_anno().box_remove(box_id);
        //box_count--;

        box_select->Clear();
        for(int i=0; i<labeltool->get_anno().box_number();i++){
            box_select->Append(wxString::Format("box id: %d cls: %d", i, labeltool->get_anno().get_box(i).get_cls()));
        }
    }
    else
    {
        wxMessageBox("No item selected", "Error", wxOK | wxICON_ERROR);
        return;
    }
    box_id--;
    ShowImage();
}

void Annotator::OnBoxCopy(wxCommandEvent & WXUNUSED(event)){
    if (box_id != wxNOT_FOUND)
    {
        std::cout << "Copy box id: " << box_id << std::endl;
        int cls = labeltool->get_anno().get_box(box_id).get_cls();
        cv::Point3f pos = labeltool->get_anno().get_box(box_id).get_position() + cv::Point3f(0.05,0,0.05);
        cv::Point3f rot = labeltool->get_anno().get_box(box_id).get_rotation();
        cv::Point3f size = labeltool->get_anno().get_box(box_id).get_size();
        labeltool->get_anno().box_spawn(cls, pos, rot, size);
        box_select->Append(wxString::Format("box id: %d cls: %d", labeltool->get_anno().box_number()-1,cls ));
    }
    else
    {
        wxMessageBox("No item selected", "Error", wxOK | wxICON_ERROR);
        return;
    }
    ShowImage();
}
void Annotator::OnComboBoxSelect(wxCommandEvent& event)
{
    std::cout << "Event Triggered" << std::endl;
    box_id =  box_select->GetSelection();
    std::cout << "Select box: " << box_id << std::endl;

    cv::Point3f pos = labeltool->get_anno().get_box(box_id).get_position();
    cv::Point3f size = labeltool->get_anno().get_box(box_id).get_size();


    Box3d & box = labeltool->get_anno().get_box(box_id);

    paired_grasp_select->Clear();
        for(int i=0; i<box.paired_grasp_number();i++){
            paired_grasp_select->Append(wxString::Format("paired id: %d ", i));
        }
    
    updateLabel();
    ShowImage(box_id);
}

//* world settig    *//
void Annotator::OnWorldRotate(wxCommandEvent & WXUNUSED(event)){
    //rotate 90 degree in every click


    this->labeltool->world_rotation(90);
    world_degree->SetLabelText(
        wxString::Format(wxT("world degree = %f"), (float)labeltool->get_world_rotation())
    );
    ShowImage();
}
//* stride          *//

void Annotator::OnTextUpdate(wxCommandEvent& event){
    try{
        stride = stof(stride_text->GetValue().ToStdString());
        //std::cout << "change position stride to "<< stride << std::endl; 
        stride_val->SetLabelText(
            wxString::Format(wxT("position stride\n unit:(m) \n value:%f"), stride)
        );
    }catch(const std::exception& e){
        //wxLogError("Invalid input: Please input Number");
        stride = 0.05;
    }
    try{
        rotate = stof(rotation_text->GetValue().ToStdString())*M_PI/180;
        rotation_val->SetLabelText(
            wxString::Format(wxT("rotation stride\n unit:(degree) \n value:%f"), rotate/M_PI*180)
        );
    }catch(const std::exception& e){
        //wxLogError("Invalid input: Please input Number");
        rotate = 5 * M_PI/180; 
    }
    try{
        size_diff = stof(size_text->GetValue().ToStdString());
        size_val->SetLabelText(
            wxString::Format(wxT("size stride\n unit:(m) \n value:%f"), size_diff)
        );
    }catch(const std::exception& e){
        //wxLogError("Invalid input: Please input Number");
        size_diff = 0.05;
    }
}
//* configurate box *//
void Annotator::OnBoxConfigure(wxCommandEvent& event){
    cv::Point3f pos_diff(0.0, 0.0, 0.0);
    cv::Point3f rot_diff(0.0, 0.0, 0.0);
    cv::Point3f size_d(0.0, 0.0, 0.0); //size_diff已經被用了= =

    //========================================//
    // Get Button ID                          //
    //========================================//

    buttonid id = event.GetId();
    switch(id){
        case ID_X_PLUS:
            pos_diff.x = stride;
            break;
        case ID_X_MINUS:
            pos_diff.x = -stride;
            break;
        case ID_Y_PLUS:
            pos_diff.y = stride;
            break;
        case ID_Y_MINUS:
            pos_diff.y = -stride;
            break;
        case ID_Z_PLUS:
            pos_diff.z = stride;
            break;
        case ID_Z_MINUS:
            pos_diff.z = -stride;
            break;
        case ID_W_PLUS:
            size_d.x = size_diff;
            break;
        case ID_W_MINUS:
            size_d.x = -size_diff;
            break;
        case ID_H_PLUS:
            size_d.y = size_diff;
            break;
        case ID_H_MINUS:
            size_d.y = -size_diff;
            break;
        case ID_D_PLUS:
            size_d.z = size_diff;
            break;
        case ID_D_MINUS:
            size_d.z = -size_diff;
            break;
        case ID_RX_PLUS:
            rot_diff.x = rotate;
            break;
        case ID_RX_MINUS:
            rot_diff.x = -rotate;
            break;
        case ID_RY_PLUS:
            rot_diff.y = rotate;
            break;
        case ID_RY_MINUS:
            rot_diff.y = -rotate;
            break;
        case ID_RZ_PLUS:
            rot_diff.z = rotate;
            break;
        case ID_RZ_MINUS:
            rot_diff.z = -rotate;
            break;
        default:
            break;
    }
    //std::cout << "[box configure] button id = "<< id << std::endl;

    labeltool->get_anno().configure_box(this->box_id, 
                                    pos_diff, 
                                    rot_diff, 
                                    size_d
    );
    updateLabel();
    ShowImage(box_id);

}


//* Grasp         *//


//set grasp
void Annotator::OnSingleGraspSpawn(wxCommandEvent & WXUNUSED(event)){
    try{
        Box3d& box =  labeltool->get_anno().get_box(box_id);
    
        int cls;
        try{
            int cls = stoi(cls_grasp_select0->GetValue().ToStdString());
        }catch(const std::exception& e){
            cls = 0;
        }

        cv::Point3f pos = box.get_position();
        cv::Vec3f size = box.get_size();

        cv::Point3f grasp_position(
            pos.x,
            pos.y + size[1]/2,
            pos.z
        );
    
        cv::Vec3f grasp_rotation(
            0.0,
            0.0,
            0.0
        );
       


        try
        {
            
            if (cls<0){
                wxLogError("Number should >=  0");
                return;
            }
            //box_count ++;
            //box_id  = box_count -1; //set to latest box    
            box.single_grasp_spawn(
                cls,
                grasp_position,
                grasp_rotation,
                0.04
            );
            cout << "Single Grasp Spawn" << endl;
            
            //paired_grasp_select->Append(wxString::Format("single grasp id: %d ", box.single_grasp_number()-1 ));
            update_grasp_select();

            ShowImage(box_id, box.single_grasp_number()-1, -1);

            return; 
        }
        catch(const std::exception& e)
        {
            wxLogError("Please input Number");
            return;
        }

    }catch(const std::exception& e){
        wxLogError("invalid box_id");
        return;
    }
}
void Annotator::OnPairedGraspSpawn(wxCommandEvent & WXUNUSED(event)){
    try{
        Box3d& box =  labeltool->get_anno().get_box(box_id);
    
        int cls0;
        int cls1;
        try{
            int cls0 = stoi(cls_grasp_select0->GetValue().ToStdString());
            int cls1 = stoi(cls_grasp_select1->GetValue().ToStdString());
        }catch(const std::exception& e){
            cls0 = 0;
            cls1 = 0;
        }

        cv::Point3f pos = box.get_position();
        cv::Vec3f size = box.get_size();

        cv::Point3f grasp0_position(
            pos.x + size[0]/2,
            pos.y,
            pos.z
        );
        cv::Point3f grasp1_position(
            pos.x - size[0]/2,
            pos.y,
            pos.z
        );
        cv::Vec3f grasp0_rotation(
            0.0,
            180.0 * M_PI/180.0,
            0.0
        );
        cv::Vec3f grasp1_rotation(
            0.0,
            0.0,
            0.0
        );


        try
        {
            
            if (cls0 <0  || cls1 <0){
                wxLogError("Number should >=  0");
                return;
            }
            //box_count ++;
            //box_id  = box_count -1; //set to latest box    
            box.paired_grasp_spawn(
                cls0, cls1,
                grasp0_position,
                grasp0_rotation,
                0.04,
                grasp1_position,
                grasp1_rotation,
                0.04
            );
            cout << "Paired Grasp Spawn" << endl;
            
            //paired_grasp_select->Append(wxString::Format("paired grasp id: %d ", box.paired_grasp_number()-1 ));
            update_grasp_select();

            ShowImage(box_id, 
                box.single_grasp_number() + box.paired_grasp_number()-1, 
                -1);

            return; 
        }
        catch(const std::exception& e)
        {
            wxLogError("Please input Number");
            return;
        }

    }catch(const std::exception& e){
        wxLogError("invalid box_id");
        return;
    }
}

void Annotator::update_grasp_select(){
    Box3d& box =  labeltool->get_anno().get_box(box_id);

    paired_grasp_select->Clear();

    for(int i=0; i<box.single_grasp_number();i++){
        paired_grasp_select->Append(wxString::Format("s-grasp id: %d ", i));
    }
    for(int i=0; i<box.paired_grasp_number();i++){
        paired_grasp_select->Append(wxString::Format("p-grasp id: %d ", i));
    }
}


void Annotator::OnPairedGraspRemove(wxCommandEvent & WXUNUSED(event)){
    
    Box3d& box =  labeltool->get_anno().get_box(box_id);
    //box.paired_grasp_remove(paired_grasp_id);
    grasp_id = paired_grasp_select->GetSelection();
    if (grasp_id != wxNOT_FOUND)
    {
        int gid;
        if(grasp_id>=0 && grasp_id <box.single_grasp_number()){
            gid = grasp_id;
            box.single_grasp_remove(gid);
        }
        else{
            gid = grasp_id - box.single_grasp_number();
            box.paired_grasp_remove(gid);
        }
        //box_count--;

        update_grasp_select();
    }
    else
    {
        wxMessageBox("No item selected", "Error", wxOK | wxICON_ERROR);
        return;
    }
    grasp_id--;


    updateLabel();
    ShowImage();
}
void Annotator::OnPairedGraspSelect(wxCommandEvent& event){
    //Combo box event
    Box3d& box =  labeltool->get_anno().get_box(box_id);
    grasp_id = paired_grasp_select->GetSelection();
    
    std::cout << "Select grasp: " << grasp_id << std::endl;

    updateLabel();
    ShowImage(box_id, grasp_id, -1);
}
void Annotator::OnPairedIdSelect(wxCommandEvent & event){
    //Combo box event
    Box3d& box =  labeltool->get_anno().get_box(box_id);


    paired_id = paired_index_select->GetSelection();
    std::cout << "Select paired_id:  " << paired_id << std::endl;
    updateLabel();
    ShowImage(box_id , grasp_id, paired_id);
}
void Annotator::OnPairedGraspCopy(wxCommandEvent & WXUNUSED(event)){
    
}

//configurate
void Annotator::OnGraspConfigure(wxCommandEvent & event){
    Box3d& box =  labeltool->get_anno().get_box(box_id);
    
    //=======================================//
    // Get Rotation and Position Stride      //
    //=======================================//
    float grasp_stride, grasp_rotate_stride; 
    // position stride
    try{
        grasp_stride = stof(grasp_stride_text->GetValue().ToStdString());
    }catch(const std::exception& e){
        grasp_stride = 0.01;
    }
    // rotation stride
    try{
        grasp_rotate_stride = stof(grasp_rotate_text->GetValue().ToStdString())* M_PI/180.0;
    }catch(const std::exception& e){
        grasp_rotate_stride = 1 * M_PI/180.0;
    }


    //========================================//
    // Get Button ID                          //
    //========================================//

    buttonid id = event.GetId();  // Get the ID of the button that triggered the event
    
    cv::Point3f pos_diff(0.0, 0.0, 0.0);
    cv::Vec3f rot_diff(0.0, 0.0, 0.0);
    float width_diff = 0.0;

    switch (id)
    {
        case ID_GRASP_X_PLUS:
            pos_diff.x = grasp_stride;
            break;
        case ID_GRASP_X_MINUS:
            pos_diff.x = -grasp_stride;
            break;
        case ID_GRASP_Y_PLUS:
            pos_diff.y = grasp_stride;
            break;
        case ID_GRASP_Y_MINUS:
            pos_diff.y = -grasp_stride;
            break;
        case ID_GRASP_Z_PLUS:
            pos_diff.z = grasp_stride;
            break;
        case ID_GRASP_Z_MINUS:
            pos_diff.z = -grasp_stride;
            break;
        case ID_GRASP_RX_PLUS:
            rot_diff[0] = grasp_rotate_stride;
            break;
        case ID_GRASP_RX_MINUS:
            rot_diff[0] = -grasp_rotate_stride;
            break;
        case ID_GRASP_RY_PLUS:
            rot_diff[1] = grasp_rotate_stride;
            break;
        case ID_GRASP_RY_MINUS:
            rot_diff[1] = -grasp_rotate_stride;
            break;
        case ID_GRASP_RZ_PLUS:
            rot_diff[2] = grasp_rotate_stride;
            break;
        case ID_GRASP_RZ_MINUS:
            rot_diff[2] = -grasp_rotate_stride;
            break;
        case ID_GRASP_W_PLUS:
            width_diff = 0.01;
            break;
        case ID_GRASP_W_MINUS:
            width_diff = -0.01;
            break;
        default:
            return;
    }

    //std::cout << "[grasp configure] button id = "<< id << std::endl;

    int paired_grasp_id = grasp_id - box.single_grasp_number();
    if(paired_grasp_id<0)
    {
        box.configure_single_grasp(
            grasp_id,
            pos_diff,
            rot_diff,
            width_diff
        );
    }
    else
    {
        if(paired_id == 2){
            box.configure_paired_grasp(
                paired_grasp_id, 0, 
                pos_diff,
                rot_diff,
                width_diff
            );

            box.configure_paired_grasp(
                paired_grasp_id, 1, 
                pos_diff,
                rot_diff,
                width_diff
            );

        }else{
            box.configure_paired_grasp(
                paired_grasp_id, paired_id, 
                pos_diff,
                rot_diff,
                width_diff
            );
        }
    }
    updateLabel();
    ShowImage(box_id , grasp_id, paired_id);

    return;
}

//* Choose Image *//
void Annotator::OnNextClick(wxCommandEvent & WXUNUSED(event))
{   
    image_id = (image_id < labeltool->get_data_length()-1 )? image_id + 1 : image_id;
    cout << "image_id:" << this->image_id << endl;
    img_id_text->SetLabel(wxString::Format(wxT(" %d "), image_id));
    //cv::imshow("Stream",labeltool->get_imgdat(image_id).get_image());
    //cv::waitKey();
    ShowImage();
}

void Annotator::OnPreviousClick(wxCommandEvent & WXUNUSED(event))
{
    image_id = (image_id > 0 )? image_id - 1 : image_id;
    cout << "image_id:" << this->image_id << endl;
    img_id_text->SetLabel(wxString::Format(wxT("%d"), image_id));
    //cv::imshow("Stream",labeltool->get_imgdat(image_id).get_image());
    //cv::waitKey();
    ShowImage();
}

void Annotator::OnNext10Click(wxCommandEvent & WXUNUSED(event))
{
    int max_img = labeltool->get_data_length();
    image_id = (image_id < max_img-1-10 )? image_id + 10 : max_img;
    if(image_id>max_img-1) image_id = max_img-1;


    cout << "image_id:" << this->image_id << endl;
    img_id_text->SetLabel(wxString::Format(wxT(" %d "), image_id));
    //cv::imshow("Stream",labeltool->get_imgdat(image_id).get_image());
    //cv::waitKey();
    ShowImage();
}

void Annotator::OnPrevious10Click(wxCommandEvent & WXUNUSED(event))
{
    image_id = (image_id > 9 )? image_id - 10 : 0;
    cout << "image_id:" << this->image_id << endl;
    img_id_text->SetLabel(wxString::Format(wxT("%d"), image_id));
    //cv::imshow("Stream",labeltool->get_imgdat(image_id).get_image());
    //cv::waitKey();
    ShowImage(box_id);
}
void Annotator::ShowImage(
    int show_selected_box_direction,
    int show_selected_paired_grasp,
    int show_selected_paired_id
){

    cv::Mat RGB_img;
    cv::cvtColor(
        labeltool->imshow_with_label(
            image_id, 
            show_selected_box_direction,
            show_selected_paired_grasp,
            show_selected_paired_id
            ), 
        RGB_img, 
        cv::COLOR_RGB2BGR
        );
    cv::imshow("Stream", RGB_img);
}
void Annotator::updateLabel(){
    // box
    cv::Point3f pos = labeltool->get_anno().get_box(box_id).get_position();
    cv::Point3f size = labeltool->get_anno().get_box(box_id).get_size();
    cv::Point3f rotate = labeltool->get_anno().get_box(box_id).get_rotation();
    box_x_val->SetLabelText(
        wxString::Format(wxT("%f"), pos.x)
    );

    box_y_val->SetLabelText(
        wxString::Format(wxT("%f"), pos.y)
    );

    box_z_val->SetLabelText(
        wxString::Format(wxT("%f"), pos.z)
    );

    box_w_val->SetLabelText(
        wxString::Format(wxT("%f"), size.x)
    );

    box_h_val->SetLabelText(
        wxString::Format(wxT("%f"), size.y)
    );

    box_d_val->SetLabelText(
        wxString::Format(wxT("%f"), size.z)
    );

    box_rx_val->SetLabelText(
        wxString::Format(wxT("%f"), 180*rotate.x/M_PI)
    );

    box_ry_val->SetLabelText(
        wxString::Format(wxT("%f"), 180*rotate.y/M_PI)
    );

    box_rz_val->SetLabelText(
        wxString::Format(wxT("%f"), 180*rotate.z/M_PI)
    );
    //pos
    
    Box3d& box =  labeltool->get_anno().get_box(box_id);

    if(box.paired_grasp_number() + box.single_grasp_number() == 0 || grasp_id > box.single_grasp_number() + box.paired_grasp_number()-1) return;
    
    cv::Point3f grasp_pos;
    cv::Point3f grasp_rotate; 
    float grasp_width;
    
    std::cout << "update label()->grasp_id: " << grasp_id << std::endl;
    if(grasp_id<box.single_grasp_number())
    {
        Grasp& single_grasp = box.get_single_grasp(grasp_id);
        grasp_pos = single_grasp.get_position();
        grasp_rotate = single_grasp.get_rotation();
        grasp_width = single_grasp.get_width();
    }
    else // grasp_id >= single_grasp_number()
    {
        int paired_grasp_id = grasp_id-box.single_grasp_number();
        std::tuple<Grasp, Grasp> & paired_grasp = box.get_paired_grasp(paired_grasp_id);


        if(paired_id % 2 == 0){ //paired_id == 0,2 
            grasp_pos = std::get<0>(paired_grasp).get_position();
            grasp_rotate = std::get<0>(paired_grasp).get_rotation();
            grasp_width = std::get<0>(paired_grasp).get_width();
        }else{ //paired_id == 1
            grasp_pos = std::get<1>(paired_grasp).get_position();
            grasp_rotate = std::get<1>(paired_grasp).get_rotation();
            grasp_width = std::get<1>(paired_grasp).get_width();
        }
    }
    

    //grasp position
    grasp_x_val->SetLabelText(
        wxString::Format(wxT("X = %f"), grasp_pos.x)
    );
    grasp_y_val->SetLabelText(
        wxString::Format(wxT("Y = %f"), grasp_pos.y)
    );
    grasp_z_val->SetLabelText(
        wxString::Format(wxT("Z = %f"), grasp_pos.z)
    );

    grasp_rx_val->SetLabelText(
        wxString::Format(wxT("RX = %f"), grasp_rotate.x)
    );
    grasp_ry_val->SetLabelText(
        wxString::Format(wxT("RY = %f"), grasp_rotate.y)
    );
    grasp_rz_val->SetLabelText(
        wxString::Format(wxT("RZ = %f"), grasp_rotate.z)
    );
    grasp_w_val->SetLabelText(
        wxString::Format(wxT("W = %f"), grasp_width)
    );
}

void Annotator::OnRemoveClick(wxCommandEvent & WXUNUSED(event)){
    std::cout << "remove image id: " << image_id << std::endl;
    labeltool->remove_imgdat(image_id);
    ShowImage(box_id);
}

void Annotator::OnSaveJson(wxCommandEvent & WXUNUSED(event)){
    wxDirDialog dirDialog(this, "Choose a directory", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dirDialog.ShowModal() == wxID_OK)
    {
        wxString dirPath = dirDialog.GetPath();
        //wxMessageBox("Selected directory: " + dirPath, "Info", wxOK | wxICON_INFORMATION);
        labeltool->dump_annotaion_json(dirPath.ToStdString(), labeltool->get_world_rotation());
        std::cout << "save json file to " << dirPath.ToStdString() << std::endl;
    }
    
   
}

void Annotator::OnLoadJson(wxCommandEvent & WXUNUSED(event)){
    wxFileDialog openFileDialog(this, _("Open JSON File"), "", "",
                                     "JSON files (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    labeltool->get_anno().box_clean();

    if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return; // the user changed their mind
    }else{
        std::string filename = openFileDialog.GetPath().ToStdString();
        std::cout << "loading json file from " << filename << std::endl;
        int err = labeltool->load_annotation_json(filename);
        if(err)
            wxLogError("failed to load json");
        else{
            std::cout << "Successfully load json file from " << filename << std::endl;

            //combo box
            box_select->Clear();
            for(int i=0; i<labeltool->get_anno().box_number();i++){
                box_select->Append(wxString::Format("box id: %d cls: %d", i, labeltool->get_anno().get_box(i).get_cls()));
            }
            box_id = 0;
            update_grasp_select();
            ShowImage();
        }

    }
    
}

void Annotator::OnDumpDataset(wxCommandEvent & WXUNUSED(event)){
    labeltool->dump_dataset_json();
    /*
    wxDirDialog dirDialog(this, "Choose a directory", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    
    if (dirDialog.ShowModal() == wxID_OK)
    {
        wxString storePath = dirDialog.GetPath();
        std::cout << "Saving Dataset to " << storePath.ToStdString()<< std::endl;

        wxTextEntryDialog textDialog(this, "Enter the name of the new directory:", "Directory Name");
        if (textDialog.ShowModal() == wxID_CANCEL) {
            return; // User canceled the dialog
        }


        wxString dirName = textDialog.GetValue();
        fs::path fullPath = fs::path(storePath.ToStdString()) / dirName.ToStdString();
        std::cout << "fullpath ok"<< std::endl;
        // Create the directory
        if (!fs::exists(fullPath)) {
            if (fs::create_directory(fullPath)) {
                

                fs::path fullPath = fs::path(storePath.ToStdString()) / dirName.ToStdString();
                std::cout << "fullpath :"<< std::string(fullPath)<< std::endl;
                //wxMessageBox("Selected directory: " + dirPath, "Info", wxOK | wxICON_INFORMATION);

                labeltool->dump_dataset_json(fullPath);
                wxMessageBox("Directory created successfully!", "Success", wxOK | wxICON_INFORMATION);
            } else {
                wxMessageBox("Failed to create directory!", "Error", wxOK | wxICON_ERROR);
                return;
            }
        } else {
            wxMessageBox("Directory already exists!", "Error", wxOK | wxICON_ERROR);
            return ;
        }

        
    }*/
}