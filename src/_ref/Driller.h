#ifndef DrillerAPPH
#define DrillerAPPH
#include <eikapp.h>
#include <e32base.h>

#include <sdlapp.h>

class CDrillerApp: public CSDLApp {
public:
	CDrillerApp();
   	~CDrillerApp();
	
#ifdef UIQ3
	/**
	 * Returns the resource id to be used to declare the views supported by this UIQ3 app
 	 * @return TInt, resource id
   	*/
	TInt ViewResourceId();
#endif
	
	TUid AppDllUid() const;

};

#endif


