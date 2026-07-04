#include "../includes/InputHandler.h"
#include "../includes/RadioConfig.h"
#include "../includes/ProcessHelper.h"
#include "../includes/SomeMacros.h"
#include <windows.h>

typedef SHORT(WINAPI *fnGetKeyState)(int);
static fnGetKeyState pGetKeyState = nullptr;


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