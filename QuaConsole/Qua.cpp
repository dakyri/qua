// QuaConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "StdDefs.h"
#include "Sym.h"
#include "Qua.h"
#include "QuaEnvironment.h"
#include "QuaCommandLine.h"

#include <iostream>

#if defined(WIN32)
#endif
int main(char **argv, int argc)
{
	define_global_symbols();
	//	VstPlugin::LoadTest(
	//		"F:/progs/VstPlugins/vb1.dll"
	//		"F:/progs/VstPlugins/Native Instruments/Absynth 3.dll"
	//		);
	//	exit(0);
	string homePath(argv[0]);
#if defined(WIN32)
	HMODULE quaAppModule = NULL;
	if ((quaAppModule = GetModuleHandle(NULL)) == NULL) {
		cerr << "Can't find Home..." << endl;
	}
	char	vuf[1024];
	int		nc;
	if ((nc = GetModuleFileNameA(quaAppModule, vuf, 1024)) == 0) {
		cerr << "Can't find Home..." << endl;
	}
#endif
	environment.Setup(homePath);	// some app setup may rely on command line
	environment.SetupDevices();
	QuaCommandLine cmd;
	cmd.processCommandLine(argc, argv);
//	setup_application_globals();	// some app setup may rely on command line, but probably not these days
	string input;
	for (string qnm: cmd.loadNames) {
		Qua * q = Qua::loadScriptFile(qnm.c_str(), defaultDisplay);
		if (q != NULL) {
			fprintf(stderr, "got a qua\n");
			//			q->sym->Dump(stderr, 0);
			q->Start(); // start up main thread
//			ReportError("Press ok to halt and delete");
			//				q->Start();
			//			if (wait_stop) {
			//				q->WaitUntilStop();
			//			} else {
			//				q->WaitUntil(end_time);
			//				q->Stop();
			//			}
			cin >> input;
			fprintf(stderr, "deleting a qua\n");
			delete q;
		}
		// clean up, unload any sym}

	}
}