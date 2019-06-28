#ifndef __MP3_EDIT__
#define __MP3_EDIT__
#include "parse.h"
#include <wx/wx.h>
#include <wx/generic/statbmpg.h>
#endif

enum { ID_TREE = wxID_HIGHEST + 1, ID_LIST, ID_BUTTONOK, ID_BUTTONCANCEL, ID_LIST_CTRL, ID_BITMAP };
class MainFace : public wxFrame
{
 public:
  MainFace(const wxString& title);
 private:
  wxMenuBar* menuBar;
  wxStatusBar* statusBar;
  wxTextCtrl* tagValue[7]; //  vector<wxTextCtrl*> tagValue;
  wxGenericDirCtrl* folderTree;
  wxListCtrl* fileListCtrl;
  wxStaticBitmap* coverPic;
  
  void SetTagValue(const wxString& filename);
  void FillList(const wxString& filename);
  
  void OnOpen(wxCommandEvent& event);
  void OnClose(wxCommandEvent& event);
  void OnTreeItemChanged(wxTreeEvent& event);     // Added
  void OnListItemActivated(wxListEvent& event);
};
