#ifndef SAWTOOTHOSC
#define SAWTOOTHOSC

class SawtoothOscilator
{
public:
							SawtoothOscilator(float amp = 1.);
	
	SawtoothOscilator&		Amplitude(float amp);
	SawtoothOscilator&		Frequency(float freq, float sampleRate);	
	SawtoothOscilator&		Frequency(float freq);
	
	Sample					operator ()	(void);
	void					operator ()	(Signal& outSig);
	
private:
	float 					currentValue;
};


// SAWTOOTH

inline SawtoothOscilator::SawtoothOscilator(float amp)
	: currentValue(1.)
{
}

inline SawtoothOscilator& SawtoothOscilator::Amplitude(float amp)
{
	currentValue /= inc;
	inc = ( 2 * amp ) / ( rval / inc );
	currentValue *= inc;
	min = -amp;
	rval = 2*amp;
	
	return *this;
}

inline SawtoothOscilator&
SawtoothOscilator::Frequency(float freq, float sampleRate)
{
	inc = rval / (sampleRate / freq);
	return *this;
}

inline float SawtoothOscilator::operator()(float inc, float amp)
{
	currentValue -= inc;
	if(currentValue < amp) {
		currentValue += 2*amp;
	}
	
	return currentValue;
}
	
inline void SawtoothOscilator::operator()(float *outSig, long nFramesReqd, float inc, float amp)
{
	for(long i=0;i<nFramesReqd;i++) {
		outSig[i]=(*this)(inc, amp);
	}
}

class SquareOscilator
{
public:
							SquareOscilator(float amp = 1.);
	
	void					Width(float amp);
	void					Amplitude(float amp);
	void					Frequency(float freq, float sampleRate=signalSampleRate);	
	
	Sample					operator ()	(void);
	void					operator ()	(Signal& outSig);
	
private:
	long					sample;
	float					cycle;
	float 					amp;
	long					up;
	long					halfCycle;
	float					width;
};


inline SquareOscilator::SquareOscilator(float a)
{
	sample = -1;
	amp = a;
	width = 1;
	Frequency(256);
}

inline void
SquareOscilator::Amplitude(float a)
{
	amp = a;
}

inline void
SquareOscilator::Width(float a)
{
	width = (a<0?0:a>1?1:a);
}

inline void
SquareOscilator::Frequency(float freq, float sampleRate)
{
	cycle = (sampleRate / freq);
	halfCycle = cycle/2;
	up = (1.0+7.0*width)*halfCycle/8.0;
}

	
inline Sample
SquareOscilator::operator()(void)
{
	float	val=0;
	if (++sample < up)
		return amp;
	else if (sample >= halfCycle && sample <= halfCycle+up)
		return -amp;
	if (sample >= cycle)
		sample = -1;
	return val;
}
	
inline void
SquareOscilator::operator()(Signal& outSig)
{
	for(unsigned long i=0;i<maxFramesPerChunk;i++)
		outSig[i]=(*this)();
}

class TriangleOscilator
{
public:
							TriangleOscilator(float amp = 1.);
	
	void					Skew(float amp);
	void					Skuar(float amp);
	void					Amplitude(float amp);
	void					Phase(float phs);
	void					Frequency(float freq, float sampleRate, float sk);	
	
	Sample					operator ()	(void);
	void					operator ()	(Signal& outSig);
	
private:
	float 					amp;
	float					inc_u;
	float					inc_d;
	float					inc;
	float					peak;
	float					currentValue;
	long					cycle;
	float					frequency;
	bool					up;
	float					skew;
	float					skuar;
};

inline void
TriangleOscilator::Phase(float phs)
{
	float	sam = (phs/(2*M_PI));
	if (sam > 0.5) {
		currentValue = inc_u * (sam-0.5) - amp;
		up = true;
	} else {
		currentValue = amp - inc_d * sam;
		up = false;
	}
}

inline
TriangleOscilator::TriangleOscilator(float a)
{
	currentValue = peak = amp = a;
	skew = 0;
	skuar = 0;
	up = false;
	frequency = 0;
	
	Frequency(256, signalSampleRate, 0);
}

inline void
TriangleOscilator::Amplitude(float a)
{
	peak = amp = a;
}

inline void
TriangleOscilator::operator()(Signal& outSig)
{
	for(unsigned long i=0;i<maxFramesPerChunk;i++)
		outSig[i]=(*this)();
}

inline void
TriangleOscilator::Frequency(float freq, float sampleRate, float a)
{
	if (freq != frequency || skew != a) {
		frequency = freq;
		skew = (a<0?0:a>1?1:a);
		
		cycle = (sampleRate / freq);
		int		rx = (1-skew)*cycle/2.0;
		if (rx < 1) rx = 1;
		inc_u = (2*peak)/rx;
		inc_d = (2*peak)/(cycle-rx);
		inc = up? inc_u: -inc_d;
	}
}

inline Sample
TriangleOscilator::operator()(void)
{
	currentValue += inc;
	if (currentValue <= -peak) {
		currentValue = -peak;
		inc = inc_u;
		up = true;
	} else if (currentValue >= peak) {
		currentValue = peak;
		inc = -inc_d;
		up = false;
	}

	return currentValue;
}



#endif