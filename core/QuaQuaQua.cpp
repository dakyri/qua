#include "qua_version.h"


#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <stdafx.h>
//#include <afxwin.h>
#endif
/*
 * Qua:
 * the hacked Be version
 *  more of the same. the guts of it
 */
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#include <string>

#include "StdDefs.h"

#include "Block.h"
#include "Voice.h"
#include "Sym.h"
#include "QuaMidi.h"
#include "QuaParallel.h"
#include "Qua.h"
#include "Expression.h"
#include "Note.h"
#include "Pool.h"
#include "Method.h"
#include "QuaJoystick.h"
#include "Template.h"
#include "MidiDefs.h"
#include "Metric.h"
#ifdef QUA_V_AUDIO
#include "QuaAudio.h"
#endif
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "Sym.h"


flag		debug_gen = 0;


/*
 * PutNextNote:
 */

ResultValue
CreateNewNote(int type, Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static 			Note	noteTo;
    ResultValue		val,
    				ret_val;
    

    noteTo.cmd = MIDI_NOTE_ON;
    
    ret_val.Set(TypedValue::S_NOTE, TypedValue::REF_POINTER);
	ret_val.SetValue((void *)NULL);
	ret_val.flags  = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    if (val.type == TypedValue::S_STRING)
    	noteTo.pitch = StringToNote(val.StringValue());
    else
    	noteTo.pitch = val.ByteValue(NULL);

    val = EvaluateExpression(P->next, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
	noteTo.dynamic = val.ByteValue(NULL);

    val = EvaluateExpression(P->next->next, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    noteTo.duration = val.FloatValue(NULL);

	noteTo.properties = NULL;
	noteTo.AddProperties(P->next->next->next);
	ret_val.SetValue(&noteTo);
	if (debug_gen)
		fprintf(stderr, "creating type %d: pitch %d dynamic %d duration %d\n", ret_val.type,
					noteTo.pitch,
					noteTo.dynamic,
					noteTo.duration
					);
   
    return ret_val;
}


ResultValue
CreateNewCtrl(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static Ctrl		ctrlTo;
//    float			dur;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
    ret_val.Set(TypedValue::S_CTRL, TypedValue::REF_POINTER);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	ctrlTo.controller = (ctrl_t)val.IntValue(NULL);

    val = EvaluateExpression(P->next, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    ctrlTo.amount = (amt_t)val.IntValue(NULL);

	if (debug_gen)
		fprintf(stderr, "creating %d: %d %d\n", ret_val.type,
					ctrlTo.controller,
					ctrlTo.amount);
	ret_val.SetValue(&ctrlTo);
   
    return ret_val;
}

ResultValue
CreateNewBend(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static Bend		bendTo;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_BEND, TypedValue::REF_POINTER);
	ret_val.SetValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	bendTo.bend = (bend_t)val.IntValue(NULL);

	if (debug_gen)
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					bendTo.bend);
	ret_val.SetValue(&bendTo);
   
    return ret_val;
}

ResultValue
CreateNewProg(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static Prog		progTo;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_PROG, TypedValue::REF_POINTER);
	ret_val.SetValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	progTo.program = (prg_t)val.IntValue(NULL);
	progTo.bank = NON_PROG;
	progTo.subbank = NON_PROG;
	if (debug_gen)
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					progTo.program);
	ret_val.SetValue(&progTo);
   
    return ret_val;
}

ResultValue
CreateNewSysX(Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
     static SysX	sysXTo;
     ResultValue	val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_SYSX, TypedValue::REF_POINTER);
	ret_val.SetValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
    if (val.type == TypedValue::S_METHOD) {
   		Method *S = val.MethodValue();
//   		S->sym->FindData(&SysXTo.data, &SysXTo.length);
   	} else {
   		sysXTo.length = 0;
   		sysXTo.data = 0;
   	}

	if (debug_gen)
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					sysXTo.length);
	ret_val.SetValue(&sysXTo);
   
    return ret_val;
}

