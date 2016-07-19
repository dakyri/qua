#ifndef _QUAVSTPLUGIN
#define _QUAVSTPLUGIN

#define LNGRNFRA_VST_NAME	24

#include <vector>
#include <string>
using namespace std;

#ifdef QUA_V_VST_HOST
#include "aeffectx.h"
#endif

#include "QuaDisplay.h"

#include "BaseVal.h"
#include "Stackable.h"

class Qua;

enum {
	VST_PLUG_UNSET=0,
	VST_PLUG_UNLOADED,
	VST_PLUG_LOADED,
	VST_PLUG_READY,
	VST_PLUG_LOST,
	VST_PLUG_BROKEN
};


class Stream;

class VstPlugin: public Stackable
{
public:
						VstPlugin(char *, char *name, bool, bool, bool, int32, int32, int32, int32);
						~VstPlugin();

	bool				mapParams;
	bool				loadPlugin;

	long				CheckPluginStatus();
	void				Init();
	
	status_t			SetPluginPath(char *);

	status_t			Load(ErrorHandler &display);
	status_t			Unload();
#ifdef QUA_V_VST_HOST
	static void			LoadTest(char *, ErrorHandler & display );
	static void			TestDrive(AEffect *, ErrorHandler & display);

	AEffect				*AEffectInstance();

	static long __cdecl HostCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
#endif
	string pluginExecutablePath;
	string name;
	bool enabled;

	status_t status;
#ifdef QUA_V_VST_HOST
	AEffect* (__cdecl* aEffectFactory)(audioMasterCallback);

	void* libhandle;
#endif
	static void ScanFile(string path, FILE *scriptFile, FILE *lstfile, bool recurse);

// plugin properties, so we can know beforehand
	char				effectName[32];	// original effect name. may be different to the qua name. qua names don't have spaces or odd characters.
	long				vstVersion;
	long				numPrograms;	// number of programs we have
	long				numParams;		// all programs are assumed to have numParams parameters
	long				numInputs;		// number of inputs
	long				numOutputs;		// number of outputs
	long				flags;			// see constants
	long				initialDelay;	// for algorithms which need input in the first place
	long				realQualities;	// number of realtime qualities (0: realtime)
	long				offQualities;	// number of offline qualities (0: realtime only)
	long				uniqueID;		// pls choose 4 character as unique as possible.
										// this is used to identify an effect for save+load
	long				version;		//

	long				resvd1;		// reserved, must be 0
	long				resvd2;		// reserved, must be 0
	float				ioRatio;		// input samplerate to output samplerate ratio, not used yet
	void				*object;		// for class access (see AudioEffect.hpp), MUST be 0 else!
	void				*user;			// user access

// vst1 capabilities
	bool				hasEditor;//Meaning: plugin has its own GUI editor
	bool				hasClip;//Meaning: plugin can provide information to drive a clipping display
	bool				hasVu;//Meaning: plugin can provide information to drive a VU display
	bool				canMono;//Meaning: plugin can accept mono input
	bool				canReplacing;//Meaning: plugin supports a replacing process
	bool				programChunks;
// vst2 capabilities
	bool				isSynth;	//Meaning: plugin will require MIDI events, and processEvents must be
									//called before process or processReplacing. Also, plugin cannot be
									//used as an insert or send effect and will declare its own number of
									//outputs, which should be assigned by the host as mixer channels

	bool				noSoundInStop;
	bool				extIsAsync;		//supports external DSP
	bool				hasBuffer;

	bool				sendVstEvents;
	bool				sendVstMidiEvent;
	bool				sendVstTimeInfo;
	bool				receiveVstEvents;
	bool				receiveVstMidiEvent;
	bool				receiveVstTimeInfo;
	bool				offline;
	bool				plugAsChannelInsert;
	bool				plugAsSend;
	bool				mixDryWet;

#ifdef QUA_V_VST_HOST
	VstPinProperties	*inputPinProperties;
	VstPinProperties	*outputPinProperties;
	VstParameterProperties	*paramProperties;


	static char	*			GetEffectName(AEffect *plugin);
	static char	*			GetErrorText(AEffect *plugin);
	static char	*			GetVendorName(AEffect *plugin);
	static long				GetVendorVersion(AEffect *plugin);
	static char	*			GetProductName(AEffect *plugin);
	static long				SetSampleRate(AEffect *, float sr);
	static long				SetBlockSize(AEffect *, long blocksize);
	static long				Open(AEffect *);
	static long				Close(AEffect *);
	static long				MainsOn(AEffect *);
	static long				MainsOff(AEffect *);
	static long				ConnectInput(AEffect *, long);
	static long				ConnectOutput(AEffect *, long);
	static long				DisconnectInput(AEffect *, long);
	static long				DisconnectOutput(AEffect *, long);
	static long				SetProgram(AEffect *, long);
	static long				GetProgram(AEffect *);
	static long				GetChunk(AEffect *, void **data, bool preset=false);
	static long				SetChunk(AEffect *, void *data, long size, bool preset=false);
	static long				CopyProgram(AEffect *, long);
	static long				SetProgramName(AEffect *, long, char *);
	static char *			GetProgramName(AEffect *, long);
	static void				SetParameter(AEffect *, long, float);
	static float			GetParameter(AEffect *, long);
	static string getParameterName(AEffect *, long ind);
	static string getParameterLabel(AEffect *, long ind);
	static string getParameterDisplay(AEffect *, long ind);
	static VstPinProperties	*GetOutputProperties(AEffect *, long ind);
	static VstPinProperties	*GetInputProperties(AEffect *, long ind);
	static VstParameterProperties	*GetParamProperties(AEffect *, long ind);

	void					OutputStream(AEffect *, Stream &S);
	void					InputStream(AEffect *, Stream &S);
#endif
#ifdef QUA_V_VST_HOST_GUI
	static	long			EditorGetRect(AEffect *afx, CRect &r);
	static	long			EditorOpen(AEffect *afx, HWND);
	static	long			EditorClose(AEffect *afx);
#endif

};

class VstPluginList: public std::vector<VstPlugin *>
{
public:
	VstPluginList();
	inline VstPlugin *itemAt(int i) {
		return at(i);
	}
	inline void add(VstPlugin *v) {
		push_back(v);
	}
	inline bool add(VstPluginList &vl) {
		for (auto v : vl) {
			push_back(v);
		}
		return true;
	}

	VstPlugin *item4Path(char *);
	VstPlugin *item4Name(char *);
};

#endif
