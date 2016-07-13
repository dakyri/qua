#include "qua_version.h"

#if defined(WIN32)


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>

#include "StdDefs.h"
#include "MidiDefs.h"

#include "QuaMidi.h"
#include "Block.h"
#include "Channel.h"
#include "Stream.h"
#include "Qua.h"
#include "QuaInsert.h"
#include "QuaTypes.h"
#include "QuaDisplay.h"

#include <unordered_map>

int	debug_midi=0;

#if defined(WIN32)
/* general error return values */
//#define MMSYSERR_NOERROR      0                    /* no error */
//#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  /* unspecified error */
//#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  /* device ID out of range */
//#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  /* driver failed enable */
//#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  /* device already allocated */
//#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  /* device handle is invalid */
//#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  /* no device driver present */
//#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  /* memory allocation error */
//#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  /* function isn't supported */
//#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  /* error value out of range */
//#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) /* invalid flag passed */
//#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) /* invalid parameter passed */
//#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) /* handle being used */
						   /* simultaneously on another */
						   /* thread (eg callback) */
//#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) /* specified alias not found */
//#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) /* bad registry database */
//#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) /* registry key not found */
//#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) /* registry read error */
//#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) /* registry write error */
//#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) /* registry delete error */
//#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) /* registry value not found */
//#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) /* driver does not call DriverCallback */
//#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 20) /* last error in range */

std::unordered_map<std::string, int> dfltMidiCtrlLabelIndex = {
	{"control 0: Bank Select MSB", 0},
	{"control 1: Modulation Wheel", 1},
	{"control 2: Breath Contoller", 2},
	{"control 4: Foot Controller", 4},
	{"control 5: Portamento Time", 5},
	{"control 6: Data Entry MSB", 6},
	{"control 7: Main Volume", 7},
	{"control 8: Balance", 8},
	{"control 10: Pan", 10},
	{"control 12: Effect Control 1", 12},
	{"control 13: Effect Control 2", 13},
	{"control 19: General Purpose Controllers 1", 19},
	{"control 20: General Purpose Controllers 2", 20},
	{"control 21: General Purpose Controllers 3", 21},
	{"control 22: General Purpose Controllers 4", 22},
	{"control 22: General Purpose Controllers 4", 22},
	{"control 32: Bank Select LSB", 32},
	{"control 38: Data Entry MSB", 38},
	{"control 64: Damper Pedal (Sustain)", 64},
	{"control 65: Portamento", 65},
	{"control 66: Sostenuto", 66},
	{"control 67: Soft Pedal", 67},
	{"control 68: Legato Footswitch", 68},
	{"control 69: Hold 2", 69},
	{"control 70: Sound Controller 1 (default: Sound Variation)", 70},
	{"control 71: Sound Controller 2 (default: Timbre/Harmonic Content)", 71},
	{"control 72: Sound Controller 3 (default: Release Time)", 72},
	{"control 73: Sound Controller 4 (default: Attack Time)", 73},
	{"control 74: Sound Controller 5 (default: Brightness)", 74},
	{"control 75: Sound Controller 6", 75},
	{"control 76: Sound Controller 7", 76},
	{"control 77: Sound Controller 8", 77},
	{"control 78: Sound Controller 9", 78},
	{"control 79: Sound Controller 10", 79},
	{"control 80: General Purpose Controller 5", 80},
	{"control 81: General Purpose Controller 6", 81},
	{"control 82: General Purpose Controller 7", 82},
	{"control 83: General Purpose Controller 8", 83},
	{"control 84: Portamento", 84},
	{"control 91: Effects 1 Depth", 91},
	{"control 92: Effects 2 Depth", 92},
	{"control 93: Effects 3 Depth", 93},
	{"control 94: Effects 4 Depth", 94},
	{"control 95: Effects 5 Depth", 95},
	{"control 96: Data Increment", 96},
	{"control 97: Data Decrement", 97},
	{"control 98: Non-Registered Parameter Number LSB", 98},
	{"control 99: Non-Registered Parameter Number LSB", 99},
	{"control 100: Registered Parameter Number LSB", 100},
	{"control 101: Registered Parameter Number MSB", 101},
	{"control 121: Reset All Controllers", 121},
	{"control 122: Local Control", 122},
	{"control 123: All Notes Off", 123},
	{"control 124: Omni Off", 124},
	{"control 125: Omni On", 125},
	{"control 126: Mono On (Poly Off)", 126},
	{"control 127: Poly On (Mono Off)", 127}
};

std::unordered_map<std::string, int> midiTechNameIndex = {
	{"MIDI hardware port", 		MOD_MIDIPORT},
	{"Synthesizer", 			MOD_SYNTH},
	{"Square wave synthesizer", MOD_SQSYNTH},
	{"FM synthesizer", 			MOD_FMSYNTH},
	{"Microsoft MIDI mapper", 	MOD_MAPPER}
#ifdef MOD_WAVETABLE
	,{"Hardware wavetable synthesizer", MOD_WAVETABLE}
#endif
#ifdef MOD_SWSYNTH
	,{"Software synthesizer", 	MOD_SWSYNTH}
#endif
};
#endif



QuaMidiPort::QuaMidiPort(std::string portnm, std::string nm, QuaMidiManager *qp, short md, int32 p):
	QuaPort(nm, QUA_DEV_MIDI, QUA_DEV_GENERIC, md)
{
	quaMidi = qp;
	id = p;

#if defined(WIN32)

	if (mode & QUA_PORT_IN)
		AddInsert("in", 0, INPUT_INSERT, 1, 0);	
	if (mode & QUA_PORT_OUT)
		AddInsert("out", 1, OUTPUT_INSERT, 1, 0);
	

#endif
	fprintf(stderr, "QuaMidiPort: created midi port \"%s\"\n", portnm.c_str());
}


QuaMidiPort::~QuaMidiPort()
{
#if defined(WIN32)
#endif
}

const char *
QuaMidiPort::name(uchar)
{
	return sym->name.c_str();
}

