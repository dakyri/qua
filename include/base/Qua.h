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
#include <list>

class Voice;
class Lambda;
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
class B3dUniverse;

#include "Metric.h"
#include "QuaTime.h"
#include "QuaTypes.h"
#include "QuasiStack.h"
#include "Stacker.h"
#include "Executable.h"
#include "Clip.h"
#include "TimeKeeper.h"
#include "Stream.h"
#include "QuaDisplay.h"

char *NameStr(short s);

namespace tinyxml2 {
	class XMLElement;
}

#include <vector>
#include <thread>
#include <mutex>

typedef list<Instance*> sched_t;

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
	Qua(string nm, QuaReflection &display=defaultDisplay, bool chan_add=true);							
	~Qua();

	status_t OnCreationInit(bool chan_add);
	status_t PostCreationInit();
	long Start();
	status_t StartRecording();
	status_t StopRecording();
	long Pause();
	long Restart();
	long Stop();
	long Rewind();
	long ToNextMarker();
	long ToPreviousMarker();
	long FastForward();

	static string getVersionString();
	static string getCapabilityString();

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

#ifdef  QUA_V_SAVE_INITASXML
	status_t				DoSave(const char *file);
#endif

	void					AddSchedulable(Schedulable *p);
	void					RemoveSchedulable(Schedulable *p, bool andD, bool andDisplay);
	void					RemoveMethod(Lambda *p, bool andD, bool andDisplay);

	void					SetTempo(float tempo, bool display);

	Pool					*NewPool(short);
	Sample					*CreateSample(std::string, bool andD);
	Voice					*CreateVoice(std::string, bool andD);
	Lambda					*CreateMethod(std::string, StabEnt *ctxt, bool andD);
	bool					ParsePass2(class Parser *p, StabEnt **loadable);
	void setName(const string &nm, bool setTitle=true);
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

	static Qua *			loadScriptFile(const char *path, QuaReflection &display);
	status_t				loadSnapshotFile(const char *path);
	status_t				loadSnapshotElement(tinyxml2::XMLElement *element);
	status_t				loadSnapshotChildren(tinyxml2::XMLElement *element);

	std::string				projectScriptPath;
	std::string				projectSnapshotPath;
	std::string				projectDirectoryPath;
	std::string				sampleDirectoryPath;

	class QuasiStack *theStack;

	QuaReflection &bridge;

	Lambda *methods;
	Schedulable *schedulees;
	
	Channel *channel[MAX_CHANNEL];
	long nChannel;

#ifdef QUA_V_BETTER_SCHEDULER
	list<Instance*> schedule;
	list<Instance*>::iterator cueItem;
	std::vector<Instance*> activeInstances;
	std::mutex scheduleLock;

	status_t checkScheduledActivations();
	bool addToSchedule(Instance *);
	bool removeFromSchedule(Instance *);
	bool updateInstanceSchedule(Instance *);

	sched_t::iterator findSchedulePos(const sched_t::iterator it, const Time &t);
	sched_t::iterator findScheduleItem(const sched_t::iterator it, const Instance *i);
	sched_t::iterator findSchedulePos(const Time &t);
	sched_t::iterator findScheduleItem(const Instance *i);

	status_t Wake(Instance *inst);
	status_t Sleep(Instance *inst);
#endif

	void					Cue(Time &t);
	void					SetToRegionStart();
	void					SetToRegionEnd();

	status_t WaitUntilStop();
	status_t WaitUntil(Time &t);

	int						iterCounter;	// main iteration
	int						syncCounter;	// # of midi syncs
	
// clip of zero or less in length is a marker,
// clips here have no associated data ... ie 'take' field is null
	Clip *addClip(const std::string &nm, const Time&at, const Time&dur, const bool disp);
	Clip *addClip(const std::string &nm, bool disp);
	void removeClip(Clip *, bool disp);
	inline Clip *regionClip(long i) { return clips[i]; }
	long nClip() { return clips.size(); }

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

	bool					setTimingMode(flag, QuaPort *);
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


int stringToNote(); // this now serves no purpose other than to remind me that this all started in 1992 on an atari st;)


class QuaEnvironment;

extern QuaEnvironment	environment;



enum {
	QERR_WRONG_DEVICE=-5000
};

int select_list(int n, float pr[]);
bool roll(float chance);
float frandom();

#endif
