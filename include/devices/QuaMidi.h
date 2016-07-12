#ifndef _QUAMIDICTRL
#define _QUAMIDICTRL

#if defined(WIN32)

#include <windows.h>
#include <mmsystem.h>

#ifndef MOD_SWSYNTH
#define MOD_SWSYNTH 7
#endif

#endif

#include <vector>

#include "Note.h"
#include "QuaTime.h"
#include "Stream.h"
#include "QuaPort.h"

class RosterView;
class Qua;
class KeyIndex;
class KeyVal;
class QuaMidiPort;
class Input;
class Output;

inline uchar			PitchToKey(Note *tp) {	return tp->pitch; }

#if defined(WIN32)

#define FMT_MIDI_3(S,C,P,V)	((V<<16)|(P<<8)|(S|C))
#define FMT_MIDI_2(S,C,D)	((D<<8)|(S|C))
#define FMT_MIDI_S(S,C,D)	((D<<16)|(C<<8)|S)

#define MMMSG_STATUS(W)	(W&0xff)
#define MMMSG_CHANNEL(W)(W&0x0f)
#define MMMSG_CMD(W)	(W&0xf0)
#define MMMSG_PARAM1(W)	((W>>8)&0xff)
#define MMMSG_PARAM2(W)	((W>>16)&0xff)

#define MIDI_NOTE_ALL	0xff
#define MIDI_CHANNEL_ANY	0xff
#define MIDI_CHANNEL_SYS	0

#include "QuaStreamIO.h"

class QuaMidiIn: public QuaStreamIO
{
public:
					QuaMidiIn(Qua *q, QuaMidiPort *);
					~QuaMidiIn();

	status_t		Open();
	status_t		Close();
	bool			GetStreamItems(uchar channel, Stream *S);
	bool			HasStreamItems();
	void			FlushInput();
					
	Stream			received;
	std::mutex		rexMutex;

	uchar			sysxBuffer[256];
	uchar			sysxFlag;
	std::vector<uchar*> sysxBuf;
	std::vector<int> sysxBufLen;
	MIDIHDR			midiHdr;

#ifdef QUA_V_DIRECT_MIDI
	HMIDIIN			handle;

	void			MMMsgReceived(HMIDIIN handle, UINT uMsg, DWORD dwParam1, DWORD dwTimestamp);
	static void CALLBACK	midiCallback(
								HMIDIIN handle, UINT uMsg, DWORD dwInstance,
								DWORD dwParam1, DWORD dwParam2);
#endif
};

class QuaMidiOut: public QuaStreamIO
{
public:
					QuaMidiOut(Qua *q, QuaMidiPort *);
					~QuaMidiOut();

	void			MidiCmd(uchar channel, Note *tp);
	bool			OutputStream(Time TC, Stream *A, uchar chan);
	status_t		Open();
	status_t		Close();
	void			SendClk();
	void			SendStart();
	void			SendStop();
	void			SendCont();
	void			SendKeyPressure(uchar channel, uchar key, uchar vel);
	void			SendChannelPressure(uchar channel, uchar vel);
	void			SendSystemCommon(uchar code, uchar data1, uchar data2);
	void			SendProgChange(uchar chan, uchar program);
	void			SendABender(uchar chan, uchar lsb, uchar msb);
	void			SendControl(uchar chan, uchar controller, uchar amount);
	void			SendSystemExclusive(void *data, long length);
	void			SendNoteOn(uchar channel, uchar key, uchar vel);
	void			SendNoteOff(uchar channel, uchar key, uchar vel);
	bool			ClearStream(Stream *A, uchar chan);
#ifdef QUA_V_DIRECT_MIDI
	HMIDIOUT		handle;
#endif
};

#endif

class QuaMidiManager: public QuaPortManager<QuaMidiPort>
{
public:

	 QuaMidiManager();
	 ~QuaMidiManager();
	QuaMidiPort	 *dfltInput;
	QuaMidiPort	 *dfltOutput;

	inline QuaMidiPort *port(int i) {
		return (QuaMidiPort *)QuaPortManager::port(i);
	}

	QuaMidiPort *midiPortForId(int32);

	virtual QuaPort *findPortByName(string nm) override;

	virtual status_t connect(Input *);
	virtual status_t connect(Output *);
	virtual status_t disconnect(Input *);
	virtual status_t disconnect(Output *);

#if defined(WIN32)

	QuaMidiOut *OpenOutput(Qua *q, QuaMidiPort *d);
	QuaMidiIn *OpenInput(Qua *q, QuaMidiPort *d);
	bool closeOutput(QuaMidiOut *d);
	bool closeInput(QuaMidiIn *d);
	bool remove(QuaMidiOut *d);
	bool remove(QuaMidiIn *d);
	
	QuaMidiIn *inputConnectionForPort(Qua *q, QuaMidiPort *);
	QuaMidiOut *outputConnectionForPort(Qua *q, QuaMidiPort *);
	QuaMidiIn *addInputConnection();
	QuaMidiOut *addOutputConnection();

	static MIDIINCAPS *midiInDevices(int32 *nDevices);
	static MIDIOUTCAPS *midiOutDevices(int32 *nDevices);
	static const char *mmTechName(int);

	std::vector<QuaMidiIn*>	inputs;
	std::vector<QuaMidiOut*> outputs;
#endif
};

#include <string>

class QuaMidiPort: 	public QuaPort
{
public:
	QuaMidiPort(std::string dmm, std::string nm, QuaMidiManager *, short, int32 myid);
	~QuaMidiPort();
	virtual const char * name(uchar);
					
	int32			id;
	
	QuaMidiManager	*quaMidi;

};

#include <unordered_map>
extern std::unordered_map<std::string, int> dfltMidiCtrlLabelIndex;

#endif