#ifndef _QUA_ASIO
#define _QUA_ASIO

#ifdef QUA_V_AUDIO_ASIO

#include "StdDefs.h"

#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"

class QuaAudioIn;
class QuaAudioOut;
class Qua;
class QuaAudioManager;

#define ASIONormalize16bit	(1<<15)
#define ASIONormalize24bit	(1<<23)
#define ASIONormalize32bit	(1<<31)

class QuaAsio
{
public:
	QuaAsio();
	~QuaAsio();

	AsioDrivers	 drivers;
	char *DriverName(int32);
	char *CurrentDriver();
	void UnloadDriver();
	static short ByteSize(long);

	status_t Load();
	status_t LoadDriver(char *);

	void SetPreferredDriver(char *);

	char **installedDrivers;
	long nDrivers;
	char *preferredDriver;

// standard asio callbacks
	static void sampleRateChanged(ASIOSampleRate sRate);
	static long asioMessages(
							long selector,
							long value,
							void* message,
							double* opt);
	static void bufferSwitch(
							long doubleBufferIndex,
							ASIOBool directProcess);
	static ASIOTime* bufferSwitchTimeInfo(
							ASIOTime* params,
							long doubleBufferIndex,
							ASIOBool directProcess);
	static ASIOCallbacks callbacks;

	status_t SetBufSize(long bs);

	bool loaded;
	bool running;

	long nInputChannels;
	long nOutputChannels;
	long inputLatency;
	long outputLatency;

	static long minBufSize;			// asio buffer sizes are in samples/frames
	static long maxBufSize;
	static long preferredBufSize;
	static long bufSizeGranularity;
	static long bufSize;
	static bool outputReady;

	static QuaAudioIn	**input;
	static QuaAudioOut	**output;

	QuaAudioIn *EnableInput(Qua *q, long id);
	bool DisableInput(long id);
	QuaAudioOut *EnableOutput(Qua *q, long id);
	bool DisableOutput(long id);
	status_t AllocateBuffers();
	status_t Start();
	status_t Stop();
	static char *ErrorString(ASIOError);
	static long nActiveInput;
	static long nActiveOutput;
	static ASIOBufferInfo *buffers;

//	static Qua				*uberQua;
	static QuaAudioManager	*audio;
};

#endif

#endif