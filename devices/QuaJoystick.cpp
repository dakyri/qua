#include "qua_version.h"

#ifdef QUA_V_JOYSTICK

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "QuaJoystick.h"
#include "QuaInsert.h"
#include "Destination.h"
#include "Channel.h"
#include "Qua.h"
#include "QuaEnvironment.h"

#ifdef QUA_V_JOYSTICK_DX
#include "QuaDirect.h"
#endif



#define AXIS_NOISE 10
#define ZERO_DRIFT 0.10

int debug_joy=0;

#ifdef QUA_V_JOYSTICK_MMC
// here for now as it probably should be in a library
char *
mmc_error_string(MMRESULT err)
{
	switch (err) {
		case JOYERR_NOERROR: return "Nort an eror";
		case MMSYSERR_NODRIVER: return "The joystick driver is not present"; 
		case MMSYSERR_INVALPARAM:  return "An invalid parameter was passed."; 
		case MMSYSERR_BADDEVICEID: return "The specified joystick identifier is invalid. ";
		case JOYERR_UNPLUGGED: return "The specified joystick is not connected to the system.";
	}
	return "Unexpected mmc error";
}
#endif

#ifdef QUA_V_JOYSTICK_DX
joy_cap::joy_cap(GUID gui, char *nm, bool hf)
{
	subType = QUA_JOY_DX;
	hasFeedback = hf;
	guid = guid;
	if (nm != nullptr) name = nm;
}

joy_channel::joy_channel(char *nm, long oi, long fl, bool iff)
{
	if (nm != nullptr) name = nm;
	flags = fl;
	id = oi;
	isFeedback = iff;
}


#endif

QuaJoystickPort::QuaJoystickPort(const string &devnm, QuaJoystickManager *qj, short subt,
		 bool hf
#ifdef QUA_V_JOYSTICK_MMC
		, int32 jid
#endif
						):
	QuaPort(devnm, QUA_DEV_JOYSTICK, subt, QUA_PORT_IO)
{
	quaJoystick = qj;

#ifdef QUA_V_JOYSTICK_MMC
	stickId = jid;
#endif

	isOpen = false;

	lastUpdate = 0;
	rate = 10;
	minx = -1;
	miny = -1;
	maxx = 1;
	maxy = 1;
	minUpdateMSec = 100;
	
	AddInsert("Joy!", 0, OUTPUT_INSERT, 1, 0);	
}

const char *
QuaJoystickPort::name(uchar)
{
	return sym->name.c_str();
}

#ifdef QUA_V_JOYSTICK_DX
BOOL CALLBACK
QuaJoystickPort::DXEnumDeviceObjects(LPCDIDEVICEOBJECTINSTANCE oi)
{
	HRESULT	err;
	if (oi->dwType & DIDFT_AXIS) {
		DIPROPRANGE diprg; 

		diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprg.diph.dwHow        = DIPH_BYID; 
		diprg.diph.dwObj        = oi->dwType; 
		diprg.lMin              = -QUA_JOY_AXIS_AMBIT;; 
		diprg.lMax              = +QUA_JOY_AXIS_AMBIT; 

		err = dxStick->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (err != DI_OK) {
//			return DIENUM_CONTINUE;
		}
	}
	if (oi->dwType & (DIDFT_AXIS|DIDFT_BUTTON|DIDFT_POV|DIDFT_FFACTUATOR)) {
		dxObjects.push_back(joy_channel(
			(char *)oi->tszName, DIDFT_GETINSTANCE(oi->dwType),
			(oi->dwType)&0xff, (oi->dwType&DIDFT_FFACTUATOR)!=0 ));
	}
	return DIENUM_CONTINUE;
}

BOOL CALLBACK
QuaJoystickPort::DXEnumDeviceObjectsCallback(
					LPCDIDEVICEOBJECTINSTANCE lpddoi,
					LPVOID pvRef 
			)
{
	return ((QuaJoystickPort*)pvRef)->DXEnumDeviceObjects(lpddoi);
}

void
QuaJoystickPort::ClearDXObjects()
{
	dxObjects.clear();
}

#elif defined(QUA_V_JOYSTICK_MMC)

#endif