ResultValue
CreateNewSysC(int which, Block *P,
			StreamItem *items,
			Stacker *stacker,
			StabEnt *stackCtxt,
			QuasiStack *stack)
{
    static SysC		sysCTo;
    ResultValue		val,
    				ret_val;
    bool			comp = true;
    
	ret_val.Set(TypedValue::S_SYSC, TypedValue::REF_POINTER);
	ret_val.SetValue((void*)NULL);
	ret_val.flags = ResultValue::COMPLETE;

	sysCTo.cmd = which;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	sysCTo.data1 = (int8)val.IntValue(NULL);

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
   	sysCTo.data2 = (int8)val.IntValue(NULL);

	if (debug_gen)
		fprintf(stderr, "creating %d: %d\n", ret_val.type,
					sysCTo.cmd);
	ret_val.SetValue(&sysCTo);
   
    return ret_val;
}

#ifdef QUA_V_APP_HANDLER
ResultValue
CreateNewMesg(Block *P,
			StreamItem *items,
			Stacker *stacker, QuasiStack *stack)
{
	BMessage		*msg;
    ResultValue		val,
    				ret_val;
    int				type;

	ret_val.Set(TypedValue::S_MESSAGE, REF_POINTER);
	ret_val.SetValue((void*)NULL);
	ret_val.complete = TRUE;

    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	if (val.Blocked()) {
		ret_val.flags |= ResultValue::BLOCKED;
		return ret_val;
	}
	if (!val.Complete()) {
		ret_val.flags |= ~ResultValue::COMPLETE;
	}
// mess id!
    type = val.IntValue(NULL);

	msg = new BMessage(type);
// items to add!	
	P = P->next;
	while (P != NULL) {
	    val = EvaluateExpression(P, items, stacker, stackCtxt, stack);
	    if (!val.blocked && val.type == TypedValue::S_LIST) {
	    	val.ListValue()->AddToMessage(msg);
	    	delete val.ListValue();
	    }
    	if (!val.complete) ret_val.complete = FALSE;
	    P = P->next;
	}
	ret_val.SetValue(msg);
   
    return ret_val;
}
#endif

ResultValue
Find(Stream *S, Block *Query)
{
	ResultValue ret_val;
	
	ret_val = TypedValue::List();

	for (StreamItem *p=S->head; p!=NULL; p=p->next) {
		ResultValue cond_val = EvaluateExpression(Query, p, NULL, NULL);
		if (cond_val.IntValue(NULL)) {
			ret_val.ListValue()->AddItem(p, TypedValue::S_STREAM_ITEM);
			break;
		}
	}
	return ret_val;
}

QuaGlobalContext::QuaGlobalContext()
{
	version = "qua 0.94, universal";
#ifdef QUA_V_AUDIO
	quaAudio = NULL;
#endif
#ifdef QUA_V_JOYSTICK
	quaJoystick = NULL;
#endif
    quaMidi = NULL;    	
    quaParallel = NULL;  
}

int
QuaGlobalContext::SetupDevices(Qua &q)
{
#ifdef QUA_V_AUDIO
	quaAudio = new QuaAudioManager(q);
#endif
#ifdef QUA_V_JOYSTICK
	quaJoystick = new QuaJoystickManager(q);
#endif
    quaMidi = new QuaMidiManager(q);    	
    quaParallel = new QuaParallelManager(q);  

	return B_OK;
}

bool
QuaGlobalContext::SetVstPluginDir(char *path, bool add, bool reload)
{
#ifdef QUA_V_VST_HOST
	std::string	cfgPath=configDirectory + "/" + "vstplugins.qs";
	FILE	*fp = fopen(cfgPath.c_str(), add?"w+":"w");
	if (fp == NULL) {
		return false;
	}
	setbuf(fp, NULL);
	VstPlugin::ScanFile(path, fp, NULL, true);
	fclose(fp);
#ifdef QUA_V_ARRANGER_INTERFACE
	if (reload) {
		display.RefreshVstPluginList();
	}
#endif
#endif
	return true;
}

