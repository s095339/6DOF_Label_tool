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
    int box_id;
    int box_count;
    int image_id;
    int max_img;
    void ShowImage();
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
    
    //* choose image
    wxButton * previous_10_image;
    wxButton * next_10_image;
    wxButton * previous_image;
    wxButton * next_image;
    wxStaticText * img_id_text;// = new wxStaticText(this, -1, wxT("0"),
    int pos;//slider value

    void OnBoxSpawn(wxCommandEvent & WXUNUSED(event));
    void OnBoxRemove(wxCommandEvent & WXUNUSED(event));

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

#endif

