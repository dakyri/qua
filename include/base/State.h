#ifndef _STATEH
#define _STATEH

#include <string>
#include <unordered_map>
using namespace std;

// kind of a crude enum, for state machine type thang, numbered in order of arrival
class State
{
public:
	State(char *);
	State(char *, unordered_map<int,string>&map, int);
	~State();

	static void	removeAllStates();	

	void addState(char *nm);
	void setStates(unordered_map<int, string>&map);
	void clearStates();

	string name;
	unordered_map<int, string> states;
	State *next;
};

extern State *quaStates;
extern State play_state;

enum {
	PLAYSTATE_PLAY = 0,
	PLAYSTATE_LOOPEND = 1,
	PLAYSTATE_STOPPED = 2,
	PLAYSTATE_LOOPSTART = 3
};


#endif