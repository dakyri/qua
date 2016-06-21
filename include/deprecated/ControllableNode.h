#ifndef _CTRLNODE
#define _CTRLNODE

#ifdef NEW_MEDIA

#include "Schedulable.h"

class QuaAudioPort;
class StabEnt;

class ControllableNode: public Schedulable
{
public:
						ControllableNode(QuaAudioPort *);
						~ControllableNode();
						
};

#endif

#endif