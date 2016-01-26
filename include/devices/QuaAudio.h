#ifndef _QUA_AUDIO
#define _QUA_AUDIO
#ifdef QUA_V_AUDIO
#include "qua_version.h"

#include "QuaTypes.h"
#include "QuaPort.h"

class RosterView;
class QuaPort;
class ControllableNode;
class Instance;
class QuaAudioManager;
class Input;
class Output;
class Sampler;
class SampleInstance;
class FloatQueue;
class QuaAudioPort;
class Channel;
class Sample;
class KeyIndex;



#if defined(WIN32)

#include "QuaStreamIO.h"

// corresponds to an output in an audio device
class QuaAudioStreamIO: public QuaStreamIO
{
public:
						QuaAudioStreamIO(Qua *q, QuaAudioPort *, long, long, char *, short, short);
	char				insertName[QUA_INSERT_NAME_LENGTH+1];
	long				insertId;
	long				groupId;

	short				sampleFormat;
	short				nChannel; // stereo, mono, or better.
};

class QuaAudioIn: public QuaAudioStreamIO
{
public:
						QuaAudioIn(Qua *q, QuaAudioPort *, long, long, char *, short, short);
						~QuaAudioIn();
	FloatQueue			*data;
};

class QuaAudioOut: public QuaAudioStreamIO
{
public:
						QuaAudioOut(Qua *q, QuaAudioPort *, long, long, char *, short, short);
						~QuaAudioOut();
	float				*outbuf;
	long				offset;
};

#endif

class QuaAudioPort
	: public QuaPort
{
public:
						QuaAudioPort(char *, QuaAudioManager *, short st
#if defined(BEOS) && defined(NEW_MEDIA)
									, media_node *
#endif
									 );
						~QuaAudioPort();
					

#ifdef QUA_V_PORT_PARAM
	void				ZotParameterGroup(BParameterGroup *, StabEnt *);
#endif
#ifdef QUA_V_ARRANGER_INTERFACE
#ifdef _BEOS
	void				ZotParameterMenu(BMenu *, StabEnt *, class ArrangerObject *);
#endif
#endif
	void				ZotInsertCheck(bool, bool, bool, bool);

	virtual char *		Name(uchar);

	short				NInputs();				
	short				NOutputs();
	short				NInputChannels(short);
	short				NOutputChannels(short);
	const char			*InputName(port_chan_id);
	const char			*OutputName(port_chan_id);

	QuaAudioManager		*quaAudio;

#if defined(BEOS) && defined(NEW_MEDIA)
	void				KickStart();
	void				KickStop();
	media_node			mediaNode;
	BParameterWeb		*parameterWeb;
#endif
};

#ifdef QUA_V_AUDIO_ASIO
#include "QuaAsio.h"
#else
enum {
	ASIOSTInt16MSB   = 0,
	ASIOSTInt24MSB   = 1,		// used for 20 bits as well
	ASIOSTInt32MSB   = 2,
	ASIOSTFloat32MSB = 3,		// IEEE 754 32 bit float
	ASIOSTFloat64MSB = 4,		// IEEE 754 64 bit double float

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can be more easily used with these
	ASIOSTInt32MSB16 = 8,		// 32 bit data with 18 bit alignment
	ASIOSTInt32MSB18 = 9,		// 32 bit data with 18 bit alignment
	ASIOSTInt32MSB20 = 10,		// 32 bit data with 20 bit alignment
	ASIOSTInt32MSB24 = 11,		// 32 bit data with 24 bit alignment
	
	ASIOSTInt16LSB   = 16,
	ASIOSTInt24LSB   = 17,		// used for 20 bits as well
	ASIOSTInt32LSB   = 18,
	ASIOSTFloat32LSB = 19,		// IEEE 754 32 bit float, as found on Intel x86 architecture
	ASIOSTFloat64LSB = 20, 		// IEEE 754 64 bit double float, as found on Intel x86 architecture

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can more easily used with these
	ASIOSTInt32LSB16 = 24,		// 32 bit data with 18 bit alignment
	ASIOSTInt32LSB18 = 25,		// 32 bit data with 18 bit alignment
	ASIOSTInt32LSB20 = 26,		// 32 bit data with 20 bit alignment
	ASIOSTInt32LSB24 = 27		// 32 bit data with 24 bit alignment
};

#endif

class QuaAudioManager
	: public QuaPortManager
{
public:
						QuaAudioManager();
						~QuaAudioManager();
	
	inline QuaAudioPort *Port(int i) {return (QuaAudioPort *)QuaPortManager::Port(i); }

	virtual status_t	Connect(Input *);
	virtual status_t	Connect(Output *);
	virtual status_t	Disconnect(Input *);
	virtual status_t	Disconnect(Output *);

	status_t			StartAudio();
	status_t			StopAudio();

	char *				ErrorString(status_t err);

#if defined(WIN32)
	bool				Generate(size_t nFrames);

#endif

#ifdef QUA_V_AUDIO_ASIO
	static QuaAsio		asio;
#endif
	static char *		SampleFormatName(long);

	
	status_t			StartRecording(SampleInstance *ri);
	void				StopRecording(SampleInstance *ri);

	void				StartInstance(Instance *s);
	void				StopInstance(Instance *s);

	void				StartChannel(Channel *);
	
	void				AddSample(Sample *S);
	void				RemoveSample(Sample *S);

	static int32		BuffaeratorWrapper(void *data);
	int32				Buffaerator();
	thread_id			buffyThread;

	static long			WriterWrapper(void *data);
	long				Writer();
	thread_id			writerThread;

	float				sampleRate;
	long				bufferSize;

	RWLock				sampleLock;
	SchLock				recordInstanceLock;
	SchLock				channelLock;
	BList				samples;
	BList				recordInstances;
	BList				channels;
	


#if defined(WIN32)
	QuaAudioPort *		dfltOutput;
	QuaAudioPort *		dfltInput;

	QuaAudioIn			*OpenInput(QuaAudioPort *, short which, short nch);
	status_t			CloseInput(QuaAudioIn *);
	QuaAudioOut			*OpenOutput(QuaAudioPort  *, short which, short nch);
	status_t			CloseOutput(QuaAudioOut *);

	QuaAudioIn			*InputConnectionForPort(QuaAudioPort *, short);
	QuaAudioOut			*OutputConnectionForPort(QuaAudioPort *, short);
	QuaAudioIn			*AddInputConnection();
	QuaAudioOut			*AddOutputConnection();

#endif
};

enum {
	AUDIO_NONE = 0,
	AUDIO_HAS_FX = 1,
	AUDIO_HAS_PLAYER = 2
};

extern QuaAudioManager	*audioManager;
#endif
#endif