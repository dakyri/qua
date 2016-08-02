#include "qua_version.h"

#include "tinyxml2.h"

#include "Destination.h"
#include "Instance.h"
#include "QuaMidi.h"
#include "QuaOSC.h"
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
#include "QuaDisplay.h"

#include <iostream>

string
PortSpec::toString() {
	string ret = portDeviceTypeName(type)+"/"+ name;
	if (chanIds.size()) ret += ":" + to_string(chanIds[0]);
	return ret;
}



Place::Place(StabEnt *sy, Channel *c, const PortSpec &ps, int direction, bool  en)
	: sym(sy), channel(c), portSpec(ps)
{
	int m = qut::maxel(portSpec.chanIds);
	QuaPort *port = findQuaPort(portSpec.type, portSpec.name, direction, m > 0 ? m + 1 : -1);
	cerr << "Place::Place " + sy->name + " dirn " + portDirectionName(direction) + "portspec " + portSpec.toString()
			+ " found port " + (port != nullptr ? port->name(NMFMT_NAME) : "null") << endl;
	device = port;
	deviceChannel = portSpec.chanIds.size() > 0? portSpec.chanIds[0]: -1;
	for (short i = 1; i<portSpec.chanIds.size(); i++) {
		setPortInfo(port, portSpec.chanIds[i], i);
	}

	if (device) {
		switch (device->deviceType) {
		case QuaPort::Device::JOYSTICK:
			break;
		case QuaPort::Device::MIDI:
			break;
		case QuaPort::Device::AUDIO:
			break;
		case QuaPort::Device::NOT:
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

/*
 * specifies it's destination in a more general way
 */
Input::Input(const std::string & nm, Channel *c, const PortSpec &ps, bool en)
	: Place( DefineSymbol(nm, TypedValue::S_INPUT, 0,
			this, c->sym, TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
		c, ps, QUA_PORT_IN, en)
{
	gain = 1;
	pan = 0;
	src.audio.port = src.audio.xport = nullptr;
}


/* more general version
 */
Output::Output(const std::string & nm,
		Channel *c, const PortSpec &ps, const bool en) :
	Place( DefineSymbol(nm, TypedValue::S_OUTPUT, 0,
			this, c->sym, TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
		c, ps, QUA_PORT_OUT, en)
{
	gain = 1;
	pan = 0;
	dst.audio.port = dst.audio.xport = nullptr;
}
void
Input::SaveSnapshot(ostream &out, Channel *chan)
{
	out << "<input name=\""<< sym->name <<"\">" << endl;
	StabEnt	*p = sym->children;
	while (p != nullptr) {
		p->SaveSimpleTypeSnapshot(out, chan, nullptr);
		p = p->sibling;
	}
	out << "</input>" << endl;
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
Output::SaveSnapshot(ostream &out, Channel *chan)
{
	out <<"<output name=\""<< sym->name <<"\">" <<endl;
	StabEnt	*p = sym->children;
	while (p != nullptr) {
		p->SaveSimpleTypeSnapshot(out, chan, nullptr);
		p = p->sibling;
	}
	out <<"</output>" << endl;
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
	

char *
Place::Name(uchar dfmt, uchar cfmt)
{
	static string buf;
	
	if (device == nullptr) {
		return "null device";
	}

	if (dfmt == NMFMT_NONE && cfmt == NMFMT_NONE) {
		return
			(device->deviceType == QuaPort::Device::AUDIO)? "audio":
			(device->deviceType == QuaPort::Device::MIDI)? "midi":
			(device->deviceType == QuaPort::Device::JOYSTICK)? "joy":"device";
	}

	if (device->deviceType == QuaPort::Device::MIDI) {
		buf = device->name(dfmt);
		if (cfmt == NMFMT_NONE) {
			return (char *)device->name(dfmt);
		} else {
			if (deviceChannel == -1)
				buf += ":*";
			else if (deviceChannel == 0)
				buf += ":!";
			else
				buf += ":"+to_string(deviceChannel);
		}
	} else if (device->deviceType == QuaPort::Device::AUDIO) {
		if (xDevice) {
			buf += ":" + to_string(deviceChannel);
			buf += "," + string(xDevice->name(dfmt)) + ":" + to_string(xChannel);
		} else {
			buf += ":"+to_string(deviceChannel);
		}
	} else if (device->deviceType == QuaPort::Device::JOYSTICK) {
	}

	return const_cast<char*>(buf.c_str());
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
	if (device->deviceType == QuaPort::Device::AUDIO)
		return false;
	if (dev->deviceType == QuaPort::Device::AUDIO)
		return false;
	return true;
}

bool
Output::ValidDevice(QuaPort *dev)
{
	if (!Place::ValidDevice(dev))
		return false;
	if (dev->deviceType == QuaPort::Device::JOYSTICK) {
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
	case QuaPort::Device::MIDI: {
		dst.midi->OutputStream(t, stream, deviceChannel);
		break;
	}
	
#ifdef QUA_V_JOYSTICK
	case QuaPort::Device::JOYSTICK:		// not quite sure how this looks. 07/05
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
			device->deviceType == QuaPort::Device::MIDI?
				src.midi->HasStreamItems():
#ifdef QUA_V_JOYSTICK
			device->deviceType == QuaPort::Device::JOYSTICK?
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
	case QuaPort::Device::MIDI: {
		dst.midi->ClearStream(outStream, deviceChannel);
		break;
	}
	
	case QuaPort::Device::JOYSTICK:		// v clever joy
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
	case QuaPort::Device::MIDI: {
		src.midi->GetStreamItems(deviceChannel, recvStream);
		break;
	}
#ifdef QUA_V_JOYSTICK
	case QuaPort::Device::JOYSTICK: {
		src.joy->GetStreamItems(recvStream);
		break;
	}
#endif
	}
	return true;
}

bool
Input::Save(ostream &out, short indent, short i)
{
	out << tab(indent) << "input";
	if (device) {
		switch (device->deviceType) {
			case QuaPort::Device::AUDIO: {
				out << " \\audio " << device->sym->name << " " << deviceChannel;
				if (xDevice != nullptr) {
					if (xDevice != device) {
						out << " \\audio " << xDevice->sym->name << xChannel;
					} else {
						out << " " << xChannel;
					}
				}
				break;
			}
			case QuaPort::Device::JOYSTICK: {
				out << " \\joystick " << device->sym->name;
				break;
			}

			case QuaPort::Device::MIDI: {
				out << " \\midi " << device->sym->name << " " << deviceChannel;
				break;
			}
			case QuaPort::Device::PARALLEL: {
				out << " \\parallel " << device->sym->name;
				break;
			}
		}
	}
	out << " " << sym->name;
#ifndef QUA_V_SAVE_INITASXML
	if (sym->children) {
		out << " {"  << endl;
		sym->children->SaveInitialAssigns(out, indent+1, nullptr, nullptr);
		out << tab(indent) << "}";
	}
#endif
	out << endl;
	return true;
}

bool
Output::Save(ostream &out, short indent, short i)
{
	out << indent << "output";
	if (device) {
		switch (device->deviceType) {
			case QuaPort::Device::AUDIO: {
				out << " \\audio " << device->sym->name << " "<< deviceChannel;
				if (xDevice != nullptr) {
					if (xDevice != device) {
						out << " \\audio " << xDevice->sym->name << " " << xChannel;
					} else {
						out << " " << xChannel;
					}
				}
				break;
			}
			case QuaPort::Device::JOYSTICK: {
				out << " \\joystick " << device->sym->name;
				break;
			}
			case QuaPort::Device::MIDI: {
				out << " \\midi " << device->sym->name << " " << deviceChannel;
				break;
			}
			case QuaPort::Device::PARALLEL: {
				out << " \\parallel "<< device->sym->name;
				break;
			}
		}
	}
	out << " " << sym->name;
#ifndef QUA_V_SAVE_INITASXML
	if (sym->children) {
		out <<" {" << endl;
		sym->children->SaveInitialAssigns(out, indent+1, nullptr, nullptr);
		out << tab(indent) << "}";
	}
#endif
	out << endl;
	return true;
}

	