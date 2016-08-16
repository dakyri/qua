#include "qua_version.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <stdafx.h>
#include <afxwin.h>
#endif

#include <string>
#include <iostream>
using namespace std;
#include <boost/filesystem.hpp>

#ifdef QUA_V_VST_HOST
#include "aeffectx.h"
#include "AEffEditor.hpp"
#endif

#include "BaseVal.h"
#include "Stackable.h"
#include "Parse.h"
#include "VstPlugin.h"
#include "AudioBuffer.h"

#include "Sym.h"
#include "Qua.h"
#include "Stream.h"
#include "Note.h"
#include "QuasiStack.h"
#include "QuaAudio.h"
#include "QuaEnvironment.h"

// bogus avoidance of a messy issue of the
// HostCallback knowing which of the hostside
// VstPlugin objects it belongs to ... maybe
// these are global properties anyway??

flag debug_vst = 0;

VstPlugin::VstPlugin(const string &plugin, const string &nm, bool doLoad, bool doMap, bool isasyn, int32 ni, int32 no, int32 npar, int32 nprog):
	Stackable(DefineSymbol(nm, TypedValue::S_VST_PLUGIN, 0, this, nullptr, TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT))
{
	mapParams = doMap;
	loadPlugin = doLoad;
	numInputs = ni;
	numOutputs = no;
	numParams = npar;
	numPrograms = nprog;
#ifdef QUA_V_VST_HOST
	aEffectFactory = nullptr;
	inputPinProperties = nullptr;
	outputPinProperties = nullptr;
	paramProperties = nullptr;
#endif
	isSynth = isasyn;
	status = VST_PLUG_UNLOADED;	
	SetPluginPath(plugin);
	name = sym->name;
//	interfaceBridge.SetSymbol(sym);
//	cout << "path %s\n", pluginExecutable.Path());
//	afxVar.Set(TypedValue::S_STRANGE_POINTER, TypedValue::REF_STACK, sym,
//			(long)AllocStack(sym, TypedValue::S_STRANGE_POINTER, 1));
}

long
VstPlugin::CheckPluginStatus()
{
	return status;
}

void
VstPlugin::Init()
{
	status = VST_PLUG_UNSET;
}

#ifdef QUA_V_VST_HOST

AEffect *
VstPlugin::AEffectInstance()
{
	cout << "aeffect factory " << (unsigned) aEffectFactory << endl;
	if (aEffectFactory != nullptr) {
		return aEffectFactory(HostCallback);
	}
	return nullptr;
}

