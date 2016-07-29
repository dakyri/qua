#ifndef _DESTINATION
#define _DESTINATION

namespace tinyxml2 {
	class XMLElement;
}

/*
 * modes that a Place/Destination can operate in
 *... I think these are now historical only. I think these are obvious from the structure of the QuaChannel and a few other parameters
 * or perhaps this is still a good idea? @deprecated or TODO XXXX FIXME
 */
enum {
	FLOW_NORMAL = 0,	// channel for channel
	FLOW_MONO = 1,		// force mono
	FLOW_SELECT = 2,	// select lines
	FLOW_CHAN0 = 0x10,
	FLOW_CHAN1 = 0x20,
	FLOW_CHAN2 = 0x40,
	FLOW_CHAN3 = 0x80
};

class Channel;
class QuaPort;
class Stream;
class Instance;
class QuaAudioIn;
class QuaAudioOut;
class QuaAudioPort;
class QuaMidiIn;
class QuaMidiOut;
class QuaJoystickPort;
class Block;
class StabEnt;

#include "QuaTypes.h"
#include "QuaTime.h"
#include <initializer_list>
using namespace std;
/*
 * TODO  XXXX FIXME @deprecated
 * currently these constants are only used on the MFC interface controlling the name format for destinations
 */
enum {
	NMFMT_NONE = 0,
	NMFMT_SYM = 1,
	NMFMT_NAME = 2,
	NMFMT_NUM = 3
};

/*
 * confusion on terminology, but one I'll live with.
 * channel within this class refers to the hard channels ie particular midi channel or vst pin or audio channel ... but not the main Qua Channel objects
 * and of course there are a few direct pointers to Qua Channel objects
 */

/*
 * PortSpec
 * unified object to specify a port within qua ... either a hardware port or an internal routing
 * with internal routing, the port might not be mappable until a whole script is processed
 */
class PortSpec {
public:
	PortSpec(qua_port_type t, const string& nm, bool isIdent, vector<int> *ids=nullptr) :
			name(nm), type(t), nameIsIdent(isIdent) {
		setChanIds(ids);
	}
	PortSpec(qua_port_type t, const string& nm, bool isIdent, initializer_list<int> chanid) :
			name(nm), type(t), nameIsIdent(isIdent), chanIds(chanid) { }
	PortSpec(qua_port_type t, const string& nm, initializer_list<int> chanid) :
		name(nm), type(t), nameIsIdent(false), chanIds(chanid) { }
	PortSpec(qua_port_type t, const string& nm, const vector<int> &chanid) :
		name(nm), type(t), nameIsIdent(false), chanIds(chanid) { }

	void setChanIds(vector<int> *ids) {
		if (ids != nullptr) {
			chanIds.assign(ids->begin(), ids->end());
		}
	}
	string toString();

	string name;
	qua_port_type type;
	bool nameIsIdent;
	vector<int> chanIds;
};

class Place
{
public:
	Place(StabEnt *s, Channel *c, const PortSpec &, int direction, bool enable);
	virtual ~Place();

	virtual bool		ValidDevice(QuaPort *);
	virtual status_t	SetEnabled(uchar);
	virtual char		*Name(uchar nfmt, uchar cfmt);

	bool setPortInfo(QuaPort *, port_chan_id chan, short ind);
	QuaPort *currentPort() { return device;  }
	
	void Reset();
				
	Channel *channel;
	QuaPort *device;
	PortSpec portSpec;

//	short				needed_channels;	// # of channels to try for

	port_chan_id		deviceChannel;
	int8				mode;
	int8				select;
	uchar				enabled;

	StabEnt				*sym;

	bool				isinit;
	Block				*initBlock;
	status_t			Init();

	QuaPort				*xDevice;	// extra port .. probably
	port_chan_id		xChannel;	// 90% same device and next free device channel
};

class Input: public Place
{
public:
	Input(const std::string &nm, Channel *c, const PortSpec &portSpec, const bool);

	virtual bool		ValidDevice(QuaPort *);
	virtual status_t	SetEnabled(uchar);
	virtual char		*Name(uchar nfmt, uchar cfmt);

	void				SaveSnapshot(ostream &out, Channel *chan);
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);
	
	bool				HasStreamItems();
	bool				GetStreamItems(Stream *);
	bool				Save(ostream &out, short, short);

	union source_info_t
	{
		struct audio_source_info_t {
			QuaAudioIn		*port;
			QuaAudioIn		*xport;
		}					audio;
		QuaMidiIn			*midi;
		QuaJoystickPort		*joy;
	}					src;

	float				gain;
	float				pan;
		
};

class Output: public Place
{
public:
	Output(const std::string &nm,  Channel *c, const PortSpec &portSpec, const bool);

	virtual bool		ValidDevice(QuaPort *);
	virtual status_t	SetEnabled(uchar);
	virtual char		*Name(uchar nfmt, uchar cfmt);
	
	void				SaveSnapshot(ostream&out, Channel *chan);
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	bool				ClearStream(Stream *);
	
	bool				Save(ostream &out, short, short);
	bool				OutputStream(Time &t, Stream *stream);

	union destination_info_t
	{
		struct audio_destination_info_t {
			QuaAudioOut		*port;
			QuaAudioOut		*xport;
		}					audio;
		QuaMidiOut			*midi;
		QuaJoystickPort		*joy;
	}					dst;

	float				gain;
	float				pan;

};

#endif