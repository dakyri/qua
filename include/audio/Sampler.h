#ifndef _SAMPLER
#define _SAMPLER

// actually the main audio generator for qua.
// audio port and audio manager represent the actual hardware
// devices and a roster of such that this beast talks to.

#ifdef _BEOS

class Sample;
class SampleInstance;
class SampleEditor;
class QuaAudioPort;

#define kAudioBufferSize 4096
#define kNumAudioBuffers 6
#define kSamplingRate	44100

#include "QuaTypes.h"

class Sampler
{
public:
	Sampler(class Qua *);
	~Sampler();

	void				StartSampler();
	void				StopSampler();
	
	void				StartPreview(SampleEditor *s);
	void				StopPreview(SampleEditor *s);

	SchLock				editorsLock;
	BList				editors;

	Qua					*uberQua;

	qua_status			status;
};

#undef SEMWAIT	//8

#endif

#endif