status_t
VstPlugin::Load(ErrorHandler & display)
{
	if (status == VST_PLUG_LOADED) {
		return B_OK;
	}
	libhandle=AfxLoadLibrary(pluginExecutablePath.c_str());
	if (libhandle!=nullptr) {
		std::cout << "loading "<< sym->name <<" from "<< pluginExecutablePath <<" ...\n";

		aEffectFactory=(AEffect*(__cdecl*)(audioMasterCallback))GetProcAddress((HMODULE)libhandle, "main");
		std::cout << "aeffect f " << aEffectFactory << endl;
		AEffect	*plugin = AEffectInstance();

		if (plugin == nullptr) {
			display.reportError("can't create plugin instance");
			status = VST_PLUG_UNLOADED;
			return B_ERROR;
		}
		if (plugin->magic != kEffectMagic) {
			display.reportError("plugin instance not a vst plugin");
			status = VST_PLUG_UNLOADED;
			return B_ERROR;
		}
		vstVersion = plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f);
		numPrograms = plugin->numPrograms;
		numParams = plugin->numParams;
		numInputs = plugin->numInputs;
		numOutputs = plugin->numOutputs;
		resvd1 = plugin->resvd1;
		resvd2 = plugin->resvd2;
		initialDelay = plugin->initialDelay;
		realQualities = plugin->realQualities;
		offQualities = plugin->offQualities;
		ioRatio = plugin->ioRatio;
		object = plugin->object;
		user = plugin->user;
		uniqueID = plugin->uniqueID;
		version = plugin->version;

		Open(plugin);

		//VST 1.0 flags
		hasEditor = (plugin->flags & effFlagsHasEditor)!=0;
		hasClip = (plugin->flags & effFlagsHasClip)!=0;
		hasVu = (plugin->flags & effFlagsHasVu)!=0;
		canMono = (plugin->flags & effFlagsCanMono)!=0;
		canReplacing = (plugin->flags & effFlagsCanReplacing)!=0;
		programChunks = (plugin->flags & effFlagsProgramChunks)!=0;
		vstVersion = (plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f));
		isSynth = (plugin->flags & effFlagsIsSynth)!=0;
		noSoundInStop = (plugin->flags & effFlagsNoSoundInStop)!=0;
		extIsAsync = (plugin->flags & effFlagsExtIsAsync)!=0;
		hasBuffer = (plugin->flags & effFlagsExtHasBuffer)!=0;
		//VST 2.0 Plugin Can Do's
		sendVstEvents = (plugin->dispatcher(plugin,effCanDo,0,0,"sendVstEvents",0.0f)>0);
		sendVstMidiEvent = (plugin->dispatcher(plugin,effCanDo,0,0,"sendVstMidiEvent",0.0f)>0);
		sendVstTimeInfo = (plugin->dispatcher(plugin,effCanDo,0,0,"sendVstTimeInfo",0.0f)>0);
		receiveVstEvents = (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstEvents",0.0f)>0);
		receiveVstMidiEvent = (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstMidiEvent",0.0f)>0);
		receiveVstTimeInfo = (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstTimeInfo",0.0f)>0);
		offline = (plugin->dispatcher(plugin,effCanDo,0,0,"offline",0.0f)>0);
		plugAsChannelInsert = (plugin->dispatcher(plugin,effCanDo,0,0,"plugAsChannelInsert",0.0f)>0);
		plugAsSend = (plugin->dispatcher(plugin,effCanDo,0,0,"plugAsSend",0.0f)>0);
		mixDryWet = (plugin->dispatcher(plugin,effCanDo,0,0,"mixDryWet",0.0f)>0);

		int i;
		if (vstVersion >= 2) {
			if (numInputs> 0) {
				inputPinProperties = new VstPinProperties[numInputs];
				if (plugin->dispatcher(plugin,effGetInputProperties,
						0,0,inputPinProperties,0.0f)==0) {
					delete inputPinProperties;
					inputPinProperties = nullptr; 
				} else {
					for (i=0; i<numInputs; i++) {
						if (plugin->dispatcher(plugin,effGetInputProperties,
								i,0,inputPinProperties+i,0.0f)==1) {
							cout << "plugin input "<< i <<" flags = " << inputPinProperties[i].flags << "\n"/*inputPinProperties[i].label,*/ ;
						}
					}
				}
			}
			if (numOutputs > 0) {
				outputPinProperties = new VstPinProperties[numOutputs];
				if (plugin->dispatcher(plugin,effGetOutputProperties,
						0,0,outputPinProperties,0.0f)==0) {
					delete outputPinProperties;
					outputPinProperties = nullptr; 
				} else {
					for (i=0; i<numOutputs; i++) {
						if (plugin->dispatcher(plugin,effGetOutputProperties,
								i,0,outputPinProperties+i,0.0f)==1) {
							cout << "plugin output "<< i <<" flags = "<< outputPinProperties[i].flags<<"\n" /*outputPinProperties[i].label,*/ ;
						}
					}
				}
			}
		}
		if (numParams > 0) {
			paramProperties = new VstParameterProperties[numParams];
			for (i=0; i<numParams; i++) {
				if (plugin->dispatcher(plugin,effGetParameterProperties,
						i,0,paramProperties+i,0.0f)==1) {
				} else {
					if (plugin->dispatcher(plugin,effGetParamName,
							i,0,&paramProperties[i].label,0.0f)==1) {
					}
					if (plugin->dispatcher(plugin,effGetParamName,
							i,0,&paramProperties[i].shortLabel,0.0f)==1) {
					}
				}

			}
		}

		DefineSymbol("program",
			TypedValue::S_VST_PROGRAM, 0, 0, sym,
			TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);

		if (true) {//mapParams) {			// define symbols for params
			for (i=0; i<numParams; i++) {
				string p = getParameterName(plugin, i);//paramProperties[i].shortLabel,
				if (p.size()) {
					DefineSymbol(p,
						TypedValue::S_VST_PARAM, 0, ((void*)i), sym,
						TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
				}
			}
		}
//		MainsOn(plugin);
		TestDrive(plugin, display);
//		MainsOff(plugin);
		Close(plugin);
		status = VST_PLUG_LOADED;
	} else { // can't load the dll, what next?
		display.reportError("load of vst plugin %s at %s fails", sym->name.c_str(), pluginExecutablePath.c_str());
		return B_ERROR;
	}

	return B_OK;
}

status_t
VstPlugin::Unload()
{
	if (libhandle!=nullptr) {
		aEffectFactory = nullptr;
		AfxFreeLibrary((HINSTANCE)libhandle);
		libhandle = nullptr;
	}
	status = VST_PLUG_UNLOADED;
	return B_OK;
}
#endif

status_t
VstPlugin::SetPluginPath(const string &plug)
{
	pluginExecutablePath = plug;
	status = VST_PLUG_UNLOADED;
	return B_OK;
}

VstPlugin::~VstPlugin()
{
#ifdef QUA_V_VST_HOST
	if (inputPinProperties)
		delete inputPinProperties;
	if (outputPinProperties)
		delete outputPinProperties;
	if (paramProperties)
		delete paramProperties;
#endif
}

// wrappers round commmon dispatcher calls
//	long (VSTCALLBACK *dispatcher)(AEffect *effect, long opCode, long index, long value,
//		void *ptr, float opt);

#ifdef QUA_V_VST_HOST
void
VstPlugin::LoadTest(char *plugpath, ErrorHandler & display)
{
	HINSTANCE libhandle=AfxLoadLibrary(plugpath);
	if (libhandle != nullptr) {
		cout << "loading from " << plugpath << " " << libhandle << "...\n";

		AEffect*			(__cdecl* aEffectFactory)(audioMasterCallback);
		aEffectFactory=(AEffect*(__cdecl*)(audioMasterCallback))GetProcAddress(libhandle, "main");

		AEffect	*plugin = aEffectFactory(HostCallback);

		if (plugin == nullptr) {
			display.reportError("can't create plugin instance");
			return;
		}
		if (plugin->magic != kEffectMagic) {
			display.reportError("plugin instance not a vst plugin");
			return;
		}
		long	vstVersion = plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f);
		long	numPrograms = plugin->numPrograms;
		long	numParams = plugin->numParams;
		long	numInputs = plugin->numInputs;
		long	numOutputs = plugin->numOutputs;
		long	resvd1 = plugin->resvd1;
		long	resvd2 = plugin->resvd2;
		long	initialDelay = plugin->initialDelay;
		long	realQualities = plugin->realQualities;
		long	offQualities = plugin->offQualities;
		long	ioRatio = plugin->ioRatio;
		void *	object = plugin->object;
		void *	user = plugin->user;
		long	uniqueID = plugin->uniqueID;
		long	version = plugin->version;
		int	blocksize=512;

		plugin->dispatcher(plugin,effOpen,0,0,nullptr,0.0f);
		plugin->dispatcher(plugin,effMainsChanged,0,0,nullptr,0.0f);
		plugin->dispatcher(plugin,effSetSampleRate,0,0,nullptr,44100);
		plugin->dispatcher(plugin,effSetBlockSize,0,blocksize,nullptr,0.0f);
	//switch the plugin back on (calls Resume)
		plugin->dispatcher(plugin,effMainsChanged,0,1,nullptr,0.0f);

	////////////////////////////////////////////////////////////////////////////

	//set to program zero and fetch back the name
		plugin->dispatcher(plugin,effSetProgram,0,0,nullptr,0.0f);

		char strProgramName[48];
		plugin->dispatcher(plugin,effGetProgramName,0,0,strProgramName,0.0f);

		cout <<  "Set plug to program zero, name is "<< strProgramName << " " << strlen(strProgramName) << "\n";

	//get the parameter strings
		char strName[24];
		char strDisplay[24];
		char strLabel[24];

		plugin->dispatcher(plugin,effGetParamName,0,0,strName,0.0f);
		cout << "Parameter name is " << strName << "\n";

		plugin->dispatcher(plugin,effGetParamLabel,0,0,strLabel,0.0f);
		cout << "Parameter label is " << strLabel << "\n";

		plugin->dispatcher(plugin,effGetParamDisplay,0,0,strDisplay,0.0f);
		cout << "Parameter display is " << strDisplay << "\n";

	//call the set and get parameter functions
		plugin->setParameter(plugin,0,0.7071f);
		float newval=plugin->getParameter(plugin,0);

		cout << "Parameter 0 was changed to " << newval << "\n";
		plugin->dispatcher(plugin,effGetParamDisplay,0,0,strDisplay,0.0f);
		cout << "Parameter display is now " << strDisplay << "\n";

		VstEvent* ptrEvents=nullptr;
		char* ptrEventBuffer=nullptr;
		int nEvents=2;

		if ((plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f)==2) &&
			((plugin->flags & effFlagsIsSynth) ||
			 (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstEvents",0.0f)>0))) {
			//create some MIDI messages to send to the plug if it is a synth or can
			//receive MIDI messages

			//create a block of appropriate size
			int pointersize=sizeof(VstEvent*);
			int bufferSize=sizeof(VstEvents)-(pointersize*2);
			bufferSize+=pointersize*(nEvents);

			//create the buffer
			ptrEventBuffer=new char[bufferSize+1];

			//now, create some memory for the events themselves
			VstMidiEvent* ptrEvents=new VstMidiEvent[nEvents];
			VstMidiEvent* ptrWrite=ptrEvents;

			//Create first event
			ptrWrite->type=kVstMidiType;
			ptrWrite->byteSize=24L;
			ptrWrite->deltaFrames=0L;
			ptrWrite->flags=0L;
			ptrWrite->noteLength=0L;
			ptrWrite->noteOffset=0L;

			ptrWrite->midiData[0]=(char)0x90;	//status & channel
			ptrWrite->midiData[1]=(char)36;	//MIDI byte #2
			ptrWrite->midiData[2]=(char)0xFF;	//MIDI byte #3
			ptrWrite->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

			ptrWrite->detune=0x00;
			ptrWrite->noteOffVelocity=0x00;
			ptrWrite->reserved1=0x00;
			ptrWrite->reserved2=0x00;

			//Create second event
			ptrWrite++;
			ptrWrite->type=kVstMidiType;
			ptrWrite->byteSize=24L;
			ptrWrite->deltaFrames=512;
			ptrWrite->flags=0L;
			ptrWrite->noteLength=0L;
			ptrWrite->noteOffset=0L;

			ptrWrite->midiData[0]=(char)0x90;	//status & channel
			ptrWrite->midiData[1]=(char)36;	//MIDI byte #2
			ptrWrite->midiData[2]=(char)0x00;	//MIDI byte #3
			ptrWrite->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

			ptrWrite->detune=0x00;
			ptrWrite->noteOffVelocity=0x00;
			ptrWrite->reserved1=0x00;
			ptrWrite->reserved2=0x00;

			//copy the addresses of our events into the eventlist structure
			VstEvents* ev=(VstEvents*)ptrEventBuffer;
			for (int i=0;i<nEvents;i++)
			{
				ev->events[i]=(VstEvent*)(ptrEvents+i);
			}

			//do the block header
			ev->numEvents=nEvents;
			ev->reserved=0L;

		}
		////////////////////////////////////////////////////////////////////////////
		float** ptrInputBuffers=nullptr;
		float** ptrOutputBuffers=nullptr;

		if (plugin->numInputs)
		{

			if (plugin->flags & effFlagsCanMono)
			{
				//only one input buffer required, others are just copies?
				//This could be an incorrect interpretation

				ptrInputBuffers=new float*[plugin->numInputs];

				//create the input buffers
				for (int i=0;i<plugin->numInputs;i++)
				{
					if (i==0)	
					{
						ptrInputBuffers[i]=new float[blocksize];
					}
					else	
					{
						ptrInputBuffers[i]=ptrInputBuffers[0];
					}
				}
			}
			else
			{
				//Plug requires independent input signals
				ptrInputBuffers=new float*[plugin->numInputs];

				//create the input buffers
				for (int i=0;i<plugin->numInputs;i++)
				{
					ptrInputBuffers[i]=new float[blocksize];
				}
			}
		}

		if (plugin->numOutputs)
		{
			ptrOutputBuffers=new float*[plugin->numOutputs];

			//create the output buffers
			for (int i=0;i<plugin->numOutputs;i++)
			{
				ptrOutputBuffers[i]=new float[blocksize];
			}
		}

		if (plugin->numInputs>0)
		{
			//fill the input buffers with ones to simulate some input having been
			//received

			//We don't have to do this for synths, obviously
			for (int i=0;i<plugin->numInputs;i++)
			{
				for (int j=0;j<blocksize;j++)
				{
					*(ptrInputBuffers[i]+j)=1.0f;
				}
			}
		}

		if (plugin->numOutputs>0)		{
			//fill the output buffers with ones to simulate some prior output to be
			//accumulated on
			for (int i=0;i<plugin->numOutputs;i++)
			{
				for (int j=0;j<blocksize;j++)
				{
					//*(ptrOutputBuffers[i]+j)=-1.0f;

					//note that VSTXsynth, when replacing, requires the host
					//to clear the buffer first!
					*(ptrOutputBuffers[i]+j)=0.0f;
				}
			}
		}

		if ((plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f)>1) &&
			((plugin->flags & effFlagsIsSynth) ||
			 (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstEvents",0.0f)>0)))
		{
	// problem with legacy cell
			if (plugin->dispatcher(plugin,effProcessEvents,0,0,(VstEvents*)ptrEventBuffer,0.0f)==1)
			{
				cout << "plug processed events OK and wants more" << endl;
			}
			else
			{
				cout << "plug does not want any more events\n";
			}
		}

		//process (replacing)
		if (plugin->flags & effFlagsCanReplacing)
		{
			cout <<  "Process (replacing)" << endl;
			plugin->processReplacing(plugin,ptrInputBuffers,ptrOutputBuffers,blocksize);
		}
	/*
		//process (accumulating)
		cout << "Process (accumulating)" << endl;
		ptrPlug->process(ptrPlug,ptrInputBuffers,ptrOutputBuffers,sampleframes);
	*/

		//Dump output to disk
		for (int i=0;i<plugin->numOutputs;i++)
		{
			for (int j=0;j<blocksize;j++)
			{
				cout << " " << i << " " << j << " " << *(ptrOutputBuffers[i] + j) << "\n";
			}
		}
		////////////////////////////////////////////////////////////////////////////
		FreeLibrary(libhandle);
	}
}



void
VstPlugin::TestDrive(AEffect *plugin, ErrorHandler & display)
{
	long i;
	int	blocksize=512;
	cout << "!!!!!!!!!!!! VST test drive !!!!!!!!!!!!!!!!!" << endl;
	cout << "" << plugin->numInputs << " inputs " << plugin->numOutputs << " outputs\n";
#define VST_TEST_MOD
#if defined(VST_TEST_MOD)
//	if (inputPinProperties)
//		for (i=0; i<plugin->numInputs; i++) {
//			cout << "plugin input %d flags = %x\n", i, inputPinProperties[i].flags);
//		}
//	if (inputPinProperties)
//		for (i=0; i<plugin->numOutputs; i++) {
//			cout << "plugin output %d flags = %x\n", i, outputPinProperties[i].flags);
//		}
//	for (short i=0; i<numPrograms; i++) {
//		cout << "prog %s\n", GetProgramName(plugin, i));
//	}
	Open(plugin);
	MainsOff(plugin);
	SetSampleRate(plugin, 44100);
	SetBlockSize(plugin, blocksize);
	MainsOn(plugin);

	////////////////////////////////////////////////////////////////////////////

	//set to program zero and fetch back the name
		SetProgram(plugin,0);

	for (short i=0; i<plugin->numParams; i++) {
		cout << "getting ";
		cout << "par " << i << " is " << plugin->getParameter(plugin, i) << "\n";
	}


//	VstEvent* ptrEvents=nullptr;
	int nEvents=1;
	int bufferSize=sizeof(VstEvents)+sizeof(VstEvent*)*(nEvents-2);

	//create the buffer
	char* enventListBuff=new char[bufferSize];

	//now, create some memory for the events themselves
	VstMidiEvent* midiEvents=new VstMidiEvent[nEvents];
	VstMidiEvent* midiEventP=midiEvents;

	midiEventP->type=kVstMidiType;
	midiEventP->byteSize=24L;
	midiEventP->deltaFrames=0L;
	midiEventP->flags=0L;
	midiEventP->noteLength=200000;
	midiEventP->noteOffset=0L;

	midiEventP->midiData[0]=(char)0x90;	//status & channel
	midiEventP->midiData[1]=(char)0x24;	//MIDI byte #2
	midiEventP->midiData[2]=(char)0xFF;	//MIDI byte #3
	midiEventP->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

	midiEventP->detune=0x00;
	midiEventP->noteOffVelocity=0x00;
	midiEventP->reserved1=0x00;
	midiEventP->reserved2=0x00;

//	midiEventP++;
//	midiEventP->type=kVstMidiType;
//	midiEventP->byteSize=24L;
//	midiEventP->deltaFrames=20000;
//	midiEventP->flags=0L;
//	midiEventP->noteLength=0;
//	midiEventP->noteOffset=0L;
//	midiEventP->midiData[0]=(char)0x90;	//status & channel
//	midiEventP->midiData[1]=(char)0x24;	//MIDI byte #2
//	midiEventP->midiData[2]=(char)0x00;	//MIDI byte #3
//	midiEventP->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

//	midiEventP->detune=0x00;
//	midiEventP->noteOffVelocity=0x00;
//	midiEventP->reserved1=0x00;
//	midiEventP->reserved2=0x00;

	//copy the addresses of our events into the eventlist structure
	VstEvents* ev=(VstEvents*)enventListBuff;
	for (int i=0;i<nEvents;i++)	{
		ev->events[i]=(VstEvent*)(midiEventP+i);
	}

	//do the block header
	ev->numEvents=nEvents;
	ev->reserved=0L;
	if (plugin->dispatcher(plugin,effProcessEvents,0,0,ev,0.0f)==1)	{
		cout << "More ev" << endl;
	} else {
		cout << "No more ev" << endl;
	}
	float **inTmp, **outTmp;
	outTmp = sample_buf_alloc(plugin->numOutputs, blocksize);
	inTmp = sample_buf_alloc(plugin->numInputs, blocksize);
	sample_buf_zero(inTmp, plugin->numInputs, blocksize);
	for (short j=0; j<plugin->numInputs; j++) {
		for (short i=0; i<256; i++) {
			inTmp[j][i] = .42;
		}
		for (short i=256; i<blocksize; i++) {
			inTmp[j][i] = 0;
		}
	}
	for (short k=0; k<2; k++) {
		if ((plugin->flags & effFlagsCanReplacing)) {
			for (short j=0; j<plugin->numOutputs; j++) {
				for (short i=0; i<blocksize; i++) {
					outTmp[j][i] = .42;
				}
			}
			cout << "processReplacing" << endl;
			plugin->processReplacing(plugin, inTmp, outTmp, blocksize);
		} else {
			cout << "process" << endl;
			sample_buf_zero(outTmp, plugin->numOutputs, blocksize);
			plugin->process(plugin, inTmp, outTmp, blocksize);
		}
//		for (short i=0; i<blocksize; i++) {
//			if (i %8 == 0) cout << "" << endl;
//			cout << "<%g %g ", inTmp[0][i], inTmp[1][i]);
//		}
#ifdef TESTDRIVE_VST
		cout << "" << endl;
		for (short i=0; i<blocksize; i++) {
			if (i %8 == 0) cout << "" << endl;
			cout << ">" << << " " << << " ", outTmp[0][i], outTmp[1][i]);
		}
#endif
	}
	sample_buf_free(inTmp, plugin->numInputs);
	sample_buf_free(outTmp, plugin->numOutputs);
#elif defined(VST_TEST_ORIG)
		plugin->dispatcher(plugin,effOpen,0,0,nullptr,0.0f);
		plugin->dispatcher(plugin,effMainsChanged,0,0,nullptr,0.0f);
		plugin->dispatcher(plugin,effSetSampleRate,0,0,nullptr,44100);
		plugin->dispatcher(plugin,effSetBlockSize,0,blocksize,nullptr,0.0f);
	//switch the plugin back on (calls Resume)
		plugin->dispatcher(plugin,effMainsChanged,0,1,nullptr,0.0f);

	////////////////////////////////////////////////////////////////////////////

	//set to program zero and fetch back the name
		plugin->dispatcher(plugin,effSetProgram,0,0,nullptr,0.0f);

//		char strProgramName[24+2];
//		plugin->dispatcher(plugin,effGetProgramName,0,0,strProgramName,0.0f);

//		cout << "Set plug to program zero, name is %s\n", strProgramName);

	//get the parameter strings
		char strName[24];
		char strDisplay[24];
		char strLabel[24];

		plugin->dispatcher(plugin,effGetParamName,0,0,strName,0.0f);
		cout << "Parameter name is " << << "\n", strName);

		plugin->dispatcher(plugin,effGetParamLabel,0,0,strLabel,0.0f);
		cout << "Parameter label is " << << "\n", strLabel);

		plugin->dispatcher(plugin,effGetParamDisplay,0,0,strDisplay,0.0f);
		cout << "Parameter display is " << << "\n", strDisplay);

	//call the set and get parameter functions
		plugin->setParameter(plugin,0,0.7071f);
		float newval=plugin->getParameter(plugin,0);

		cout << "Parameter 0 was changed to " << << "\n", newval);
		plugin->dispatcher(plugin,effGetParamDisplay,0,0,strDisplay,0.0f);
		cout << "Parameter display is now " << << "\n", strDisplay);

		VstEvent* ptrEvents=nullptr;
		char* ptrEventBuffer=nullptr;
		int nEvents=2;

		if ((plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f)==2) &&
			((plugin->flags & effFlagsIsSynth) ||
			 (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstEvents",0.0f)>0)))
		{
			//create some MIDI messages to send to the plug if it is a synth or can
			//receive MIDI messages

			//create a block of appropriate size
			int pointersize=sizeof(VstEvent*);
			int bufferSize=sizeof(VstEvents)-(pointersize*2);
			bufferSize+=pointersize*(nEvents);

			//create the buffer
			ptrEventBuffer=new char[bufferSize+1];

			//now, create some memory for the events themselves
			VstMidiEvent* ptrEvents=new VstMidiEvent[nEvents];
			VstMidiEvent* ptrWrite=ptrEvents;

			//Create first event
			ptrWrite->type=kVstMidiType;
			ptrWrite->byteSize=24L;
			ptrWrite->deltaFrames=0L;
			ptrWrite->flags=0L;
			ptrWrite->noteLength=0L;
			ptrWrite->noteOffset=0L;

			ptrWrite->midiData[0]=(char)0x90;	//status & channel
			ptrWrite->midiData[1]=(char)36;	//MIDI byte #2
			ptrWrite->midiData[2]=(char)0xFF;	//MIDI byte #3
			ptrWrite->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

			ptrWrite->detune=0x00;
			ptrWrite->noteOffVelocity=0x00;
			ptrWrite->reserved1=0x00;
			ptrWrite->reserved2=0x00;

			//Create second event
			ptrWrite++;
			ptrWrite->type=kVstMidiType;
			ptrWrite->byteSize=24L;
			ptrWrite->deltaFrames=512;
			ptrWrite->flags=0L;
			ptrWrite->noteLength=0L;
			ptrWrite->noteOffset=0L;

			ptrWrite->midiData[0]=(char)0x90;	//status & channel
			ptrWrite->midiData[1]=(char)36;	//MIDI byte #2
			ptrWrite->midiData[2]=(char)0x00;	//MIDI byte #3
			ptrWrite->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

			ptrWrite->detune=0x00;
			ptrWrite->noteOffVelocity=0x00;
			ptrWrite->reserved1=0x00;
			ptrWrite->reserved2=0x00;

			//copy the addresses of our events into the eventlist structure
			VstEvents* ev=(VstEvents*)ptrEventBuffer;
			for (int i=0;i<nEvents;i++)
			{
				ev->events[i]=(VstEvent*)(ptrEvents+i);
			}

			//do the block header
			ev->numEvents=nEvents;
			ev->reserved=0L;

		}
		////////////////////////////////////////////////////////////////////////////
		float** ptrInputBuffers=sample_buf_alloc(plugin->numInputs, blocksize);;
		float** ptrOutputBuffers=sample_buf_alloc(plugin->numOutputs, blocksize);;


		if (plugin->numInputs>0)
		{
			//fill the input buffers with ones to simulate some input having been
			//received

			//We don't have to do this for synths, obviously
			for (int i=0;i<plugin->numInputs;i++)
			{
				for (int j=0;j<blocksize;j++)
				{
					*(ptrInputBuffers[i]+j)=1.0f;
				}
			}
		}

		if (plugin->numOutputs>0)		{
			//fill the output buffers with ones to simulate some prior output to be
			//accumulated on
			for (int i=0;i<plugin->numOutputs;i++)
			{
				for (int j=0;j<blocksize;j++)
				{
					//*(ptrOutputBuffers[i]+j)=-1.0f;

					//note that VSTXsynth, when replacing, requires the host
					//to clear the buffer first!
					*(ptrOutputBuffers[i]+j)=0.0f;
				}
			}
		}

		if ((plugin->dispatcher(plugin,effGetVstVersion,0,0,nullptr,0.0f)>1) &&
			((plugin->flags & effFlagsIsSynth) ||
			 (plugin->dispatcher(plugin,effCanDo,0,0,"receiveVstEvents",0.0f)>0)))
		{
	// problem with legacy cell
			if (plugin->dispatcher(plugin,effProcessEvents,0,0,(VstEvents*)ptrEventBuffer,0.0f)==1)
			{
				cout << "plug processed events OK and wants more" << endl;
			}
			else
			{
				cout << "plug does not want any more events\n";
			}
		}

		//process (replacing)
		if (plugin->flags & effFlagsCanReplacing)
		{
			cout <<  "Process (replacing)" << endl;
			plugin->processReplacing(plugin,ptrInputBuffers,ptrOutputBuffers,blocksize);
		}
	/*
		//process (accumulating)
		cout << "Process (accumulating)" << endl;
		ptrPlug->process(ptrPlug,ptrInputBuffers,ptrOutputBuffers,sampleframes);
	*/

		//Dump output to disk
		for (int i=0;i<plugin->numOutputs;i++)
		{
			for (int j=0;j<blocksize;j++)
			{
				cout << "" << << " " << << " " << << "\n", i, j, *(ptrOutputBuffers[i]+j));
			}
		}
		////////////////////////////////////////////////////////////////////////////
#endif

}