status_t
QuaJoystickPort::Open(Qua *q)
{
	if (isOpen) {
		if (uberQua == q) {
			return B_OK;
		} else {
			return B_ERROR;
		}
	}
#if defined(WIN32)

#if defined(QUA_V_JOYSTICK_DX)
    HRESULT	err = directInput->CreateDevice(dxGuid, &dxStick, nullptr);
	if (err != DI_OK) {
		fprintf(stderr, "Open CreateDevice() error %s\n", direct_error_string(err));
		return B_ERROR;
	}
	err = dxStick->SetDataFormat(&c_dfDIJoystick);
	if (err != DI_OK) {
		fprintf(stderr, "Open SetDataFormat() error %s\n", direct_error_string(err));
		return B_ERROR;
	}
//c_dfDIKeyboard 
//c_dfDIMouse 
//c_dfDIMouse2 
//c_dfDIJoystick2 
	err = dxStick->SetCooperativeLevel(nullptr, /* HWND hDlg, ... a big doozy !!!! ??? this must be a valid window*/ 
						  DISCL_EXCLUSIVE | DISCL_BACKGROUND);
	if (err != DI_OK) {
		fprintf(stderr, "Open SetDataFormat() error %s\n", direct_error_string(err));
		return B_ERROR;
	}

	DIDEVCAPS	caps;
	caps.dwSize = sizeof(caps);
	err = dxStick->GetCapabilities(&caps);
	if (err != DI_OK) {
		fprintf(stderr, "Open get cap error: %s\n", direct_error_string(err));
	}
	
	nAxes = caps.dwAxes;
	nButtons = caps.dwButtons;
	nHats = caps.dwPOVs; 

	minUpdateMSec = caps.dwFFSamplePeriod ;

	ClearDXObjects();
	err = dxStick->EnumObjects(DXEnumDeviceObjectsCallback, this, DIDFT_ALL);
	if (err != DI_OK) {
		fprintf(stderr, "Open enum obj error: %s\n", direct_error_string(err));
	}

    err = dxStick->Acquire();
	while(err == DIERR_INPUTLOST) {
        err = dxStick->Acquire();
	}
	if (err != DI_OK) {
		fprintf(stderr, "Open acquire obj error: %s\n", direct_error_string(err));
	}
/*
	caps.dwFlags & DIDC_FORCEFEEDBACK;
DIDC_ALIAS 
// The device is a duplicate of another DirectInput device.
// Alias devices are by default not enumerated by IDirectInput8::EnumDevices. 
DIDC_ATTACHED 
// The device is physically attached. 
DIDC_DEADBAND 
// The device supports deadband for at least one force-feedback condition. 
DIDC_EMULATED 
// If this flag is set, the data is coming from a user mode device interface, such as a Human Interface Device (HID), or by some other ring 3 means. If it is not set, the data is coming directly from a kernel mode driver. 
 
// The device supports force feedback. 
DIDC_FFFADE 
// The force-feedback system supports the fade parameter for at least one effect.
// If the device does not support fade, the fade level and fade time members of
// the DIENVELOPE structure are ignored by the device. 
// After a call to the IDirectInputDevice8::GetEffectInfo method, 
// an individual effect sets the DIEFT_FFFADE flag if fade is supported for
// that effect. 

DIDC_FFATTACK 
// The force-feedback system supports the attack parameter for at least one effect.
// If the device does not support attack, the attack level and attack time members
// of the DIENVELOPE structure are ignored by the device. 
// After a call to the IDirectInputDevice8::GetEffectInfo method,
// an individual effect sets the DIEFT_FFATTACK flag if attack is supported 
// for that effect. 

DIDC_HIDDEN 
// Fictitious device created by a device driver so that it can generate
// keyboard and mouse events. Such devices are not normally enumerated by
// IDirectInput8::EnumDevices or configured by IDirectInput8::ConfigureDevices. 
DIDC_PHANTOM 
// Placeholder for a device that might exist later. 
// Phantom devices are by default not enumerated by IDirectInput8::EnumDevices. 
DIDC_POLLEDDATAFORMAT 
// At least one object in the current data format is polled, rather than interrupt-driven. For these objects, the application must explicitly call the IDirectInputDevice8::Poll method to obtain data. 
DIDC_POLLEDDEVICE 
// At least one object on the device is polled, rather than interrupt-driven. For these objects, the application must explicitly call the IDirectInputDevice8::Poll method to obtain data. HID devices can contain a mixture of polled and nonpolled objects. 
DIDC_POSNEGCOEFFICIENTS 
// The force-feedback system supports two coefficient values for 
// conditions (one for the positive displacement of the axis and one for
// the negative displacement of the axis) for at least one condition. 
// If the device does not support both coefficients, 
// the negative coefficient in the DICONDITION structure is ignored. 
// After a call to the IDirectInputDevice8::GetEffectInfo method, 
// an individual condition sets the DIEFT_POSNEGCOEFFICIENTS flag 
// if separate positive and negative coefficients are supported for that condition. 

DIDC_POSNEGSATURATION 
//The force-feedback system supports a maximum saturation for both positive
// and negative force output for at least one condition. If the device does
// not support both saturation values, the negative saturation in the DICONDITION 
// structure is ignored. After a call to the IDirectInputDevice8::GetEffectInfo
// method, an individual condition sets the DIEFT_POSNEGSATURATION flag if
// separate positive and negative saturation are supported for that condition. 

DIDC_SATURATION 
//The force-feedback system supports the saturation of condition effects for
// at least one condition. If the device does not support saturation, the force
// generated by a condition is limited only by the maximum force that the device
// can generate. After a call to the IDirectInputDevice8::GetEffectInfo method, 
// an individual condition sets the DIEFT_SATURATION flag if saturation is 
// supported for that condition. 

DIDC_STARTDELAY 
// The force-feedback system supports the start delay parameter for at least
// one effect. If the device does not support start delays, the dwStartDelay
// member of the DIEFFECT structure is ignored. 
			;
*/
#elif defined(QUA_V_JOYSTICK_MMC)

#endif

#else
;
#endif
	return B_NO_ERROR;
}

