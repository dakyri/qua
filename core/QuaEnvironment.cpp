#include "qua_version.h"

#include "StdDefs.h"
#include "Qua.h"
#include "QuaMidi.h"
#include "QuaAudio.h"
#include "QuaParallel.h"
#include "QuaJoy.h"
#include "QuaOSC.h"
#include "QuaEnvironment.h"
#include "Template.h"
#include "Lambda.h"
#include "Parse.h"

#include <iostream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

QuaEnvironment::QuaEnvironment(QuaEnvironmentDisplay &d)
	: display(d)
{
	version = Qua::getVersionString();
}

/*
 * this should be guaranteed to be called after primary app setup is done.
 * necessary for some devices ... in particular, we need to have the app instance
 * for direct input joysticks to work (and not explode)
 */
int
QuaEnvironment::setupDevices(void *instance)
{
#ifdef QUA_V_JOYSTICK
	quaJoystick.setup(instance);
#endif
	return B_OK;
}

bool
QuaEnvironment::SetVstPluginDir(char *path, bool add, bool reload)
{
#ifdef QUA_V_VST_HOST
	std::string	cfgPath = configDirectory + "/" + "vstplugins.qua";
	FILE	*fp = fopen(cfgPath.c_str(), add ? "w+" : "w");
	if (fp == NULL) {
		return false;
	}
	setbuf(fp, NULL);
	VstPlugin::ScanFile(path, fp, NULL, true);
	fclose(fp);
	if (reload) {
		display.RefreshVstPluginList();
	}
#endif
	return true;
}

QuaEnvironment::~QuaEnvironment()
{
#if defined(QUA_V_POOLPLAYER)
	delete poolPlayer;
#endif
}


int
QuaEnvironment::setup(string appExecutablePath, bool loadRC)
{
#ifdef QUA_V_MULTIMEDIA_TIMER
	TIMECAPS		tc;
	timeGetDevCaps(&tc, sizeof(tc));
	cout << "multimedia timer period min " << tc.wPeriodMin << " max " << tc.wPeriodMax << endl;
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
	appPath = appExecutablePath;
	path rcPath(appPath);
	rcPath = rcPath.parent_path();
	homePath = rcPath.string();
	rcPath += "/QuaRC";
	configDirectory = rcPath.string();

	cout << "App path " << appPath << endl;
	cout << "App home path " << homePath << endl;
	cout << "config directory " << configDirectory << endl;

	//////////////////////////////////
	// load global stuff
	//////////////////////////////////
	if (loadRC) {
		if (exists(rcPath)) {
			loadInitScript(rcPath);
		} else {
			create_directories(rcPath);
		}

#ifdef QUA_V_VST_HOST
		for (int i = 0; i < vstPlugins.size(); i++) {
			display.CreateVstPluginBridge(vstPlugins.itemAt(i));
			if (vstPlugins.itemAt(i)->loadPlugin) {
				if (vstPlugins.itemAt(i)->Load(display) != B_OK) {
				}
			}
		}
#endif
	}
	return B_OK;
}


bool
QuaEnvironment::loadInitScript(const path &rcPath) {
	try {
		if (!exists(rcPath)) {
			return false;
		}
		if (!is_directory(rcPath)) {
			for (directory_entry& file : directory_iterator(rcPath)) {
				bool succ = loadInitScript(file.path());
			}
		} else {
			
			string ex = rcPath.extension().string();
			if (ex == "qs" || ex == "qua") {
				string basename = rcPath.stem().string();
				cout << "loading inititialization script from " << rcPath << ", " << basename << "...\n";
				//
				// TODO XXX FIXME  this is a problem we won't need a context for a lot of things defined here, 
				// but we do need a qua context for the display aspects of the context ie error reporting etc.
				FILE *theFile = fopen(rcPath.string().c_str(), "r");
				Parser *p = new Parser(theFile, basename, display, nullptr);
				if (p->ParseProgFile()) {
					vstPlugins.add(p->vstplugins);
					Lambda		*nextm, *m;
					for (m = p->methods; m != nullptr; m = nextm) {
						nextm = m->next;
						if (m->sym->context == nullptr) {
							if (m->Init()) {
								display.CreateMethodBridge(m);
								//					    	S->next = methods;
								//					    	methods = S;
							}
						}
					}

					Template *t, *nextt;
					for (t = p->templates; t != nullptr; t = nextt) {
						nextt = t->next;
						if (t->sym->context == nullptr) {
							if (t->initialize()) {
								display.CreateTemplateBridge(t);
								//								F->next = templates;
								//								templates = F;
							}
						}
					}

					if (p->schedulees) {
						cout << "schedulable objects cannot be defined in null context" << endl;
					}
				}
			}
		}
	} catch (const filesystem_error& ex) {
		return false;
	}
	return true;
}

QuaAudioManager &getAudioManager() { return environment.quaAudio; }
QuaMidiManager &getMidiManager() { return environment.quaMidi; }
#ifdef QUA_V_JOYSTICK
QuaJoystickManager &getJoyManager() { return environment.quaJoystick; }
#endif
QuaParallelManager &getParallelManager() { return environment.quaParallel; }
QuaOSCManager &getOSCManager() { return environment.quaOSC; }