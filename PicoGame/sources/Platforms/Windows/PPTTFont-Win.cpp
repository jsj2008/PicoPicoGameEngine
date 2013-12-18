#include "PPTTFont.h"
#include "PPTTFont-Def.h"
#include <string>
#include <sstream>
#include <shlwapi.h>

int PPTTFont::loadSystemFont(const char* name,int size)
{
  return 1;
}

PPTTFontTile* PPTTFont::imageSystemFont(const char* string)
{
  return NULL;
}

#include "PPFont.h"

static std::string sFontFile="";

static const char* fontFiles[] = {
  "meiryo.ttc",
  "MSGOTHIC.TTC",
  "Arialbd.ttf",
  "Arial.ttf",
  NULL
};

const char* PPFont::systemFontFilePath()
{
  if (sFontFile!="") return sFontFile.c_str();
  for (int i=0;fontFiles[i]!=NULL;i++) {
    CHAR winDir[MAX_PATH];
    GetWindowsDirectory(winDir, MAX_PATH);
    std::stringstream ss;
    ss << winDir << "\\Fonts\\" << fontFiles[i];
    sFontFile = ss.str();
    if (PathFileExists(sFontFile.c_str())) {
      return sFontFile.c_str();
    }
    sFontFile = "";
  }
  return "System";
}
