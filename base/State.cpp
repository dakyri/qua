#include "qua_version.h"

// are these global objects or connected with a symbol table entry?

#include "State.h"

/*
* todo xxxx this stuff was in transition I think ... there seem to be two approaches running in parallel. the POP list is probably the older
* and defunkt path ... need to reexamine this ... the idea has potential even if the implementation sucks
*/
unordered_map<int, string> player_state {
		{ PLAYSTATE_PLAY, "playing" },
		{ PLAYSTATE_LOOPEND, "loopend" },
		{ PLAYSTATE_STOPPED, "stopped" },
		{ PLAYSTATE_LOOPSTART, "started" }
	};

State	*quaStates = nullptr;

State::State(char *nm)
{
	name = nm;
	next = quaStates;
	quaStates = next;
}

State::~State()
{
	// shouldn't delete a state if we're adding at the beginning of a linked list
	// doin that so we could just use the enums without quotes within scripts 
	// yeah right
}

State::State(char *nm, unordered_map<int, string> &map, int n)
{
	name = nm;
	states = map;
	next = quaStates;
	quaStates = next;
}

void
State::removeAllStates()
{
	State *p = quaStates, *q;

	while (p) {
		q = p->next;
		delete p;
		p = q;
	}
}

void
State::addState(char *nm)
{
	int n = states.size();
	states[n] = nm;
}

void
State::setStates(unordered_map<int, string> &map)
{
	states = map;
}

void
State::clearStates()
{
	states.clear();
}
