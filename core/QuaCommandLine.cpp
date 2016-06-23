#include "qua_version.h"
#include "StdDefs.h"
#include "Qua.h"
#include "QuaMidi.h"
#include "QuaAudio.h"


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
	int		i = 1;
	bool	wait_stop = true;
	Time	end_time;

	while (i<argc) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
			ProcessCommandLineWord(i, argv[i] + 1, true);
		}
		else {
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
		if (strcmp(arg, "help") == 0 || strcmp(arg, "about") == 0) {
			last_command = PRINT_HELP;
			commands |= PRINT_HELP;
			return true;
		}
		else if (strcmp(arg, "version") == 0) {
			last_command = PRINT_VERSION;
			commands |= PRINT_VERSION;
			return true;
		}
		else if (strcmp(arg, "setvst") == 0) {
			last_command = SET_VST;
			commands |= SET_VST;
			return true;
		}
		else if (strcmp(arg, "addvst") == 0) {
			last_command = ADD_VST;
			commands |= ADD_VST;
			return true;
		}
		else if (strcmp(arg, "listvst") == 0) {
			last_command = LIST_VST;
			commands |= LIST_VST;
			return true;
		}
		else if (strcmp(arg, "lint") == 0) {
			last_command = LINT_QUA;;
			commands |= LINT_QUA;
			return true;
		}
		else if (strcmp(arg, "listglob") == 0) {
			glob.DumpGlobals(fp);
			return true;
		}
		else if (strcmp(arg, "listjoy") == 0) {
			last_command = LIST_JOY;
			commands |= LIST_JOY;
			return true;
		}
		else if (strcmp(arg, "listasio") == 0) {
			last_command = LIST_ASIO;
			commands |= LIST_ASIO;
			return true;
		}
		else if (strcmp(arg, "loadasio") == 0) {
			last_command = LOAD_ASIO;
			commands |= LOAD_ASIO;
			return true;
		}
		else if (strcmp(arg, "listmidi") == 0) {
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
	}
	else {
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
	if ((commands & (LIST_GLOB | LIST_VST | LIST_ASIO | LIST_MIDI | LIST_JOY)) != 0) {
		FILE	*fp = fopen("qua.lst", "w");
		fprintf(fp, "****** Qua listing ******\n");
		if (commands & LIST_GLOB) {
		}
#ifdef QUA_V_ASIO

		if (commands & LOAD_ASIO) {
			for (short i = 0; i<asioLoad.CountItems(); i++) {
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
			for (short i = 0; i<vstList.CountItems(); i++) {
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
QuaCommandLine::LoadAsio(int devind, FILE *fp)
{
#ifdef QUA_V_AUDIO_ASIO
	int32		na = QuaAudioManager::asio.nDrivers;
	if (devind >= 0 && devind < na) {
		char	*p;
		if (p == NULL || !(*p)) {
			return;
		}
		status_t err = QuaAudioManager::asio.LoadDriver(p);
		if (err != ASE_OK) {
			fprintf(fp, "ASIO error while loading %s: %d\n", QuaAudioManager::asio.DriverName(devind), err);
			return;
		}
	}
	else {
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
		fprintf(fp, "input %d: %s, sample type %s\n", j, inp->insertName, QuaAudioManager::SampleFormatName(inp->sampleFormat));
	}
	for (j = 0; j<QuaAudioManager::asio.nOutputChannels; j++) {
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
	for (j = 0; j<ni; j++) {
		fprintf(fp, "In %u: \"%s\", product %d/%d driver %d/%d\n", j, wc2string(icap[j].szPname),
			icap[j].wMid, icap[j].wPid,
			icap[j].vDriverVersion & 0xFF, (icap[j].vDriverVersion & 0xFF00) >> 8);
	}
	for (j = 0; j<no; j++) {
		fprintf(fp, "Out %u: \"%s\", product %d/%d driver %d/%d: %s\n", j, wc2string(ocap[j].szPname),
			ocap[j].wMid, ocap[j].wPid,
			ocap[j].vDriverVersion & 0xFF, (ocap[j].vDriverVersion & 0xFF00) >> 8,
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
		for (j = 0; j<nJoy; j++) {
			fprintf(fp, "Joystick %d: %s, %d axes %d buttons %d hats\n",
				j, cap[j].szPname, cap[j].wNumAxes, cap[j].wNumButtons, (cap[j].wCaps&JOYCAPS_HASPOV) ? 1 : 0);
		}
	}
	delete cap;
#endif
#endif
}

