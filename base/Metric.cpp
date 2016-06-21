#include "qua_version.h"

///////////////// System headers and underlying types

#include <vector>

#include "StdDefs.h"


///////////////// Application headers
#include "TimeKeeper.h"
#include "QuaTime.h"
#include "Metric.h"

TimeKeeper::TimeKeeper(Metric *m)
{
	metric = m;
	theTime.Set(0, m);
}
	
Metric Metric::std("bbq", 12,4,4,120);
Metric Metric::mSec("msm", 1000,60,60,60);
Metric Metric::sampleRate("msf", 44100,60,60,60);

std::vector<Metric*> metrix;

Metric *
findMetric(std::string nm)
{
	Metric		*fnd = nullptr;
	for (short i=0; ((size_t)i)<metrix.size(); i++) {
		if (metrix[i]->name == nm)
			return metrix[i];
	}
	return &Metric::std;
}


Metric::Metric(const char *nm, const uint32 gr, const uint32 bb, const uint32 bbg, const float t)
{
	granularity = gr;
	beatsPerBar = bb;
	barsPerGroup = bbg;
	tempo = t;
	
	name.assign(nm);
}

Metric::Metric(const Metric &m)
{
	granularity = m.granularity;
	beatsPerBar = m.beatsPerBar;
	barsPerGroup = m.barsPerGroup;
	tempo = m.tempo;
	
	name.assign(m.name);
	
	metrix.push_back(this);
}

Metric::~Metric()
{
	for (auto m=metrix.begin(); m!=metrix.end(); ++m) {
		if (*m == this) {
			metrix.erase(m);
			return;
		}
	}
}

// equivalence ops.
//  if 2 metrics are equal, times measured in them are comparable
bool
Metric::operator == (Metric &m)
{
	return granularity == m.granularity && tempo == m.tempo;
}

bool
Metric::operator != (Metric &m)
{
	return !(granularity == m.granularity && tempo == m.tempo);
}
