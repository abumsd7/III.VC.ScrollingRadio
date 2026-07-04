#include "..\includes\Radio.h"
#include "..\includes\RadioConfig.h"
#include "..\includes\InputHandler.h"
#include "..\includes\SomeMacros.h"
#include "extensions/Screen.h"
#include <cmath>
#include "CFileLoader.h"
#include "cDMAudio.h"
#include "CMenuManager.h"
#include "CPad.h"

#ifdef GTA3
#include "cMusicManager.h"
#include "eRadioStations.h"
#define RADIO_MANAGER gMusicManager
#define MUSIC_VOLUME_PREF CMenuManager::m_nPrefsMusicVolume
#else
#define RADIO_MANAGER DMAudio
#define MUSIC_VOLUME_PREF FrontEndMenuManager.m_nPrefsMusicVolume
#endif

using namespace plugin;

RwTexDictionary *Radio::radioTexDict = nullptr;

#ifdef GTA3
CSprite2d Radio::radioIcons[21];
#else
CSprite2d Radio::radioIcons[23];
#endif

const char *Radio::radioIconNamesIII[21] = {
    "chatterbox", "chatterbox_gray", "doublecleff", "doublecleff_gray",
    "flashback",  "flashback_gray",  "gameradio",   "gameradio_gray",
    "headradio",  "headradio_gray",  "kjah",        "kjah_gray",
    "lips",       "lips_gray",       "mp3player",   "mp3player_gray",
    "msx",        "msx_gray",        "off_button",  "rise",
    "rise_gray"
};

const char *Radio::radioIconNamesVC[23] = {
    "810vcn",     "810vcn_gray", "emotion",      "emotion_gray", "espantoso",     "espantoso_gray",
    "fever",      "fever_gray",  "flash",        "flash_gray",   "kchat",         "kchat_gray",
    "mp3",        "mp3_gray",    "off_button",   "vcpr",         "vcpr_gray",     "vrock",
    "vrock_gray", "wave103",     "wave103_gray", "wildstyle",    "wildstyle_gray"
};

static int GetActiveStations(RadioStation *outStations) {
    int count;

    count = 0;
#ifdef GTA3
    outStations[count++] = { 0, 8, 9 };    // HEAD_RADIO -> headradio
    outStations[count++] = { 1, 2, 3 };    // DOUBLE_CLEF_FM -> doublecleff
    outStations[count++] = { 2, 10, 11 };  // JAH_RADIO -> kjah
    outStations[count++] = { 3, 19, 20 };  // RISE_FM -> rise
    outStations[count++] = { 4, 12, 13 };  // LIPS_106 -> lips
    outStations[count++] = { 5, 6, 7 };    // GAME_FM -> gameradio
    outStations[count++] = { 6, 16, 17 };  // MSX_FM -> msx
    outStations[count++] = { 7, 4, 5 };    // FLASHBACK_95_6 -> flashback
    outStations[count++] = { 8, 0, 1 };    // CHATTERBOX_109 -> chatterbox
    outStations[count++] = { 9, 14, 15 };  // MP3_PLAEYR -> mp3player
    outStations[count++] = { 11, 18, -1 }; // RADIO_OFF -> off_button
#else
    outStations[count++] = { 0, 21, 22 };  // WILDSTYLE
    outStations[count++] = { 1, 8, 9 };    // FLASH_FM
    outStations[count++] = { 2, 10, 11 };  // KCHAT
    outStations[count++] = { 3, 6, 7 };    // FEVER
    outStations[count++] = { 4, 17, 18 };  // V_ROCK
    outStations[count++] = { 5, 15, 16 };  // VCPR
    outStations[count++] = { 6, 4, 5 };    // RADIO_ESPANTOSO
    outStations[count++] = { 7, 2, 3 };    // EMOTION
    outStations[count++] = { 8, 19, 20 };  // WAVE
    if (RADIO_MANAGER.IsMP3RadioChannelAvailable()) {
        outStations[count++] = { 9, 12, 13 };  // MP3
        outStations[count++] = { 10, 14, -1 }; // RADIO_OFF (10)
    } else {
        outStations[count++] = { 9, 14, -1 };  // RADIO_OFF (9)
    }
#endif
    return count;
}

