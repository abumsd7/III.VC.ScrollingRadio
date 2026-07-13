#include "includes/Hooks.h"
#include "includes/Radio.h"
#include "includes/RadioConfig.h"
#include "includes/InputHandler.h"
#include <plugin.h>
#include "CHud.h"
#include "CCamera.h"
#include "CReplay.h"

using namespace plugin;

static void OnInitRw() {
    config.GenerateDefault();
    config.Read();
    Radio::InitRadio();
}

static void OnGameProcess() {
    InputHandler::UpdateOldKeyState();
    if (config.ReloadKey > 0 && InputHandler::IsKeyJustPressed(config.ReloadKey)) {
        config.Read();
    }
}

static void OnDrawHud() {
}

static void OnShutdownRw() {
    Radio::ShutdownRadio();
}

class Home {
  public:
    Home() {
#ifdef GTA3
        Hooks::TemplateHooksIII();
#elif defined(GTAVC)
        Hooks::TemplateHooksVC();
#else
        Hooks::TemplateHooksSA();
#endif
        Events::initRwEvent += OnInitRw;
        Events::gameProcessEvent += OnGameProcess;
        Events::drawHudEvent += OnDrawHud;
        Events::shutdownRwEvent += OnShutdownRw;
    }
} home;
