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
    //int box_count;

// box configuration
    float stride = 0.05;
    float rotate = 5 * M_PI/180; 
    float size_diff = 0.05;

// grasp settting
    int grasp_id;
    int paired_id;


// image choose
    int image_id;
    //int max_img;
    void ShowImage(
        int show_selected_box_direction = -1,
        int show_selected_paired_grasp = -1,
        int show_selected_paired_id = -1
        );
    void updateLabel();
    void update_grasp_select();
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
    wxButton *box_copy;

    //* world setting
    wxStaticText * world_degree;
    wxButton * rotate_world; 
    //*stride
    wxStaticText * stride_val;
    wxStaticText * rotation_val;
    wxStaticText * size_val;
    wxTextCtrl *stride_text;
    wxTextCtrl *rotation_text;
    wxTextCtrl *size_text;
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
    //* paired grasp
    //set paired grasp
    wxTextCtrl *cls_grasp_select0;
    wxTextCtrl *cls_grasp_select1;

    wxButton *single_grasp_spawn;
    wxButton *paired_grasp_spawn;
    wxButton *paired_grasp_remove;
    wxComboBox *paired_grasp_select;
    wxComboBox *paired_index_select;
    wxButton *paired_grasp_copy;
    // grasp position
    wxTextCtrl *grasp_stride_text; //grasp position 的 stride
    wxStaticText * grasp_position_label;//顯示這裡是location
        //x
        wxButton *grasp_x_plus;
        wxStaticText * grasp_x_val;
        wxButton *grasp_x_minus;
        //y
        wxButton *grasp_y_plus;
        wxStaticText * grasp_y_val;
        wxButton *grasp_y_minus;
        //z
        wxButton *grasp_z_plus;
        wxStaticText * grasp_z_val;
        wxButton *grasp_z_minus;
    
    // grasp rotation
    wxTextCtrl *grasp_rotate_text; //grasp position 的 stride
    wxStaticText * grasp_rotation_label;//顯示這裡是location
        //rx
        wxButton *grasp_rx_plus;
        wxStaticText * grasp_rx_val;
        wxButton *grasp_rx_minus;
        //ry
        wxButton *grasp_ry_plus;
        wxStaticText * grasp_ry_val;
        wxButton *grasp_ry_minus;
        //rz
        wxButton *grasp_rz_plus;
        wxStaticText * grasp_rz_val;
        wxButton *grasp_rz_minus;
    // grasp width

    //* choose image
    wxButton * previous_10_image;
    wxButton * next_10_image;
    wxButton * previous_image;
    wxButton * next_image;
    wxButton * remove_image;
    wxStaticText * img_id_text;// = new wxStaticText(this, -1, wxT("0"),

    //* json
    wxButton * save_json; 
    wxButton* load_json;
    wxButton* dumpDataset;

    int pos;//slider value

    //*set box
    void OnBoxSpawn(wxCommandEvent & WXUNUSED(event));
    void OnBoxRemove(wxCommandEvent & WXUNUSED(event));
    void OnComboBoxSelect(wxCommandEvent& event);
    void OnBoxCopy(wxCommandEvent & WXUNUSED(event));
    //*world setting
    void OnWorldRotate(wxCommandEvent & WXUNUSED(event));
    //*stride
    void OnTextUpdate(wxCommandEvent& event);

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

    //* set grasp
    void OnSingleGraspSpawn(wxCommandEvent & WXUNUSED(event));
    void OnPairedGraspSpawn(wxCommandEvent & WXUNUSED(event));
    void OnPairedGraspRemove(wxCommandEvent & WXUNUSED(event));
    void OnPairedGraspSelect(wxCommandEvent& event);
    void OnPairedIdSelect(wxCommandEvent & WXUNUSED(event));
    void OnPairedGraspCopy(wxCommandEvent & WXUNUSED(event));
    //*grasp configuration
    //position
        //x
        void OnGraspXPlus(wxCommandEvent & WXUNUSED(event));
        void OnGraspXMinus(wxCommandEvent & WXUNUSED(event));
        //y
        void OnGraspYPlus(wxCommandEvent & WXUNUSED(event));
        void OnGraspYMinus(wxCommandEvent & WXUNUSED(event));
        //z
        void OnGraspZPlus(wxCommandEvent & WXUNUSED(event));
        void OnGraspZMinus(wxCommandEvent & WXUNUSED(event));
    //rotation
        //rx
        void OnGraspRXPlus(wxCommandEvent & WXUNUSED(event));
        void OnGraspRXMinus(wxCommandEvent & WXUNUSED(event));
        //ry
        void OnGraspRYPlus(wxCommandEvent & WXUNUSED(event));
        void OnGraspRYMinus(wxCommandEvent & WXUNUSED(event));
        //rz
        void OnGraspRZPlus(wxCommandEvent & WXUNUSED(event));
        void OnGraspRZMinus(wxCommandEvent & WXUNUSED(event));
    //width

    //*image choose
    void OnPreviousClick(wxCommandEvent & WXUNUSED(event));
    void OnNextClick(wxCommandEvent & WXUNUSED(event));
    void OnPrevious10Click(wxCommandEvent & WXUNUSED(event));
    void OnNext10Click(wxCommandEvent & WXUNUSED(event));
    void OnRemoveClick(wxCommandEvent & WXUNUSED(event));

    //*image
    void OnSaveJson(wxCommandEvent & WXUNUSED(event));
    void OnLoadJson(wxCommandEvent & WXUNUSED(event));
    void OnDumpDataset(wxCommandEvent & WXUNUSED(event));

};
const int ID_CLS_SELECT = 108;
const int ID_BOX_SPAWN = 105;
const int ID_BOX_REMOVE = 106;
const int ID_COMBLEBOX = 107;
const int ID_BOX_COPY = 124;

