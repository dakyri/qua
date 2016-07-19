#ifndef _QUA_DIRECT
#define _QUA_DIRECT

#if defined(WIN32) || defined(_WIN32)

#include <Dinput.h>
#include <Dinputd.h>

char	*direct_error_string(HRESULT);
void	direct_setup(void *instance);
void	direct_finish();

extern LPDIRECTINPUT8	directInput;

#endif

#endif