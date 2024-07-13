

#include <iostream>
#include <wx/wx.h>


#include "DataReader.h"
class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};



bool MyApp::OnInit()
{

    //Communicate *communicate = new Communicate(wxT("Widgets communicate"));
    //communicate->Show(true);
  
    //menu
    //SimpleMenu *menu = new SimpleMenu(wxT("Simple Menu"));
    //menu->Show(true);

    //submenu
    //SubMenu *smenu = new SubMenu(wxT("Submenu"));
    //smenu->Show(true);

    //toolbar (don't use)
    //Toolbar *toolbar = new Toolbar(wxT("Toolbar"));
    //toolbar->Show(true);
    
    //Sizer
    //Sizer *sizer = new Sizer(wxT("Sizer"));
    //sizer->Show(true);
    
    //Border (box sizer)
    //Border *border = new Border(wxT("Border"));
    //border->Show(true);

    //align
    //Align *align = new Align(wxT("Align"));
    //align->Show(true);
    
    //datareader
    DataReader *datareader = new DataReader(wxT("DataLoader"));
    datareader->Show(true);
    
    return true;
}

IMPLEMENT_APP(MyApp)