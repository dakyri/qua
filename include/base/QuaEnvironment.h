#pragma once

#include <string>
using namespace std;

#include "QuaJoy.h"
#include "QuaAudio.h"
#include "QuaParallel.h"
#include "QuaOSC.h"
#include "QuaMidi.h"

class Qua;
class QuaMidiManager;
class QuaJoystickManager;
class QuaAudioManager;
class QuaParallelManager;
class QuaOSCManager;
class QuaEnvironmentDisplay;

class QuaEnvironment {
public:
	QuaEnvironment(QuaEnvironmentDisplay &d);
	~QuaEnvironment();

	int Setup(string vuf);
	int SetupDevices();
	void setDisplay(QuaEnvironmentDisplay &d);

	bool SetVstPluginDir(char *path, bool add, bool reload);

	string version;
	string appPath;
	string homePath;
	string configDirectory;
#ifdef QUA_V_VST_HOST
	VstPluginList vstPlugins;
#endif
#ifdef QUA_V_POOLPLAYER
	PoolPlayer *poolPlayer;
#endif

#ifdef QUA_V_AUDIO
	QuaAudioManager quaAudio;
#endif
#ifdef QUA_V_JOYSTICK
	QuaJoystickManager quaJoystick;
#endif
	QuaMidiManager quaMidi;
	QuaParallelManager quaParallel;
	QuaOSCManager quaOSC;

	QuaEnvironmentDisplay &display;
};

QuaAudioManager &getAudioManager();
QuaMidiManager &getMidiManager();
QuaJoystickManager &getJoyManager();
QuaParallelManager &getParallelManager();
QuaOSCManager &getOSCManager();
