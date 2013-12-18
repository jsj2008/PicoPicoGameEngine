#include "PPTTFont.h"
#include "PPTTFont-Def.h"
#include <string>
#include <sstream>

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
  "Roboto-Bold.ttf",
  "DroidSansJapanese.ttf",
  "DroidSansFallback.ttf",
  "DroidSans.ttf",
  NULL
};

static int file_exist (const char *filename)
{
  FILE *fp;
  fp = fopen(filename,"r");
  if (fp == NULL) return 0;
  fclose(fp);
  return 1;
}

const char* PPFont::systemFontFilePath()
{
  if (sFontFile!="") return sFontFile.c_str();
  for (int i=0;fontFiles[i]!=NULL;i++) {
    std::stringstream ss;
    ss << "/system/fonts/" << fontFiles[i];
    sFontFile = ss.str();
    if (file_exist(sFontFile.c_str())) {
      return sFontFile.c_str();
    }
    sFontFile = "";
  }
  for (int i=0;fontFiles[i]!=NULL;i++) {
    std::stringstream ss;
    ss << "/system/font/" << fontFiles[i];
    sFontFile = ss.str();
    if (file_exist(sFontFile.c_str())) {
      return sFontFile.c_str();
    }
    sFontFile = "";
  }
  for (int i=0;fontFiles[i]!=NULL;i++) {
    std::stringstream ss;
    ss << "/data/fonts/" << fontFiles[i];
    sFontFile = ss.str();
    if (file_exist(sFontFile.c_str())) {
      return sFontFile.c_str();
    }
    sFontFile = "";
  }
  return "System";
}
