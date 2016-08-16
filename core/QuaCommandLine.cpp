#include "qua_version.h"
#include "StdDefs.h"
#include "Qua.h"
#include "QuaMidi.h"
#include "QuaAudio.h"
#include "QuaJoystick.h"
#include "QuaCommandLine.h"
#include "VstPlugin.h"

#include <iostream>
#include <fstream>

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

#include <iostream>

int
QuaCommandLine::processCommandLine(int argc, char **argv)
{
	int		i = 1;
	bool	wait_stop = true;
	Time	end_time;

	while (i<argc) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
			processCommandLineWord(i, argv[i] + 1, true);
		} else {
			processCommandLineWord(i, argv[i], false);
		}
		i++;
	}
	closeListFile();
	return 1;
}

QuaCommandLine::QuaCommandLine()
{
	last_command = 0;
	listFile = nullptr;
}

bool
QuaCommandLine::processCommandLineWord(long argno, string arg, bool cmd)
{
	if (cmd) {
		if (arg == "help" || arg == "about") {
			cout << usage_str << endl;
			return true;
		} else if (arg == "version") {
			cout << Qua::getVersionString() << endl;
			return true;
		} else if (arg == "caps") {
			cout << Qua::getCapabilityString() << endl;
			return true;
		} else if (arg =="setvst") {
			last_command = SET_VST;
			return true;
		} else if (arg =="addvst") {
			last_command = ADD_VST;
			return true;
		} else if (arg =="listvst") {
#ifdef QUA_V_VST_HOST
			for (short i = 0; i<vstList.size(); i++) {
				VstPlugin::ScanFile(vstList[i], ofstream(), cerr, true);
			}
#endif
			return true;
		} else if (arg =="lint") {
			last_command = LINT_QUA;;
			return true;
		} else if (arg =="loadasio") {
			last_command = LOAD_ASIO;
			return true;
		} else if (arg =="listglob") {
			openListFile();
			glob.dumpGlobals(cerr);
			return true;
		} else if (arg =="listjoy") {
			openListFile();
			listJoy(listFile);
			return true;
		} else if (arg =="listasio") {
			openListFile();
			listAsio(listFile);
			return true;
		} else if (arg =="listmidi") {
			openListFile();
			listMidi(listFile);
			return true;
		}
		/*
		else if (arg =="wait") {
		if (++i > argc) {
		printf(usage_str);
		return 1;
		}
		if (arg =="stop") {
		wait_stop = true;
		} else {
		}
		}*/
	} else {
		// anything else will be a file to run, using any prior commandline options/settings
		switch (last_command) {
		case LINT_QUA: {
			Qua * q = Qua::loadScriptFile((char *)arg.c_str(), defaultDisplay);
			if (q != nullptr) {
				fprintf(stderr, "got a qua\n");
				q->sym->dump(cerr, 0);
				delete q;
			}
			return true;
		}
#ifdef QUA_V_VST_HOST
		case SET_VST: {
			VstPlugin::ScanFile(arg, ofstream("vstplugins.qua", ios_base::out), ofstream(), true);
			return true;
		}
		case ADD_VST: {
			VstPlugin::ScanFile(arg, ofstream("vstplugins.qua", ios_base::app), ofstream(), true);
			return true;
		}
#endif
#ifdef QUA_V_AUDIO_ASIO
		case LOAD_ASIO: {
			int n = atoi(arg.c_str());
			loadAsio(n, listFile);
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

void
QuaCommandLine::closeListFile()
{
	if (listFile != nullptr) {
		fclose(listFile);
		listFile = nullptr;
	}
}

void
QuaCommandLine::openListFile()
{
	if (listFile != nullptr) {
		closeListFile();
	}
	listFile = fopen("qua.lst", "w");
	fprintf(listFile, "%s\n", Qua::getVersionString().c_str());
}

void
QuaCommandLine::listAsio(FILE *fp)
{
#ifdef QUA_V_AUDIO_ASIO
	int32		na = QuaAudioManager::asio.nDrivers;
	for (long i = 0; i<na; i++) {
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
QuaCommandLine::loadAsio(int devind, FILE *fp)
{
#ifdef QUA_V_AUDIO_ASIO
	int32		na = QuaAudioManager::asio.nDrivers;
	if (devind >= 0 && devind < na) {
		char *p = QuaAudioManager::asio.DriverName(devind);
		if (p == nullptr || !(*p)) {
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
	for (j = 0; j<QuaAudioManager::asio.nInputChannels; j++) {
		QuaAudioIn	*inp = QuaAudioManager::asio.input[j];
		fprintf(fp, "input %d: %s, sample type %s\n", j, inp->insertName.c_str(), QuaAudioManager::sampleFormatName(inp->sampleFormat));
	}
	for (j = 0; j<QuaAudioManager::asio.nOutputChannels; j++) {
		QuaAudioOut	*inp = QuaAudioManager::asio.output[j];
		fprintf(fp, "output %d: %s, sample type %s\n", j, inp->insertName.c_str(), QuaAudioManager::sampleFormatName(inp->sampleFormat));
	}
	QuaAudioManager::asio.UnloadDriver();
#else
	fprintf(fp, "ASIO drivers not supported\n");
#endif
}

void
QuaCommandLine::listMidi(FILE *fp)
{
#ifdef QUA_V_DIRECT_MIDI
	int32		ni = 0;
	int32		no = 0;
	MIDIINCAPS *icap = QuaMidiManager::midiInDevices(&ni);
	MIDIOUTCAPS *ocap = QuaMidiManager::midiOutDevices(&no);
	short j;
	fprintf(fp, "%d input ports and %d output ports\n", ni, no);
	for (j = 0; j<ni; j++) {
		fprintf(fp, "In %u: \"%s\", product %d/%d driver %d/%d\n", j, string(icap[j].szPname).c_str(),
			icap[j].wMid, icap[j].wPid,
			icap[j].vDriverVersion & 0xFF, (icap[j].vDriverVersion & 0xFF00) >> 8);
	}
	for (j = 0; j<no; j++) {
		fprintf(fp, "Out %u: \"%s\", product %d/%d driver %d/%d: %s\n", j, string(ocap[j].szPname).c_str(),
			ocap[j].wMid, ocap[j].wPid,
			ocap[j].vDriverVersion & 0xFF, (ocap[j].vDriverVersion & 0xFF00) >> 8,
			//					ocap[j].wTechnology);
			QuaMidiManager::mmTechName(ocap[j].wTechnology));
	}
	delete icap;
	delete ocap;
#endif
}

void
QuaCommandLine::listJoy(FILE *fp)
{
#ifdef QUA_V_JOYSTICK
#ifdef QUA_V_JOYSTICK_DX

#elif defined(QUA_V_JOYSTICK_MMC)
	int32		nJoy;
	short		j;
	JOYCAPS		*cap = QuaJoystickManager::GetJoyCaps(&nJoy);
	fprintf(fp, "%d mmc joysticks available\n", nJoy);
	if (nJoy > 0 && cap != NULL) {
		for (j = 0; j<nJoy; j++) {
			fprintf(fp, "Joystick %d: %s, %d axes %d buttons %d hats\n",
				j, cap[j].szPname, cap[j].wNumAxes, cap[j].wNumButtons, (cap[j].wCaps&JOYCAPS_HASPOV) ? 1 : 0);
		}
	}
	delete cap;
#endif
#endif
}

