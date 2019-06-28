#include "parse.h"

const wxString id3TagInfo[] = { wxT("TIT2"), wxT("TPE1"), wxT("TALB"), wxT("TYER"), wxT("TCOM"), wxT("TRCK"), wxT("TEXT") };

parse::parse(const wxString& filename)
{
  char header[10];

  Cover = NULL;
  wxFile mp3File(filename, wxFile::read);
  mp3File.Read(header, 10);
  if (strncmp(header, "ID3\03\00", 5) == 0) {
    char* p = header + 6;
    size_t sz = (p[0] & 0x7F) * 0x200000 + (p[1] & 0x7F) * 0x4000 + (p[2] & 0x7F) * 0x80 + (p[3] & 0x7F);
    
    char* id3TagBuff = (char *) malloc(sz);
    mp3File.Read(id3TagBuff, sz);
    FillPairs(id3TagBuff, sz);
    free(id3TagBuff);
  } else {
    mp3File.Seek(-128, wxFromEnd);
    struct mp3id3v1 info;
    mp3File.Read(&info, 128);
    if (strncmp(info.header, "TAG", 3) == 0) {
      const wxCSConv GB2UNI(wxT("GB18030"));
      Tag2ValuePairs[wxT("TIT2")] = wxString(info.title, GB2UNI, 30).Trim();
      Tag2ValuePairs[wxT("TPE1")] = wxString(info.artist, GB2UNI, 30).Trim();
      Tag2ValuePairs[wxT("TALB")] = wxString(info.album, GB2UNI, 30).Trim();
      Tag2ValuePairs[wxT("TYER")] = wxString(info.year, GB2UNI, 4).Trim();
      Tag2ValuePairs[wxT("COMM")] = wxString(info.comment, GB2UNI, 30).Trim();
    }
  }
  mp3File.Close();
}

void parse::FillPairs(char* frame, size_t sz)
{
  char* p;
  for (int i = 0; i < 7; i++) {
    wxString tagName = wxString(id3TagInfo[i]);
    if ((p = (char *) memmem(frame, sz, id3TagInfo[i].c_str(), 4)) != NULL) {
      p += 4;         // 指向帧的size部分，开始计算这一帧的大小，获取帧的内容
      size_t fsz = p[0] * 0x100000000 + p[1] * 0x10000 + p[2] * 0x100 + p[3];
      //      if (strncmp(id3TagInfo[i], "PRIV", 4) != 0)
      fsz -= 1;       // 帧的大小包含了表示编码的一个字节，需要减去
      int encode = (*(p += 6) == 1? 1 : 0);  // encode == 1 是UNICODE，否则是GB18030编码

      if (encode == 1) {
	p += 3;
	Tag2ValuePairs[tagName] = wxString(p, wxCSConv(wxT("UTF16")), fsz - 2);  // p + 3指向内容,fsz-2是因为有两个0
      } else {
	Tag2ValuePairs[tagName] = wxString(++p, wxCSConv(wxT("GB18030")), fsz);
      }
    } else {
      Tag2ValuePairs[tagName] = wxString(wxT(""));
    }
  }
  while ((p = (char *) memmem(frame, sz, "APIC", 4)) != NULL) {
    Cover = new cover(p);
    break;
  }
}

wxString& parse::GetTagValue(const wxString& tagName)
{
  map<wxString, wxString>::iterator tagIt;
  tagIt = Tag2ValuePairs.find(tagName);
  if (tagIt != Tag2ValuePairs.end()) {
    return tagIt->second;
  } else {
    Tag2ValuePairs[tagName] = wxString(wxT(""));
    return Tag2ValuePairs[tagName];
  }
}

cover* parse::GetCover()
{
  return Cover;
}
