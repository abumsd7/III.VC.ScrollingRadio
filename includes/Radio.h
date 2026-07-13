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
#elif defined(GTAVC)
    static CSprite2d radioIcons[23];
#else
    static CSprite2d radioIcons[25];
#endif
    static const char *radioIconNamesIII[21];
    static const char *radioIconNamesVC[23];
    static const char *radioIconNamesSA[25];

    static void InitRadio();
    static void __fastcall DrawRadioIcons(void* self = nullptr, int edx = 0);
    static void ShutdownRadio();
};