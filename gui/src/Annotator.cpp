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
    cv::resizeWindow("Stream", 640, 480);
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
    //position
    Connect(ID_X_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnXPlus));
    Connect(ID_X_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnXMinus));
    Connect(ID_Y_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnYPlus));
    Connect(ID_Y_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnYMinus));
    Connect(ID_Z_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnZPlus));
    Connect(ID_Z_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnZMinus));
    //size
    Connect(ID_W_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnWPlus));
    Connect(ID_W_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnWMinus));
    Connect(ID_H_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnHPlus));
    Connect(ID_H_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnHMinus));
    Connect(ID_D_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnDPlus));
    Connect(ID_D_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnDMinus));
    //rotation
    Connect(ID_RX_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRXPlus));
    Connect(ID_RX_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRXMinus));
    Connect(ID_RY_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRYPlus));
    Connect(ID_RY_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRYMinus));
    Connect(ID_RZ_PLUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRZPlus));
    Connect(ID_RZ_MINUS, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(Annotator::OnRZMinus));

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
//position

void Annotator::OnXPlus(wxCommandEvent & WXUNUSED(event)){
    //float rotate_angle = 5.0;
    //float rotate_x =  3.14159265358979323846/180.0 * rotate_angle; 
    
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(stride, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnXMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(-stride, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnYPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, stride, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnYMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, -stride, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnZPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, stride), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnZMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, -stride), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
//siZe

void Annotator::OnWPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(size_diff,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnWMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(-size_diff,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnHPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, size_diff/2, 0), //確保箱子還是放在地板上
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,size_diff,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnHMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, -size_diff/2, 0), //確保箱子還是放在地板上
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,-size_diff,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnDPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,size_diff)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnDMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,-size_diff)
    );

    ShowImage(box_id);
    updateLabel();
}

//rotation
void Annotator::OnRXPlus(wxCommandEvent & WXUNUSED(event)){
    //float rotate_angle = 5.0;
    //float rotate_x =  3.14159265358979323846/180.0 * rotate_angle; 
    std::cout << "rotateRX" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(rotate,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRXMinus(wxCommandEvent & WXUNUSED(event)){
    std::cout << "rotate-RX" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(-rotate,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRYPlus(wxCommandEvent & WXUNUSED(event)){
    std::cout << "rotateRY" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,rotate,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRYMinus(wxCommandEvent & WXUNUSED(event)){
    std::cout << "rotate-RY" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,-rotate,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRZPlus(wxCommandEvent & WXUNUSED(event)){
    std::cout << "rotateRZ" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,rotate), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRZMinus(wxCommandEvent & WXUNUSED(event)){
    std::cout << "rotate-RZ" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,-rotate), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
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
void Annotator::ShowImage(int show_selected_box_direction){
    cv::imshow("Stream", labeltool->imshow_with_label(image_id, show_selected_box_direction));
}
void Annotator::updateLabel(){
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
            ShowImage();
        }

    }
    
}

void Annotator::OnDumpDataset(wxCommandEvent & WXUNUSED(event)){
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

        
    }

}