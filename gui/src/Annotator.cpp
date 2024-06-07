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

#include "DataLoader.h"
#include "LabelTool.h"
#include "Annotation.h"



using namespace std;


map<string, double> _intrinsic_para = {
    {"fx", 908.78692627},
    {"fy", 907.3694458},
    {"cx", 638.07818604},
    {"cy", 358.32138062}
};

map<int, cv::Point3f> _refMarkerArray = {
    //===============================
        {20, cv::Vec3f(0.0, 0.0, 0.0) },
        {21, cv::Vec3f(0.5, 0.0, 0.0) },
        {22, cv::Vec3f(0.5, 0.0, 0.7) },
        {23, cv::Vec3f(0.0, 0.0, 0.7) },
    //=={==, cv::Vec3f(=============) }===========
        {25, cv::Vec3f(0.0, 0.0, 0.1) },
        {26, cv::Vec3f(0.0, 0.0, 0.2) },
        {27, cv::Vec3f(0.0, 0.0, 0.3) },
        {28, cv::Vec3f(0.0, 0.0, 0.4) },
        {29, cv::Vec3f(0.0, 0.0, 0.5) },
        {30, cv::Vec3f(0.0, 0.0, 0.6) },
    //=={==, cv::Vec3f(=============) }============
        {14, cv::Vec3f(0.1, 0.0, 0.0) },
        {24, cv::Vec3f(0.2, 0.0, 0.0) },
        {19, cv::Vec3f(0.3, 0.0, 0.0) },
    //=={=== cv::Vec3f(=============) }============
        {7,  cv::Vec3f(0.5, 0.0, 0.1) },
        {8,  cv::Vec3f(0.5, 0.0, 0.2) },
        {9,  cv::Vec3f(0.5, 0.0, 0.3) },
        {10, cv::Vec3f(0.5, 0.0, 0.4) },
        {11, cv::Vec3f(0.5, 0.0, 0.5) },
        {12, cv::Vec3f(0.5, 0.0, 0.6) },
    //=={=== cv::Vec3f(=============) }=============
        {15, cv::Vec3f(0.1, 0.0, 0.7) },
        {16, cv::Vec3f(0.2, 0.0, 0.7) },
        {17, cv::Vec3f(0.3, 0.0, 0.7) },
        {18, cv::Vec3f(0.4, 0.0, 0.7) },
};
cv::Mat _intrinsic = (cv::Mat_<double>(3,3) << _intrinsic_para["fx"], 0.0, _intrinsic_para["cx"], 0.0, _intrinsic_para["fy"] ,\
                                                        _intrinsic_para["cy"], 0.0, 0.0, 1.0);
cv::Mat _dist = (cv::Mat_<double>(1,5) << 0,0,0,0,0);

Annotator::Annotator(const wxString& title,  wxBitmapType format, string dir_path)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,300))
{   

    cv::namedWindow("Stream", cv::WINDOW_NORMAL);
    cv::resizeWindow("Stream", 640, 480);
    wxInitAllImageHandlers();
    // build LabelTool===============================
    this->dataloader = new DataLoader(dir_path);
    dataloader->set_Camera_intrinsic(_intrinsic);
    dataloader->set_Camera_dist(_dist);
    this->labeltool = new LabelTool(*dataloader);
    labeltool->build_data_list(_refMarkerArray, 5);

    box_id = 0;
    box_count = 0;
    max_img = labeltool->get_data_length();
    image_id = 0;
    //arrangement===============================================
    wxPanel *paintPanel = new wxPanel(this, -1);
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hbox0 = new wxBoxSizer(wxHORIZONTAL);// put box and choose box
    
    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);// configure box
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
    
    //* set bounding box [hbox0]
    cls_select = new wxTextCtrl(paintPanel, ID_CLS_SELECT, "", wxDefaultPosition, wxDefaultSize);
    box_spawn  = new wxButton(paintPanel, ID_BOX_SPAWN, wxT(" BoxSpawn "), wxDefaultPosition);
    box_remove = new wxButton(paintPanel, ID_BOX_REMOVE, wxT(" BoxRemove "), wxDefaultPosition);
    box_select = new wxComboBox(paintPanel, ID_COMBLEBOX, wxT("-no box-"));

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



    //* set box
    hbox0->Add(img_id_text);
    hbox0->Add(box_spawn);
    hbox0->Add(box_remove);
    hbox0->Add(box_select);
    vbox->Add(hbox0, 1, wxEXPAND);

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

    hbox1->Add(vbox_x, 1, wxEXPAND);
    hbox1->Add(vbox_y, 1, wxEXPAND);
    hbox1->Add(vbox_z, 1, wxEXPAND);
    hbox1->Add(vbox_w, 1, wxEXPAND);
    hbox1->Add(vbox_h, 1, wxEXPAND);
    hbox1->Add(vbox_d, 1, wxEXPAND);
    hbox1->Add(vbox_rx, 1, wxEXPAND);
    hbox1->Add(vbox_ry, 1, wxEXPAND);
    hbox1->Add(vbox_rz, 1, wxEXPAND);

    vbox->Add(hbox1 , 1, wxEXPAND);

    hbox2->Add(previous_10_image);
    hbox2->Add(previous_image);
    hbox2->Add(img_id_text);
    hbox2->Add(next_image);
    hbox2->Add(next_10_image);

    vbox->Add(hbox2, 0 ,  wxEXPAND, 0);

    paintPanel->SetSizer(vbox);
    //*set Event
    Connect(ID_BOX_SPAWN, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnBoxSpawn));
    Connect(ID_BOX_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnBoxRemove));
    box_select->Connect(ID_COMBLEBOX, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(Annotator::OnComboBoxSelect), NULL, this);
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
        box_count ++;
        //box_id  = box_count -1; //set to latest box    
        labeltool->get_anno().box_spawn(cls);
        cout << "Box Spawn" << endl;
        
        box_select->Append(wxString::Format("box id: %d cls: %d", box_count-1,cls ));

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
        box_count--;

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
                                    cv::Point3f(stride,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnWMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(-stride, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(-stride,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnHPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,stride,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnHMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,-stride,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnDPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,stride)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnDMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,0), 
                                    cv::Point3f(0,0,-stride)
    );

    ShowImage(box_id);
    updateLabel();
}

//rotation
void Annotator::OnRXPlus(wxCommandEvent & WXUNUSED(event)){
    //float rotate_angle = 5.0;
    //float rotate_x =  3.14159265358979323846/180.0 * rotate_angle; 
    std::cout << "rotate!" << std::endl;
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(rotate,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRXMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(-rotate,0,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRYPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,rotate,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRYMinus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,-rotate,0), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRZPlus(wxCommandEvent & WXUNUSED(event)){
    labeltool->get_anno().configure_box(this->box_id, 
                                    cv::Point3f(0, 0, 0), 
                                    cv::Point3f(0,0,rotate), 
                                    cv::Point3f(0,0,0)
    );

    ShowImage(box_id);
    updateLabel();
}
void Annotator::OnRZMinus(wxCommandEvent & WXUNUSED(event)){
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
    image_id = (image_id < max_img-1 )? image_id + 1 : image_id;
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
        wxString::Format(wxT("%f"), rotate.x)
    );

    box_ry_val->SetLabelText(
        wxString::Format(wxT("%f"), rotate.y)
    );

    box_rz_val->SetLabelText(
        wxString::Format(wxT("%f"), rotate.z)
    );
}