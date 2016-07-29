#include "qua_version.h"

#include "QuaPort.h"
#include "Sym.h"
#include "QuaAudio.h"
#include "Block.h"
#include "Qua.h"
#include "QuaEnvironment.h"
#include "QuaDisplay.h"

#include <iostream>

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

const char *
QuaPort::name(uchar)
{
	return sym->name.c_str();
}

bool
QuaPort::isMultiSchedulable()
{
	if (deviceType != QuaPort::Device::AUDIO)
		return false;
	QuaAudioPort	*p = (QuaAudioPort *)this;
	return false;
}

bool
QuaPort::hasMode(const int m) {
	if (m == QUA_PORT_UNKNOWN)  return true; 
	if (m == QUA_PORT_IO) return mode == m;
	return (mode & m) != 0;
}

status_t
QuaPort::save(ostream &out, short indent)
{
	status_t	err=B_NO_ERROR;
	out << tab(indent) << "port";
	
//	if (appFilePathName)
//		fprintf(fp, "\n\t#path \"%s\"", appFilePathName);
//	if (appMimeType)
//		fprintf(fp, "\n\t#mime \"%s\"", appMimeType);
		
	out << " " << sym->printableName();
	if (countControllers() > 0) {
		out << "(";
		err = saveControllers(out, indent+2);
		out << ")";
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

