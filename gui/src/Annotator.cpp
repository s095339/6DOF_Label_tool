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
    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);// choose the image
    
    //* set bounding box [hbox0]
    cls_select = new wxTextCtrl(paintPanel, ID_CLS_SELECT, "", wxDefaultPosition, wxDefaultSize);
    box_spawn  = new wxButton(paintPanel, ID_BOX_SPAWN, wxT(" BoxSpawn "), wxDefaultPosition);
    box_remove = new wxButton(paintPanel, ID_BOX_REMOVE, wxT(" BoxRemove "), wxDefaultPosition);
    box_select = new wxComboBox(paintPanel, ID_COMBLEBOX, wxT("-no box-"));

    //TODO Configure bounding box
    
    
    //* choose image [hbox2]
  
    //hbox2 
    img_id_text = new wxStaticText(paintPanel, -1, wxT("-------"),wxDefaultPosition,wxSize(20,-1));
    previous_image = new wxButton(paintPanel, ID_PRE_IMG, wxT("<-"));
    next_image = new wxButton(paintPanel, ID_NEXT_IMG, wxT("->"));
    previous_10_image = new wxButton(paintPanel, ID_PRE10_IMG, wxT("10<-"));
    next_10_image = new wxButton(paintPanel, ID_NEXT10_IMG, wxT("->10"));



    // set sizer 
    hbox0->Add(img_id_text);
    hbox0->Add(box_spawn);
    hbox0->Add(box_remove);
    hbox0->Add(box_select);
    vbox->Add(hbox0, 1, wxEXPAND);

    //hbox1->Add(img_panel);
    vbox->Add(hbox1 , 1, wxEXPAND);

    hbox2->Add(previous_10_image);
    hbox2->Add(previous_image);
    hbox2->Add(img_id_text);
    hbox2->Add(next_image);
    hbox2->Add(next_10_image);

    vbox->Add(hbox2, 0 ,  wxALIGN_BOTTOM|wxBOTTOM, 0);

    paintPanel->SetSizer(vbox);
    //set Event
    Connect(ID_BOX_SPAWN, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnBoxSpawn));
    Connect(ID_BOX_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnBoxRemove));


    Connect(ID_PRE_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnPreviousClick));
    Connect(ID_NEXT_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnNextClick));
    Connect(ID_PRE10_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnPrevious10Click));
    Connect(ID_NEXT10_IMG, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(Annotator::OnNext10Click));
    cout << "Annotator created" << endl;
}
//Set box
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



//Choose Image
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
    ShowImage();
}
void Annotator::ShowImage(){
    cv::imshow("Stream", labeltool->imshow_with_label(image_id));
}