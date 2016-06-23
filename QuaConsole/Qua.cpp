// QuaConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "StdDefs.h"
#include "Sym.h"
#include "Qua.h"
#include "QuaEnvironment.h"
#include "QuaCommandLine.h"

int main(char **argv, int argc)
{
	define_global_symbols();
	//	VstPlugin::LoadTest(
	//		"F:/progs/VstPlugins/vb1.dll"
	//		"F:/progs/VstPlugins/Native Instruments/Absynth 3.dll"
	//		);
	//	exit(0);
	environment.Setup();	// some app setup may rely on command line
	environment.SetupDevices();
	QuaCommandLine cmd;
	cmd.processCommandLine(argc, argv);
	setup_application_globals();	// some app setup may rely on command line, but probably not these days

	for (string qnm: cmd.loadNames) {
		Qua * q = Qua::loadScriptFile(qnm.c_str());
		if (q != NULL) {
			fprintf(stderr, "got a qua\n");
			//			q->sym->Dump(stderr, 0);
			q->Start(); // start up main thread
			ReportError("Press ok to halt and delete");
			//				q->Start();
			//			if (wait_stop) {
			//				q->WaitUntilStop();
			//			} else {
			//				q->WaitUntil(end_time);
			//				q->Stop();
			//			}
			fprintf(stderr, "deleting a qua\n");
			delete q;
		}
		// clean up, unload any sym}

	}
}