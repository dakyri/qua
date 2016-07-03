#include "qua_version.h"

#include "StdDefs.h"
#include "Qua.h"
#include "QuaEnvironment.h"
#include "QuaMidi.h"
#include "QuaAudio.h"
#include "QuaParallel.h"
#include "QuaJoy.h"
#include "QuaOSC.h"

QuaEnvironment::QuaEnvironment(QuaEnvironmentDisplay &d)
	: display(d)
{
	version = Qua::getVersionString();
#ifdef QUA_V_AUDIO
	quaAudio = nullptr;
#endif
#ifdef QUA_V_JOYSTICK
	quaJoystick = nullptr;
#endif
	quaMidi = nullptr;
	quaParallel = nullptr;
}

int
QuaEnvironment::SetupDevices()
{
#ifdef QUA_V_AUDIO
	quaAudio = new QuaAudioManager();
#endif
#ifdef QUA_V_JOYSTICK
	quaJoystick = new QuaJoystickManager();
#endif
	quaMidi = new QuaMidiManager();
	quaParallel = new QuaParallelManager();

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
#ifdef QUA_V_ARRANGER_INTERFACE
	if (reload) {
		display.RefreshVstPluginList();
	}
#endif
#endif
	return true;
}

QuaEnvironment::~QuaEnvironment()
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
QuaEnvironment::Setup()
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
	if ((quaAppModule = GetModuleHandle(NULL)) == NULL) {
		tragicError("Can't find Home...");
	}
	char	vuf[1024];
	int		nc;
	if ((nc = GetModuleFileName(quaAppModule, vuf, 1024)) == 0) {
		TragicError("Can't find Home...");
	}
	appPath.SetTo(vuf);
	stderr, printf("App path %s\n", appPath.Path());
	appPath.GetParent(&homePath);
	configDirectory.SetTo(homePath.Path(), "QuaRC");
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
	while ((rcPath = configDirectory.NextFile()) != NULL) {
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
						Lambda		*nextm, *m;
						for (m = p->methods; m != NULL; m = nextm) {
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
						for (t = p->templates; t != NULL; t = nextt) {
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
	for (int i = 0; i<vstPlugins.CountItems(); i++) {
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
