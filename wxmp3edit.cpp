#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include "wxmp3edit.h"

MainFace::MainFace(const wxString& title)
  : wxFrame(NULL, wxID_ANY, title, wxPoint(-1, -1), wxSize(720, 640))
{
  wxPanel* panel = new wxPanel(this, wxID_ANY);
  wxFlexGridSizer* GFgs = new wxFlexGridSizer(2, 2, 5, 5);

  wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
  
  folderTree = new wxGenericDirCtrl(panel, ID_TREE, "/", wxPoint(-1, -1), wxSize(350, 275));
  GFgs->Add(folderTree, 1, wxEXPAND);

  fileListCtrl = new wxListCtrl(panel, ID_LIST_CTRL, wxPoint(-1, -1), wxSize(320, 275), wxLC_REPORT);
  fileListCtrl->AppendColumn(wxT("文件名"));
  fileListCtrl->AppendColumn(wxT("文件大小"));
  fileListCtrl->AppendColumn(wxT("访问日期"));
  GFgs->Add(fileListCtrl, 1, wxEXPAND);

  coverPic = new wxStaticBitmap(panel, ID_BITMAP, wxBitmap(350, 310));
  GFgs->Add(coverPic, 1, wxEXPAND);
  
  wxFlexGridSizer* FGS = new wxFlexGridSizer(7, 2, 3, 10);
  for (int i = 0; i < 7; i++) {
    wxStaticText* tagName = new wxStaticText(panel, wxID_ANY, id3TagInfo[i]);
    tagValue[i] = new wxTextCtrl(panel, wxID_ANY);
    FGS->Add(tagName);
    FGS->Add(tagValue[i], 1, wxEXPAND);
  }
  FGS->AddGrowableCol(1, 1);
  
  GFgs->Add(FGS, 1, wxEXPAND);
  GFgs->AddGrowableCol(1, 2);
  GFgs->AddGrowableRow(0, 1);
  
  wxButton* okButton = new wxButton(panel, ID_BUTTONOK, wxT("OK"));
  wxButton* cancelButton = new wxButton(panel, ID_BUTTONCANCEL, wxT("Cancel"));
  hbox2->Add(okButton, 0, wxALL, 5);
  hbox2->Add(cancelButton, 0, wxALL, 5);
  
  vbox->Add(GFgs, 1, wxEXPAND);
  vbox->Add(hbox2, 0, wxALIGN_RIGHT);
  panel->SetSizer(vbox);
  Centre();
  
  wxMenu* file = new wxMenu;
  wxMenuBar* menuBar = new wxMenuBar;
  file->Append(wxID_OPEN, wxT("打开"));
  file->Append(wxID_EXIT, wxT("退出"));
  menuBar->Append(file, wxT("文件"));
  SetMenuBar(menuBar);
  
  statusBar = new wxStatusBar(panel);
  statusBar->SetStatusText(wxEmptyString);
  SetStatusBar(statusBar);
  
  Bind(wxEVT_MENU, &MainFace::OnOpen, this, wxID_OPEN);
  Bind(wxEVT_MENU, &MainFace::OnClose, this, wxID_EXIT);
  Bind(wxEVT_BUTTON, &MainFace::OnClose, this, ID_BUTTONCANCEL);
  Bind(wxEVT_DIRCTRL_SELECTIONCHANGED, &MainFace::OnTreeItemChanged, this, ID_TREE);
  Bind(wxEVT_LIST_ITEM_ACTIVATED, &MainFace::OnListItemActivated, this, ID_LIST_CTRL);
}

void MainFace::FillList(const wxString& nodeText)
{
  fileListCtrl->DeleteAllItems();   // 需要删除所有的项，否则在选择到其它目录时，前面显示的列表内容不消除
  wxDir dir(nodeText);
  
  wxSetWorkingDirectory(nodeText);
  statusBar->SetStatusText(wxGetCwd());   // not correct here
  
  wxString filename;
  wxStructStat stat;
  bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

  while (cont) {
    
    wxStat(filename, &stat);
    wxDateTime lmt(stat.st_mtime);
    long index = fileListCtrl->InsertItem(0, filename);
    fileListCtrl->SetItem(index, 1, wxString::Format(wxT("%d"),(int) stat.st_size));
    fileListCtrl->SetItem(index, 2, lmt.Format());
    cont = dir.GetNext(&filename);
  }
}


void MainFace::OnTreeItemChanged(wxTreeEvent& event)
{
  wxTreeItemId selectedNode = event.GetItem();
  wxString nodeText = folderTree->GetPath(selectedNode);
  if (wxDirExists(nodeText)) {      // wxDirExists()函数判断给定的字符串是不是一个目录，如果是个目录，希望获取该目录下文件的信息
    FillList(nodeText);
  }
}

void MainFace::OnListItemActivated(wxListEvent& event)
{
  
  const wxString filename = event.GetText();
  if (wxDirExists(filename)) {   // filename是一个目录，打开这个目录
    FillList(filename);
  } else {
    wxFileName FileName(filename);
    //    wxMessageBox("The file is", filename);
    if (FileName.GetExt() == "mp3") {
      SetTagValue(filename);
      //}
    }
  }
}
void MainFace::OnClose(wxCommandEvent& event)
{
  Close();
}

void MainFace::SetTagValue(const wxString& filename)
{
  parse* Parse = new parse(filename);
  //const wxString TagInfo[] = { "TIT2", "TPE1", "TALB", "TYER", "TCOM", "TRCK", "TEXT" };
  for (int i = 0; i < 7; i++) {
    tagValue[i]->Clear();
    tagValue[i]->WriteText(Parse->GetTagValue(id3TagInfo[i]));
  }

  wxImage* picture = new wxImage(350, 310);
  wxBitmap pic = wxBitmap(*picture);
  
  cover* Cover = Parse->GetCover();
  if (Cover == NULL) {
    coverPic->SetBitmap(pic);     // 这段代码主要是为了防止没有图像的MP3文件显示上次的图像文件
    return;
  }

  picture = Cover->GetImage();
  if (picture == NULL) {
    coverPic->SetBitmap(pic);
    return;    
  }

  pic = wxBitmap(*picture);
  //  coverPic->SetScaleMode(Scale_AspectFit); // use this after 3.10
  coverPic->SetBitmap(pic);
}

void MainFace::OnOpen(wxCommandEvent& event)
{
  wxFileDialog* openFileDialog = new wxFileDialog(this);
  wxString filename;
  
  if (openFileDialog->ShowModal() == wxID_OK) {
    filename = openFileDialog->GetPath();         // 可以加上一个判断文件是不是有效的MP3文件
  }
  SetTagValue(filename);
}


class MyApp : public wxApp
{
public:
  virtual bool OnInit();
};

bool MyApp::OnInit()
{
  MainFace* mainface = new MainFace("MP3 Audio Tag");
  mainface->Show(true);
  return true;
}

wxIMPLEMENT_APP(MyApp);
