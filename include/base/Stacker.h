#ifndef _ENVELOPPER_H
#define _ENVELOPPER_H

class Envelope;
class ArrangerObject;
class Qua;

#include <mutex>
#include <vector>

#include "QuaTime.h"
#include "QuaTypes.h"

#include "QuaDisplay.h"

class Stacker
{
public:
						Stacker(Qua *);
						~Stacker();
						
	virtual Time		RelativeTime(Time &t);
	
	void				AddEnvelope(Envelope *e);
	void				RemoveEnvelope(Envelope *e);
	Envelope			*EnvelopeAt(int i);
	int					NEnvelopes();
	void				StartEnvelopes();
	void				LoopEnvelopes();
	void				UpdateEnvelopes(Time &t);
	
	std::mutex stackLock;

	std::vector<Envelope*> envelopes;
	Qua					*uberQua;
	qua_status			status;
};

#endif