#include "qua_version.h"

#include "StdDefs.h"

///////////////// Application headers
#include "QuaTime.h"

Time	Time::infinity(INFINITE_TICKS,&Metric::std);
Time	Time::zero(0,&Metric::std);

Time::Time(long c, class Metric *g)
{
	ticks = c;
	if (g == nullptr)
		g = &Metric::std;
	metric = g;
}

void
Time::Set(long c, class Metric *g)
{
	ticks = c;
	if (g == nullptr)
		g = &Metric::std;
	metric = g;
}

void
Time::Set(const long b, const long bt, const long c, class Metric *g)
{
	if (g == nullptr)
		g = &Metric::std;
	ticks = c + g->granularity*bt + g->granularity*g->beatsPerBar*b;
	metric = g;
}

void
Time::Set(const char *txt)
{
	uint32		ba=0, be=0, c=0;
	char		metric_nm[128];
	metric_nm[0] = '\0';
	sscanf(txt, "%d:%d.%d %s", &ba, &be, &c, metric_nm);
	if (*metric_nm) {
		metric = findMetric(metric_nm);
	}
	if (metric == nullptr) {
		metric = &Metric::std;
	}
	ticks = c + metric->granularity*be + metric->granularity*metric->beatsPerBar*ba;
}

void
Time::SetToSecs(float t)
{
	ticks = ((tick_t) (t * metric->granularity * (metric->tempo / 60.0)));
}

long
Time::TickValue(Metric *m)
{
	if (metric == m || *metric == *m)
		return ticks;
	return (ticks * m->granularity *m->tempo )/(metric->granularity*metric->tempo);
}

void
Time::Reset()
{
	ticks = 0;
}

Time
Time::operator ~()
{
	uint32 gr = metric->granularity;
	return Time(gr * (ticks / gr), metric);
}

long
Time::BeatValue()
{
	return ticks/((long)metric->granularity);
}

float
Time::SecsValue() const
{
	return 60.0*(((double)ticks) / (metric->granularity*metric->tempo));
}

void
Time::IncrementTick()
{
	ticks++;
}

void
Time::IncrementBeat()
{
	ticks += metric->granularity;
}

void
Time::IncrementBar()
{
	ticks += metric->beatsPerBar*metric->granularity;
}

void
Time::DecrementTick()
{
	ticks--;
}

void
Time::DecrementBeat()
{
	ticks -= metric->granularity;
}

void
Time::DecrementBar()
{
	ticks -= metric->beatsPerBar*metric->granularity;
}

bool
Time::operator >= (const Time &t2)
{
	if (ticks == INFINITE_TICKS) {
		return true;
	} else if (t2.ticks == INFINITE_TICKS) {
		return false;
	} else if(*metric == *t2.metric) {
		return ticks >= t2.ticks;
	} else {
		return SecsValue() >= t2.SecsValue();
	}
}

bool
Time::operator <= (const Time& t2)
{
	if (t2.ticks == INFINITE_TICKS) {
		return true;
	} else if (ticks == INFINITE_TICKS) {
		return false;
	} else if (*metric == *t2.metric) {
		return ticks <= t2.ticks;
	} else {
		return SecsValue() <= t2.SecsValue();
	}
}

bool
Time::operator < (const Time& t2)
{
	if (t2.ticks == INFINITE_TICKS) {
		return true;
	} else if (ticks == INFINITE_TICKS) {
		return false;
	} else if (*metric == *t2.metric) {
		return ticks < t2.ticks;
	} else {
		return SecsValue() < t2.SecsValue();
	}
}

bool
Time::operator > (const Time& t2)
{
	if (ticks == INFINITE_TICKS) {
		return true;
	} else if (t2.ticks == INFINITE_TICKS) {
		return false;
	} else if (*metric == *t2.metric) {
		return ticks > t2.ticks;
	} else {
		return SecsValue() > t2.SecsValue();
	}
}

bool
Time::operator != (const Time &t2)
{
	if (ticks == INFINITE_TICKS && t2.ticks == INFINITE_TICKS) {
		return false;
	} else if (*metric == *t2.metric) {
		return ticks != t2.ticks;
	} else {
		return SecsValue() != t2.SecsValue();
	}
}

bool
Time::operator == (const Time& t2)
{
	if (ticks == INFINITE_TICKS && t2.ticks == INFINITE_TICKS) {
		return false;
	} else if (*metric == *t2.metric) {
		return ticks == t2.ticks;
	} else {
		return SecsValue() == t2.SecsValue();
	}
}

Time
Time::operator + (const Time &t2)
{
	Time	time;

	if (ticks == INFINITE_TICKS || t2.ticks==INFINITE_TICKS) {
		time.ticks = INFINITE_TICKS;
		time.metric = &Metric::std;
	} else if (*metric == *t2.metric) {
		time.ticks = ticks + t2.ticks;
		time.metric = metric;
	} else {
		time.ticks = (tick_t)(1000.0*(SecsValue() + t2.SecsValue()));
		time.metric = &Metric::mSec;
	}
	return time;
}
 
