#ifndef _ENVELOPPER_H
#define _ENVELOPPER_H

class Envelope;
class ArrangerObject;
class Qua;

#include <mutex>
#include <vector>
#include <algorithm>

#include "QuaTime.h"
#include "QuaTypes.h"

#include "QuaDisplay.h"


class QuasiAFXStack;

class Stacker
{
public:
	 Stacker(Qua *);
	 ~Stacker();
						
	virtual Time RelativeTime(Time &t);
	
	void AddEnvelope(Envelope *e);
	void RemoveEnvelope(Envelope *e);
	Envelope *EnvelopeAt(int i);
	int NEnvelopes();
	void StartEnvelopes();
	void LoopEnvelopes();
	void UpdateEnvelopes(Time &t);

	vector<QuasiAFXStack*> afxStax;

	void initAFXStackBuffers() {
		stackLock.lock();
		for (auto it : afxStax) {
			it->initBuffers();
		}
		stackLock.unlock();
	}

	void addAFXStack(QuasiAFXStack * stk) {
		stackLock.lock();
		auto it = find(afxStax.begin(), afxStax.end(), stk);
		if (it == afxStax.end()) {
			afxStax.push_back(stk);
		}
		stackLock.unlock();
	}

	void removeAFXStack(QuasiAFXStack* stk) {
		stackLock.lock();
		auto it = find(afxStax.begin(), afxStax.end(), stk);
		if (it == afxStax.end()) {
			afxStax.erase(it);
		}
		stackLock.unlock();
	}
	
	mutex stackLock;

	vector<Envelope*> envelopes;
	Qua *uberQua;
	qua_status status;
};

#endif