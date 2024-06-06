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
    int image_id;
    int max_img;
    void ShowImage();
public:
    Annotator(const wxString& title,  wxBitmapType format, std::string);
    //API
    DataLoader *dataloader;
    LabelTool *labeltool;
    //Image 
    //wxImagePanel *img_panel;
    //Control widget
    //wxSlider *image_slider;
    

    //* choose image
    wxButton * previous_10_image;
    wxButton * next_10_image;
    wxButton * previous_image;
    wxButton * next_image;
    wxStaticText * img_id_text;// = new wxStaticText(this, -1, wxT("0"),
    int pos;//slider value
    void OnPreviousClick(wxCommandEvent & WXUNUSED(event));
    void OnNextClick(wxCommandEvent & WXUNUSED(event));
    void OnPrevious10Click(wxCommandEvent & WXUNUSED(event));
    void OnNext10Click(wxCommandEvent & WXUNUSED(event));

};


const int ID_PRE_IMG = 101;
const int ID_NEXT_IMG = 102;
const int ID_PRE10_IMG = 103;
const int ID_NEXT10_IMG = 104;

#endif