QuaGlobalContext::~QuaGlobalContext()
{
#if defined(QUA_V_POOLPLAYER)
	delete poolPlayer;
#endif
#ifdef QUA_V_JOYSTICK
	delete quaJoystick;
#endif
#ifdef QUA_V_AUDIO
	delete quaAudio;
#endif
	delete quaMidi;
	delete quaParallel;
}
int
QuaGlobalContext::Setup()
{
#ifdef WIN32
#ifdef QUA_V_MULTIMEDIA_TIMER
	TIMECAPS		tc;
	timeGetDevCaps(&tc, sizeof(tc));
	fprintf(stderr, "multimedia timer period min %d max %d\n", tc.wPeriodMin, tc.wPeriodMax);
#endif
#endif

//	BPath	applicationBaseDirectory;
//	VstPlugin::LoadVstDirectories(&vstDirectories, &vstPlugins);
//	BList	preload;
// this is a bit random should eventually go in symbol table ...

	metrix.push_back(&Metric::std);
	metrix.push_back(&Metric::mSec);
	metrix.push_back(&Metric::sampleRate);


////////////////////
// set up paths
////////////////////
#if defined(WIN32)
#ifdef QUA_V_APPLICATION
	if ((quaAppModule=GetModuleHandle(NULL)) == NULL) {
		tragicError("Can't find Home...");
	}
	char	vuf[1024];
	int		nc;
	if ((nc=GetModuleFileName(quaAppModule, vuf, 1024)) == 0) { 
		TragicError("Can't find Home...");
	}
	appPath.SetTo(vuf);
	stderr, printf("App path %s\n", appPath.Path());
	appPath.GetParent(&homePath);
	configDirectory.SetTo(homePath.Path(),"QuaRC");
	fprintf(stderr, "App path %s\n", appPath.Path());
	fprintf(stderr, "App home path %s\n", homePath.Path());
	fprintf(stderr, "config directory %s\n", configDirectory.Path());
#endif
#endif

///////////////////////////////
// collect up pretty pictures
///////////////////////////////
// mostly for windows or max these would be compiled resources?? maybe not linux?
//	quaBigIcon->SetBits(quaiconbits, 1024, 0, B_CMAP8);
//	quaSmallIcon->SetBits(quasiconbits, 256, 0, B_CMAP8);
//	backgroundPattern = GetBitmapForImage(((char *)imgPath.Path()));
//	midiSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	joySmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioInSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioOutSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	midiBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	joyBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioInBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioOutBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
//	audioCtrlIcon = GetBitmapForImage(((char *)imgPath.Path()));


//////////////////////////////////
// load global stuff
//////////////////////////////////
#ifdef QUA_V_RC_INIT
	std::string	rcPath;
	if (!configDirectory.isValid) {
		configDirectory.Create();
	}
	while ((rcPath=configDirectory.NextFile()) != NULL) {
		FILE		*theFile;

		theFile = fopen(rcPath->Path(), "r");
		if (theFile != NULL) {
			char	*leaf = (char *)rcPath->Leaf();
			char	*ex = extension(leaf);
			if (leaf && ex) {
				if (strcmp(ex, "qs") == 0) {
					fprintf(stderr, "loading inititialization script from %s...\n", rcPath->Path());
					Parser		*p = new Parser(theFile, rcPath->Leaf(), NULL);
					if (p->ParseProgFile()) {
				
						fprintf(stderr, "parsed...\n");
		//				toolboxWindow->Lock();
						vstPlugins.AddList(&p->vstplugins);				
						Method		*nextm, *m;
						for(m=p->methods; m!=NULL; m=nextm) {
							nextm = m->next;
							if (m->sym->context == NULL) {
			 					if (m->Init()) {
#ifdef QUA_V_ARRANGER_INTERFACE
									display.CreateMethodBridge(m);
#endif
	//					    	S->next = methods;
	//					    	methods = S;
							}
							}
						}
				    
						Template	*t, *nextt;
						for(t=p->templates; t!=NULL; t=nextt) {
			    			nextt = t->next;
							if (t->sym->context == NULL) {
			 					if (t->Init()) {
#ifdef QUA_V_ARRANGER_INTERFACE
									display.CreateTemplateBridge(t);
#endif
	//								F->next = templates;
	//								templates = F;
								}
							}
						}
				    
						if (p->schedulees) {
			    			fprintf(stderr,
								"schedulable objects cannot be defined in null context");
						}
	//   				toolboxWindow->Unlock();
		
					}
				}
				fclose(theFile);
			}
		}
	}

#ifdef QUA_V_VST_HOST
	for (int i=0; i<vstPlugins.CountItems(); i++) {
		display.CreateVstPluginBridge(vstPlugins.ItemAt(i));
		if (vstPlugins.ItemAt(i)->loadPlugin) {
			if (vstPlugins.ItemAt(i)->Load() != B_OK) {
//				reportError("Load of %s failed\n", vstPlugins.ItemAt(i)->sym->name);
			}
		}
	}
#endif
#endif
	return B_OK;
}


