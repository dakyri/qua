#ifndef _STATEH
#define _STATEH

#include "KeyVal.h"

// kind of a crude enum, for state machine type thang, numbered in order of arrival
class State
{
public:
	State(char *);
	State(char *, KeyVal *, int);
	~State();

	static void	RemoveAllStates();	

	void		AddState(char *nm);
	void		SetStates(KeyVal *, int);
	void		ClearStates();

	char		*name;
	KeyIndex	states;
	State		*next;
};

extern State	*quaStates;

extern State	play_state;

enum {
	PLAYSTATE_PLAY = 0,
	PLAYSTATE_LOOPEND = 1,
	PLAYSTATE_STOPPED = 2,
	PLAYSTATE_LOOPSTART = 3
};


#endif