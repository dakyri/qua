#include "qua_version.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#endif

// are these global objects or connected with a symbol table entry?

#include "State.h"

KeyVal	player_state_values[] = {
	{ "playing", PLAYSTATE_PLAY},
	{ "loopend", PLAYSTATE_LOOPEND},
	{ "stopped", PLAYSTATE_STOPPED},
	{ "started", PLAYSTATE_LOOPSTART}
};

State	player_state("player state", player_state_values, sizeof(player_state_values)/sizeof(KeyVal));

State	*quaStates = nullptr;

State::State(char *nm)
{
	name = new char[strlen(nm)+1];
	strcpy(name, nm);
	next = quaStates;
	quaStates = next;
}

State::~State()
{
	delete name;
	// shouldn't delete a state if we're adding at the beginning of a linked list
	// doin that so we could just use the enums without quotes within scripts 
	// yeah right
	states.ClearIndex(); // keyindex should do this anyway
}

State::State(char *nm, KeyVal *k, int n)
{
	name = new char[strlen(nm)+1];
	strcpy(name, nm);
	states.SetKeyVal(k, n);
	next = quaStates;
	quaStates = next;
}

void
State::RemoveAllStates()
{
	State *p = quaStates, *q;

	while (p) {
		q = p->next;
		delete p;
		p = q;
	}
}

void
State::AddState(char *nm)
{
	states.AddToIndex(nm, states.nkv);
}

void
State::SetStates(KeyVal *k, int n)
{
	states.SetKeyVal(k, n);
}

void
State::ClearStates()
{
	states.ClearIndex();
}