char			*QuaCommandLine::usage_str = "\
Usage: qua [options|files]*\n\
Options:\n\
-help        this information\n\
-about       this information\n\
-version     display application version info\n\
-lint file   just try parseing with no load or run\n\
-listmidi    list available midi devices\n\
-listasio    list available asio drivers\n\
-listjoy     list available joystick devices\n\
-listglob    dump globals in symbol table\n\
-listvst path lists all vst plugins found in 'path' plus some gory details\n\
-setvst path sets up a new vst initialisation script adding all plugins in 'path'\n\
-addvst path adds the vst plugin at 'path' to the current vst initialisation script\n\
-wait time   following scripts will be processed until the given sequecer time (of the form bar:beat,tick\n\
-wait stop   following scripts will be processed until the sequencer halts\n\
\n\
Anything else is presumed to be a script file to be run,\n\
Scripts are run with preceding command line options/conditions set.";

int
QuaCommandLine::ProcessCommandLine(int argc, char **argv)
{
	int		i=1;
	bool	wait_stop=true;
	Time	end_time;

	while (i<argc) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
            ProcessCommandLineWord(i, argv[i]+1, true);
		} else {
            ProcessCommandLineWord(i, argv[i], false);
		}
		i++;
	}

	return 1;
}

QuaCommandLine::QuaCommandLine()
{
	commands = 0;
	last_command = 0;
}