status_t
QuaJoystickPort::Close()
{
#if defined(WIN32)
#ifdef QUA_V_JOYSTICK_DX
	if (isOpen) {
		uberQua = nullptr;
		dxStick->Unacquire();
		dxStick->Release();
	}
#elif defined(QUA_V_JOYSTICK_MMC)
#endif
#endif
	return B_NO_ERROR;
}

bool
QuaJoystickPort::GetStreamItems(Stream *S)
{
	if (recv.nItems > 0) {	

		schlock.lock();
		StreamItem	*p=recv.head,
					**pp = &recv.head,
					*prev = nullptr;
		while (p!=nullptr) {
			bool	add_this = FALSE;
			switch (p->type) {
			case TypedValue::S_JOY: {
				add_this = TRUE;
			    break;
			}
	
			default:	// whatever the fuck this is!
				break;
		    }
		    
		    if (add_this) {
		    	if (S->tail)
		    		S->tail->next = p;
		    	else
		    		S->head = p;
		    	S->tail = p;
		    	S->nItems++;
				recv.nItems--;
		    	*pp = p->next;
			    p = p->next;
		    } else {
		    	prev = p;
		    	pp = &p->next;
		    	p = p->next;
		    }
	    }
	    recv.tail = prev;
	
		schlock.unlock();
		return true;
	}
	return false;
}

