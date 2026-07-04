#include "../includes/Hooks.h"
#include "../includes/SomeMacros.h"
using namespace plugin;

void Hooks::TemplateHooksVC() {
    patch::PutRetn(0x5F9EE0); // radio nop
}

void Hooks::TemplateHooksIII() {
    patch::Nop(0x48E400, 5); // radio nop
}
