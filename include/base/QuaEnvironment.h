#pragma once

#include <string>
#include <boost/filesystem.hpp>

#include "QuaJoy.h"
#include "QuaAudio.h"
#include "QuaParallel.h"
#include "QuaOSC.h"
#include "QuaMidi.h"
#include "QuaJoystick.h"
#include "VstPlugin.h"

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

	int setup(string vuf, bool loadRC);
	int setupDevices(void *);
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

protected:
	bool loadInitScript(const boost::filesystem::path &rcPath);
};

QuaAudioManager &getAudioManager();
QuaMidiManager &getMidiManager();
QuaJoystickManager &getJoyManager();
QuaParallelManager &getParallelManager();
QuaOSCManager &getOSCManager();
