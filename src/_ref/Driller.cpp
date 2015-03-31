
#include "Driller.h"
#include <string.h>
#include <eikenv.h>
#include <MissDriller.rsg>

CApaApplication* NewApplication() {
	return new CDrillerApp;
}

#include <eikstart.h>
// E32Main() contains the program's start up code, the entry point for an EXE.
GLDEF_C TInt E32Main() {
 	return EikStart::RunApplication(NewApplication);
}


CDrillerApp::CDrillerApp() {
}

CDrillerApp::~CDrillerApp() {
}
#ifdef UIQ3
/**
 * Returns the resource id to be used to declare the views supported by this UIQ3 app
 * @return TInt, resource id
 */
TInt CDrillerApp::ViewResourceId() {
	return R_SDL_VIEW_UI_CONFIGURATIONS;
}
#endif

/**
 *   Responsible for returning the unique UID of this application
 * @return unique UID for this application in a TUid
 **/
TUid CDrillerApp::AppDllUid() const {
	return TUid::Uid(0xA00007B9);
}

/////////////////////////////////////////////////////////////////////////////////////////////////



