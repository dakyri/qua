#include "qua_version.h"

#include "tinyxml2.h"

#include "Destination.h"
#include "Instance.h"
#include "QuaMidi.h"
#include "QuaPort.h"
#include "Channel.h"
#include "Qua.h"
#include "Schedulable.h"
#include "Sample.h"
#include "Pool.h"
#include "Sampler.h"
#include "QuaAudio.h"
#include "Block.h"

#ifdef QUA_V_JOYSTICK
#include "QuaJoystick.h"
#endif

#ifdef QUA_V_ARRANGER_INTERFACE
#include "Colors.h"
#if defined(WIN32)
#include "QuaDisplay.h"
#endif
#endif

Place::Place(StabEnt *sy,
			 Channel *c,
			 QuaPort *dev,
			 port_chan_id devch,
			 bool en)
{
	channel = c;
	device = dev;
	deviceChannel = devch;
	sym = sy;
	if (device) {
		switch (device->deviceType) {
		case QUA_DEV_JOYSTICK:
			break;
		case QUA_DEV_MIDI:
			break;
		case QUA_DEV_AUDIO:
			break;
		case QUA_DEV_NOT:
			break;
		}
	}
	
	DefineSymbol("enabled", TypedValue::S_BOOL, 0,
					&enabled, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_NOT);
	mode = FLOW_NORMAL;
	select = 0;
	enabled = en;
//	needed_channels = 2;
	isinit = true;		// initialised and empty
	initBlock = nullptr;
	

//	SetViewColor(mdGray);
//	ResizeTo(3+StringWidth(Name())+3+12, 11);
	xDevice = nullptr;
	xChannel = 0;
}


Input::Input(std::string nm,
			 Channel *c,
			 QuaPort *dev,
			 port_chan_id devch,
			 bool en):
	Place(
		DefineSymbol(nm, TypedValue::S_INPUT, 0,
					this, c->sym,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
		c, dev, devch, en)
{
	gain = 1;
	pan = 0;
	src.audio.port = src.audio.xport = nullptr;
}	

Output::Output(std::string nm,
			 Channel *c,
			 QuaPort *dev,
			 port_chan_id devch,
			 bool en):
	Place(
		DefineSymbol(nm, TypedValue::S_OUTPUT, 0,
					this, c->sym,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
		c, dev, devch, en)
{
	gain = 1;
	pan = 0;
	dst.audio.port = dst.audio.xport = nullptr;
}

void
Input::SaveSnapshot(FILE *fp, Channel *chan)
{
	fprintf(fp, "<input name=\"%s\">\n", sym->name);
	StabEnt	*p = sym->children;
	while (p != nullptr) {
		p->SaveSimpleTypeSnapshot(fp, chan, nullptr);
		p = p->sibling;
	}
	fprintf(fp, "</input>\n");
}


status_t
Input::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *valAttrVal = element->Attribute("value");
	const char *positionVal = element->Attribute("position");
	const char *nameAttrVal = element->Attribute("name");
	const char *typeAttrVal = element->Attribute("type");
	const char *encodingAttrVal = element->Attribute("encoding");

	std::string valAttr;
	std::string	nameAttr;
	std::string	typeAttr;

	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTypeAttr = false;

	int		encoding = 0;
	int		position = 0;

	if (valAttrVal != nullptr) {
		valAttr = valAttrVal;
	}
	if (positionVal != nullptr) {
		position = atoi(valAttrVal);
	}
	if (nameAttrVal != nullptr) {
		nameAttr = nameAttrVal;
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		typeAttr = typeAttrVal;
		hasTypeAttr = true;
	}
	if (encodingAttrVal != nullptr) {
		if (std::string(encodingAttrVal) == "base64") {
			encoding = 1;
		}
	}

	std::string namestr = element->Value();

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "input") {
		LoadSnapshotChildren(element);
	} else if ((namestr == "fixed") || namestr == "envelope") {
		if (hasNameAttr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, nullptr, nullptr, nullptr);	
			}
		}
	} else {
	}

	return B_OK;
}

status_t
Input::LoadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (LoadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
		childElement = childElement->NextSiblingElement();
	}
	return B_OK;
}


void
Output::SaveSnapshot(FILE *fp, Channel *chan)
{
	fprintf(fp, "<output name=\"%s\">\n", sym->name);
	StabEnt	*p = sym->children;
	while (p != nullptr) {
		p->SaveSimpleTypeSnapshot(fp, chan, nullptr);
		p = p->sibling;
	}
	fprintf(fp, "</output>\n");
}