bool
QuaCommandLine::ProcessCommandLineWord(long argno, char *arg, bool cmd)
{
	if (cmd) {
		if (strcmp(arg, "help") == 0 ||
			strcmp(arg, "about") == 0) {
			last_command = PRINT_HELP;
			commands |= PRINT_HELP;
			return true;
		} else if (strcmp(arg, "version") == 0) {
			last_command = PRINT_VERSION;
			commands |= PRINT_VERSION;
			return true;
		} else if (strcmp(arg, "setvst") == 0) {
			last_command = SET_VST;
			commands |= SET_VST;
			return true;
		} else if (strcmp(arg, "addvst") == 0) {
			last_command = ADD_VST;
			commands |= ADD_VST;
			return true;
		} else if (strcmp(arg, "listvst") == 0) {
			last_command = LIST_VST;
			commands |= LIST_VST;
			return true;
		} else if (strcmp(arg, "lint") == 0) {
			last_command = LINT_QUA;;
			commands |= LINT_QUA;
			return true;
		} else if (strcmp(arg, "listglob") == 0) {
			last_command = LIST_GLOB;
			commands |= LIST_GLOB;
			return true;
		} else if (strcmp(arg, "listjoy") == 0) {
			last_command = LIST_JOY;
			commands |= LIST_JOY;
			return true;
		} else if (strcmp(arg, "listasio") == 0) {
			last_command = LIST_ASIO;
			commands |= LIST_ASIO;
			return true;
		} else if (strcmp(arg, "loadasio") == 0) {
			last_command = LOAD_ASIO;
			commands |= LOAD_ASIO;
			return true;
		} else if (strcmp(arg, "listmidi") == 0) {
			last_command = LIST_MIDI;
			commands |= LIST_MIDI;
			return true;
		}
		/*
		else if (strcmp(arg, "wait") == 0) {
			if (++i > argc) {
				printf(usage_str);
				return 1;
			}
			if (strcmp(arg, "stop") == 0) {
				wait_stop = true;
			} else {
			}
		}*/
	} else {
			// anything else will be a file to run, using any prior commandline options/settings
		switch (last_command) {
			case LINT_QUA: {
				Qua * q = Qua::LoadScriptFile((char *)arg);
				if (q != NULL) {
					fprintf(stderr, "got a qua\n");
					q->sym->Dump(stderr, 0);
					delete q;
				}
				return true;
			}
#ifdef QUA_V_VST_HOST
			case SET_VST: {
				FILE	*fp = fopen("vstplugins.qs", "w");
				setbuf(fp, NULL);
				VstPlugin::ScanFile(arg, fp, NULL, true);
				fclose(fp);
				return true;
			}
			case ADD_VST: {
				FILE	*fp = fopen("vstplugins.qs", "w+");
				VstPlugin::ScanFile(arg, fp, NULL, true);
				fclose(fp);
				return true;
			}
#endif
#ifdef QUA_V_ASIO
			case LOAD_ASIO: {
				int n = atoi(arg);
				asioLoad.push_back(n);
				return true;
			}
#endif
			default: {
				loadNames.push_back(arg);
				break;
			}
		}

	}

	return false;
}

bool
QuaCommandLine::ListingCommands()
{
	if ((commands & (LIST_GLOB|LIST_VST|LIST_ASIO|LIST_MIDI|LIST_JOY)) != 0) {
		FILE	*fp = fopen("qua.lst", "w");
		fprintf(fp, "****** Qua listing ******\n");
		if (commands & LIST_GLOB) {
			glob.DumpGlobals(fp);
		}
#ifdef QUA_V_ASIO

		if (commands & LOAD_ASIO) {
			for (short i=0; i<asioLoad.CountItems(); i++) {
				LoadAsio((int)vstList.ItemAt(i), fp);
			}
		}
#endif
		if (commands & LIST_ASIO) {
			ListAsio(fp);
		}
		if (commands & LIST_MIDI) {
			ListMidi(fp);
		}
		if (commands & LIST_JOY) {
			ListJoy(fp);
		}
#ifdef QUA_V_VST_HOST
		if (commands & LIST_VST) {
			for (short i=0; i<vstList.CountItems(); i++) {
				VstPlugin::ScanFile((char *)vstList.ItemAt(i), NULL, fp, true);
			}
		}
#endif
		fclose(fp);
		return true;
	}
	return false;
}

void
QuaCommandLine::ListAsio(FILE *fp)
{
#ifdef QUA_V_AUDIO_ASIO
	int32		na = QuaAudioManager::asio.nDrivers;
	for (long i=0; i<na; i++) {
		char	*p = QuaAudioManager::asio.DriverName(i);
		if (p && *p) {
			fprintf(fp, "Asio device %d: %s\n", i, p);
		}
	}
#else
	fprintf(fp, "ASIO drivers not supported\n");
#endif
}