Time
Time::operator - (const Time& t2)
{
	Time	ti;

	if (ticks == INFINITE_TICKS || t2.ticks==INFINITE_TICKS) {
		ti.ticks = INFINITE_TICKS;
		ti.metric = &Metric::std;
	} else if (*metric == *t2.metric) {
		ti.ticks = ticks - t2.ticks;
		ti.metric = metric;
	} else {
		ti.ticks = (tick_t)(1000.0*(SecsValue() - t2.SecsValue()));
		metric = &Metric::mSec;
	}
	return ti;
}

bool
Time::operator ! () const
{
	return ticks == 0;
}

Time
Time::operator - (const int t2)
{
	Time	ti;

	if (ticks == INFINITE_TICKS || t2 == INFINITE_TICKS) {
		ti.ticks = INFINITE_TICKS;
		ti.metric = metric;
	} else {
		ti.ticks = ticks - t2;
		ti.metric = metric;
	}
	return ti;
}

Time
Time::operator + (const int t2)
{
	Time	ti;

	if (ticks == INFINITE_TICKS || t2 == INFINITE_TICKS) {
		ti.ticks = INFINITE_TICKS;
		ti.metric = metric;
	} else {
		ti.ticks = ticks + t2;
		ti.metric = metric;
	}
	return ti;
}

Time
Time::operator += (const Time& t2)
{
	if (ticks == INFINITE_TICKS || t2.ticks==INFINITE_TICKS) {
		ticks = INFINITE_TICKS;
	} else if (*metric == *t2.metric) {
		ticks += t2.ticks;
	} else {
		ticks = (tick_t)(1000.0*(SecsValue() + t2.SecsValue()));
		metric = &Metric::mSec;
	}

	return *this;
}

Time
Time::operator -= (const Time &t2)
{
	if (ticks == INFINITE_TICKS || t2.ticks==INFINITE_TICKS) {
		ticks = INFINITE_TICKS;
	} else if (*metric == *t2.metric) {
		ticks -= t2.ticks;
	} else {
		ticks = (tick_t)(1000.0*(SecsValue() - t2.SecsValue()));
		metric = &Metric::mSec;
	}

	return *this;
}

Time
Time::operator %= (const Time &t2)
{
	if (ticks == INFINITE_TICKS || t2.ticks==INFINITE_TICKS) {
		ticks = INFINITE_TICKS;
	} else if (*metric == *t2.metric) {
		ticks %= t2.ticks;
	} else {
		ticks = (tick_t)(1000.0*(((int)SecsValue())%((int)t2.SecsValue())));
		metric = &Metric::mSec;
	}
	return *this;
}


Time
Time::operator / (const int n)
{
	Time	ti;
	
	if (ticks == INFINITE_TICKS || n == 0) {
		ti.ticks = INFINITE_TICKS;
		ti.metric = metric;
	} else {
		ti.ticks = ticks / n;
		ti.metric = metric;
	}
	return ti;
}



long
Time::operator = (const long n)
{
	ticks = n;
	return n;
}

Time
Time::operator &(Metric * const m)
{
	if (m == nullptr || metric == m || *metric == *m) {
		return *this;
	}

	Time	t1;
	t1.metric = m;
	double	f = (m->tempo*m->granularity)/(metric->tempo*metric->granularity);
	t1.ticks = ((double)ticks)*f;
	return t1;
}

Time
Time::operator &=(Metric *const m)
{
	if (m != nullptr) {
		if (metric != m && *metric != *m) {
			double	f = (m->tempo*m->granularity)/(metric->tempo*metric->granularity);
			ticks = ((double)ticks)*f;
		}
		metric = m;
	}

	return *this;
}

Time
Time::operator &(const Metric &m)
{
	if (*metric == m || metric == nullptr) {
		return *this;
	}

	Time	t1;
	t1.metric = const_cast<Metric*>(&m);
	double	f = (m.tempo*m.granularity)/(metric->tempo*metric->granularity);
	t1.ticks = ((double)ticks)*f;
	return t1;
}

Time
Time::operator &=(const Metric &m)
{
	if (*metric != m && metric != nullptr) {
		double	f = (m.tempo*m.granularity)/(metric->tempo*metric->granularity);
		ticks = ((double)ticks)*f;
		metric = const_cast<Metric*>(&m);
	}

	return *this;
}

char *
Time::StringValue()
{
	static string buf;
	int bar, barBeat, beatTick;
	if (ticks == INFINITE_TICKS) {
		buf = "forever";
	} else {
		GetBBQValue(bar, barBeat, beatTick);
		buf = to_string(bar) + ":" + to_string(barBeat) + "." + to_string(beatTick);
	}
	return const_cast<char *>(buf.c_str());
}

void
Time::GetBBQValue(int &bar, int &bbeat, int &btick)
{
	btick = ticks % metric->granularity;
	bbeat = (ticks / metric->granularity) % metric->beatsPerBar;
	bar = ticks / (metric->granularity * metric->beatsPerBar);
	if (btick < 0) {
		btick += metric->granularity;
		bbeat--;
	}
	if (bbeat < 0) {
		bbeat += metric->beatsPerBar;
		bar--;
	}
}