QuaStreamIO::QuaStreamIO(Qua *q, QuaPort *p)
{
	uberQua = q;
	status = STATUS_UNKNOWN;
	connected = false;
	enabled = false;
	quaport = p;
}

QuaMidiOut::QuaMidiOut(Qua *q, QuaMidiPort *p):
	QuaStreamIO(q, p)
{
	connected = false;
	
#ifdef QUA_V_DIRECT_MIDI
	handle = nullptr;
#endif
	//	char		buf[30];
//	sprintf(buf, "Qua Midi In %c", quaport->id+'A');
//	SetName(buf);
}

QuaMidiIn::QuaMidiIn(Qua *q, QuaMidiPort *p):
	QuaStreamIO(q, p)
{
	connected = false;
#ifdef QUA_V_DIRECT_MIDI
	handle = nullptr;
#endif
//	char		buf[30];
//	sprintf(buf, "Qua Midi Out %c", id+'A');
//	SetName(buf);
	sysxFlag = 0;
}

QuaMidiIn::~QuaMidiIn()
{
	for (short i=0; ((size_t)i) < sysxBuf.size(); i++) {
		delete (char *)sysxBuf[i];
	}
}

QuaMidiOut::~QuaMidiOut()
{

}
#if defined(WIN32)
/*************************** midiCallback() *****************************
 * Here's my callback that Windows calls whenever 1 of 4 possible things
 * happen:
 *
 * 1).	I open a MIDI In Device via midiInOpen(). In this case, the
 *		uMsg arg to my callback will be MIM_OPEN. The handle arg will
 *		be the same as what is returned from midiInOpen(). The
 *		dwInstance arg is whatever I passed to midiInOpen() as its
 *		dwInstance arg.
 *
 * 2).	I close a MIDI In Device via midiInClose(). In this case, the
 *		uMsg arg to my callback will be MIM_CLOSE. The handle arg will
 *		be the same as what was passed to midiInClose(). The
 *		dwInstance arg is whatever I passed to midiInClose() as its
 *		dwInstance arg when I initially opened this handle.
 *
 * 3).	One, regular (ie, everything except System Exclusive messages) MIDI
 *		message has been completely input. In this case, the uMsg arg to my
 *		callback will be MIM_DATA. The handle arg will be the same as what
 *		is passed to midiInOpen(). The dwInstance arg is whatever I passed
 *		to midiInOpen() as its dwInstance arg when I initially opened this
 *		handle. The dwParam1 arg is the bytes of the MIDI Message packed
 *		into an unsigned long in the same format that is used by
 *		midiOutShort(). The dwParam2 arg is a time stamp that the device
 *		driver created when it recorded the MIDI message.
 *
 * 4).	midiInOpen has either completely filled a MIDIHDR's memory buffer
 *		with part of a System Exclusive message (in which case we had better
 *		continue queuing the MIDIHDR again in order to grab the remainder
 *		of the System Exclusive), or the MIDIHDR's memory buffer contains the
 *		remainder of a System Exclusive message (or the whole message if it
 *		happened to fit into the memory buffer intact). In this case, the
 *		uMsg arg to my callback will be MIM_LONGDATA. The handle arg will be
 *		the same as what is passed to midiInOpen(). The dwInstance arg is
 *		whatever I passed to midiInOpen() as its dwInstance arg when I
 *		initially opened this handle. The dwParam1 arg is a pointer to the
 *		MIDIHDR whose memory buffer contains the System Exclusive data. The
 *		dwParam2 arg is a time stamp that the device driver created when it
 *		recorded the MIDI message.
 *
 * 5).	This callback is not processing data fast enough such that the MIDI
 *		driver (and possibly the MIDI In port itself) has had to throw away
 *		some incoming, regular MIDI messages. In this case, the uMsg arg to my
 *		callback will be MIM_MOREDATA. The handle arg will be the same as what
 *		is passed to midiInOpen(). The dwInstance arg is whatever I passed
 *		to midiInOpen() as its dwInstance arg when I initially opened this
 *		handle. The dwParam1 arg is the bytes of the MIDI Message that was
 *		not handled (by an MIM_DATA call) packed into an unsigned long in the
 *		same format that is used by midiOutShort(). The dwParam2 arg is a time
 *		stamp that the device driver created when it recorded the MIDI message.
 *		In handling a series of these events, you should store the MIDI data
 *		in a global buffer, until such time as you receive another MIM_DATA
 *		(which indicates that you can now do the more time-consuming processing
 *		that you obviously were doing in handling MIM_DATA).
 *		NOTE: Windows sends an MIM_MOREDATA event only if you specify the
 *		MIDI_IO_STATUS flag to midiInOpen().
 *
 * 6).	An invalid, regular MIDI message was received. In this case, the uMsg
 *		arg to my callback will be MIM_ERROR. The handle arg will be the same
 *		as what is passed to midiInOpen(). The dwInstance arg is whatever I
 *		passed to midiInOpen() as its dwInstance arg when I initially opened
 *		this handle. The dwParam1 arg is the bytes of the MIDI Message that was
 *		not handled (by an MIM_DATA call) packed into an unsigned long in the
 *		same format that is used by midiOutShort(). The dwParam2 arg is a time
 *		stamp that the device driver created when it recorded the MIDI message.
 *
 * 7).	An invalid, System Exclusive message was received. In this case, the uMsg
 *		arg to my callback will be MIM_LONGERROR. The handle arg will be the same
 *		as what is passed to midiInOpen(). The dwInstance arg is whatever I
 *		passed to midiInOpen() as its dwInstance arg when I initially opened
 *		this handle. The dwParam1 arg is a pointer to the MIDIHDR whose memory
 *		buffer contains the System Exclusive data. The dwParam2 arg is a time
 *		stamp that the device driver created when it recorded the MIDI message.
 *
 * The time stamp is expressed in terms of milliseconds since your app
 * called midiInStart().
 *************************************************************************/
