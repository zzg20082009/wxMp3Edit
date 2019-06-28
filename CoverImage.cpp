#include "CoverImage.h"
#include <wx/mstream.h>
#include <wx/file.h>
cover::cover(char* p)
{
  image = NULL;
  picframe = p;   // 要记录下图像帧的大小
  p += 4;
  int framesz = (p[0] & 0x7F) << 21 + (p[1] & 0x7F) << 14 + (p[2] &0x7F) << 7 + (p[3] & 0x7F);
    //  uint framesz = p[0] * 0x100000000 + p[1] * 0x10000 + p[2] * 0x100 + p[3];
  wxString encode = (*(p += 6) == 0)? "GB18030" : "UTF16";
  p++;            // p现在指向图像的MIME类型， image/png or image/jpeg
  size_t mtypelength = strlen(p);
  MIMETYPE = wxString(p, strlen(p));
  if (MIMETYPE == "image/jpg") MIMETYPE = wxString("image/jpeg");
  p += mtypelength;
  p++;            // p指向图像type
  switch (*p) {
  case 3:
    picType = wxString("Cover (front)");
    break;
  case 4:
    picType = wxString("Cover (back)");
    break;
  case 8:
    picType = wxString("Artist/Performer");
    break;
  default:
    picType = wxString("Other");
    break;
  }
  p++;      // p现在指向Description字符串的开始
  char* pend = p;
  while (*pend) pend++;
  while (*pend == 0) pend++;
  if (encode == "UTF16") {   // pend 现在指向图像数据的开始
    int length = pend - p - 4;
    description = wxString(p += 2, wxCSConv(wxT("UTF16")), length);
  } else {
    int length = pend - p - 1;
    description = wxString(p, wxCSConv(wxT("GB18030")), length);
  }
  
  uint imageSize = framesz - (pend - picframe - 10);
  
  wxInitAllImageHandlers();
  wxMemoryInputStream picture(pend, imageSize);     // picture指向提供图像数据的流
  image = new wxImage(picture, MIMETYPE);  // 创建了一个wxImage对象
  if (image == NULL)
    wxMessageBox("Error to Create wxImage", "Image");
}

const wxString& cover::GetPicType()
{
  return picType;
}

const wxString& cover::GetDescription()
{
  return description;
}

const wxString& cover::GetMimeType()
{
  return MIMETYPE;
}

wxImage* cover::GetImage()
{
  return image;
}
