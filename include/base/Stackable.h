#ifndef _STACKABLE
#define _STACKABLE

#include "QuaTypes.h"


class StabEnt;
class Block;
class QuasiStack;
class Qua;

class Stackable
{
public:
						Stackable(StabEnt *S);
						~Stackable();
	void				ReplaceBlock(Block *&, Block *);
	void				ReAllocateChildren();
	void				AllocateDataStacks(StabEnt *s);
	void				ResizeStacks(short);
	
	inline void addController(StabEnt *sym) { controllers.push_back(sym); }
	inline bool removeController(StabEnt *sym) {
		for (auto ci = controllers.begin(); ci != controllers.end(); ++ci) {
			if (*ci == sym) {
				controllers.erase(ci);
				return true;
			}
		}
		return false;
	}
	inline void clearControllers() { return controllers.clear(); }
	inline StabEnt *controller(int i) { return i>=0 && i<controllers.size()? controllers[i]:nullptr; }
	inline int countControllers() { return controllers.size(); }
	status_t saveControllers(FILE *, int);
	std::vector<StabEnt*> controllers;

    short stackSize;
    StabEnt *sym;
#ifdef LOTSALOX    
    std::mutex stackableLock;
#endif
	inline QuasiStack *stack(int i) { return i>=0 && i<stacksO.size()? stacksO[i]:nullptr; }
	inline void addStack(QuasiStack *i) { stacksO.push_back(i); }
	inline bool removeStack(QuasiStack *i) {
		for (auto ci = stacksO.begin(); ci != stacksO.end(); ++ci) {
			if (*ci == i) {
				stacksO.erase(ci);
				return true;
			}
		}
		return false;
	}
private:
	std::vector<QuasiStack*> stacksO;							
};

#endif