status_t
Output::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *valAttrVal = element->Attribute("value");
	const char *positionVal = element->Attribute("position");
	const char *nameAttrVal = element->Attribute("name");
	const char *typeAttrVal = element->Attribute("type");
	const char *encodingAttrVal = element->Attribute("encoding");

	std::string valAttr;
	std::string	nameAttr;
	std::string	typeAttr;

	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTypeAttr = false;

	int		encoding = 0;
	int		position = 0;

	if (valAttrVal != nullptr) {
		valAttr = valAttrVal;
	}
	if (positionVal != nullptr) {
		position = atoi(valAttrVal);
	}
	if (nameAttrVal != nullptr) {
		nameAttr = nameAttrVal;
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		typeAttr = typeAttrVal;
		hasTypeAttr = true;
	}
	if (encodingAttrVal != nullptr) {
		if (std::string(encodingAttrVal) == "base64") {
			encoding = 1;
		}
	}

	std::string namestr = element->Value();

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "output") {
		LoadSnapshotChildren(element);
	} else if ((namestr == "fixed") || namestr == "envelope") {
		if (hasNameAttr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, nullptr, nullptr, nullptr);	
			}
		}
	} else {
	}

	return B_OK;
}

status_t
Output::LoadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (LoadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
		childElement = childElement->NextSiblingElement();
	}
	return B_OK;
}


bool
Place::setPortInfo(QuaPort *port, port_chan_id chan, short ind)
{
	if (ind > 1) {
		internalError("OOpps. Only 1 extra port is allowed per destination");
		return false;
	}
	bool	wasEn = false;
	if (enabled) {
		wasEn = true;
		SetEnabled(0);
	}

	if (ind == 0) {
		device = port;
		deviceChannel = chan;
	} else {
		if (port == nullptr) {
			xDevice = device;
		} else {
			xDevice = port;
		}
		xChannel = chan;
	}
	if (wasEn) {
		SetEnabled(1);
	}
	return true;
}


Place::~Place()
{
	if (initBlock) {
		initBlock->DeleteAll();
		initBlock = nullptr;
	}
	SetEnabled(false);
}


status_t
Place::Init()
{
	if (initBlock) {
		;
	}
	return B_ERROR;
}


status_t
Place::SetEnabled(uchar)
{
	return B_ERROR;
}

status_t
Output::SetEnabled(uchar en)
{
	enabled = 2;
//	DrawEnableButton();
//	Sync();
// todo maybe inform the display?
	return B_OK;
}


status_t
Input::SetEnabled(uchar en)
{
	if (enabled < 2) {
		enabled = 2;
//		DrawEnableButton();
//		Sync();
	}
	return B_OK;
}
	

void merge(char *d, char *s, char *t);
void merge(char *d, char *s, char *t)
{
	if (s != nullptr) {
		char *ss = s;
		while (*ss) {
			*d++ = *ss++;
		}
		if (t != nullptr) {
			*d++ = ',';
		}
	}
	if (t != nullptr) {
		while (*s && *t && *t == *s) {
			t++;
			s++;
		}
		while (*t) {
			*d++ = *t++;
		}
	}
	*d = 0;
}

char *
Place::Name(uchar dfmt, uchar cfmt)
{
	static char buf[512];
	
	if (device == nullptr) {
		return "null device";
	}

	if (dfmt == NMFMT_NONE && cfmt == NMFMT_NONE) {
		return
			(device->deviceType == QUA_DEV_AUDIO)? "audio":
			(device->deviceType == QUA_DEV_MIDI)? "midi":
			(device->deviceType == QUA_DEV_JOYSTICK)? "joy":"device";
	}

	if (device->deviceType == QUA_DEV_MIDI) {
		if (cfmt == NMFMT_NONE) {
			return device->name(dfmt);
		} else {
			if (deviceChannel == -1)
				sprintf(buf, "%s:*", device->name(dfmt));
			else if (deviceChannel == 0)
				sprintf(buf, "%s:!", device->name(dfmt));
			else
				sprintf(buf, "%s:%d", device->name(dfmt), deviceChannel);
		}
	} else if (device->deviceType == QUA_DEV_AUDIO) {
		if (xDevice) {
			char buf2[512];
			char buf3[512];
			sprintf(buf2, "%s:%d", device->name(dfmt), deviceChannel);
			sprintf(buf3, "%s:%d", xDevice->name(dfmt), xChannel);
			merge(buf, buf2, buf3);
		} else {
			sprintf(buf, "%s:%d", device->name(dfmt), deviceChannel);
		}
	} else if (device->deviceType == QUA_DEV_JOYSTICK) {
		return device->name(dfmt);
	}

	return buf;
}


char *
Input::Name(uchar dfmt, uchar cfmt)
{
	static char buf[512];
	
	if (device == nullptr)
		return "spare in";

	return Place::Name(dfmt, cfmt);
}

char *
Output::Name(uchar dfmt, uchar cfmt)
{
	static char buf[512];
	
	if (device == nullptr)
		return "spare out";

	return Place::Name(dfmt, cfmt);
}


bool
Place::ValidDevice(QuaPort *dev)
{
	if (dev == nullptr)
		return false;
	if (device == nullptr)
		return true;
	if (device->deviceType == QUA_DEV_AUDIO)
		return false;
	if (dev->deviceType == QUA_DEV_AUDIO)
		return false;
	return true;
}

