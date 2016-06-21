#ifndef _SCHEDULABLE_H
#define _SCHEDULABLE_H

#include "QuaTime.h"
#include "Metric.h"
#include "Stream.h"
#include "QuaTypes.h"
#include "QuaDisplay.h"
#include "Notifiable.h"
#include "Insertable.h"

#include <vector>
#include <mutex>

class Qua;
class StabEnt;
class Editor;
class Instance;
class TimeArrangeView;
class QuaInsert;

class Schedulable:	public Notifiable,
					public Insertable
{
public:
							Schedulable(StabEnt *s, Qua *, Metric *m);
							~Schedulable();
						
	virtual bool			Init();
	virtual Instance		*addInstance(std::string, Time startt, Time dur, Channel * chan);
	virtual Instance		*addInstance(std::string, short ch_idx, Time *startt, Time *dur, bool disp);
	virtual void			removeInstance(Instance *i, bool display);

	virtual void			Cue(Time &t)=0;
	virtual status_t		Wake(Instance *i);
	virtual status_t		Sleep(Instance *i);
	virtual status_t		Recv(Stream &stream)=0;
	virtual status_t		QuaStop();
	virtual status_t		QuaStart();
	virtual status_t		QuaRecord();
	
	virtual	bool			Trigger();
	virtual	bool			UnTrigger();
	
	void					SetName(char *nm);
	status_t				Schedule(Block *b);

	Qua						*uberQua;
    Metric					*metric;

	Event					sleep,
							wake,
							cue,
							rx,
							start,
							stop,
							record,
							init;
						
	StabEnt					*starttSym;
	StabEnt					*durSym;
	StabEnt					*chanSym;
	
    std::mutex instanceLock;
	inline Instance *instanceAt(int i) {
		instanceLock.lock();
		Instance *inst = i >= 0 && ((size_t)i) < instances.size() ? instances[i] : nullptr;
		instanceLock.unlock();
		return inst;
	}
	inline int countInstances() {
		return instances.size();
	}
	inline void addInstToList(Instance *i) {
		instanceLock.lock();
		instances.push_back(i);
		instanceLock.unlock();
	}
	inline bool removeInstFromList(Instance *i) {
		instanceLock.lock();
		for (auto ci = instances.begin(); ci != instances.end(); ++ci) {
			if (*ci == i) {
				instances.erase(ci);
				instanceLock.unlock();
				return true;
			}
		}
		instanceLock.unlock();
		return false;
	}
//    RWLock				activeLock;
//	BList					activeInstances;
    
    qua_status				status;

	Schedulable				*next;
	std::vector<Instance*>	instances;
};


#endif