#ifdef QUA_V_DIRECT_MIDI
inline void
QuaMidiIn::MMMsgReceived(HMIDIIN handle, UINT uMsg, DWORD dwParam1, DWORD dwTimestamp)
{
	LPMIDIHDR		lpMIDIHeader;
	unsigned char *	ptr;
//	TCHAR			buffer[80];
//	unsigned char 	bytes;

	switch (uMsg){
		
		case MIM_DATA:	/* Received some regular MIDI message */
		{  
			uchar	cmd=MMMSG_CMD(dwParam1);
//			fprintf(stderr, "midi mess %x %x\n", dwParam1, cmd);
			switch (cmd) {
				case MIDI_NOTE_OFF:
				case MIDI_NOTE_ON:
				case MIDI_KEY_PRESS: {
					Note t;
					t.dynamic = MMMSG_PARAM2(dwParam1);
					if (t.dynamic == 0 && cmd == MIDI_NOTE_ON) {
						cmd = MIDI_NOTE_OFF;
					}
					t.cmd = cmd|MMMSG_CHANNEL(dwParam1);
					t.pitch = MMMSG_PARAM1(dwParam1);
					t.duration = 0;
					if (debug_midi) {
						fprintf(stderr, "Midi in: note %d cmd %x chan %d vel %d\n",
								t.pitch, cmd, MMMSG_CHANNEL(dwParam1), t.dynamic);
					}
					rexMutex.lock();
					received.AddToStream(t, uberQua->theTime);
					rexMutex.unlock();
					break;
				}

				case MIDI_CHAN_PRESS: {
					Note		t;
					t.set(MIDI_NOTE_ALL, MMMSG_PARAM1(dwParam1), MMMSG_STATUS(dwParam1));
					if (debug_midi) {
						fprintf(stderr, "Midi in: note %d chan %d %x %d\n",
								t.pitch, MMMSG_CHANNEL(dwParam1), MMMSG_CMD(dwParam1), t.dynamic);
					}
					rexMutex.lock();
					received.AddToStream(t, uberQua->theTime);
					rexMutex.unlock();
					break;
				}

				case MIDI_CTRL: {
					Ctrl t;
					t.cmd = MMMSG_STATUS(dwParam1);
					t.controller = MMMSG_PARAM1(dwParam1);
					t.amount = MMMSG_PARAM2(dwParam1);
					if (debug_midi) {
						fprintf(stderr, "Midi in: %x ctrl %d chan %d val %d\n", cmd,
								t.controller, MMMSG_CHANNEL(dwParam1), t.amount);
					}
					rexMutex.lock();
					received.AddToStream(t, uberQua->theTime);
					rexMutex.unlock();
					break;
				}

				case MIDI_PROG: {
					Prog		t;
					t.bank = NON_PROG;
					t.subbank = NON_PROG;
					t.program = MMMSG_PARAM1(dwParam1);
					t.cmd = MMMSG_STATUS(dwParam1);
					if (debug_midi) {
						fprintf(stderr, "Midi in: prog %d %d %d chan %d\n", t.program, t.bank, t.subbank, MMMSG_CHANNEL(dwParam1));
					}
					rexMutex.lock();
					received.AddToStream(t, uberQua->theTime);
					rexMutex.unlock();
					break;
				}

				case MIDI_BEND: {
					Bend		t;
					t.bend = (MMMSG_PARAM2(dwParam1)<<8)|MMMSG_PARAM1(dwParam1);
					t.cmd = MMMSG_STATUS(dwParam1);
					if (debug_midi) {
						fprintf(stderr, "Midi in: bend %d chan %d\n", t.bend,  MMMSG_CHANNEL(dwParam1));
					}
					rexMutex.lock();
					received.AddToStream(t, uberQua->theTime);
					rexMutex.unlock();
					break;
				}

				case MIDI_SYS:
				default:
					cmd = MMMSG_STATUS(dwParam1);
					switch (cmd) {
					case MIDI_SENSING: {
						if (debug_midi) {
							fprintf(stderr, "Midi in: active sensing\n");
						}
						break;
					}
					case MIDI_CLOCK: {
						if (debug_midi) {
							fprintf(stderr, "Midi in: clock\n");
						}
						break;
					}
					case MIDI_TIME_CODE: {
						if (debug_midi) {
							fprintf(stderr, "Midi in: time code\n");
						}
						break;
					}
					case MIDI_START: {
						if (debug_midi) {
							fprintf(stderr, "Midi in: system start\n");
						}
						break;
					}
					case MIDI_CONT: {
						if (debug_midi) {
							fprintf(stderr, "Midi in: system continue\n");
						}
						break;
					}
					case MIDI_STOP: {
						if (debug_midi) {
							fprintf(stderr, "Midi in: system stop\n");
						}
						break;
					}
					case MIDI_SYSX_START:	// would be pretty wierd, pass it through with the rest of it.
					case MIDI_SYSX_END:	// would be pretty wierd, pass it through with the rest of it.
						// if it ever actually happens, I'll write some annoying contingency code
					case MIDI_SONG_POS:
					case MIDI_SONG_SEL:
					case MIDI_CABLE_MSG:
					case MIDI_TUNE_REQ:
					case MIDI_SYS_RESET: {
						SysC		t;
						t.cmd = MMMSG_STATUS(dwParam1);
						t.data1 = MMMSG_PARAM1(dwParam1);
						t.data2 = MMMSG_PARAM2(dwParam1);
						if (debug_midi) {
							fprintf(stderr, "Midi in: sys common %x %x %x\n", (uchar)t.cmd, (uchar)t.data1, (uchar)t.data2);
						}
						rexMutex.lock();
						received.AddToStream(t, uberQua->theTime);
						rexMutex.unlock();
						break;
					}
				 }
			}
			break;
		}

		case MIM_LONGDATA:/* Received all or part of some System Exclusive message */
		{
			
			if (!(sysxFlag & 0x80))	{	/* If ready to close down, then don't midiInAddBuffer() again */
				if (debug_midi) {
					fprintf(stderr, "Midi in: sysx \n");
				}
				lpMIDIHeader = (LPMIDIHDR)dwParam1;

				ptr = (unsigned char *)(lpMIDIHeader->lpData);
				long len = lpMIDIHeader->dwBytesRecorded;

				if (!sysxFlag) {					/* first chunk of sysx */
					sysxFlag |= 0x01;
					ptr++; len--;
					for (short i=0; ((unsigned)i)<sysxBuf.size(); i++) {
						delete (char *)sysxBuf[i];
					}
					sysxBuf.clear();
					sysxBufLen.clear();
				}

				if (ptr[len - 1] == MIDI_SYSX_END)	{ // last sysx block?
					sysxFlag &= (~0x01);
					len--;
				}
				uchar	*buf = new uchar [len];
				memcpy(buf, ptr, len);
				sysxBuf.push_back(buf);
				sysxBufLen.push_back(len);
				if (!sysxFlag) {	// send it
					int		sysxLen = 0;
					short	i;
					for (i=0; ((unsigned)i)<sysxBufLen.size(); i++) {
						sysxLen += (int)sysxBufLen[i];
					}
					char *d = new char[sysxLen];
					char *p = d;
					for (i=0; ((unsigned)i)<sysxBufLen.size(); i++) {
						memcpy(p, (char *)sysxBuf[i], (int)sysxBufLen[i]);
						p += (int)sysxBufLen[i];
					}
					// !!!! ownership of this buffer will pass down by assignemnt
					// c++ in VS won't let us make this less painful with a non trivial assign override. 
					// this data should end up in a StreamSysX item, and that item will be the one doing the deleting
					SysX t;
					t.set(d, sysxLen);

					rexMutex.lock();
					received.AddToStream(t, uberQua->theTime);
					rexMutex.unlock();
				}

				midiInAddBuffer(handle, lpMIDIHeader, sizeof(MIDIHDR));/* Queue the MIDIHDR for more input */
			}

			break;
		}

		case MIM_OPEN:
        case MIM_CLOSE:
        case MIM_ERROR:
        case MIM_LONGERROR:
        case MIM_MOREDATA:

			break;

	}
}