char *
VstPlugin::GetEffectName(AEffect *plugin)
{
	static char nmb[32];
	nmb[0] = 0;
	plugin->dispatcher(plugin,effGetEffectName,0,0,nmb,0);
	return nmb;
}

char *
VstPlugin::GetErrorText(AEffect *plugin)
{
	static char nmb[256];
	nmb[0] = 0;
	plugin->dispatcher(plugin,effGetErrorText,0,0,nmb,0);
	return nmb;
}

char *
VstPlugin::GetVendorName(AEffect *plugin)
{
	static char nmb[64];
	nmb[0] = 0;
	plugin->dispatcher(plugin,effGetVendorString,0,0,nmb,0);
	return nmb;
}

char *
VstPlugin::GetProductName(AEffect *plugin)
{
	static char nmb[64];
	nmb[0] = 0;
	plugin->dispatcher(plugin,effGetProductString,0,0,nmb,0);
	return nmb;
}

long
VstPlugin::GetVendorVersion(AEffect *plugin)
{
	return plugin->dispatcher(plugin,effGetVendorVersion,0,0,nullptr,0);
}

long
VstPlugin::SetSampleRate(AEffect *plugin, float samplerate)
{
	return plugin->dispatcher(plugin,effSetSampleRate,0,0,nullptr,samplerate);
}

