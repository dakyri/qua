#ifndef _QUA_JOYSTICK
#define _QUA_JOYSTICK

class RosterView;
class QuaJoystickManager;

#include "Stream.h"
#include "QuaTypes.h"
#include "QuaPort.h"

#if defined(WIN32)

#ifdef QUA_V_JOYSTICK_DX
#include "QuaDirect.h"
#elif defined(QUA_V_JOYSTICK_MMC)
#include <mmsystem.h>
#endif

#endif
#include <vector>
using namespace std;


struct joy_channel
{
	joy_channel(char *, long, long, bool);

	string name;
	long id;
	long flags;
	bool isFeedback;
};


struct joy_cap {
#ifdef QUA_V_JOYSTICK_DX
	joy_cap(GUID gui, char *nm, bool hf);
#endif
	short subType;
	string name;
	bool hasFeedback;
#ifdef QUA_V_JOYSTICK_DX
	GUID guid;
#endif
};

class QuaJoystickPort: public QuaPort {
public:
	QuaJoystickPort(const string &nm, QuaJoystickManager *, short subt, bool hf
#if defined(QUA_V_JOYSTICK_MMC)
							, int32 id
#endif
							);
	status_t Open(Qua *);
	status_t Close();
	virtual const char * name(uchar) override;
	
	bool				CheckPortOpen(Qua *q);
	bool				CheckPortClose();

	bool				OutputStream(Time TC, Stream *A, short chan);
	bool				GetStreamItems(Stream *S);
	bool				HasStreamItems();
	bool				Update();
	
	QuaJoystickManager	*quaJoystick;
		
	Stream				recv;
	std::mutex			schlock;
	
	int					nSticks;

	bool				hasFeedback;

	int					nAxes;
	int					nHats;
	int					nButtons;
	int					nSlider;
	
	bool				isOpen;
	Qua					*uberQua;

	float				rate;
	long				minUpdateMSec;
	bigtime_t			lastUpdate;

	float				maxx,
						maxy,
						minx,
						miny;

#if defined(WIN32)
// maybe these will ber better off as seperate subclass of QuaJoystickPort??
// maybe devices need to support multiple protocols ... 

#ifdef QUA_V_JOYSTICK_DX
	LPDIRECTINPUTDEVICE8 dxStick;
	GUID				dxGuid;
	vector<joy_channel> dxObjects;
	DIJOYSTATE			joyState;
	DIJOYSTATE			lastJoyState;

	void				ClearDXObjects();
	BOOL CALLBACK		DXEnumDeviceObjects(
								LPCDIDEVICEOBJECTINSTANCE lpddoi
						);

	static BOOL CALLBACK DXEnumDeviceObjectsCallback(
								LPCDIDEVICEOBJECTINSTANCE lpddoi,
								LPVOID pvRef 
						);
#elif defined(QUA_V_JOYSTICK_MMC)
	JOYINFOEX			joyState;
	JOYINFOEX			lastJoyState;
	int					stickId;
#endif

#endif
};

class QuaJoy;

class QuaJoystickManager: public QuaPortManager<QuaJoystickPort>
{
public:
	 QuaJoystickManager();
	 ~QuaJoystickManager();

	 virtual QuaPort *findPortByName(const string nm, int direction, int nports) override;

	virtual status_t connect(Input *) override;
	virtual status_t connect(Output *) override;
	virtual status_t disconnect(Input *) override;
	virtual status_t disconnect(Output *) override;

	QuaJoystickPort *OpenInput(Qua *, QuaJoystickPort*);
	QuaJoystickPort *OpenOutput(Qua *, QuaJoystickPort*);

	static int32 UpdateWrapper(void *userData);
	virtual int32 Update();
	
	inline QuaJoystickPort *Port(int i) {
		return (QuaJoystickPort *)QuaPortManager::port(i);
	}

	bool readerRunning;
	std::thread	updateThread;
	mutex updateMux;
	condition_variable updateCV;
	void resumeUpdater();

#ifdef QUA_V_JOYSTICK_DX
	void FetchDIJoysticks();
	void ClearDIJoysticks();
	vector<joy_cap> diJoystix;
	static BOOL CALLBACK EnumJoysticksCallback(
								const DIDEVICEINSTANCE* pdidInstance,
								VOID* pContext);
	BOOL CALLBACK		EnumJoysticks(const DIDEVICEINSTANCE*pdidInstance);
#elif defined(QUA_V_JOYSTICK_MMC)
	static JOYCAPS *GetJoyCaps(int32 *);
#endif

};



//RAW_AXIS_RANGE	32767
#define	QUA_JOY_AXIS_AMBIT	1000

#endif