void CALLBACK 
QuaMidiIn::midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwTimestamp)
{
	((QuaMidiIn *)dwInstance)->MMMsgReceived(handle, uMsg, dwParam1, dwTimestamp);
}
#endif
#endif

#if defined(WIN32)

inline void
QuaMidiOut::SendNoteOn(uchar channel, uchar key, uchar vel)
{
#ifdef QUA_V_DIRECT_MIDI
    midiOutShortMsg(handle, FMT_MIDI_3(MIDI_NOTE_ON, channel, key, vel));
#endif
}

inline void
QuaMidiOut::SendNoteOff(uchar channel, uchar key, uchar vel)
{
#ifdef QUA_V_DIRECT_MIDI
    midiOutShortMsg(handle, FMT_MIDI_3(MIDI_NOTE_ON, channel, key, vel));
#endif
}

inline void
QuaMidiOut::SendKeyPressure(uchar channel, uchar key, uchar vel)
{
#ifdef QUA_V_DIRECT_MIDI
    midiOutShortMsg(handle, FMT_MIDI_3(MIDI_KEY_PRESS, channel, key, vel));
#endif
}

inline void
QuaMidiOut::SendChannelPressure(uchar channel, uchar vel)
{
#ifdef QUA_V_DIRECT_MIDI
    midiOutShortMsg(handle, FMT_MIDI_2(MIDI_CHAN_PRESS, channel, vel));
#endif
}

inline void
QuaMidiOut::SendProgChange(uchar chan, uchar program)
{
#ifdef QUA_V_DIRECT_MIDI
	midiOutShortMsg(handle, FMT_MIDI_2(MIDI_PROG, chan, program));
#endif
}

inline void
QuaMidiOut::SendABender(uchar chan, uchar lsb, uchar msb)
{
#ifdef QUA_V_DIRECT_MIDI
	midiOutShortMsg(handle, FMT_MIDI_3(MIDI_BEND, chan, lsb, msb));
#endif
}

inline void
QuaMidiOut::SendControl(uchar channel, uchar controller, uchar amount)
{
#ifdef QUA_V_DIRECT_MIDI
    midiOutShortMsg(handle, FMT_MIDI_3(MIDI_CTRL, channel, controller, amount));
#endif
}

inline void
QuaMidiOut::SendSystemExclusive(void *data, long length)
{
#ifdef QUA_V_DIRECT_MIDI
	MIDIHDR	midiHdr;
    midiHdr.lpData = (char *)data;
    midiHdr.dwBufferLength = length;
    midiHdr.dwFlags = 0;
    status_t err = midiOutPrepareHeader(handle,  &midiHdr, sizeof(MIDIHDR));
    if (!err)  {
        err = midiOutLongMsg(handle, &midiHdr, sizeof(MIDIHDR));
        /* Unprepare the buffer and MIDIHDR */
        while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR))) {
            /* Should put a delay in here rather than a busy-wait */		    
        }
    }
#endif
}

void
QuaMidiOut::SendSystemCommon(uchar code, uchar data1, uchar data2)
{
	switch(code) {
	case MIDI_SYSX_START	:
	case MIDI_SYSX_END:
		break;
	case MIDI_TIME_CODE:
	case MIDI_SONG_POS:
	case MIDI_SONG_SEL:
	case MIDI_TUNE_REQ	:
#ifdef QUA_V_DIRECT_MIDI
		midiOutShortMsg(handle, FMT_MIDI_S(code, data1, data2));
#endif
		break;
	case MIDI_CLOCK:
	case MIDI_START:
	case MIDI_CONT	:
	case MIDI_STOP:
	case MIDI_SENSING:
	case MIDI_SYS_RESET	:
#ifdef QUA_V_DIRECT_MIDI
		midiOutShortMsg(handle, code);
#endif
		break;		
	}
}