long
VstPlugin::SetBlockSize(AEffect *plugin, long blocksize)
{
	return plugin->dispatcher(plugin,effSetBlockSize,0,blocksize,nullptr,0.0f);
}


long
VstPlugin::GetProgram(AEffect *plugin)
{
	return plugin->dispatcher(plugin,effGetProgram,0,0,0,0.0f);
}

long
VstPlugin::SetProgram(AEffect *plugin, long ind)
{
	long v;
	v = plugin->dispatcher(plugin,effBeginSetProgram,0,0,0,0.0f);
	v = plugin->dispatcher(plugin,effSetProgram,0,ind,0,0.0f);
	v = plugin->dispatcher(plugin,effEndSetProgram,0,ind,0,0.0f);
	return v;
}

long
VstPlugin::GetChunk(AEffect *plugin, void **data, bool preset)
{
	long v;
	v = plugin->dispatcher(plugin,effGetChunk,preset,0,data,0.0f);
	return v;
}

long
VstPlugin::SetChunk(AEffect *plugin, void *data, long len, bool preset)
{
	long v;
	v = plugin->dispatcher(plugin,effSetChunk,preset,len,data,0.0f);
	return v;
}

long
VstPlugin::CopyProgram(AEffect *plugin, long ind)
{
	return plugin->dispatcher(plugin,effCopyProgram,0,ind,0,0.0f);
}

