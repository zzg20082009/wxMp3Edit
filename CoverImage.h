#ifndef __COVER_IMAGE__
#define __COVER_IMAGE__
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif
#endif

class cover
{
public:
  cover(char* );
  const wxString& GetPicType();
  const wxString& GetDescription();
  const wxString& GetMimeType();
  wxImage* GetImage();
  void SaveImage(const wxString& filename);
private:
  char* picframe;
  wxString picType;
  wxString description;
  wxString MIMETYPE;
  wxImage* image;
};
