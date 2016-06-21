#ifndef _DESTINATION
#define _DESTINATION

namespace tinyxml2 {
	class XMLElement;
}

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

enum {
	NMFMT_NONE = 0,
	NMFMT_SYM = 1,
	NMFMT_NAME = 2,
	NMFMT_NUM = 3
};

class Place
{
public:
						Place(StabEnt *s, Channel *c, QuaPort *, port_chan_id chan, bool);
						~Place();

	virtual bool		ValidDevice(QuaPort *);
	virtual status_t	SetEnabled(uchar);
	virtual char		*Name(uchar nfmt, uchar cfmt);

	bool setPortInfo(QuaPort *, port_chan_id chan, short ind);
	
	void				Reset();
				
	Channel				*channel;
	QuaPort				*device;

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
						Input(std::string nm, Channel *c, QuaPort *, port_chan_id chan, bool);

	virtual bool		ValidDevice(QuaPort *);
	virtual status_t	SetEnabled(uchar);
	virtual char		*Name(uchar nfmt, uchar cfmt);

	void				SaveSnapshot(FILE *fp, Channel *chan);
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);
	
	bool				HasStreamItems();
	bool				GetStreamItems(Stream *);
	bool				Save(FILE *, short, short);

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
						Output(std::string nm, Channel *c, QuaPort *, port_chan_id chan, bool);


	virtual bool		ValidDevice(QuaPort *);
	virtual status_t	SetEnabled(uchar);
	virtual char		*Name(uchar nfmt, uchar cfmt);
	
	void				SaveSnapshot(FILE *fp, Channel *chan);
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	bool				ClearStream(Stream *);
	
	bool				Save(FILE *, short, short);
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