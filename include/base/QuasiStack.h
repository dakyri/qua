#ifndef _QUASISTACK_H
#define _QUASISTACK_H

#include "StdDefs.h"

typedef	uint16	stack_size_t;

class StabEnt;
class QuaControllerBridge;
class ControlPanel;
class Block;
class Executable;
class ControlInfoList;
class Instance;
class Qua;
class TimeKeeper;
class Stacker;
class Stackable;

namespace tinyxml2 {
	class XMLElement;
}

#include "QuaTypes.h"
#include "AudioBuffer.h"

class QuasiStack;

// frame map.
// first half assed attempt to build a protocol for mapping qua
// controllers in a way that might work over a network
// frame and subframes with controllers in a single flat structure
//  frame map hdr
//  controllers for frame
//   children
struct frame_map_hdr
{
	StabEnt			*context;
	status_t		status;
	QuasiStack		*frame_handle;
	long			n_children;
	long			n_controller;
};

class QuasiStack {
public:
	QuasiStack(StabEnt *, Stacker *i, StabEnt *sc, Block *B, Block *L, QuasiStack *, TimeKeeper *, char *);
	virtual ~QuasiStack();

	bool				Thunk();
	bool				UnThunk();
	bool				Delete();
	bool				IsInteresting();
	bool				RemoveHigherFrames();
	bool				RemoveControlVariables();
	bool				AddControlVariables();
	bool				RemoveHigherFrameRepresentations();
	bool				PopHigherFrameRepresentations();
	bool				CheckMulchSize();

	bool				GetFrameMap(frame_map_hdr *&map);
	
	void				Dump();
	status_t			Save(ostream &out, short);
	virtual status_t SaveSnapshot(ostream &out, const char *label);
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element, std::vector<std::string> textFrags);

	bool				SetValue(Block *b);

	void				SetAudio(uchar);

	bool				Trigger();
	bool				UnTrigger();
	
	bool				isLocus;
	bool				isActive;
	bool				isLeaf;
	uchar				hasAudio;

	Stackable			*stackable; // corresponding function, code block, vst plugin
	qua_status			locusStatus;
	Block				*callingBlock; // if this is a call
	Block				*enclosingList; // siblings .. mainly used if we have a modal state
	TimeKeeper			*timeKeeper;

	string label;
	Stacker *stacker; // this is either our Instance, or Channel
	StabEnt *stackerSym; // and it's symbol
	StabEnt *context; // and the context that's defined in
	QuasiStack *lowerFrame; // parent of this frame

// TODO XXXX FIXME possbly these belong in a subclass of QuasiStack, similar to QuasiAFXStack eg QuasiVarStack
	char *mulch; // scratch storrage
	uchar *vars; // the base address with our scratch storage .. we have some extra padding for byte alignment, possible

	inline QuasiStack * frameAt(int i) {
		return i >= 0 && ((size_t)i) < higherFrame.size()? higherFrame[i]:nullptr; 
	}
	inline void addFrame(QuasiStack * i) {
		higherFrame.push_back(i);
	}
	inline bool removeFrame(QuasiStack * i) {
		for (auto ci = higherFrame.begin(); ci != higherFrame.end(); ++ci) {
			if (*ci == i) {
				higherFrame.erase(ci);
				return true;
			}
		}
		return false;
	}
	inline int countFrames() { return higherFrame.size(); }

	std::vector<QuasiStack*> higherFrame;
};

class QuasiAFXStack : public QuasiStack {
public:
	QuasiAFXStack(StabEnt *, Stacker *i, StabEnt *sc, Block *B, Block *L,
		QuasiStack *, TimeKeeper *, char *);
	virtual ~QuasiAFXStack();

	virtual status_t SaveSnapshot(ostream &out, const char *label);

	void initBuffers() {
		inputs.zero();
	}

	struct AEffect *afx; // the vst plugin .. at the moment, vst's have all their allocations relative to the plugin instance
	// TODO XXXX FIXME that may well change
	AudioBuffer inputs;
	AudioBuffer outputs;

};


long	AllocStack(StabEnt *, base_type_t, int nelem=1);
void	EndStructureAllocation(StabEnt *sym);
stack_size_t	GetTypeSize(base_type_t);
stack_size_t	GetTypeAlign(base_type_t);

QuasiStack	*FindStackFor(QuasiStack *from, StabEnt *context, StabEnt *label, StabEnt *obj, long frameInd);

#endif