bool
Output::ValidDevice(QuaPort *dev)
{
	if (!Place::ValidDevice(dev))
		return false;
	if (dev->deviceType == QUA_DEV_JOYSTICK) {
		return false;
	}
	return true;
}

bool
Input::ValidDevice(QuaPort *dev)
{
	if (!Place::ValidDevice(dev))
		return false;
	return true;
}


bool
Output::OutputStream(Time &t, Stream *stream)
{
	if (device == nullptr)
		return false;
	switch (device->deviceType) {
	case QUA_DEV_MIDI: {
		dst.midi->OutputStream(t, stream, deviceChannel);
		break;
	}
	
#ifdef QUA_V_JOYSTICK
	case QUA_DEV_JOYSTICK:		// not quite sure how this looks. 07/05
		dst.joy->OutputStream(t, stream, deviceChannel);
		break;
#endif
	default: {
		internalError("Unim chan");
		return false;
	}}
	return true;
}

bool
Input::HasStreamItems()
{
	return	device == nullptr? false:
			device->deviceType == QUA_DEV_MIDI?
				src.midi->HasStreamItems():
#ifdef QUA_V_JOYSTICK
			device->deviceType == QUA_DEV_JOYSTICK?
				((QuaJoystickPort *)device)->HasStreamItems():
#endif
			false;
}

bool
Output::ClearStream(Stream *outStream)
{
	if (device == nullptr)
		return false;
	switch (device->deviceType) {
	case QUA_DEV_MIDI: {
		dst.midi->ClearStream(outStream, deviceChannel);
		break;
	}
	
	case QUA_DEV_JOYSTICK:		// v clever joy
		break;
	
	default: {
		internalError("Unim chan");
		return false;
	}}
	return true;
}

bool
Input::GetStreamItems(Stream *recvStream)
{
	if (device == nullptr)
		return false;
	switch (device->deviceType) {
	case QUA_DEV_MIDI: {
		src.midi->GetStreamItems(deviceChannel, recvStream);
		break;
	}
#ifdef QUA_V_JOYSTICK
	case QUA_DEV_JOYSTICK: {
		src.joy->GetStreamItems(recvStream);
		break;
	}
#endif
	}
	return true;
}

bool
Input::Save(FILE *fp, short indent, short i)
{
	tab(fp, indent);
	fprintf(fp, "input");
	if (device) {
		switch (device->deviceType) {
			case QUA_DEV_AUDIO: {
				fprintf(fp, " \\audio %s %d", device->sym->name, deviceChannel);
				if (xDevice != nullptr) {
					if (xDevice != device) {
						fprintf(fp, " \\audio %s %d", xDevice->sym->name, xChannel);
					} else {
						fprintf(fp, " %d", xChannel);
					}
				}
				break;
			}
			case QUA_DEV_JOYSTICK: {
				fprintf(fp, " \\joystick %s", device->sym->name);
				break;
			}

			case QUA_DEV_MIDI: {
				fprintf(fp, " \\midi %s %d", device->sym->name, deviceChannel);
				break;
			}
			case QUA_DEV_PARALLEL: {
				fprintf(fp, " \\parallel %s", device->sym->name);
				break;
			}
		}
	}
	fprintf(fp, " %s", sym->name);
#ifndef QUA_V_SAVE_INITASXML
	if (sym->children) {
		fprintf(fp, " {\n");
		sym->children->SaveInitialAssigns(fp, indent+1, nullptr, nullptr);
		tab(fp, indent);fprintf(fp, "}");
	}
#endif
	fprintf(fp, "\n");
	return true;
}

bool
Output::Save(FILE *fp, short indent, short i)
{
	tab(fp, indent);
	fprintf(fp, "output");
	if (device) {
		switch (device->deviceType) {
			case QUA_DEV_AUDIO: {
				fprintf(fp, " \\audio %s %d", device->sym->name, deviceChannel);
				if (xDevice != nullptr) {
					if (xDevice != device) {
						fprintf(fp, " \\audio %s %d", xDevice->sym->name, xChannel);
					} else {
						fprintf(fp, " %d", xChannel);
					}
				}
				break;
			}
			case QUA_DEV_JOYSTICK: {
				fprintf(fp, " \\joystick %s", device->sym->name);
				break;
			}
			case QUA_DEV_MIDI: {
				fprintf(fp, " \\midi %s %d", device->sym->name, deviceChannel);
				break;
			}
			case QUA_DEV_PARALLEL: {
				fprintf(fp, " \\parallel %s", device->sym->name);
				break;
			}
		}
	}
	fprintf(fp, " %s", sym->name);
#ifndef QUA_V_SAVE_INITASXML
	if (sym->children) {
		fprintf(fp, " {\n");
		sym->children->SaveInitialAssigns(fp, indent+1, nullptr, nullptr);
		tab(fp, indent);fprintf(fp, "}");
	}
#endif
	fprintf(fp, "\n");
	return true;
}

	