bool
QuaJoystickPort::Update()
{
	if (isOpen) {
		Stream	upd_stream;
		Time	t;
		t = uberQua->theTime;

#if defined(WIN32)
#if defined(QUA_V_JOYSTICK_DX)
//		if ((timeDelta = ((stick.timestamp - lastUpdate) / 1000000.0))
//								> minUpdate) {
//			return true;
//		}

		HRESULT	err;
		err = dxStick->Poll(); 
		if (err != DI_OK) {
			while(err == DIERR_INPUTLOST) {
				err = dxStick->Acquire();
			}
		}
		if (err != DI_OK) {
			fprintf(stderr, "Open poll obj error: %s\n", direct_error_string(err));
			return false;
		}
	    
		err = dxStick->GetDeviceState(sizeof(DIJOYSTATE), &joyState);
		if (err != DI_OK) {
			fprintf(stderr, "Update device state obj error: %s\n", direct_error_string(err));
			return false;
		}

		int	axi = 0;
		int	i = 0;
		if (i < nAxes) {
			if (joyState.lX != lastJoyState.lX) {
				float	axisv = ((float)joyState.lX)/QUA_JOY_AXIS_AMBIT;
				upd_stream.AddJoyAxisToStream(0,0,axisv,t);
			}
			i++;
			if (i < nAxes) {
				if (joyState.lY != lastJoyState.lY) {
					float	axisv = ((float)joyState.lY)/QUA_JOY_AXIS_AMBIT;
					upd_stream.AddJoyAxisToStream(0, 1, axisv, t);
				}
				i++;
				if (i < nAxes) {
					if (joyState.lZ != lastJoyState.lZ) {
						float	axisv = ((float)joyState.lZ)/QUA_JOY_AXIS_AMBIT;
						upd_stream.AddJoyAxisToStream(0, 2, axisv, t);
					}
					i++;
					if (i < nAxes) {
						if (joyState.lRx != lastJoyState.lRx) {
							float	axisv = ((float)joyState.lRx) / QUA_JOY_AXIS_AMBIT;
							upd_stream.AddJoyAxisToStream(0, 3, axisv, t);
						}
						i++;
						if (i < nAxes) {
							if (joyState.lRy != lastJoyState.lRy) {
								float	axisv = ((float)joyState.lRy) / QUA_JOY_AXIS_AMBIT;
								upd_stream.AddJoyAxisToStream(0, 4, axisv, t);
							}
							i++;
							if (i < nAxes) {
								if (joyState.lRz != lastJoyState.lRz) {
									float	axisv = ((float)joyState.lRz)/QUA_JOY_AXIS_AMBIT;
									upd_stream.AddJoyAxisToStream(0, 5, axisv, t);
								}
								i++;
							}
						}
					}
				}
			}
		}
		for (i=0; i<nHats; i++) {
			if (joyState.rgdwPOV[i] != lastJoyState.rgdwPOV[i]) {
				long	hatval = joyState.rgdwPOV[i];
				if (hatval < 0) {
					hatval = 0;
				} else {
					hatval = (hatval/36000)+1;
				}
				upd_stream.AddJoyHatToStream(0, i, hatval, t);
			}
		}
		for (i=0; i<nButtons; i++) {
			if (joyState.rgbButtons[i] != lastJoyState.rgbButtons[i]) {
				upd_stream.AddJoyButtonToStream(0, i, joyState.rgbButtons[i] != 0, t);
			}
		}
		lastJoyState = joyState;
#elif defined(QUA_V_JOYSTICK_MMC)
		HRESULT	err = joyGetPosEx(stickId, &joyState);

		if (err != JOYERR_NOERROR) {
			fprintf(stderr, "Open poll obj error: %s\n", mmc_error_string(err));
			return false;
		}
	    
		int	axi = 0;
		int	i = 0;
		if (i < nAxes) {
			if (joyState.dwXpos != lastJoyState.dwXpos) {
				float	axisv = ((float)joyState.dwXpos)/QUA_JOY_AXIS_AMBIT;
				upd_stream.AddJoyAxisToStream(0,0,axisv,t);
			}
			i++;
			if (i < nAxes) {
				if (joyState.dwXpos != lastJoyState.dwXpos) {
					float	axisv = ((float)joyState.dwXpos)/QUA_JOY_AXIS_AMBIT;
					upd_stream.AddJoyAxisToStream(0,1,axisv,t);
				}
				i++;
				if (i < nAxes) {
					if (joyState.dwXpos != lastJoyState.dwXpos) {
						float	axisv = ((float)joyState.dwXpos)/QUA_JOY_AXIS_AMBIT;
						upd_stream.AddJoyAxisToStream(0,2,axisv,t);
					}
					i++;
					if (i < nAxes) {
						if (joyState.dwXpos != lastJoyState.dwXpos) {
							float	axisv = ((float)joyState.dwXpos)/QUA_JOY_AXIS_AMBIT;
							upd_stream.AddJoyAxisToStream(0,3,axisv,t);
						}
						i++;
						if (i < nAxes) {
							if (joyState.dwXpos != lastJoyState.dwXpos) {
								float	axisv = ((float)joyState.dwXpos)/QUA_JOY_AXIS_AMBIT;
								upd_stream.AddJoyAxisToStream(0,4,axisv,t);
							}
							i++;
							if (i < nAxes) {
								if (joyState.dwXpos != lastJoyState.dwXpos) {
									float	axisv = ((float)joyState.dwXpos)/QUA_JOY_AXIS_AMBIT;
									upd_stream.AddJoyAxisToStream(0,5,axisv,t);
								}
								i++;
							}
						}
					}
				}
			}
		}
		for (i=0; i<nHats; i++) {
			if (joyState.dwPOV != lastJoyState.dwPOV) {
				long	hatval = joyState.dwPOV;
				if (hatval < 0) {
					hatval = 0;
				} else {
					hatval = (hatval/36000)+1;
				}
				upd_stream.AddJoyHatToStream(0,i,hatval,t);
			}
		}
		for (i=0; i<nButtons; i++) {
			if ((joyState.dwButtons&(1<<i)) != (lastJoyState.dwButtons&(1<<i))) {
				upd_stream.AddJoyButtonToStream(0,i, (joyState.dwButtons&(1<<i)) != 0, t);
			}
		}
		lastJoyState = joyState;
#endif
#else

#endif
		if (upd_stream.nItems > 0) {
			schlock.lock();
			recv.AppendStream(upd_stream);
			schlock.unlock();
		}
	}
	return true;
}

