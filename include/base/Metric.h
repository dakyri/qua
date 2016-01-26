#ifndef _METER
#define _METER
#include "StdDefs.h"
enum {
	NO_METRIC=0,
	STD_METRIC=1
};

typedef short	metric_id;

#include <string>
#include <vector>

class Metric {
public:
	Metric(const char *, const uint32 gr, const uint32 bb, const uint32 bbg, const float t);
	Metric(const Metric &m);
	~Metric();
				
	bool operator==(Metric &m);
	bool operator!=(Metric &m);

	inline double secsPerTick() { return 60.0 / (granularity * tempo); }
	
	float tempo;
	uint32 granularity;
	uint32 beatsPerBar;
	uint32 barsPerGroup;
	
	static Metric std;
	static Metric mSec;
	static Metric sampleRate;

	std::string name;
};


Metric			*FindMetric(char *);
extern std::vector<Metric*>		metrix;
#endif
