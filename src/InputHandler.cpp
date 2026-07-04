#include "../includes/InputHandler.h"
#include "../includes/RadioConfig.h"
#include "../includes/ProcessHelper.h"
#include "../includes/SomeMacros.h"
#include <windows.h>

typedef SHORT(WINAPI *fnGetKeyState)(int);
static fnGetKeyState pGetKeyState = nullptr;

const char *controlKeysStrings[62] = {
    "ESC",    "F1",      "F2",      "F3",       "F4",      "F5",     "F6",      "F7",       "F8",
    "F9",     "F10",     "F11",     "F12",      "INS",     "DEL",    "HOME",    "END",      "PGUP",
    "PGDN",   "UP",      "DOWN",    "LEFT",     "RIGHT",   "DIVIDE", "TIMES",   "PLUS",     "MINUS",
    "PADDEL", "PADEND",  "PADDOWN", "PADPGDN",  "PADLEFT", "PAD5",   "NUMLOCK", "PADRIGHT", "PADHOME",
    "PADUP",  "PADPGUP", "PADINS",  "PADENTER", "SCROLL",  "PAUSE",  "BACKSP",  "TAB",      "CAPSLK",
    "ENTER",  "LSHIFT",  "RSHIFT",  "SHIFT",    "LCTRL",   "RCTRL",  "LALT",    "RALT",     "LWIN",
    "RWIN",   "APPS",    "NULL",    "LMB",      "MMB",     "RMB",    "MWHU",    "MWHD",
};

static bool MyKeyPressed(unsigned int keyCode) {
    if (!pGetKeyState) {
        pGetKeyState = (fnGetKeyState)ProcessHelper::GetFunc("user32.dll", "GetKeyState");
    }
    if (pGetKeyState) {
        return (pGetKeyState(keyCode) & 0x8000) != 0;
    }
    return false;
}

bool InputHandler::oldKeyState[256] = {false};

__declspec(noinline) bool InputHandler::IsKeyJustPressed(unsigned int key) {
    bool current, pressed;

    if (key >= 256) {
        return false;
    }
    current = MyKeyPressed(key);
    pressed = current && !oldKeyState[key];
    oldKeyState[key] = current;
    return pressed;
}

void InputHandler::UpdateOldKeyState() {
    int i, key;
    int keysToPoll[1];

    keysToPoll[0] = config.ReloadKey;
    for (i = 0; i < 1; i++) {
        key = keysToPoll[i];
        if (key > 0 && key < 256) {
            if (!MyKeyPressed(key)) {
                oldKeyState[key] = false;
            }
        }
    }
}

int InputHandler::StringToKey(const char *str) {
    int key, i;

    key = str[0];
    if (str[0] && str[1] != '\0') {
        if (!ProcessHelper::faststrcmp(str, "SPACEBAR", 0)) {
            return rsSPACE;
        }

        for (i = 0; i < ARRAY_SIZE(controlKeysStrings); i++) {
            if (!ProcessHelper::faststrcmp(controlKeysStrings[i], str, 0)) {
                if (i > (rsNULL - rsESC)) {
                    key = (i - (rsNULL - rsESC)) + MOUSE_CUSTOM_OFFSET;
                } else {
                    key = i + rsESC;
                }
            }
        }
    }

    return key;
}

const char *InputHandler::KeyToString(int key) {
    char c;
    static char s[2];

    if (key >= rsESC && key < MOUSE_CUSTOM_OFFSET) {
        return controlKeysStrings[key - rsESC];
    } else if (key > MOUSE_CUSTOM_OFFSET) {
        return controlKeysStrings[(key + rsNULL) - (MOUSE_CUSTOM_OFFSET + rsESC)];
    } else {
        c = (char)key;
        s[0] = c;
        s[1] = '\0';

        if (c == ' ') {
            return "SPACEBAR";
        }

        return s;
    }
}
