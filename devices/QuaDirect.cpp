#include "qua_version.h"

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Qua.h"
#include "QuaDirect.h"

#include <initguid.h>

#include <iostream>

LPDIRECTINPUT8	directInput=nullptr;

char *
direct_error_string(HRESULT res)
{
	switch(res) {
	case DI_OK:							return "Okey dokey";
	case DIERR_BETADIRECTINPUTVERSION:	return "Beta tester";
	case DIERR_INVALIDPARAM:			return "Invalid param";
	case DIERR_OLDDIRECTINPUTVERSION:	return "Old version";
	case DIERR_OUTOFMEMORY:				return "Out of memory";  
	case DIERR_DEVICENOTREG:			return "Device not registered with DI";
	case DIERR_NOINTERFACE:				return "No interface";  
	case DIERR_NOTINITIALIZED: 			return "Not initialised"; 
	case DIERR_ACQUIRED:	 			return "Operation cannot be performed while aquired";   
	case DIERR_OBJECTNOTFOUND:	 		return "Object not found";
	case DIERR_UNSUPPORTED:	 			return "Operation not supported";   
	}
	return "Unknown error";
}


void
direct_setup(void *instance)
{
	HINSTANCE quaAppInstance = (HINSTANCE) instance;
#if defined(QUA_V_JOYSTICK_DX)
	if (directInput == nullptr) {
		HRESULT err = DirectInput8Create(
			quaAppInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,	// REFIID riidltf,
			(LPVOID*)&directInput,
			nullptr				// LPUNKNOWN punkOuter
		);
		if (err != DI_OK) {
			cout << "Error " << direct_error_string(err) << endl;
			directInput = nullptr;	// just in case
		} else {
			cout << "Got direct input ok" << endl;
		}
	} else {
		cout << "Already got direct input, maybe" << endl;
	}
#endif
}


void
direct_finish()
{
	if (directInput) {
		directInput->Release();
		directInput = nullptr;
	}
}

#endif