static int GetScrollIndex(int gameId, const RadioStation *activeStations, int count) {
    int i;
    for (i = 0; i < count; i++) {
        if (activeStations[i].gameId == gameId) {
            return i;
        }
    }
    return count - 1;
}

static void DrawIcon(int index, float x, float y, float alpha, float relPos, const RadioStation *activeStations) {
    float finalWidth, finalHeight, absRel, colorFactor;
    int colorIdx, grayIdx, colorType;
    CRect rect;

    if (index < 0) {
        return;
    }

    finalWidth = SCREEN_SCALE_X(128.0f * (config.IconSizeMultiplier / 100.0f));
    finalHeight = SCREEN_SCALE_Y(128.0f * (config.IconSizeMultiplier / 100.0f));
    
    rect.left = x - finalWidth / 2.0f;
    rect.right = x + finalWidth / 2.0f;
    rect.top = y - finalHeight / 2.0f;
    rect.bottom = y + finalHeight / 2.0f;

    colorIdx = activeStations[index].colorIconIdx;
    grayIdx = activeStations[index].grayIconIdx;
    colorType = config.ColorType;

    if (colorType == 1) {
        Radio::radioIcons[colorIdx].Draw(rect, CRGBA(255, 255, 255, (unsigned char)alpha));
    } else if (colorType == 3) {
        if (grayIdx != -1) {
            Radio::radioIcons[grayIdx].Draw(rect, CRGBA(255, 255, 255, (unsigned char)alpha));
        } else {
            Radio::radioIcons[colorIdx].Draw(rect, CRGBA(150, 150, 150, (unsigned char)alpha));
        }
    } else {
        if (grayIdx == -1) {
            Radio::radioIcons[colorIdx].Draw(rect, CRGBA(255, 255, 255, (unsigned char)alpha));
        } else {
            absRel = fabs(relPos);
            if (absRel >= 1.0f) {
                Radio::radioIcons[grayIdx].Draw(rect, CRGBA(255, 255, 255, (unsigned char)alpha));
            } else {
                colorFactor = 1.0f - absRel;
                Radio::radioIcons[grayIdx].Draw(rect, CRGBA(255, 255, 255, (unsigned char)alpha));
                Radio::radioIcons[colorIdx].Draw(rect, CRGBA(255, 255, 255, (unsigned char)(alpha * colorFactor)));
            }
        }
    }
}

void Radio::InitRadio() {
    int i;
#ifdef GTA3
    radioTexDict = CFileLoader::LoadTexDictionary(GAME_PATH("charlesvercetti_mods\\ScrollingRadio\\radio_iii.txd"));
    for (i = 0; i < 21; ++i) {
        radioIcons[i].m_pTexture = RwTexDictionaryFindNamedTexture(radioTexDict, radioIconNamesIII[i]);
    }
#else
    radioTexDict = CFileLoader::LoadTexDictionary(GAME_PATH("charlesvercetti_mods\\ScrollingRadio\\radio_vc.txd"));
    for (i = 0; i < 23; ++i) {
        radioIcons[i].m_pTexture = RwTexDictionaryFindNamedTexture(radioTexDict, radioIconNamesVC[i]);
    }
#endif
}

