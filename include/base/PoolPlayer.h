#ifndef _POOLPLAYER
#define _POOLPLAYER

class PoolInstance;
class Qua;

#include "QuaTime.h"
#include "QuaTypes.h"

#include <mutex>

class PoolPlayer
{
public:
	PoolPlayer(Qua *q);
	~PoolPlayer();
					
	void StartInstance(PoolInstance *s);
	void StopInstance(PoolInstance *s);
	void Play(Time &t);
	void Cue(Time &t);
	
	std::mutex schlock;
	std::vector<PoolInstance*> instances;
	qua_status status;
	Qua *uberQua;
};
#endif