inline void
QuaMidiOut::SendStart()
{
#ifdef QUA_V_DIRECT_MIDI
	midiOutShortMsg(handle, MIDI_START);
#endif
}

inline void
QuaMidiOut::SendStop()
{
#ifdef QUA_V_DIRECT_MIDI
	midiOutShortMsg(handle, MIDI_STOP);
#endif
}

inline void
QuaMidiOut::SendCont()
{
#ifdef QUA_V_DIRECT_MIDI
	midiOutShortMsg(handle, MIDI_CONT);
#endif
}

inline void
QuaMidiOut::SendClk()
{
#ifdef QUA_V_DIRECT_MIDI
	midiOutShortMsg(handle, MIDI_CLOCK);
#endif
}

void
QuaMidiOut::MidiCmd(uchar channel, Note *tp)
{
    int		cmd;

    cmd = tp->cmd & 0xf0;
	switch (cmd) {
	case MIDI_NOTE_ON:	SendNoteOn(channel, tp->pitch, tp->dynamic);break;
	case MIDI_NOTE_OFF:	SendNoteOff(channel, tp->pitch, tp->dynamic);break;
	case MIDI_KEY_PRESS:SendKeyPressure(channel, tp->pitch, tp->dynamic);break;
	case MIDI_CHAN_PRESS:SendChannelPressure(channel, tp->dynamic);break;
	case MIDI_CLOCK:	SendClk();	break;
	case MIDI_START:	SendStart();	break;
	case MIDI_STOP:		SendStop();		break;
	case MIDI_CONT:		SendCont();		break;
	default:
		internalError("unexpected midi cmd %x\n", cmd);
	}
}

#endif


bool
QuaMidiIn::HasStreamItems()
{
	return received.nItems > 0;
}

// maybe should sort into channels on receipt?????????
bool
#if defined(WIN32)
QuaMidiIn::GetStreamItems(uchar channel, Stream *S)
{
	if (received.nItems > 0) {
		rexMutex.lock();
		StreamItem	*p=received.head,
					**pp = &received.head,
					*prev = nullptr;
		while (p!=nullptr) {
			bool	add_this = false;
			if (channel == MIDI_CHANNEL_ANY) {
				add_this = true;
			} else {
				switch (p->type) {
				case TypedValue::S_NOTE: {
					if ((((StreamNote*)p)->note.cmd & 0xf) == (channel-1)) {
						add_this = true;
					}
					break;
				}
		
				case TypedValue::S_CTRL: {
					if ((((StreamCtrl*)p)->ctrl.cmd & 0xf) == (channel-1)) {
						add_this = true;
					}
					break;
				}
		
				case TypedValue::S_PROG: {
					if ((((StreamProg*)p)->prog.cmd & 0xf) == (channel-1)) {
						add_this = true;
					}
					break;
				}
		
				case TypedValue::S_BEND: {
					if ((((StreamBend*)p)->bend.cmd & 0xf) == (channel-1)) {
						add_this = true;
					}
					break;
				}
		
				case TypedValue::S_SYSC: {
					if (channel == MIDI_CHANNEL_SYS) {
						add_this = true;
					}
					break;
				}
		
				case TypedValue::S_SYSX: {
					if (channel == MIDI_CHANNEL_SYS)
						add_this = true;
					break;
				}
		
				default:	// whatever the fuck this is!
					break;
				}
			}

		    if (add_this) {
		    	if (S->tail) {
		    		S->tail->next = p;
		    	} else {
		    		S->head = p;
		    	}
		    	S->tail = p;
		    	S->nItems++;
				received.nItems--;
		    	*pp = p->next;
			    p = p->next;
		    } else {
		    	prev = p;
		    	pp = &p->next;
		    	p = p->next;
		    }
	    }
	    received.tail = prev;
	
		rexMutex.unlock();
		return true;
	}
	return false;
}
#endif

bool
QuaMidiOut::ClearStream(Stream *A, uchar chan)
{
	StreamItem	*p;
	
	if (debug_midi)
		fprintf(stderr, "clear stream ...");

	for (p=A->head;p!=nullptr;p=p->next) {
		if (p->type == TypedValue::S_NOTE) {
			StreamNote *q = (StreamNote *)p;
		    SendNoteOff(chan, q->note.pitch, q->note.dynamic);
		}
	}
	A->ClearStream();

    return TRUE;
}