bool
QuaJoystickPort::CheckPortOpen(Qua *q)
{
	bool open=(Open(q)==B_OK);
	getJoyManager().resumeUpdater();
	return open;
}

bool
QuaJoystickPort::CheckPortClose()
{
	return Close() == B_NO_ERROR;
}

//ControlPanel *
//QuaJoystickPort::PortControlPanel(BRect r, float maxw)
//{
//	if (!controlPanel) {
//		controlPanel = new JoyPanel(r, maxw, "Joy",	this);
//	}
//	return controlPanel;
//}
//

bool
QuaJoystickPort::HasStreamItems()
{
	return recv.nItems > 0;
}

bool
QuaJoystickPort::OutputStream(Time TC, Stream *A, short chan)
{
	StreamItem	*p, *prev;

	p=A->head;
	prev = nullptr;
	while (p!=nullptr) {
		bool		del_this=FALSE,
					sched_noff = FALSE;

		switch (p->type) {
/*
		case S_NOTE: {
			StreamNote *q=(StreamNote*)p;
			
		    if (q->note.cmd & MIDI_CMD_BYTE) {
				if (TimeCounter >= q->time) {
					MidiCmd(chan, &q->note);
					if (  q->note.cmd == MIDI_NOTE_OFF ||
						  q->note.cmd == MIDI_KEY_PRESS ||
						  q->note.duration == INFINITE_TICKS) {
						del_this = TRUE;
					} else {
						sched_noff = (q->note.cmd == MIDI_NOTE_ON);
					}
				}
		    } else {
		        if (TimeCounter >= q->time) {
				    if (q->note.pitch <= 127) {
				    	SendNoteOn(chan, q->note.pitch, q->note.dynamic);
						if (q->note.duration == INFINITE_TICKS)
							del_this = TRUE;
						else
							sched_noff = TRUE;
					} else
						del_this = TRUE;
		        }
		    }
			if (sched_noff) { // reschedule note off
	        	q->time += Time(q->note.duration, q->time.metric);
	        	q->note.duration = INFINITE_TICKS; // ensure delete
	        	q->note.cmd = MIDI_NOTE_OFF;
				if (debug_midi)
					fprintf(stderr, "reschedule noff\n");
				del_this = FALSE;
	        }
        
		    break;
		}
*/		
		case TypedValue::S_JOY: {
			StreamJoy	*q = (StreamJoy *)p;
// need to look at duration, force, envelopes maybe, direction
//  because were a sequencer maybe not delay
//			SendSystemCommon(q->joy, q->sysC.data1, q->sysC.data2);
// create effect, no params for each force affector
//HRESULT CreateEffect(
//  REFGUID rguid,                 
//  LPCDIEFFECT lpeff,             
//  LPDIRECTINPUTEFFECT *ppdeff,  
//  LPUNKNOWN punkOuter            
//);
//HRESULT SetParameters(
//  LPCDIEFFECT peff,  
//  DWORD dwFlags      
//);
//typedef struct DIEFFECT { 
//    DWORD   dwSize; 
//    DWORD   dwFlags; 
//    DWORD   dwDuration; 
//    DWORD   dwSamplePeriod; 
//    DWORD   dwGain; 
//    DWORD   dwTriggerButton; 
//    DWORD   dwTriggerRepeatInterval;
 //   DWORD   cAxes; 
//    LPDWORD rgdwAxes; 
//    LPLONG  rglDirection; 
//    LPDIENVELOPE lpEnvelope; 
//    DWORD   cbTypeSpecificParams; 
//    LPVOID  lpvTypeSpecificParams; 
 //   DWORD   dwStartDelay; 
//  set parameters
//DIEP_AXES 
//The cAxes and rgdwAxes members contain data. 
//DIEP_DIRECTION 
//The cAxes and rglDirection members contain data. The dwFlags member specifies (with DIEFF_CARTESIAN or DIEFF_POLAR) the coordinate system in which the values should be interpreted. 
//DIEP_DURATION 
//The dwDuration member contains data. 
//DIEP_ENVELOPE 
//The lpEnvelope member points to a DIENVELOPE structure that contains data. To detach any existing envelope from the effect, pass this flag and set the lpEnvelope member to nullptr. 
//DIEP_GAIN 
//The dwGain member contains data. 
// download
//   start it schedule a stop or play infinite
// unload
// release it
			del_this = TRUE;
			break;
		}

		default:	// whatever the fuck this is!
			del_this = TRUE;
			break;
	    }
	    
        if (del_this) {
					// no more to do... remove from stream
			StreamItem	*q = p;
			if (prev == nullptr) {
				A->head = p->next;
			} else {
				prev->next = p->next;
			}
			A->nItems--;
			p=p->next;
			delete q;
		} else {
			prev = p;
			p = p->next;
		}
    }
    A->tail = prev;

    return TRUE;
}