void Radio::DrawRadioIcons() {
    static float currentScrollPos = -1.0f;
    static int lastStationId = -1;
    static unsigned int displayTimeout = 0;
    static unsigned char savedVolume = 64;
    static bool isMuted = false;
    CVehicle *vehicle;
    CPed *player;
    RadioStation activeStations[12];
    int radioId, isDriving, targetScrollSlot, loopIndex, stationCount;
    float screenWidth, screenHeight, firstOffset, secondOffset, centerX, centerY, scrollDiff, interpStep, relPos, drawX, drawY, drawAlpha;


    vehicle = FindPlayerVehicle();
    player = FindPlayerPed();
    radioId = vehicle ? vehicle->m_nRadioStation : 0;
    isDriving = player ? (player->m_ePedState == PEDSTATE_DRIVING) : 0;

    if (!isDriving) {
        return;
    }

    stationCount = GetActiveStations(activeStations);

    // Restore music volume if a radio key is pressed and we are muted
    if (isMuted) {
        if (CPad::GetPad(0)->ChangeStationJustDown()) {
            DMAudio.SetMusicMasterVolume(savedVolume);
            isMuted = false;
        }
    }

    // Quick mute button 'X' handling
    if (InputHandler::IsKeyJustPressed('X')) {
        savedVolume = MUSIC_VOLUME_PREF;
        DMAudio.SetMusicMasterVolume(0);
        isMuted = true;
#ifdef GTA3
        DMAudio.SetRadioInCar(10);
        vehicle->m_nRadioStation = 11; // RADIO_OFF (11)
#else
        if (DMAudio.IsMP3RadioChannelAvailable()) {
            DMAudio.SetRadioInCar(10);
            vehicle->m_nRadioStation = 10;
        } else {
            DMAudio.SetRadioInCar(9);
            vehicle->m_nRadioStation = 9;
        }
#endif
    }

    // Auto-update display timeout if radio station changes
    if (radioId != lastStationId) {
        displayTimeout = CTimer::m_snTimeInMilliseconds + 4000;
        lastStationId = radioId;
    }

    // Keep scrolling animation running even if HUD is not shown to keep it in sync
    targetScrollSlot = GetScrollIndex(radioId, activeStations, stationCount);
    if (currentScrollPos < 0.0f) {
        currentScrollPos = (float)targetScrollSlot;
    }

    scrollDiff = (float)targetScrollSlot - currentScrollPos;
    while (scrollDiff < -((float)stationCount / 2.0f)) {
        scrollDiff += (float)stationCount;
    }
    while (scrollDiff > ((float)stationCount / 2.0f)) {
        scrollDiff -= (float)stationCount;
    }

    if (fabs(scrollDiff) > 0.001f) {
        interpStep = 0.15f * CTimer::ms_fTimeStep;
        if (interpStep > 1.0f) {
            interpStep = 1.0f;
        }
        currentScrollPos += scrollDiff * interpStep;
        while (currentScrollPos < 0.0f) {
            currentScrollPos += (float)stationCount;
        }
        while (currentScrollPos >= (float)stationCount) {
            currentScrollPos -= (float)stationCount;
        }
    } else {
        currentScrollPos = (float)targetScrollSlot;
    }

    // Only render the icons if we are within the display timeout
    if ((unsigned int)CTimer::m_snTimeInMilliseconds <= displayTimeout) {
        screenHeight = screen::GetScreenMaxY();
        screenWidth = screen::GetScreenMaxX();
        
        // Calculate offsets and positions
        secondOffset = screenHeight / (float)config.IconGap;
        firstOffset = secondOffset / 2.0f;

        centerX = screenWidth / 2.0f;
        centerY = screenHeight / 12.0f;

        RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);

        for (loopIndex = 0; loopIndex < stationCount; loopIndex++) {
            relPos = (float)loopIndex - currentScrollPos;
            while (relPos < -((float)stationCount / 2.0f)) {
                relPos += (float)stationCount;
            }
            while (relPos > ((float)stationCount / 2.0f)) {
                relPos -= (float)stationCount;
            }

            if (fabs(relPos) <= 2.5f) {
                drawX = centerX + relPos * firstOffset;
                drawY = centerY;

                drawAlpha = 0.0f;
                if (fabs(relPos) <= 1.0f) {
                    drawAlpha = 192.0f - fabs(relPos) * (192.0f - (float)config.Alpha1stNonActive);
                } else if (fabs(relPos) <= 2.0f) {
                    drawAlpha = (float)config.Alpha1stNonActive - (fabs(relPos) - 1.0f) * ((float)config.Alpha1stNonActive - (float)config.Alpha2ndNonActive);
                } else if (fabs(relPos) <= 2.5f) {
                    drawAlpha = (float)config.Alpha2ndNonActive - (fabs(relPos) - 2.0f) * 2.0f * (float)config.Alpha2ndNonActive;
                }

                DrawIcon(loopIndex, drawX, drawY, drawAlpha, relPos, activeStations);
            }
        }
    }
}

void Radio::ShutdownRadio() {
    int i;
    if (radioTexDict) {
#ifdef GTA3
        for (i = 0; i < 21; ++i) {
            radioIcons[i].m_pTexture = nullptr;
        }
#else
        for (i = 0; i < 23; ++i) {
            radioIcons[i].m_pTexture = nullptr;
        }
#endif
        radioTexDict = nullptr;
    }
}