char *
VstPlugin::GetProgramName(AEffect *plugin, long ind)
{
	static char nm[24];
	nm[0] = 0;
	long pr = plugin->dispatcher(plugin,effGetProgram,0,0,0,0.0f);
	plugin->dispatcher(plugin,effSetProgram,0,ind,0,0.0f);
	plugin->dispatcher(plugin,effGetProgramName,0,0,nm,0.0f);
	plugin->dispatcher(plugin,effSetProgram,0,pr,0,0.0f);
	return nm;
}

long
VstPlugin::SetProgramName(AEffect *plugin, long ind, char *nm)
{
	status_t err;
	long pr = plugin->dispatcher(plugin,effGetProgram,0,0,0,0.0f);
	plugin->dispatcher(plugin,effSetProgram,0,ind,0,0.0f);
	err = plugin->dispatcher(plugin,effSetProgramName,0,0,nm,0.0f);
	plugin->dispatcher(plugin,effSetProgram,0,pr,0,0.0f);
	return err;
}

string
VstPlugin::getParameterName(AEffect *plugin, long ind)
{
	char nm[24]; // todo xxxx vst says this is up to 8 chars
	nm[0] = 0;
	string nnm;

	plugin->dispatcher(plugin,effGetParamName,ind,0,nm,0.0f);
	if ((nnm=SymTab::MakeValidSymbolName(nm)).size() == 0) {

		nnm += to_string(ind);
	}

	return nnm;
}

string
VstPlugin::getParameterLabel(AEffect *plugin, long ind)
{
	char nm[24];
	nm[0] = 0;
	plugin->dispatcher(plugin,effGetParamLabel,ind,0,nm,0.0f);
	return nm;
}

string
VstPlugin::getParameterDisplay(AEffect *plugin, long ind)
{
	char nm[24];
	plugin->dispatcher(plugin,effGetParamDisplay,ind,0,nm,0.0f);
	return nm;
}

long
VstPlugin::ConnectInput(AEffect *plugin, long ind)
{
	return plugin->dispatcher(plugin,effConnectInput,ind,1,nullptr,0.0f);
}

long
VstPlugin::DisconnectInput(AEffect *plugin, long ind)
{
	return plugin->dispatcher(plugin,effConnectInput,ind,0,nullptr,0.0f);
}

long
VstPlugin::ConnectOutput(AEffect *plugin, long ind)
{
	return plugin->dispatcher(plugin,effConnectOutput,ind,1,nullptr,0.0f);
}

long
VstPlugin::DisconnectOutput(AEffect *plugin, long ind)
{
	return plugin->dispatcher(plugin,effConnectOutput,ind,0,nullptr,0.0f);
}

long
VstPlugin::Open(AEffect *plugin)
{
	return plugin->dispatcher(plugin,effOpen,0,0,nullptr,0.0f);
}

long
VstPlugin::Close(AEffect *plugin)
{
	return plugin->dispatcher(plugin,effClose,0,0,nullptr,0.0f);
}

long
VstPlugin::MainsOn(AEffect *plugin)
{
	return plugin->dispatcher(plugin,effMainsChanged,0,1,nullptr,0.0f);
}

long
VstPlugin::MainsOff(AEffect *plugin)
{
	return plugin->dispatcher(plugin,effMainsChanged,0,0,nullptr,0.0f);
}

VstPinProperties *
VstPlugin::GetOutputProperties(AEffect *plugin, long ind)
{
	static VstPinProperties pr;
	if (!plugin->dispatcher(plugin,effGetOutputProperties,ind,0,&pr,0.0f))
		return nullptr;
	return &pr;
}

VstPinProperties *
VstPlugin::GetInputProperties(AEffect *plugin, long ind)
{
	static VstPinProperties pr;
	if (!plugin->dispatcher(plugin,effGetInputProperties,ind,0,&pr,0.0f)) {
		return nullptr;
	}
	return &pr;
}

VstParameterProperties *
VstPlugin::GetParamProperties(AEffect *plugin, long ind)
{
	static VstParameterProperties pr;
	if (!plugin->dispatcher(plugin,effGetParameterProperties,ind,0,&pr,0.0f)) {
		return nullptr;
	}
	return &pr;
}
#ifdef QUA_V_VST_HOST_GUI
//	effEditGetRect,		// stuff rect (top, left, bottom, right) into ptr
//	effEditOpen,		// system dependant Window pointer in ptr
//	effEditClose,		// no arguments
//	effEditDraw,		// draw method, ptr points to rect (MAC Only)
//	effEditMouse,		// index: x, value: y (MAC Only)
//	effEditKey,			// system keycode in value
//	effEditIdle,		// no arguments. Be gentle!
//	effEditTop,			// window has topped, no arguments
//	effEditSleep,		// window goes to background

long
VstPlugin::EditorGetRect(AEffect *afx, CRect &wrect)
{
	ERect	*plugrect;
//	plugrect.left = 0;
//	plugrect.right = 0;
//	plugrect.top = 0;
//	plugrect.bottom = 0;
	wrect.left = 0;
	wrect.right = 0;
	wrect.top = 0;
	wrect.bottom = 0;
	if (!afx->dispatcher(afx,effEditGetRect,0,0,&plugrect,0.0f)) {
		return 0;
	}
	wrect.left = plugrect->left;
	wrect.top = plugrect->top;
	wrect.right = plugrect->right;
	wrect.bottom = plugrect->bottom;
	return 1;
}

long
VstPlugin::EditorOpen(AEffect *afx, HWND wnd)
{
	if (!afx->dispatcher(afx,effEditOpen,0,0,wnd,0.0f)) {
		return 0;
	}
	return 1;
}

long
VstPlugin::EditorClose(AEffect *afx)
{
	if (!afx->dispatcher(afx,effEditClose,0,0,0,0.0f)) {
		return 0;
	}
	return 1;
}

#endif