void
QuaCommandLine::LoadAsio(int devind, FILE *fp)
{
#ifdef QUA_V_AUDIO_ASIO
	int32		na = QuaAudioManager::asio.nDrivers;
	if (devind >= 0 && devind < na) {
		char	*p;
		if (p==NULL || !(*p)) {
			return;
		}
		status_t err = QuaAudioManager::asio.LoadDriver(p);
		if (err != ASE_OK) {
			fprintf(fp, "ASIO error while loading %s: %d\n", QuaAudioManager::asio.DriverName(devind), err);
			return;
		}
	} else {
		fprintf(fp, "Bad ASIO device number %d\n", devind);
		return;
	}
	fprintf(fp, "ASIO device %d: %s\n", devind, QuaAudioManager::asio.DriverName(devind));
	short j;
	fprintf(fp, "output latency %d\n", QuaAudioManager::asio.outputLatency);
	fprintf(fp, "buf sizes,  pref %d min %d, max %d\n",
		QuaAudioManager::asio.preferredBufSize,
		QuaAudioManager::asio.minBufSize,
		QuaAudioManager::asio.maxBufSize
	);
	for (j=0; j<QuaAudioManager::asio.nInputChannels; j++) {
		QuaAudioIn	*inp = QuaAudioManager::asio.input[j];
		fprintf(fp, "input %d: %s, sample type %s\n", j, inp->insertName, QuaAudioManager::SampleFormatName(inp->sampleFormat));
	}
	for (j=0; j<QuaAudioManager::asio.nOutputChannels; j++) {
		QuaAudioOut	*inp = QuaAudioManager::asio.output[j];
		fprintf(fp, "output %d: %s, sample type %s\n", j, inp->insertName, QuaAudioManager::SampleFormatName(inp->sampleFormat));
	}
	QuaAudioManager::asio.UnloadDriver();
#else
	fprintf(fp, "ASIO drivers not supported\n");
#endif
}

void
QuaCommandLine::ListMidi(FILE *fp)
{
#ifdef WIN32
#ifdef QUA_V_DIRECT_MIDI
	int32		ni = 0;
	int32		no = 0;
	MIDIINCAPS *icap = QuaMidiManager::midiInDevices(&ni);
	MIDIOUTCAPS *ocap = QuaMidiManager::midiOutDevices(&no);
	short j;
	fprintf(fp, "%d input ports and %d output ports\n", ni, no);
	for (j=0; j<ni; j++) {
		fprintf(fp, "In %u: \"%s\", product %d/%d driver %d/%d\n", j, wc2string(icap[j].szPname),
			icap[j].wMid, icap[j].wPid,
			icap[j].vDriverVersion&0xFF, (icap[j].vDriverVersion&0xFF00)>>8);
	}
	for (j=0; j<no; j++) {
		fprintf(fp, "Out %u: \"%s\", product %d/%d driver %d/%d: %s\n", j, wc2string(ocap[j].szPname),
			ocap[j].wMid, ocap[j].wPid,
			ocap[j].vDriverVersion&0xFF, (ocap[j].vDriverVersion&0xFF00)>>8,
//					ocap[j].wTechnology);
			QuaMidiManager::mmTechName(ocap[j].wTechnology));
	}
	delete icap;
	delete ocap;
#endif
#endif
}

void
QuaCommandLine::ListJoy(FILE *fp)
{
#ifdef QUA_V_JOYSTICK
#ifdef QUA_V_JOYSTICK_DX

#elif defined(QUA_V_JOYSTICK_MMC)
	int32		nJoy;
	short		j;
	JOYCAPS		*cap = QuaJoystickManager::GetJoyCaps(&nJoy);
	fprintf(fp, "%d mmc joysticks available\n", nJoy);
	if (nJoy > 0 && cap != NULL) {
		for (j=0; j<nJoy; j++) {
			fprintf(fp, "Joystick %d: %s, %d axes %d buttons %d hats\n",
				j, cap[j].szPname, cap[j].wNumAxes, cap[j].wNumButtons, (cap[j].wCaps&JOYCAPS_HASPOV)?1:0);
		}
	}
	delete cap;
#endif
#endif
}

