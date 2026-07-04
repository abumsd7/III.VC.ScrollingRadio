#pragma once
#include "plugin.h"
#include "CSprite2d.h"

struct RadioStation {
    int gameId;
    int colorIconIdx;
    int grayIconIdx;
};

class Radio {
  public:
    static RwTexDictionary *radioTexDict;
#ifdef GTA3
    static CSprite2d radioIcons[21];
#else
    static CSprite2d radioIcons[23];
#endif
    static const char *radioIconNamesIII[21];
    static const char *radioIconNamesVC[23];

    static void InitRadio();
    static void DrawRadioIcons();
    static void ShutdownRadio();
};