QuaJoystickManager::QuaJoystickManager()
{

#if defined(WIN32)

#ifdef QUA_V_JOYSTICK_DX
	direct_setup();
	FetchDIJoysticks();

	for (short i=0; i<diJoystix.size(); i++) {
		joy_cap &p = diJoystix[i];
		
		QuaJoystickPort *jp = new QuaJoystickPort(p.name, this, QUA_JOY_DX, p.hasFeedback);
		ports.push_back(jp);
	}
#endif

#endif
	readerRunning = false;
	updateThread = std::thread(UpdateWrapper, this);
}

QuaJoystickManager::~QuaJoystickManager()
{
	status_t	exit_val;
	
	readerRunning = false;
	resumeUpdater();
	updateThread.join();

	int nd = countPorts();
	for (short i=0; i<nd; i++) {
		delete (QuaJoystickPort *)Port(0);
		removePort(0);
	}

	fprintf(stderr, "deleted joystick manager\n");
}


QuaPort *
QuaJoystickManager::findPortByName(string name, int direction, int nports) {
	return ports.size() > 0 ? ports[0] : nullptr;
}


#ifdef QUA_V_JOYSTICK_DX

void
QuaJoystickManager::FetchDIJoysticks()
{
	ClearDIJoysticks();
	directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
                       nullptr, DIEDFL_ATTACHEDONLY);
}

void
QuaJoystickManager::ClearDIJoysticks()
{
	diJoystix.clear();
}

BOOL CALLBACK 
QuaJoystickManager::EnumJoysticksCallback(
						const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	return ((QuaJoystickManager*)pContext)->EnumJoysticks(pdidInstance);
}

BOOL CALLBACK
QuaJoystickManager::EnumJoysticks(const DIDEVICEINSTANCE*pdidInstance)
{
	LPDIRECTINPUTDEVICE8 joystick;
    HRESULT	err = directInput->CreateDevice(pdidInstance->guidInstance,  
                                &joystick, nullptr);
	if (err == DI_OK) {
		DIDEVCAPS	caps;
		caps.dwSize = sizeof(caps);
		err = joystick->GetCapabilities(&caps);
		if (err == DI_OK) {
		} else {
			fprintf(stderr, "DI Enum get cap error: %s\n", direct_error_string(err));
		}
		joystick->Unacquire();
		joystick->Release();
		diJoystix.push_back(joy_cap(pdidInstance->guidInstance, nullptr, true));
	} else {
		fprintf(stderr, "DI Enum error: %s\n", direct_error_string(err));
	}
	return DIENUM_CONTINUE;
}

#elif defined(QUA_V_JOYSTICK_MMC)

