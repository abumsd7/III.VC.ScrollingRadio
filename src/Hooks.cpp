#include "../includes/Hooks.h"
#include "../includes/SomeMacros.h"
#include "../includes/Radio.h"
using namespace plugin;

void Hooks::TemplateHooksVC() {
    patch::RedirectJump(0x5F9EE0, Radio::DrawRadioIcons);
}

void Hooks::TemplateHooksIII() {
    patch::RedirectJump(0x57E6D0, Radio::DrawRadioIcons);
}

void Hooks::TemplateHooksSA() {
    patch::RedirectJump(0x507030, Radio::DrawRadioIcons);
}