// bit of an issue here
// with vst plugins that pay attention to
// channel for midi data. i pretty much lose channel data
// i think this makes better sense to me in the context
// that all this comes from, qua .. channels can
// multi out to several midi channels on several midi
// ports .... blahblah
// sys commons? maybe these should be executed as ctrls timing etc on
// plugin direct
void
VstPlugin::OutputStream(AEffect *plugin, Stream &S)
{
//	cout << "output stream to vst, vers %d issynth=%d send=%d/%d %d\n", vstVersion, isSynth, sendVstEvents, sendVstMidiEvent, S->nItems);
	if ((sendVstMidiEvent || isSynth) && S.nItems > 0) {
		int	nEvents = S.nItems;
// VstEvent has space for two events. ie if 1 event 
		int bufferSize=sizeof(VstEvents)-(sizeof(VstEvent*)*(nEvents-2));
		char *eventBuffer=new char[bufferSize];

		int		i;
		//now, create some memory for the events themselves
		VstMidiEvent* midievents=new VstMidiEvent[nEvents];
		VstMidiEvent* ep=midievents;
		StreamItem	*item = S.head;
		long	delta=0;
		for (i=0; i<S.nItems; i++) {
			ep->type=kVstMidiType;
			ep->byteSize=24L;
			ep->deltaFrames=0; 
			ep->flags=0L;
			ep->noteLength=0L;
			ep->noteOffset=0L;

			ep->midiData[3]=(char)0x00;	//MIDI byte #4 - blank

			ep->detune=0x00;
			ep->noteOffVelocity=0x00;
			ep->reserved1=0x00;
			ep->reserved2=0x00;

			switch (item->type) {
				case TypedValue::S_NOTE: {
					StreamNote *q;
					q =(StreamNote*)item;
					ep->midiData[0]=(char)q->note.cmd;	//status & channel
					ep->midiData[1]=(char)q->note.pitch;	//MIDI byte #2
					ep->midiData[2]=(char)q->note.dynamic;	//MIDI byte #3
					if (debug_vst) {
						cout << "note cmd " << (uchar)ep->midiData[0] << " " << (uchar)ep->midiData[1] << " " << (uchar)ep->midiData[2] << "\n";
					}
					if (q->note.duration >0 && q->note.duration < INFINITE_TICKS) {
						;
					}
					break;
				}
				
				case TypedValue::S_PROG: {
					StreamProg	*q;
					q = (StreamProg *)item;
					ep->midiData[0]=(char)q->prog.cmd;	//status & channel
					ep->midiData[1]=(char)q->prog.program;	//MIDI byte #2
					ep->midiData[2]=(char)0x00;	//MIDI byte #3
	
					break;
				}
				case TypedValue::S_BEND: {
					StreamBend	*q;
					q = (StreamBend *)item;
					ep->midiData[0]=(char)q->bend.cmd;	//status & channel
					ep->midiData[1]=(char)((q->bend.bend&0xf0)>>8);	//MIDI byte #2
					ep->midiData[2]=(char)(q->bend.bend&0x0f);	//MIDI byte #3
	//				SendABender(chan, q->bend.bend&0x08, q->bend.bend>>8);
					break;
				}
				case TypedValue::S_CTRL: {
					StreamCtrl	*q;
					q = (StreamCtrl *)item;
					ep->midiData[0]=(char)q->ctrl.cmd;	//status & channel
					ep->midiData[1]=(char)q->ctrl.controller;	//MIDI byte #2
					ep->midiData[2]=(char)q->ctrl.amount;	//MIDI byte #3
	//				SendControl(chan, q->ctrl.controller, q->ctrl.amount);
					break;
				}
	//			case TypedValue::S_SYSX: {
	//				StreamSysX	*q = (StreamSysX *)p;
	//				SendSystemExclusive(q->sysX.data, q->sysX.length);
	//				break;
	//			}
				case TypedValue::S_SYSC: {
					StreamSysC	*q;
					q = (StreamSysC *)item;
					ep->midiData[0]=(char)q->sysC.cmd;	//status & channel
					ep->midiData[1]=(char)q->sysC.data1;	//MIDI byte #2
					ep->midiData[2]=(char)q->sysC.data2;	//MIDI byte #3
	//				SendSystemCommon(q->sysC.cmd, q->sysC.data1, q->sysC.data2);
					break;
				}
			}

			item = item->next;
			ep++;
		}

		//copy the addresses of our events into the eventlist structure
		VstEvents* ev=(VstEvents*)eventBuffer;
		for (int i=0;i<nEvents;i++)
		{
			ev->events[i]=(VstEvent*)(midievents+i);
		}

		//do the block header
		ev->numEvents=nEvents;
		ev->reserved=0L;
		if (debug_vst) {
			cout << "" << ev->numEvents << " events\n";
		}
		if (plugin->dispatcher(plugin,effProcessEvents,0,0,(VstEvents*)eventBuffer,0.0f)==1) {
//			cout << "Vstplugin::Output Stream dispatcher liked that" << endl;
		} else {
//			cout << "Vstplugin::Output Stream didn't like that" << endl;
		}
		delete eventBuffer;
		delete midievents;
	}
}

void
VstPlugin::InputStream(AEffect *plugin, Stream &S)
{
	;
}