bool
QuaMidiOut::OutputStream(Time theTime, Stream *A, uchar chan)
{
    if (debug_midi && A->nItems) {
		fprintf(stderr, "out stream %d %x chan %d\n", A->nItems, (unsigned) A->head, chan);
	}
	
	StreamItem	*p, *prev;

	p=A->head;
	prev = nullptr;
	while (p!=nullptr) {
		bool		del_this=FALSE,
					sched_noff = FALSE;

		switch (p->type) {
		case TypedValue::S_NOTE: {
			StreamNote *q=(StreamNote*)p;
			
		    if (debug_midi>=2)
				fprintf(stderr, "note %x... %d %d\n", q->note.cmd, theTime.ticks, q->time.ticks);
		
		    if (q->note.cmd & MIDI_CMD_BYTE) {
				if (theTime >= q->time) {
					MidiCmd(chan, &q->note);
					if (  q->note.cmd == MIDI_NOTE_OFF ||
						  q->note.cmd == MIDI_KEY_PRESS ||
						  q->note.duration == INFINITE_TICKS) {
						del_this = TRUE;
					} else {
						sched_noff = (q->note.cmd == MIDI_NOTE_ON);
					}
				}
		    } else {
		        if (theTime >= q->time) {
				    if (q->note.pitch <= 127) {
				    	SendNoteOn(chan, q->note.pitch, q->note.dynamic);
						if (q->note.duration == INFINITE_TICKS)
							del_this = TRUE;
						else
							sched_noff = TRUE;
					} else
						del_this = TRUE;
		        }
		    }
//		     else {	// unexpected stream item
//				del_this = TRUE;
//			}
//
			if (sched_noff) { // reschedule note off
	        	q->time += Time(q->note.duration, q->time.metric);
	        	q->note.duration = INFINITE_TICKS; // ensure delete
	        	q->note.cmd = MIDI_NOTE_OFF;
				if (debug_midi)
					fprintf(stderr, "reschedule noff\n");
				del_this = FALSE;
	        }
        
		    break;
		}
		
		case TypedValue::S_PROG: {
			StreamProg	*q = (StreamProg *)p;
			SendProgChange(chan, q->prog.program);
			del_this = TRUE;
			break;
		}
		case TypedValue::S_BEND: {
			StreamBend	*q = (StreamBend *)p;
			SendABender(chan, q->bend.bend&0x08, q->bend.bend>>8);
			del_this = TRUE;
			break;
		}
		case TypedValue::S_CTRL: {
			StreamCtrl	*q = (StreamCtrl *)p;
			SendControl(chan, q->ctrl.controller, q->ctrl.amount);
			del_this = TRUE;
			break;
		}
		case TypedValue::S_SYSX: {
			StreamSysX	*q = (StreamSysX *)p;
			SendSystemExclusive(q->sysX.data, q->sysX.length);
			del_this = TRUE;
			break;
		}
		case TypedValue::S_SYSC: {
			StreamSysC	*q = (StreamSysC *)p;
			SendSystemCommon(q->sysC.cmd, q->sysC.data1, q->sysC.data2);
			del_this = TRUE;
			break;
		}

		default:	// whatever the fuck this is!
			del_this = TRUE;
			break;
	    }
	    
        if (del_this) {
					// no more to do... remove from stream
			StreamItem	*q = p;
			if (prev == nullptr) {
				A->head = p->next;
			} else {
				prev->next = p->next;
			}
			A->nItems--;
			p=p->next;
			delete q;
			if (debug_midi)
				fprintf(stderr, "deleted %x\n", (unsigned) q);
		} else {
			prev = p;
			p = p->next;
		}
    }
    A->tail = prev;

    return TRUE;
}

void
QuaMidiIn::FlushInput()
{
	rexMutex.lock();
	received.ClearStream();
	rexMutex.unlock();
}

#ifdef WIN32
status_t
QuaMidiOut::Open()
{
	QuaMidiPort *p = (QuaMidiPort *)quaport;
	if (connected) {
		if (Close() != B_OK) {
			return B_ERROR;
		}
	}
#ifdef QUA_V_MIDI_DIRECT
	if (midiOutOpen(&handle, p->id, 0, 0, CALLBACK_nullptr) != MMSYSERR_NOERROR) {
		return B_ERROR;
	}
#endif
//MMSYSERR_ALLOCATED The specified resource is already allocated. 
//MMSYSERR_BADDEVICEID The specified device identifier is out of range. 
//MMSYSERR_INVALFLAG The flags specified by dwFlags are invalid. 
//MMSYSERR_INVALPARAM The specified pointer or structure is invalid. 
//MMSYSERR_NOMEM The system is unable to allocate or lock memory. 
	connected = true;
	return B_OK;
}

status_t
QuaMidiOut::Close()
{
//MIDIERR_STILLPLAYING Buffers are still in the queue. 
//MMSYSERR_INVALHANDLE The specified device handle is invalid. 
//MMSYSERR_NOMEM The system is unable to load mapper string description. 
	if (!connected)
		return B_OK;
	status_t	err;
	if (debug_midi)
		fprintf(stderr, "Closing midi out ...");
#ifdef QUA_V_MIDI_DIRECT
    if ((err=midiOutClose(handle)) == MMSYSERR_NOERROR) {
		err=B_OK;
	}
#endif
	if (debug_midi)
		fprintf(stderr, "done midi out close, err %d\n", err);
	connected = false;
	return B_OK;
}