const int ID_PRE_IMG = 101;
const int ID_NEXT_IMG = 102;
const int ID_PRE10_IMG = 103;
const int ID_NEXT10_IMG = 104;
const int ID_REMOVE_IMG = 121;

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

const int ID_RX_PLUS = 1121;
const int ID_RX_MINUS = 1122;
const int ID_RY_PLUS = 1123;
const int ID_RY_MINUS = 1124;
const int ID_RZ_PLUS = 1125;
const int ID_RZ_MINUS = 1126;

const int ID_SAVE_JSON = 123;
const int ID_LOAD_JSON = 122;
const int ID_DUMP_DATASET = 125;
//const int ID_STRIDE_TEXT = 127;
//const int ID_SIZE_TEXT = 128;
//const int ID_ROTATE_TEXT = 129;

const int ID_ROTATE_WORLD = 130;


/*
wxTextCtrl *cls_grasp_select0; //不用事件
wxTextCtrl *cls_grasp_select1; //不用事件
wxButton *paired_grasp_spawn;
wxButton *paired_grasp_remove;
wxComboBox *paired_grasp_select;
wxRadioButton *paired_index_select;
wxButton *paired_grasp_copy;
*/
const int ID_SINGLE_GRASP_SPAWM = 148;
const int ID_PAIRED_GRASP_SPAWM = 131;
const int ID_PAIRED_GRASP_SELECT = 132; //combo box
const int ID_PAIRED_GRASP_REMOVE = 133;
const int ID_PAIRED_INDEX_SELECT = 134;
const int ID_PAIRED_GRASP_COPY = 135;

const int ID_GRASP_X_PLUS = 136;
const int ID_GRASP_X_MINUS = 137;
const int ID_GRASP_Y_PLUS = 138;
const int ID_GRASP_Y_MINUS = 139;
const int ID_GRASP_Z_PLUS = 140;
const int ID_GRASP_Z_MINUS = 141;

const int ID_GRASP_RX_PLUS = 142;
const int ID_GRASP_RX_MINUS = 143;
const int ID_GRASP_RY_PLUS = 144;
const int ID_GRASP_RY_MINUS = 145;
const int ID_GRASP_RZ_PLUS = 146;
const int ID_GRASP_RZ_MINUS = 147;

#endif

 