//////////////////////////////////////////////
//host callback function
//this is called directly by the plug-in!!
//////////////////////////////////////////////
long VSTCALLBACK
VstPlugin::HostCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
	static VstTimeInfo	the_time;
	long retval=0;

	switch (opcode)	{
		//VST 1.0 opcodes
		case audioMasterVersion:
			cout << "Vst callback audioMasterVersion" << endl;
			//Input values:	none
			// Return Value:
			//	0 or 1 for old version
			//	2 or higher for VST2.0 host?
			retval=2;
			break;

		case audioMasterAutomate:
			cout << "Vst callback audioMasterAutomate plug id " << effect->magic << ", param " << index << " <- " << opt << "\n";
			//Input values:
			//	<index> parameter that has changed
			//<opt> new value
			//	Return value:
			//	not tested, always return 0
			//NB - this is called when the plug calls
			//setParameterAutomated
			break;

		case audioMasterCurrentId:
			cout << "Vst callback audioMasterCurrentId" << endl;
			//Input values:none
			//Return Value
			//the unique id of a plug that's currently loading
			//zero is a default value and can be safely returned if not known
			break;

		case audioMasterIdle:
			cout << "Vst callback audioMasterIdle" << endl;
			//Input values: none
			//Return Value
			//not tested, always return 0
			//NB - idle routine should also call effEditIdle for all open editors
//			Sleep(1);
			break;

		case audioMasterPinConnected:
			cout << "Vst callback audioMasterPinConnected" << endl;
			//Input values:
			//	<index> pin to be checked
			//	<value> 0=input pin, non-zero value=output pin

			//Return values:
			//	0=true, non-zero=false
			break;

		//VST 2.0 opcodes
		case audioMasterWantMidi:
			cout << "Vst callback audioMasterWantMidi "<< value <<"\n";
			//Input Values:
			//	<value> filter flags (which is currently ignored, no defined flags?)
			//Return Value:
			//	not tested, always return 0
			break;

		case audioMasterGetTime: {
			cout << "Vst callback audioMasterGetTime" << endl;
			//Input Values:
			//	<value> should contain a mask indicating which fields are required
			//	...from the following list?
			the_time.samplePos = 0;			// double, current location
			the_time.sampleRate = 44100;		// double 
			the_time.flags = 0;					// see below
			if (value & kVstNanosValid) {
				cout << "kVstNanosValid" << endl;
				the_time.nanoSeconds = 0;			// double, system time
				the_time.flags |= kVstNanosValid;
			}
			if (value & kVstPpqPosValid) {
				cout << "kVstPpqPosValid" << endl;
				the_time.ppqPos=0;				// double, 1 ppq
				the_time.flags |= kVstPpqPosValid;
			}
			if (value & kVstTempoValid) {
				cout << "kVstTempoValid" << endl;
				the_time.tempo=120;				// double, in bpm
				the_time.flags |= kVstTempoValid;
			}
			if (value & kVstBarsValid) {
				cout << "kVstBarsValid" << endl;
				the_time.barStartPos = 0;			// double last bar start, in 1 ppq
				the_time.flags |= kVstBarsValid;
			}
			if (value & kVstCyclePosValid) {
				cout << "kVstCyclePosValid" << endl;
				the_time.cycleStartPos=0;		// double, 1 ppq
				the_time.cycleEndPos=16;			// double, 1 ppq
				the_time.flags |= kVstNanosValid;
			}
			if (value & kVstTimeSigValid) {
				cout << "kVstTimeSigValid" << endl;
				the_time.timeSigNumerator = 4;		// long, time signature
				the_time.timeSigDenominator = 4;
				the_time.flags |= kVstTimeSigValid;
			}
			if (value & kVstSmpteValid) {
				cout << "kVstSmpteValid" << endl;
				the_time.smpteOffset = 0;			// long, smpte offset
				the_time.smpteFrameRate = 0;		// long, 0:24, 1:25, 2:29.97, 3:30, 4:29.97 df, 5:30 df
				the_time.flags |= kVstSmpteValid;
			}
			if (value & kVstClockValid) {
				the_time.samplesToNextClock=24;	// long, midi clock resolution (24 ppq), can be negative
				cout << "kVstClockValid" << endl;
				the_time.flags |= kVstClockValid;
			}
			//Return Value:
			//	ptr to populated const VstTimeInfo structure (or 0 if not supported)
			//NB - this structure will have to be held in memory for long enough
			//for the plug to safely make use of it
			return (long) &the_time;
			break;
		}

		case audioMasterProcessEvents:
			cout << "Vst callback audioMasterProcessEvents" << endl;
			//Input Values:
			//	<ptr> Pointer to a populated VstEvents structure

			//Return value:
			//	0 if error
			//	1 if OK
			break;

		case audioMasterSetTime:
			cout << "Vst callback audioMasterSetTime" << endl;
			//IGNORE!
			break;

		case audioMasterTempoAt:
			cout << "Vst callback audioMasterTempoAt" << endl;
			//Input Values:
			//	<value> sample frame location to be checked
			//Return Value:
			//	tempo (in bpm * 10000)
			break;

		case audioMasterGetNumAutomatableParameters:
			cout << "Vst callback audioMasterGetNumAutomatableParameters" << endl;
			//Input Values:
			//	None
			//Return Value:
			//	number of automatable parameters
			//	zero is a default value and can be safely returned if not known
			//NB - what exactly does this mean? can the host set a limit to the
			//number of parameters that can be automated?
			break;

		case audioMasterGetParameterQuantization:
			cout << "Vst callback audioMasterGetParameterQuantization" << endl;
			//Input Values:
			//	None
			//Return Value:
			//	integer value for +1.0 representation,
			//	or 1 if full single float precision is maintained
			//	in automation.
			//NB - ***possibly bugged***
			//Steinberg notes say "parameter index in <value> (-1: all, any)"
			//but in aeffectx.cpp no parameters are taken or passed
			break;

		case audioMasterIOChanged:
			cout << "Vst callback audioMasterIOChanged" << endl;
			//Input Values:
			//	None
			//Return Value:
			//	0 if error
			//non-zero value if OK
			break;

		case audioMasterNeedIdle:
			cout << "Vst callback audioMasterNeedIdle" << endl;
			//Input Values:None
			//Return Value:
			//	0 if error
			//	non-zero value if OK
			//NB plug needs idle calls (outside its editor window)
			//this means that effIdle must be dispatched to the plug
			//during host idle process and not effEditIdle calls only when its
			//editor is open
			//Check despatcher notes for any return codes from effIdle
			break;

		case audioMasterSizeWindow:
			cout << "Vst callback audioMasterSizeWindow" << endl;
			//Input Values:
			//	<index> width
			//	<value> height
			//Return Value:
			//	0 if error
			//	non-zero value if OK
			break;

		case audioMasterGetSampleRate:
			cout << "Vst callback audioMasterGetSampleRate" << endl;
			//Input Values:None
			//Return Value:
			//	not tested, always return 0
			//NB - Host must despatch effSetSampleRate to the plug in response
			//to this call
			//Check despatcher notes for any return codes from effSetSampleRate
			effect->dispatcher(effect,effSetSampleRate,0,0,nullptr, getAudioManager().sampleRate);
			break;

		case audioMasterGetBlockSize:
			cout << "Vst callback audioMasterGetBlockSize" << endl;
			//Input Values:None
			//Return Value:
			// not tested, always return 0
			//NB - Host must despatch effSetBlockSize to the plug in response
			//to this call
			//Check despatcher notes for any return codes from effSetBlockSize
			effect->dispatcher(effect,effSetBlockSize,0, getAudioManager().bufferSize, nullptr,0.0f);
			break;

		case audioMasterGetInputLatency:
			cout << "Vst callback audioMasterGetInputLatency" << endl;
			//Input Values:None
			//Return Value:
			//	input latency (in sampleframes?)
			break;

		case audioMasterGetOutputLatency:
			cout << "Vst callback audioMasterGetOutputLatency" << endl;
			//Input Values:None
			//Return Value:
			//	output latency (in sampleframes?)
			break;

		case audioMasterGetPreviousPlug:
			cout << "Vst callback audioMasterGetPreviousPlug" << endl;
			//Input Values:None
			//Return Value:
			//	pointer to AEffect structure or nullptr if not known?
			//NB - ***possibly bugged***
			//Steinberg notes say "input pin in <value> (-1: first to come)"
			//but in aeffectx.cpp no parameters are taken or passed
			break;

		case audioMasterGetNextPlug:
			cout << "Vst callback audioMasterGetNextPlug" << endl;
			//Input Values:None
			//Return Value:
			//	pointer to AEffect structure or nullptr if not known?
			//NB - ***possibly bugged***
			//Steinberg notes say "output pin in <value> (-1: first to come)"
			//but in aeffectx.cpp no parameters are taken or passed
			break;

		case audioMasterWillReplaceOrAccumulate:
			cout << "Vst callback audioMasterWillReplaceOrAccumulate" << endl;
			//Input Values:None
			//Return Value:
			//0: not supported
			//1: replace
			//2: accumulate
			break;

		case audioMasterGetCurrentProcessLevel:
			cout << "Vst callback audioMasterGetCurrentProcessLevel" << endl;
			//Input Values:None
			//Return Value:
			//0: not supported,
			//1: currently in user thread (gui)
			//2: currently in audio thread (where process is called)
			//3: currently in 'sequencer' thread (midi, timer etc)
			//4: currently offline processing and thus in user thread
			//other: not defined, but probably pre-empting user thread.
			break;

		case audioMasterGetAutomationState:
			cout << "Vst callback audioMasterGetAutomationState" << endl;
			//Input Values:None
			//Return Value:
			//0: not supported
			//1: off
			//2:read
			//3:write
			//4:read/write
			break;

		case audioMasterGetVendorString:
			cout << "Vst callback audioMasterGetVendorString" << endl;
			//Input Values:
			//	<ptr> string (max 64 chars) to be populated
			//Return Value:
			//	0 if error
			//	non-zero value if OK
			break;

		case audioMasterGetProductString:
			cout << "Vst callback audioMasterGetProductString" << endl;
			//Input Values:
			//	<ptr> string (max 64 chars) to be populated

			//Return Value:
			//	0 if error
			//	non-zero value if OK
			break;

		case audioMasterGetVendorVersion:
			cout << "Vst callback audioMasterGetVendorVersion" << endl;
			//Input Values:None
			//Return Value:
			//Vendor specific host version as integer
			break;

		case audioMasterVendorSpecific:
			cout << "Vst callback audioMasterVendorSpecific" << endl;
			//Input Values:
			//<index> lArg1
			//<value> lArg2
			//<ptr> ptrArg
			//<opt>	floatArg
			//Return Values:
			//Vendor specific response as integer
			break;

		case audioMasterSetIcon:
			cout << "Vst callback audioMasterSetIcon" << endl;
			//IGNORE
			break;

		case audioMasterCanDo:
			cout << "Vst callback audioMasterCanDo '"<< (char*)ptr <<"'\n";
			//Input Values:
			//	<ptr> predefined "canDo" string
			//Return Value:
			//0 = Not Supported
			//non-zero value if host supports that feature
			//NB - Possible Can Do strings are:
			//"sendVstEvents",
			//"sendVstMidiEvent",
			//"sendVstTimeInfo",
			//"receiveVstEvents",
			//"receiveVstMidiEvent",
			//"receiveVstTimeInfo",
			//"reportConnectionChanges",
			//"acceptIOChanges",
			//"sizeWindow",
			//"asyncProcessing",
			//"offline",
			//"supplyIdle",
			//"supportShell"
			if (  strcmp((char*)ptr,"sendVstEvents")==0 ||
				  strcmp((char*)ptr,"sendVstMidiEvent")==0 ||
//				  strcmp((char*)ptr,"sendVstTimeInfo")==0 ||
				  strcmp((char*)ptr,"receiveVstEvents")==0 ||
				  strcmp((char*)ptr,"receiveVstMidiEvent")==0 ||
//				  strcmp((char*)ptr,"receiveVstTimeInfo")==0 ||
				  strcmp((char*)ptr,"reportConnectionChanges")==0 ||
				  strcmp((char*)ptr,"supplyIdle")==0)	{
				retval=1;
			}else{
				retval=0;
			}

			break;

		case audioMasterGetLanguage:
			cout << "Vst callback audioMasterGetLanguage" << endl;
			//Input Values:None
			//Return Value:
			//	kVstLangEnglish
			//	kVstLangGerman
			//	kVstLangFrench
			//	kVstLangItalian
			//	kVstLangSpanish
			//	kVstLangJapanese
			retval=1;
			break;
/*
		MAC SPECIFIC?
*/

		case audioMasterOpenWindow:
			//Input Values:
			//<ptr> pointer to a VstWindow structure

			//Return Value:
			//0 if error
			//else platform specific ptr
			cout << "Vst callback audioMasterOpenWindow" << endl;
			break;

		case audioMasterCloseWindow:
			//Input Values:
			//<ptr> pointer to a VstWindow structure

			//Return Value:
			//0 if error
			//Non-zero value if OK
			cout << "Vst callback audioMasterCloseWindow" << endl;
			break;

		case audioMasterGetDirectory:
			cout << "Vst callback audioMasterGetDirectory" << endl;
			//Input Values:None
			//Return Value:
			//	0 if error
			//	FSSpec on MAC, else char* as integer
			//NB Refers to which directory, exactly?
			break;

		case audioMasterUpdateDisplay:
			cout << "Vst callback audioMasterUpdateDisplay" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;
	//---from here VST 2.1 extension opcodes------------------------------------------------------
		case audioMasterBeginEdit:               // begin of automation session (when mouse down), parameter index in <index>
			cout << "Vst callback audioMasterBeginEdit" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;
		case audioMasterEndEdit:                 // end of automation session (when mouse up),     parameter index in <index>
			cout << "Vst callback audioMasterEndEdit" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;
		case audioMasterOpenFileSelector:		// open a fileselector window with VstFileSelect* in <ptr>
			cout << "Vst callback audioMasterOpenFileSelector" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;
	
	//---from here VST 2.2 extension opcodes------------------------------------------------------
		case audioMasterCloseFileSelector:		// close a fileselector operation with VstFileSelect* in <ptr>: Must be always called after an open !
			cout << "Vst callback audioMasterCloseFileSelector" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;
		case audioMasterEditFile:				// open an editor for audio (defined by XML text in ptr)
			cout << "Vst callback audioMasterEditFile" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;
		case audioMasterGetChunkFile:			// get the native path of currently loading bank or project (called from writeChunk) void* in <ptr> (char[2048], or sizeof(FSSpec))
			cout << "Vst callback audioMasterGetChunkFile" << endl;
			//Input Values:None
			//Return Value:
			//	Unknown
			break;

	//---from here VST 2.3 extension opcodes------------------------------------------------------
		case audioMasterGetInputSpeakerArrangement:	// result a VstSpeakerArrangement in ret
			cout << "Vst callback audioMasterGetInputSpeakerArrangement" << endl;
			break;
		default:
			cout << "Vst callback unrecognised "<< opcode <<"\n";
			break;
	}

	return retval;
};

