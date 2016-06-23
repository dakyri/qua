#pragma once

#include <string>
using namespace std;

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

	int Setup();
	int SetupDevices();
	void setDisplay(QuaEnvironmentDisplay &d);

	bool					SetVstPluginDir(char *path, bool add, bool reload);

	string version;
	std::string				appPath;
	std::string				homePath;
	std::string				configDirectory;
#ifdef QUA_V_VST_HOST
	VstPluginList			vstPlugins;
#endif
#ifdef QUA_V_POOLPLAYER
	PoolPlayer				*poolPlayer;
#endif

#ifdef QUA_V_AUDIO
	QuaAudioManager			*quaAudio;
#endif
#ifdef QUA_V_JOYSTICK
	QuaJoystickManager		*quaJoystick;
#endif
	QuaMidiManager			*quaMidi;
	QuaParallelManager		*quaParallel;

	QuaEnvironmentDisplay		&display;
};
