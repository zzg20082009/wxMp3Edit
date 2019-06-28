#ifndef __PARSE__
#define __PARSE__
#include <vector>
#include <map>
#include <wx/wxprec.h>
#include <wx/file.h>
#include "CoverImage.h"
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif
#endif

extern const wxString id3TagInfo[];
using namespace std;
struct mp3id3v1 {
  char header[3];
  char title[30];
  char artist[30];
  char album[30];
  char year[4];
  char comment[30];
  char genre;
};

class parse
{
public:
  parse(const wxString& title);
  wxString& GetTagValue(const wxString& tagName);
  cover* GetCover();
  void WriteNewMp3(wxString newfile);       // 写一个新的MP3文件
  //  wxImage* GetCover(char*);            // 设置cover，给其图像数据
private:
  map<wxString, wxString> Tag2ValuePairs;   // ID3名字到值所指的内存区的映射
  void FillPairs(char* frame, size_t sz);              // 填充Tag2ValuePairs
  cover* Cover;
};