using namespace boost;
void
VstPlugin::ScanFile(const string &vstpath, ostream &scriptFile, ostream &logFile, bool recursive)
{
//	path = "f:/progs/vstplugins/korg legacy collection/legacycell.dll";
//	lfp = stderr;
	bool doLog = logFile.good();
	string ext = getExt(vstpath);
	if (ext == "dll") {
		if (doLog) {
			logFile << "try loading "<< vstpath <<" ...\n";
		}
		HINSTANCE libhandle=AfxLoadLibrary(vstpath.c_str());
		if (libhandle == nullptr) {
			if (doLog) {
				cout << "libhandle null in " << vstpath << endl;
			}
			return;
		}
		AEffect*	(__cdecl* fxFactory)(audioMasterCallback);
		fxFactory=(AEffect*(__cdecl*)(audioMasterCallback))
									GetProcAddress(libhandle, "main");
		cout << "fx factory " << (unsigned) fxFactory << endl;
		if (fxFactory == nullptr) {
			if (doLog) {
				logFile << "'main' null from " << vstpath << endl;
			}
			if (!AfxFreeLibrary(libhandle)) {
				cout << "afx free library fails" << endl; 
			}
			return;
		}
		AEffect	*afx = fxFactory(HostCallback);
		if (afx == nullptr) {
			if (doLog) {
				logFile << "failed to create afx from "<<vstpath << endl; 
			}
			if (!AfxFreeLibrary(libhandle)) {
				cout << "afx free library fails" << endl; 
			}
			return;
		}
		if (afx->magic != kEffectMagic) {
			cout << vstpath  << "is not magic enougfh for vst" << endl;
			if (!AfxFreeLibrary(libhandle)) {
				cout << "afx free library fails" << endl;
			}
			return;
		}
		bool asynth = (afx->flags & effFlagsIsSynth)!=0;
		if (doLog) {
			logFile << "Effect '"<< GetEffectName(afx) <<"' by "<< GetVendorName(afx) << endl;
			logFile << "\tFull path: " << vstpath << endl;
			logFile << "\t"<< afx->numInputs <<" inputs, "<< afx->numOutputs <<" outputs, " << endl; 
			logFile << "\t"<< afx->numParams <<" parameters:" << endl;
			for (short j=0; j<afx->numParams; j++) {/*
				VstParameterProperties	*p = GetParamProperties(afx, j);
				fprintf(lfp, "\t  %d. '%s' ", j, GetParameterName(afx, j));
				if (p) {
					fprintf(lfp, "label '%s' '%s' category %d/%s", p->shortLabel, p->label, p->category, p->categoryLabel);
					if (p->flags & kVstParameterIsSwitch) {
						fprintf(lfp, ", switch");
					}
					if (p->flags & kVstParameterUsesIntegerMinMax) {
						fprintf(lfp, ", int range %d %d", p->minInteger, p->maxInteger);
					}
					if (p->flags & kVstParameterUsesIntStep) {
						fprintf(lfp, ", int step %d %d", p->largeStepInteger, p->stepInteger);
					}
					if (p->flags & kVstParameterUsesFloatStep) {
						fprintf(lfp, ", float step %g %g %g", p->smallStepFloat, p->largeStepFloat, p->stepFloat);
					}
					if (p->flags & kVstParameterCanRamp) {
						fprintf(lfp, ", rampable");
					}
				}
				for (short k=0; k<afx->numPrograms; k++) {
//					fprintf(lfp, "program %d: %s" << endl; //, k, GetProgramName(afx, k));
				}

				fprintf(lfp, "" << endl;*/
			}
		}
		if (scriptFile.good()) {
			string	fpath(vstpath);
			string dnm = getBase(fpath);
			string qnm;
			string bnm = dnm;
			qnm = quascript_name(bnm);
			if (qnm.size() == 0) {
				qnm = "vstfx";
			}
			scriptFile << "vst \\path \""<< vstpath <<"%s\" \\id '"<< uintstr(afx->uniqueID) <<"'"<< endl;
			scriptFile << "\t\\noload" << endl;
			if (asynth) {
				scriptFile << "\t\\synth" << endl;
			}
			scriptFile << "\t\\ins " << afx->numInputs << " \\outs " << afx->numOutputs
						<< " \\nparam " << afx->numParams << " \\nprogram " << afx->numPrograms << " " << qnm << "(" << endl; //;
			// TODO XXX FIXME not sure about tracking and storing all parameters these days
			for (short j=0; j<afx->numParams; j++) {
				/*
				string pqnm;
				char	*pnm = GetParameterName(afx, j);
				pqnm = quascript_name(pnm);
				if (*pqnm == 0) {
					strcpy(pqnm, "param");
				}
				fprintf(scriptFile, "\t\tvstparam %d %s" << endl; //, j, pqnm);
				*/
			}
			scriptFile << "\t)" << endl;
		}
		cout << "unloading library..." << endl;
		if (!AfxFreeLibrary(libhandle)) {
			cout << "fails" << endl; //;
		} else {
			cout << "succeeds" << endl; //;
		}
	} else {	// not a dll, maybe it's a dir of dll
		filesystem::path dir(vstpath);
		if (filesystem::exists(dir) && filesystem::is_directory(dir) && recursive) {
			for (filesystem::directory_entry& file : filesystem::directory_iterator(dir)) {
				ScanFile(file.path().string(), scriptFile, logFile, recursive);
			}
		}
	}
}

#endif

VstPluginList::VstPluginList()
{
	;
}

VstPlugin *
VstPluginList::item4Path(char *path)
{
	for (short i=0; ((unsigned)i)<size(); i++) {
		VstPlugin	*p = itemAt(i);
		if (p->pluginExecutablePath == path) {
			return p;
		}
	}
	return nullptr;
}

VstPlugin *
VstPluginList::item4Name(char *nm)
{
	if (!nm) return nullptr;
	for (short i=0; ((unsigned)i)<size(); i++) {
		VstPlugin	*p = itemAt(i);
		if (p->sym != nullptr && p->sym->name == string(nm)) {
			return p;
		}
	}
	return nullptr;
}