status_t
QuaMidiIn::Open()
{
	QuaMidiPort *p = (QuaMidiPort *)quaport;
	if (connected) {
		if (Close() != B_OK) {
			return B_ERROR;
		}
	}
#ifdef QUA_V_MIDI_DIRECT
	if (midiInOpen(&handle, p->id, (DWORD)midiCallback, (DWORD)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
//MMSYSERR_ALLOCATED The specified resource is already allocated. 
//MMSYSERR_BADDEVICEID The specified device identifier is out of range. 
//MMSYSERR_INVALFLAG The flags specified by dwFlags are invalid. 
//MMSYSERR_INVALPARAM The specified pointer or structure is invalid. 
//MMSYSERR_NOMEM The system is unable to allocate or lock memory. 
		return B_ERROR;
	}

	if (midiInStart(handle) != MMSYSERR_NOERROR) {
//MMSYSERR_INVALHANDLE The specified device handle is invalid. 
	}
#endif
	connected = true;
	return B_OK;
}

status_t
QuaMidiIn::Close()
{
	status_t	err = B_OK;
//MIDIERR_STILLPLAYING Buffers are still in the queue. 
//MMSYSERR_INVALHANDLE The specified device handle is invalid. 
//MMSYSERR_NOMEM The system is unable to load mapper string description. 
	if (!connected)
		return err;
	if (debug_midi)
		fprintf(stderr, "Closing midi in ...");
#ifdef QUA_V_MIDI_DIRECT
	if ((err=midiInClose(handle)) == MMSYSERR_NOERROR) {
		err = B_OK;
	}
#endif
	if (debug_midi)
		fprintf(stderr, "midi in closed, err %d\n", err);
	connected = false;
	return err;
}

#endif

QuaMidiManager::QuaMidiManager()
{
	dfltInput = nullptr;
	dfltOutput = nullptr;
#if defined(WIN32)
	MIDIINCAPS		*mic;
	MIDIOUTCAPS		*moc;
	int32			i, nInDevices, nOutDevices;
	moc = midiOutDevices(&nOutDevices);
	mic = midiInDevices(&nInDevices);

	for (i=0; i<nOutDevices; i++) {
		QuaMidiPort *mp = new QuaMidiPort(wc2string(moc[i].szPname), wc2string(moc[i].szPname), this, QUA_PORT_OUT, i);
		/* mp->representation->SetDisplayMode(OBJECT_DISPLAY_SMALL);*/
		ports.push_back(mp);
		if (dfltOutput == nullptr) {
			dfltOutput = mp;
		}
	}

	for (i=0; i<nInDevices; i++) {
		QuaMidiPort *mp = new QuaMidiPort(wc2string(mic[i].szPname), wc2string(mic[i].szPname), this, QUA_PORT_IN, i);
		ports.push_back(mp);
		if (dfltInput == nullptr) {
			dfltInput = mp;
		}
	}

	delete moc;
	delete mic;
#endif
}

QuaMidiManager::~QuaMidiManager()
{
	fprintf(stderr, "QuaMidiManager::~QuaMidiManager()\n");

#if defined(WIN32)
	for (QuaMidiIn *pi: inputs) {
		pi->Close();
	}
	for (QuaMidiOut *pi: outputs) {
		pi->Close();
	}
#endif
	fprintf(stderr, "QuaMidiManager::deletion complete\n");
}

QuaMidiIn *
QuaMidiManager::inputConnectionForPort(Qua *toQua, QuaMidiPort *p)
{
#if defined(WIN32)
	for (QuaMidiIn *pi: inputs) {
		if (pi->quaport == p && pi->uberQua == toQua) {
			return pi;
		}
	}
#endif
	return nullptr;
}

QuaMidiOut *
QuaMidiManager::outputConnectionForPort(Qua *toQua, QuaMidiPort *p)
{
#if defined(WIN32)
	for (QuaMidiOut *pi: outputs) {
		if (pi->quaport == p && pi->uberQua == toQua) {
			return pi;
		}
	}
#endif
	return nullptr;
}


#if defined(WIN32)

QuaMidiIn *
QuaMidiManager::OpenInput(Qua *toQua, QuaMidiPort *p)
{
	QuaMidiIn	*con;
	con = inputConnectionForPort(toQua, p);
	if (con == nullptr) {
		con = new QuaMidiIn(toQua, p);
		if (con->Open() != B_OK) {
			delete con;
			return nullptr;
		}
		inputs.push_back(con);
	}
	return con;
}

QuaMidiOut *
QuaMidiManager::OpenOutput(Qua *fromQua, QuaMidiPort *p)
{
	QuaMidiOut	*con;
	con = outputConnectionForPort(fromQua, p);
	if (con == nullptr) {
		con = new QuaMidiOut(fromQua, p);
		if (con->Open() != B_OK) {
			delete con;
			return nullptr;
		}
		outputs.push_back(con);
	}
	return con;
}

bool
QuaMidiManager::closeInput(QuaMidiIn *c)
{
	c->Close();
	return true;
}

bool
QuaMidiManager::closeOutput(QuaMidiOut *c)
{
	c->Close();
	return true;
}

bool
QuaMidiManager::remove(QuaMidiOut *d)
{
	auto ci = std::find(outputs.begin(), outputs.end(), d);
	if (ci != outputs.end()) {
		outputs.erase(ci);
	}
	return true;
}

bool
QuaMidiManager::remove(QuaMidiIn *d)
{
	auto ci = std::find(inputs.begin(), inputs.end(), d);
	if (ci != inputs.end()) {
		inputs.erase(ci);
	}
	return true;
}

#endif

QuaPort *
QuaMidiManager::findPortByName(const string name, int direction, int nports) {
	for (auto p : ports) {
		if (p->hasMode(direction)) {
			return p;
		}
	}
	return nullptr;
}

status_t
QuaMidiManager::connect(Input *s)
{
	Qua	*toQua = s->channel->uberQua;
	s->src.midi = OpenInput(toQua, (QuaMidiPort *)s->device);
	if (s->src.midi != nullptr) {
		s->enabled = true;
	} else {
		s->enabled = false;
		return B_ERROR;
	}
	return B_OK;
}

status_t
QuaMidiManager::connect(Output *s)
{
	Qua	*toQua = s->channel->uberQua;
	s->dst.midi = OpenOutput(toQua, (QuaMidiPort *)s->device);
	if (s->dst.midi != nullptr) {
		s->enabled = true;
	} else {
		s->enabled = false;
		return B_ERROR;
	}
	return B_OK;
}

status_t
QuaMidiManager::disconnect(Input *s)
{
	s->enabled = false;
	bool	anyEnabled=false;
	Qua	*uberQua = s->channel->uberQua;
	for (short i=0;i<uberQua->nChannel; i++) {
		for (auto d: uberQua->channel[i]->activeStreamInputs) {
			if (d->enabled && d->device == s->device) {
				anyEnabled = true;
				break;
			}
		}
		if (anyEnabled)
			break;
	}
	if (!anyEnabled &&
		!closeInput(s->src.midi)) {
		fprintf(stderr, "Can't close input midi port %s", s->device->sym->name.c_str());
		return B_ERROR;
	}
	s->src.midi = nullptr;
	return B_OK;
}

status_t
QuaMidiManager::disconnect(Output *s)
{
	s->enabled = false;
	bool	anyEnabled=false;
	
	Qua	*uberQua = s->channel->uberQua;
	for (short i=0;i<uberQua->nChannel; i++) {
		for (auto d:uberQua->channel[i]->activeStreamOutputs) {
			if (d->enabled && d->device == s->device) {
				anyEnabled = true;
				break;
			}
		}
		if (anyEnabled)
			break;
	}
	if (!anyEnabled &&
		!closeOutput(s->dst.midi)) {
		fprintf(stderr, "Can't close input midi port %s", s->device->sym->name.c_str());
		return B_ERROR;
	}
	s->dst.midi = nullptr;
	return B_OK;
}




#ifdef WIN32

MIDIINCAPS *
QuaMidiManager::midiInDevices(int32 *nDevicesp)
{
	MIDIINCAPS     *mic;
	unsigned long  i, nDevices;

	nDevices = midiInGetNumDevs();
	mic = new MIDIINCAPS[nDevices];

	for (i = 0; i < nDevices; i++) {
		if (midiInGetDevCaps(i, &mic[i], sizeof(MIDIINCAPS)) != MMSYSERR_NOERROR) {
			nDevices = i;
			break;
//			printf("Device ID #%u: %s\r\n", i, mic.szPname);
		}
	}
	*nDevicesp = nDevices;
	return mic;
}


MIDIOUTCAPS *
QuaMidiManager::midiOutDevices(int32 *nDevicesp)
{
	MIDIOUTCAPS     *moc;
	unsigned long   i, nDevices;

	nDevices = midiOutGetNumDevs();
	moc = new MIDIOUTCAPS[nDevices];

	for (i = 0; i < nDevices; i++) {
		if (midiOutGetDevCaps(i, &moc[i], sizeof(MIDIOUTCAPS)) != MMSYSERR_NOERROR) {
			nDevices = i;
			break;
		}
	}
	*nDevicesp = nDevices;
	return moc;
}

const char *
QuaMidiManager::mmTechName(int techno)
{
	string s = qut::unfind(midiTechNameIndex, techno);
	return s.c_str();
}

#endif

#ifdef XXXX


HMIDIOUT    handle;

/* Open default MIDI Out device */
if (!midiOutOpen(&handle, (UINT)-1, 0, 0, CALLBACK_nullptr) )
{
    /* Output the C note (ie, sound the note) */
    midiOutShortMsg(handle, 0x00403C90);

    /* Output the E note */
    midiOutShortMsg(handle, 0x00404090);

    /* Output the G note */
    midiOutShortMsg(handle, 0x00404390);

    /* Here you should insert a delay so that you can hear the notes sounding */
    Sleep(1000);

    /* Now let's turn off those 3 notes */
    midiOutShortMsg(handle, 0x00003C90);
    midiOutShortMsg(handle, 0x00004090);
    midiOutShortMsg(handle, 0x00004390);

     /* Close the MIDI device */
     midiOutClose(handle);
}


HMIDIOUT    handle;
MIDIHDR     midiHdr;
UINT        err;
char		sysEx[] = {0xF0, 0x7F, 0x7F, 0x04, 0x01, 0x7F, 0x7F, 0xF7};

/* Open default MIDI Out device */
if (!midiOutOpen(&handle, (UINT)-1, 0, 0, CALLBACK_nullptr))
{
    /* Store pointer in MIDIHDR */
    midiHdr.lpData = (LPBYTE)&sysEx[0];

    /* Store its size in the MIDIHDR */
    midiHdr.dwBufferLength = sizeof(sysEx);

    /* Flags must be set to 0 */
    midiHdr.dwFlags = 0;

    /* Prepare the buffer and MIDIHDR */
    err = midiOutPrepareHeader(handle,  &midiHdr, sizeof(MIDIHDR));
    if (!err)
    {
        /* Output the SysEx message */
        err = midiOutLongMsg(handle, &midiHdr, sizeof(MIDIHDR));
        if (err)
        {
            char   errMsg[120];

            midiOutGetErrorText(err, &errMsg[0], 120);
            printf("Error: %s\r\n", &errMsg[0]);
        }

        /* Unprepare the buffer and MIDIHDR */
        while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR)))
        {
            /* Should put a delay in here rather than a busy-wait */		    
        }
    }

    /* Close the MIDI device */
    midiOutClose(handle);
}





