#ifndef _QUAQUA
#define _QUAQUA

#if defined(WIN32)
#ifdef QUA_V_MULTIMEDIA_TIMER
#include <Mmsystem.h>
#endif
#endif

#include <stdio.h>

#include <string>
#include <vector>

class Voice;
class Method;
class Pool;
class Channel;
class StabEnt;
class Application;
class StabEnt;
class Sample;
class Sampler;
class Clock;
class Schedulable;
class QuaPort;
class QuaAudioManager;
class QuaJoystickManager;
class QuaMidiManager;
class QuaParallelManager;

struct EnvelopeSegment;

// lest we forget ... 3d fly throughs of sequences in 1997!
// class B3dUniverse;

#include "Metric.h"
#include "QuaTime.h"
#include "QuaTypes.h"
#include "QuasiStack.h"
#include "Stacker.h"
#include "Executable.h"
#include "Clip.h"
#include "TimeKeeper.h"
#include "Stream.h"
#ifdef QUA_V_ARRANGER_INTERFACE
#include "QuaDisplay.h"
#endif

char				*NameStr(short s);

namespace tinyxml2 {
	class XMLElement;
}

#include <vector>
#include <thread>
#include <mutex>

struct QuaHead
{
	int32	q_magic;
};

struct QuaSaveHeadHdr
{
};

struct QuaSaveTailHdr
{
	int32	NArrangerObjects;
};

class Qua:
	public Executable, public Stacker, public TimeKeeper
{
public:
							Qua(std::string path, bool chan_add=true);
							Qua(char *nm, bool chan_add=true);							
							~Qua();
	status_t				OnCreationInit(bool chan_add);
	status_t				PostCreationInit();
	long					Start();
	status_t				StartRecording();
	status_t				StopRecording();
	long					Pause();
	long					Restart();
	long					Stop();
	long					Rewind();
	long					ToNextMarker();
	long					ToPreviousMarker();
	long					FastForward();


	void					SequencerIteration();
#ifdef QUA_V_MULTIMEDIA_TIMER
	static TIMECALLBACK MMEventProc;
	MMRESULT mmTimerId;
#else
	static int32 MainWrapper(void *arg);
	long Main();
	std::thread myThread;
#endif

	void					ResetTicks();

	void					MetroGnomeClick();

	status_t				Save(FILE *fp, short, bool clear_history);
	status_t				SaveInit(FILE *fp, short);
	status_t				WriteChunk(FILE *fp, ulong typ, void *, ulong);
	ulong					ReadChunk(FILE *fp, void **, ulong*);
	status_t				SaveQuap(FILE *fp);

	status_t				DoSave(const char *file);

	void					AddSchedulable(Schedulable *p);
	void					RemoveSchedulable(Schedulable *p, bool andD, bool andDisplay);
	void					RemoveMethod(Method *p, bool andD, bool andDisplay);

	void					SetTempo(float tempo, bool display);

	Pool					*NewPool(short);
	Sample					*CreateSample(std::string, bool andD);
	Voice					*CreateVoice(std::string, bool andD);
	Method					*CreateMethod(std::string, StabEnt *ctxt, bool andD);
	bool					ParsePass2(class Parser *p, StabEnt **loadable);
	void					SetName(char *nm, bool setTitle=true);
	void 					UpdateRecordDisplay();

	Channel					*AddChannel(
								std::string, short id,
								short nAudioIns, short nAudioOuts,
								bool au_thru, bool midi_thru,
								bool add_dflt_au_in,
								bool add_dflt_au_out,
								bool add_dflt_str_in,
								bool add_dflt_str_out,
								short midi_ch);
	status_t				RemoveChannel(short id, bool upDisp);
	status_t				MoveChannel(short id, short toId, bool updisp);
	void					SetTargets();		

	StabEnt					*loadFile(std::string path);
	StabEnt					*loadObject(StabEnt *obj);
	static std::string		identifyFile(std::string);
	static std::string		nameFromLeaf(std::string);

	static Qua *			LoadScriptFile(const char *path);
	status_t				LoadSnapshotFile(const char *path);
	status_t				LoadSnapshotElement(tinyxml2::XMLElement *element);
	status_t				LoadSnapshotChildren(tinyxml2::XMLElement *element);

	std::string				projectScriptPath;
	std::string				projectSnapshotPath;
	std::string				projectDirectoryPath;
	std::string				sampleDirectoryPath;

	class QuasiStack		*theStack;

	QuaBridge bridge;

	Method *methods;
	Schedulable *schedulees;
	
	Channel *channel[MAX_CHANNEL];
	long nChannel;

#ifdef QUA_V_BETTER_SCHEDULER
	status_t CheckScheduledActivations();
	bool AddToSchedule(Instance *);
	bool RemoveFromSchedule(Instance *);
	status_t				Wake(Instance *inst);
	status_t				Sleep(Instance *inst);

	Stream					schedule;
	StreamItem				*cueItem;
	std::vector<Instance*>	activeInstances;
	std::mutex				objectsBlockStack;
#endif

	void					Cue(Time &t);
	void					SetToRegionStart();
	void					SetToRegionEnd();

	status_t				WaitUntilStop();
	status_t				WaitUntil(Time &t);

	int						iterCounter;	// main iteration
	int						syncCounter;	// # of midi syncs
	
// clip of zero or less in length is a marker,
// clips here have no associated data ... ie 'take' field is null
	Clip					*addClip(std::string nm, Time&at, Time&dur, bool disp);
	Clip					*addClip(std::string nm, bool disp);
	void					removeClip(Clip *, bool disp);
	inline Clip				*regionClip(long i) { return clips[i]; }
	long					nClip() { return clips.size(); }

	void					SelectRegion(StabEnt *clipSym);
	void					GotoStartOfClip(StabEnt *clipSym);
	void					GotoEndOfClip(StabEnt *clipSym);

	long					CountEnvelopeSegments(StabEnt *, StabEnt *);
	void					FetchEnvelopeSegments(StabEnt *, StabEnt *, long, EnvelopeSegment *);

	Clip					*currentLoop;

	std::vector<Clip*>		clips;

	int8					loop;

	Clock					*theClock;
	
	bigtime_t				usecTime;
	bigtime_t				nextTickTime;
	bigtime_t				lastTickTime;
	bigtime_t				tickProcessEndTime;
	float					actualTime;
	Time					lastUpdate;

	int						currentBeatTick,
							currentBarBeat,
							currentBar;	
	double 			 		secsPerTick;
	long					usecsPerTick;

	bool					doMetroGnome;
	bool					autoCreateAmpEnv;
	bool					generateSync;
	bool					singleWindow;

	bool					SetTimingMode(flag, QuaPort *);
	flag					timingType;
	QuaPort					*timingDevice;
};

