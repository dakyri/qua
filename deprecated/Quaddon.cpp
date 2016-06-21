#include "Quaddon.h"
#include "Sym.h"
#include "Parse.h"


char	*AddonName(image_id, int n);
char *
AddonName(image_id id, int n)
{
	static char	name[MAX_QUA_NAME_LENGTH];
	status_t	err;
	
	char		**addon_tag_p;
	if ((err=get_image_symbol(
			id, "addon_name", B_SYMBOL_TYPE_DATA, (void**)&addon_tag_p)) >= B_NO_ERROR) {
		strcpy(name, *addon_tag_p);
	} else if ((err=get_image_symbol(
			id, "addon_name_array", B_SYMBOL_TYPE_DATA, (void**)&addon_tag_p)) >= B_NO_ERROR) {
		strcpy(name, addon_tag_p[n]);
	} else {
		strcpy(name, "boring");
	}
	return name;
}

Quaddon::Quaddon(char *path, image_id id, int n):
	Stackable(DefineSymbol(AddonName(id,n), S_QUADDON, 0,
					this, nullptr,
					REF_VALUE, ATTRIB_NONE, false, DISP_MODE_NOT))
{
	status_t	err;
	
	srcImage = id;
	srcIndex = n;
	
	fprintf(stderr, "addon %s, image %d, hook %d: name %s\n", path, id, n, sym->name);
	
	uint32	*addon_id_p;
	if ((err=get_image_symbol(
			id, "addon_id", B_SYMBOL_TYPE_DATA, (void**)&addon_id_p)) >= B_NO_ERROR) {
		subType = *addon_id_p;
	} else if ((err=get_image_symbol(
			id, "addon_id_array", B_SYMBOL_TYPE_DATA, (void**)&addon_id_p)) >= B_NO_ERROR) {
		subType = addon_id_p[n];
	} else {
		subType = 0;
	}
	
	char		**addon_formal_p;
	if ((err=get_image_symbol(
			id, "addon_params", B_SYMBOL_TYPE_DATA, (void **)&addon_formal_p)) >= B_NO_ERROR) {
		controlVariableDef = new char[strlen(*addon_formal_p)+1];
		strcpy(controlVariableDef, *addon_formal_p);
	} else if ((err=get_image_symbol(
			id, "addon_params_array", B_SYMBOL_TYPE_DATA, (void**)&addon_formal_p)) >= B_NO_ERROR) {
		controlVariableDef = new char[strlen(addon_formal_p[n])+1];
		strcpy(controlVariableDef, addon_formal_p[n]);
	} else {
		controlVariableDef = new char[strlen("()")+1];
		strcpy(controlVariableDef, "()");
	}

	InitFnPtr	*addon_init_array_p;
	if ((err=get_image_symbol(
			id, "init", B_SYMBOL_TYPE_TEXT, (void**)&init)) >= B_NO_ERROR) {
		;
	} else if ((err=get_image_symbol(
			id, "init_array", B_SYMBOL_TYPE_DATA, (void**)&addon_init_array_p)) >= B_NO_ERROR) {
		init = addon_init_array_p[n];
	} else {
		init = nullptr;
	}
	
	DisInitFnPtr	*addon_disinit_array_p;
	if ((err=get_image_symbol(
			id, "disinit", B_SYMBOL_TYPE_TEXT, (void**)&disInit)) >= B_NO_ERROR) {
		;
	} else if ((err=get_image_symbol(
			id, "disinit_array", B_SYMBOL_TYPE_DATA, (void**)&addon_disinit_array_p)) >= B_NO_ERROR) {
		disInit = addon_disinit_array_p[n];
	} else {
		disInit = nullptr;
	}
	
	ApplyFnPtr	*addon_apply_array_p;
	if ((err=get_image_symbol(
			id, "apply", B_SYMBOL_TYPE_TEXT, (void **)&apply)) >= B_NO_ERROR) {
		;
	} else if ((err=get_image_symbol(
			id, "apply_array", B_SYMBOL_TYPE_DATA, (void**)&addon_apply_array_p)) >= B_NO_ERROR) {
		apply = addon_apply_array_p[n];
	} else {
		apply = nullptr;
	}
	
	ResetFnPtr	*addon_reset_array_p;
	if ((err=get_image_symbol(
			id, "reset", B_SYMBOL_TYPE_TEXT, (void**)&reset)) >= B_NO_ERROR) {
		;
	} else if ((err=get_image_symbol(
			id, "reset_array", B_SYMBOL_TYPE_DATA, (void**)&addon_reset_array_p)) >= B_NO_ERROR) {
		reset = addon_reset_array_p[n];
	} else {
		reset = nullptr;
	}
	
	SetParamFnPtr	*addon_setp_array_p;
	if ((err=get_image_symbol(
			id, "set_parameters", B_SYMBOL_TYPE_TEXT, (void**)&setParameters)) >= B_NO_ERROR) {
		;
	} else if ((err=get_image_symbol(
			id, "set_parameters_array", B_SYMBOL_TYPE_DATA, (void**)&addon_setp_array_p)) >= B_NO_ERROR) {
		setParameters = addon_setp_array_p[n];
	} else {
		setParameters = nullptr;
	}
	
	// allocate a pointer on the stack for the cookie...
	cookieVar.Set(S_STRANGE_POINTER, REF_STACK, sym,
			(long)AllocStack(sym, S_STRANGE_POINTER, 1));

	TxtParser	*p = new TxtParser(controlVariableDef, path, nullptr);
	p->GetToken();
	p->ParseFormalsList(sym);
	next = nullptr;
}

Quaddon::~Quaddon()
{
	delete [] controlVariableDef;
}

void
Quaddon::Reset(void *cookie)
{
	if (reset)
		reset(cookie);
}

size_t
Quaddon::Apply(void *cookie, float *Sig, long nFrames, short nChan)
{
	if (apply)
		return apply(cookie, Sig, nFrames, nChan);
	else
		return nFrames;
}

bool
Quaddon::SetParameters(void *cookie, float p[], int np)
{
//	fprintf(stderr, "set par %s %g %g %g\n", name, p[0], p[1], p[2]);
	if (setParameters)
		return setParameters(cookie, p, np);
	else
		return TRUE;
}

void *
Quaddon::Init(short nc)
{
	if (init)
		return init(nc);
	return nullptr;
}

void
Quaddon::Disinit(void *c)
{
	if (disInit)
		disInit(c);
}