/*********************** PrintMidiInErrorMsg() **************************
 * Retrieves and displays an error message for the passed MIDI In error
 * number. It does this using midiInGetErrorText().
 *************************************************************************/

void PrintMidiInErrorMsg(unsigned long err)
{
#define BUFFERSIZE 200
	char	buffer[BUFFERSIZE];
	
	if (!(err = midiInGetErrorText(err, &buffer[0], BUFFERSIZE)))
	{
		printf("%s\r\n", &buffer[0]);
	}
	else if (err == MMSYSERR_BADERRNUM)
	{
		printf("Strange error number returned!\r\n");
	}
	else if (err == MMSYSERR_INVALPARAM)
	{
		printf("Specified pointer is invalid!\r\n");
	}
	else
	{
		printf("Unable to allocate/lock memory!\r\n");
	}
}



	unsigned long	err;

// Open device #device_id
	if (!(err = midiInOpen(&handle, device_id, (DWORD)midiCallback, (DWORD)this, CALLBACK_FUNCTION)))	{
		/* Store length and pointer to our input buffer for System Exclusive messages in MIDIHDR */
		midiHdr.lpData = (LPBYTE)&SysXBuffer[0];
		midiHdr.dwBufferLength = sizeof(sysxBuffer);
		midiHdr.dwFlags = 0;

		/* Prepare the buffer and MIDIHDR */
		if (!(err = midiInPrepareHeader(handle, &midiHdr, sizeof(MIDIHDR))) {
			/* Queue MIDI input buffer */
			if (!(err = midiInAddBuffer(handle, &midiHdr, sizeof(MIDIHDR))) {
				;

			}
		}
	}
	if (err) PrintMidiInErrorMsg(err);

// kick input in ...................
	if (!(err = midiInStart(handle))) {
		PrintMidiInErrorMsg(err);
	}

// end input.......................

	/* We need to set a flag to tell our callback midiCallback()
	   not to do any more midiInAddBuffer(), because when we
	   call midiInReset() below, Windows will send a final
	   MIM_LONGDATA message to that callback. If we were to
	   allow midiCallback() to midiInAddBuffer() again, we'd
	   never get the driver to finish with our midiHdr
	*/
	sysxFlag |= 0x80;
	midiInReset(handle);/* Stop recording */

// close up shop .....

	while ((err = midiInClose(handle)) == MIDIERR_STILLPLAYING) Sleep(0);
	if (err) PrintMidiInErrorMsg(err);

		/* Unprepare the buffer and MIDIHDR. Unpreparing a buffer that has not been prepared is ok */
	midiInUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR));

#endif