enum {
	QUA_TIME_INTERNAL = 0,			// run to our own loops and hope the wold keeps sync
	QUA_TIME_MIDI_CLOCK = 1,		// get timing from incoming midi clock
	QUA_TIME_MIDI_TIMECODE = 2,		// timing from incoming timecode
	QUA_TIME_AUDIO = 3,				// timing from audio subsystem
	QUA_TIME_ASIO = 4				// time specifically from asio subsystem
};


int						stringToNote();
// this now serves no purpose other than to remind me that this all started in 1992 on an atari st;)
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif

class QuaCommandLine
{
public:
	QuaCommandLine();

	int						ProcessCommandLine(int argc, char **argv);
	bool					ProcessCommandLineWord(long argno, char *arg, bool cmd);

	void					ListAsio(FILE *fp);
	void					ListMidi(FILE *fp);
	void					ListJoy(FILE *fp);
	bool					ListingCommands();
	void					LoadAsio(int n, FILE *fp);

	enum {
		LIST_GLOB = 0x0001,
		PRINT_VERSION= 0x0002,
		PRINT_HELP= 0x0004,
		LIST_VST =0x0008,
		SET_VST= 0x0010,
		ADD_VST= 0x0020,
		LIST_ASIO= 0x0040,
		LOAD_ASIO= 0x0080,
		LIST_MIDI =0x0100,
		LIST_JOY=0x0200,
		LINT_QUA= 0x0400
	};

	std::vector<std::string> asioLoad;
	std::vector<std::string> vstList;
	std::vector<char *> loadNames;
	std::vector<std::string> loaded;

	static char				*usage_str;
	long					commands;
	long					last_command;

  
};

class QuaEnvironment;

extern QuaEnvironment	environment;



// ...oh and some random global variables

#ifdef WIN32
#ifdef QUA_V_MFC
extern HINSTANCE		quaAppInstance;
extern HMODULE			quaAppModule;
#endif
#endif


enum {
	QERR_WRONG_DEVICE=-5000
};

int			select_list(int n, float pr[]);
bool		roll(float chance);
float		frandom();

#endif
