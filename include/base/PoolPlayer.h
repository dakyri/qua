#ifndef _POOLPLAYER
#define _POOLPLAYER

#ifdef __INTEL__
#include <SupportKit.h>
#endif

class PoolInstance;
class Qua;

#include "Time.h"
#include "QuaTypes.h"

class PoolPlayer
{
public:
					PoolPlayer(Qua *q);
					~PoolPlayer();
					
	void			StartInstance(PoolInstance *s);
	void			StopInstance(PoolInstance *s);
	void			Play(Time &t);
	void			Cue(Time &t);
	
	std::mutex schlock;
	std::vector<PoolInstance*> instances;
	qua_status		status;
	Qua				*uberQua;
};
#endif