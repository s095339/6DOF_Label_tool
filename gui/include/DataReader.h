#ifndef __DATAREADER_H_
#define __DATAREADER_H_

#include <wx/wx.h>


class DataReader : public wxFrame
{
public:
    DataReader(const wxString& title);

    void OnReadDirectory(wxCommandEvent& event);
    void OnQuit(wxCommandEvent & event);

    wxMenuBar *menubar;
    wxMenu *start;
    wxMenuItem *quit;

};



#endif