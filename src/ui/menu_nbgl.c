
#ifdef HAVE_NBGL

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"

#include "../globals.h"
#include "menu.h"

void app_quit(void) {
    // exit app here
    os_sched_exit(-1);
}

void ui_menu_main(void) {
    nbgl_useCaseHome(APPNAME, &C_stax_app_boilerplate_64px, NULL, false, ui_menu_about, app_quit);
}

// 'About' menu

static const char* const INFO_TYPES[] = {"Version", "Developer"};
static const char* const INFO_CONTENTS[] = {APPVERSION, "Ledger"};

static bool nav_callback(uint8_t page, nbgl_pageContent_t* content) {
    UNUSED(page);
    content->type = INFOS_LIST;
    content->infosList.nbInfos = 2;
    content->infosList.infoTypes = (const char**) INFO_TYPES;
    content->infosList.infoContents = (const char**) INFO_CONTENTS;
    return true;
}

void ui_menu_about() {
    nbgl_useCaseSettings(APPNAME, 0, 1, false, ui_menu_main, nav_callback, NULL);
}

#endif
