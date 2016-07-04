#ifndef _QUA_AUDIO
#define _QUA_AUDIO

#ifdef QUA_V_AUDIO
#include "qua_version.h"

#include "QuaTypes.h"
#include "QuaPort.h"

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


#include <vector>
#include <thread>
#include <mutex>
using namespace std;

#include "QuaStreamIO.h"

// corresponds to an output in an audio device
class QuaAudioStreamIO: public QuaStreamIO
{
public:
	QuaAudioStreamIO(Qua *q, QuaAudioPort *, long, long, char *, short, short);
	string insertName;
	long insertId;
	long groupId;

	short sampleFormat;
	short nChannel; // stereo, mono, or better.
};

class QuaAudioIn: public QuaAudioStreamIO
{
public:
	QuaAudioIn(Qua *q, QuaAudioPort *, long, long, char *, short, short);
	~QuaAudioIn();
	FloatQueue *data;
};

class QuaAudioOut: public QuaAudioStreamIO
{
public:
	QuaAudioOut(Qua *q, QuaAudioPort *, long, long, char *, short, short);
	~QuaAudioOut();
	float *outbuf;
	long offset;
};

class QuaAudioPort
	: public QuaPort
{
public:
	QuaAudioPort(char *, QuaAudioManager *, short st);
	~QuaAudioPort();
					

#ifdef QUA_V_PORT_PARAM
	void				ZotParameterGroup(BParameterGroup *, StabEnt *);
#endif
	void				ZotInsertCheck(bool, bool, bool, bool);

	virtual const char *name(uchar);

	short				NInputs();				
	short				NOutputs();
	short				NInputChannels(short);
	short				NOutputChannels(short);
	const string InputName(port_chan_id);
	const string OutputName(port_chan_id);

	QuaAudioManager		*quaAudio;

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
	: public QuaPortManager<QuaAudioPort>
{
public:
	QuaAudioManager();
	~QuaAudioManager();
	
	virtual status_t connect(Input *);
	virtual status_t connect(Output *);
	virtual status_t disconnect(Input *);
	virtual status_t disconnect(Output *);

	status_t			StartAudio();
	status_t			StopAudio();

	char *				ErrorString(status_t err);

	bool				Generate(size_t nFrames);

#ifdef QUA_V_AUDIO_ASIO
	static QuaAsio		asio;
#endif
	static char * sampleFormatName(const long);

	
	status_t startRecording(SampleInstance *ri);
	void stopRecording(SampleInstance *ri);

	void startInstance(Instance *s);
	void stopInstance(Instance *s);

	void startChannel(Channel *);
	
	void addSample(Sample *S);
	void removeSample(Sample *S);

	void buffaerator();
	void pauseBuffaerator();
	void resumeBuffaerator();
	thread buffyThread;
	mutex buffyMux;
	condition_variable buffyCV;

	void writer();
	void pauseWriter();
	void resumeWriter();
	thread writerThread;
	mutex writerMux;
	condition_variable writerCV;

	float sampleRate;
	long bufferSize;

	mutex sampleLock;// was an rwlock
	mutex recordInstanceLock; // an schlock ... is a lock with finer grain control on rescheduling
	mutex channelLock;
	vector<Sample *> samples;
	vector<SampleInstance *> recordInstances;
	vector<Channel *> channels;
	
	QuaAudioPort * dfltOutput;
	QuaAudioPort * dfltInput;

	QuaAudioIn *OpenInput(QuaAudioPort *, short which, short nch);
	status_t CloseInput(QuaAudioIn *);
	QuaAudioOut *OpenOutput(QuaAudioPort  *, short which, short nch);
	status_t CloseOutput(QuaAudioOut *);

	QuaAudioIn *InputConnectionForPort(QuaAudioPort *, short);
	QuaAudioOut *OutputConnectionForPort(QuaAudioPort *, short);
	QuaAudioIn *AddInputConnection();
	QuaAudioOut *AddOutputConnection();
};

enum {
	AUDIO_NONE = 0,
	AUDIO_HAS_FX = 1,
	AUDIO_HAS_PLAYER = 2
};

extern QuaAudioManager	*audioManager;
#endif
#endif