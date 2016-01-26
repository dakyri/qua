#ifndef _SAMPLEPLAYER
#define _SAMPLEPLAYER

#include "Time.h"

class Sample;
class ResultValue;
class SampleBuffer;

// sample player responsible for the ins and outs of sending
// a sample from an object or a script ...

class SamplePlayer
{
public:
					SamplePlayer(Sample *);
					~SamplePlayer();
	
	virtual void	Cue(long cueFrame) {}
	virtual void	NextChunks() {}
	virtual void	Generate(float *outSig, long nFramesReqd, short) {}
	virtual void	SetParameters(ResultValue *, short) {}
	
	inline Sample	*QSample() { return sample; }
					
	SampleBuffer	*currentBuffer;
	long			currentFrame;
	bool			enable;
	Sample			*sample;
};

class PlaySample: public SamplePlayer
{
public:
					PlaySample(Sample *);
					~PlaySample();

	virtual void	Generate(float *outSig, long nFramesReqd, short);
	virtual void	SetParameters(ResultValue *, short);
	
	Time			startFrame;
	Time			endFrame;
	float			gain;
	float			pan;
};

class PlayTake: public SamplePlayer
{
public:
					PlayTake(Sample *s);
};

class PlayPlayList: public SamplePlayer
{
public:
					PlayPlayList(Sample *s);
};

#endif