JOYCAPS *
QuaJoystickManager::GetJoyCaps(int32 *nJoyP)
{
	JOYINFO joyinfo; 
	uint32	nJoy;
//	uint32	wDeviceID; 
	bool	dev1Attached, dev2Attached; 
	 
	if((nJoy = joyGetNumDevs()) == 0) {
		*nJoyP = 0;
		return nullptr; 
	}
	fprintf(stderr, "%d total joysticks\n", nJoy);
	dev1Attached = joyGetPos(JOYSTICKID1,&joyinfo) != JOYERR_UNPLUGGED; 
	dev2Attached = (nJoy == 2) && joyGetPos(JOYSTICKID2,&joyinfo) != 
		JOYERR_UNPLUGGED; 
	if(!(dev1Attached || dev2Attached)) {   // decide which joystick to use 
		*nJoyP = 0;
		return nullptr;
	}
	JOYCAPS	*caps;
	int		nc = 0;
	/*
	caps = new JOYCAPS[(dev1Attached && dev2Attached)?2:1];
	if (dev1Attached) {
		MMRESULT	err = joyGetDevCaps(
							JOYSTICKID1, 
							&caps[nc],    
							sizeof(JOYCAPS));
		nc++;
	}
	if (dev2Attached) {
		MMRESULT	err = joyGetDevCaps(
							JOYSTICKID2, 
							&caps[nc],    
							sizeof(JOYCAPS));
		nc++;
	}
	*nJoyP = nc;

	*/
	JOYCAPS	c;
	int		i, n=0;
	for (i=0; i<nJoy; i++) {
		MMRESULT	err = joyGetDevCaps(
							i, 
							&c,    
							sizeof(JOYCAPS));
		if (c.wNumAxes == 0 && c.wNumButtons == 0) {
			break;
		}
		n++;
	}
	nJoy = n;
	caps = new JOYCAPS[nJoy];
	for (i=0; i<nJoy; i++) {
		MMRESULT	err = joyGetDevCaps(
							i, 
							&caps[i],    
							sizeof(JOYCAPS));
	}
	*nJoyP = nJoy;

	return caps;
}

#endif


void
QuaJoystickManager::resumeUpdater() {
	{
		std::lock_guard<std::mutex> lk(updateMux);
		//		ready = true;
	}
	updateCV.notify_all();
}

int32
QuaJoystickManager::UpdateWrapper(void* userData)
{
	return ((QuaJoystickManager*)userData)->Update();
}

int32
QuaJoystickManager::Update()
{
	readerRunning = true;
	while(readerRunning) {
		bool		nothinDoin = true;
		for (short i=0; i<countPorts(); i++) {
			if (Port(i)->isOpen)
				nothinDoin = false;
		}
		if (nothinDoin) {
			std::unique_lock<std::mutex> lk(updateMux);
			updateCV.wait(lk /*, []{return ready;}*/);
		}
		for (short i=0; i<countPorts(); i++) {
			Port(i)->Update();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}
	return B_ERROR;
}

QuaJoystickPort *
QuaJoystickManager::OpenInput(Qua *q, QuaJoystickPort *p)
{
	p->Open(q);
	return p;
}


QuaJoystickPort *
QuaJoystickManager::OpenOutput(Qua *q, QuaJoystickPort *p)
{
	p->Open(q);
	return p;
}

status_t
QuaJoystickManager::connect(Input *s)
{
	s->src.joy = OpenInput(s->channel->uberQua, (QuaJoystickPort *)s->device);
	if (s->src.midi != nullptr) {
		s->enabled = true;
	} else {
		s->enabled = false;
		return B_ERROR;
	}
	return B_OK;
}

status_t
QuaJoystickManager::connect(Output *s)
{
	s->dst.joy = OpenOutput(s->channel->uberQua, (QuaJoystickPort *)s->device);
	if (s->dst.midi != nullptr) {
		s->enabled = true;
	} else {
		s->enabled = false;
		return B_ERROR;
	}
	return B_OK;
}

status_t
QuaJoystickManager::disconnect(Input *s)
{
	s->enabled = false;
	if (s->src.joy) {
		s->src.joy->Close();
	}
	s->src.joy = nullptr;
	return B_OK;
}

status_t
QuaJoystickManager::disconnect(Output *s)
{
	s->enabled = false;
	if (s->dst.joy) {
		s->dst.joy->Close();
	}
	s->dst.joy = nullptr;
	return B_OK;
}

#endif
