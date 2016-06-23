#include "qua_version.h"

#include "QuaPort.h"
#include "Sym.h"
#include "QuaAudio.h"
#include "Block.h"
#include "Qua.h"
#include "QuaEnvironment.h"

#if defined(QUA_V_ARRANGER_INTERFACE)
#include "QuaDisplay.h"
#endif

QuaPort::QuaPort(
			std::string nm, short typ, short subt, short md):
	Stackable(
		DefineSymbol(nm, TypedValue::S_PORT, 0,
			this, nullptr, TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT)),
	Insertable()
{
	deviceType = typ;
	deviceSubType = subt;
	mode = md;
//	interfaceBridge.SetSymbol(sym);
//	insertableBridge.SetSymbol(sym);
	environment.display.CreatePortBridge(this);
}

char *
QuaPort::name(uchar)
{
	return sym->name;
}

bool
QuaPort::isMultiSchedulable()
{
	if (deviceType != QUA_DEV_AUDIO)
		return false;
	QuaAudioPort	*p = (QuaAudioPort *)this;
	return false;
}


status_t
QuaPort::save(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent);

	fprintf(fp,	"port");
	
//	if (appFilePathName)
//		fprintf(fp, "\n\t#path \"%s\"", appFilePathName);
//	if (appMimeType)
//		fprintf(fp, "\n\t#mime \"%s\"", appMimeType);
		
	fprintf(fp,	" %s", sym->PrintableName());
	if (countControllers() > 0) {
		fprintf(fp, "(");
		err = saveControllers(fp, indent+2);
		fprintf(fp, ")");
	}
//	if (representation) {
//		BPoint	atPoint = representation->atPoint;
//		fprintf(fp,	" #display {%s, {%g %g}}",
//			ColorStr(representation->color), atPoint.x, atPoint.y);
//	}
//	if (clonedControlMethod) {
//		fprintf(fp,	" #control %s", clonedControlMethod->name);
//	}
//	SaveMainBlock(mainBlock, fp, indent, sym->children, true); 
	return err;
}

