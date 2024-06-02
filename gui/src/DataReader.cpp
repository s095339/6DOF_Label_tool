#include "DataReader.h"

#include <wx/dir.h>

#include "DataLoader.h"
DataReader::DataReader(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(280, 180))
{

    menubar = new wxMenuBar;
    start = new wxMenu;
    start->Append(wxID_FILE1, wxT("&Open"));
    start->AppendSeparator();

    quit = new wxMenuItem(start, wxID_EXIT, wxT("&Quit\tCtrl+W"));
    start->Append(quit);

    menubar->Append(start, wxT("&start"));
    SetMenuBar(menubar);

    Connect(wxID_FILE1, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(DataReader::OnReadDirectory));

    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, 
        wxCommandEventHandler(DataReader::OnQuit));
    Centre();

}

void DataReader::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

void DataReader::OnReadDirectory(wxCommandEvent& event)
{
    wxString directory = wxDirSelector("Choose a directory");
    if (!directory.empty())
    {
        wxDir dir(directory);
        if (!dir.IsOpened())
        {
            wxLogError("Could not open the directory.");
            return;
        }

        wxString filename;
        bool cont = dir.GetFirst(&filename);
        std::string dir_path = std::string(dir.GetName().mb_str());
        //std::cout <<" Get data from: "<< dir.GetName() << std::endl;
        //while (cont)
        //{
        //    wxLogMessage("Found file: %s", filename);
        //    cont = dir.GetNext(&filename);
        //}

        DataLoader * dataloader = new DataLoader(dir_path);

    }
}