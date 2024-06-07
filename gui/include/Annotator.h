#ifndef _ANNOTATOR_H_ 
#define _ANNOTATOR_H_ 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <wx/wx.h>
#include "DataLoader.h"
#include "LabelTool.h"
#include "Annotation.h"

class Annotator: public wxFrame
{
private:
// box setting
    int box_id;
    int box_count;
// box configuration
    float stride = 0.05;
    float rotate = 5 * M_PI/180; 
// image choose
    int image_id;
    int max_img;
    void ShowImage(int show_selected_box_direction = -1);
    void updateLabel();
public:
    Annotator(const wxString& title,  wxBitmapType format, std::string);
    //API
    DataLoader *dataloader;
    LabelTool *labeltool;
    
    //* set bounding box 
    wxTextCtrl *cls_select;
    wxButton *box_spawn;
    wxButton *box_remove;
    wxComboBox *box_select;
    
    //* configure bounding box
    //posision
        //x
        wxButton *box_x_plus;
        wxStaticText * box_x_val;
        wxButton *box_x_minus;
        //y
        wxButton *box_y_plus;
        wxStaticText * box_y_val;
        wxButton *box_y_minus;
        //z
        wxButton *box_z_plus;
        wxStaticText * box_z_val;
        wxButton *box_z_minus;
    //size
        //width
        wxButton *box_w_plus;
        wxStaticText * box_w_val;
        wxButton *box_w_minus;
        //hieght
        wxButton *box_h_plus;
        wxStaticText * box_h_val;
        wxButton *box_h_minus;
        //depth
        wxButton *box_d_plus;
        wxStaticText * box_d_val;
        wxButton *box_d_minus;
    //rotation
        //rx
        wxButton *box_rx_plus;
        wxStaticText * box_rx_val;
        wxButton *box_rx_minus;
        //ry
        wxButton *box_ry_plus;
        wxStaticText * box_ry_val;
        wxButton *box_ry_minus;
        //rz
        wxButton *box_rz_plus;
        wxStaticText * box_rz_val;
        wxButton *box_rz_minus;
    //* choose image
    wxButton * previous_10_image;
    wxButton * next_10_image;
    wxButton * previous_image;
    wxButton * next_image;
    wxStaticText * img_id_text;// = new wxStaticText(this, -1, wxT("0"),
    int pos;//slider value

    //*set box
    void OnBoxSpawn(wxCommandEvent & WXUNUSED(event));
    void OnBoxRemove(wxCommandEvent & WXUNUSED(event));
    void OnComboBoxSelect(wxCommandEvent& event);
    //*box configuration
    //position
    void OnXPlus(wxCommandEvent & WXUNUSED(event));
    void OnXMinus(wxCommandEvent & WXUNUSED(event));
    void OnYPlus(wxCommandEvent & WXUNUSED(event));
    void OnYMinus(wxCommandEvent & WXUNUSED(event));
    void OnZPlus(wxCommandEvent & WXUNUSED(event));
    void OnZMinus(wxCommandEvent & WXUNUSED(event));
    //size
    void OnWPlus(wxCommandEvent & WXUNUSED(event));
    void OnWMinus(wxCommandEvent & WXUNUSED(event));
    void OnHPlus(wxCommandEvent & WXUNUSED(event));
    void OnHMinus(wxCommandEvent & WXUNUSED(event));
    void OnDPlus(wxCommandEvent & WXUNUSED(event));
    void OnDMinus(wxCommandEvent & WXUNUSED(event));
    //rotation
    void OnRXPlus(wxCommandEvent & WXUNUSED(event));
    void OnRXMinus(wxCommandEvent & WXUNUSED(event));
    void OnRYPlus(wxCommandEvent & WXUNUSED(event));
    void OnRYMinus(wxCommandEvent & WXUNUSED(event));
    void OnRZPlus(wxCommandEvent & WXUNUSED(event));
    void OnRZMinus(wxCommandEvent & WXUNUSED(event));
    //*image choose
    void OnPreviousClick(wxCommandEvent & WXUNUSED(event));
    void OnNextClick(wxCommandEvent & WXUNUSED(event));
    void OnPrevious10Click(wxCommandEvent & WXUNUSED(event));
    void OnNext10Click(wxCommandEvent & WXUNUSED(event));

};
const int ID_CLS_SELECT = 108;
const int ID_BOX_SPAWN = 105;
const int ID_BOX_REMOVE = 106;
const int ID_COMBLEBOX = 107;

const int ID_PRE_IMG = 101;
const int ID_NEXT_IMG = 102;
const int ID_PRE10_IMG = 103;
const int ID_NEXT10_IMG = 104;

const int ID_X_PLUS = 109;
const int ID_X_MINUS = 110;
const int ID_Y_PLUS = 111;
const int ID_Y_MINUS = 112;
const int ID_Z_PLUS = 113;
const int ID_Z_MINUS = 114;

const int ID_W_PLUS = 115;
const int ID_W_MINUS = 116;
const int ID_H_PLUS = 117;
const int ID_H_MINUS = 118;
const int ID_D_PLUS = 119;
const int ID_D_MINUS = 120;

const int ID_RX_PLUS = 121;
const int ID_RX_MINUS = 122;
const int ID_RY_PLUS = 123;
const int ID_RY_MINUS = 124;
const int ID_RZ_PLUS = 125;
const int ID_RZ_MINUS = 126;
#endif

