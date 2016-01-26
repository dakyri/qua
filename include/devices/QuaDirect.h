#ifndef _QUA_DIRECT
#define _QUA_DIRECT

#ifdef WIN32

#include <Dinput.h>
#include <Dinputd.h>

char	*direct_error_string(HRESULT);
void	direct_setup();
void	direct_finish();

extern LPDIRECTINPUT8	directInput;

#endif

#endif