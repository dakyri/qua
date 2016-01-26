#ifndef _QUADDON
#define _QUADDON

#include "AudioAddon.h"
#include "QuasiStack.h"
#include "BaseVal.h"

class Qua;

class Quaddon: public Stackable
{
public:
						Quaddon(char *path, image_id, int);
						~Quaddon();

	void				*Init(short nch);
	void				Disinit(void *);
	size_t				Apply(void *cookie, float *Sig, long nFrames, short nChan);
	bool				SetParameters(void *cookie, float p[], int np);
	void				Reset(void *cookie);
						
	image_id			srcImage;
	int					srcIndex;
	uint32				subType;
	char				*controlVariableDef;
	
	InitFnPtr			init;
	DisInitFnPtr		disInit;
	ApplyFnPtr			apply;
	SetParamFnPtr		setParameters;
	ResetFnPtr			reset;

	TypedValue			cookieVar;

	Quaddon				*next;
};

#endif