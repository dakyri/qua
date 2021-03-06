#ifndef _QUADISPLAY
#define _QUADISPLAY


#include "BaseVal.h"
#include "QuaTypes.h"

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct qua_time;

class Instance;
class Schedulable;
class QuasiStack;
class Lambda;
class Channel;
class Metric;
class Template;
class VstPlugin;
class Block;
class Qua;
class StabEnt;
class Channel;
class QuaPort;
class Insertable;
class QuaInsert;
class Input;
class Output;
class Time;

class QuaBridge;

class QuaInsertableBridge;
class QuaInstanceBridge;
class QuaVstBridge;
class QuaTemplateBridge;
class QuaStackBridge;
class QuaPortBridge;
class QuaMethodBridge;
class QuaChannelBridge;
class QuaSchedulableBridge;

class QuaObjectRepresentation;
class QuaSchedulableObjectRepresentation;
class QuaInstanceObjectRepresentation;

class QuaChannelRepresentation;
class QuaInputRepresentation;
class QuaOutputRepresentation;
class QuaInstanceRepresentation;

class QuaPerspective;

class QuaChannelRackPerspective;
class QuaArrangerPerspective;
class QuaObjectRackPerspective;
class QuaTransportPerspective;
class QuaIndexPerspective;

class QuaGlobalIndexPerspective;
class QuaEnvironmentPerspective;

struct frame_map_hdr;
enum qua_perpective_type {
	QUAPSCV_NOT = 0,
	QUAPSCV_OBJECTRACK= 1,
	QUAPSCV_CHANNELRACK = 2,
	QUAPSCV_ARRANGER = 3,
	QUAPSCV_TRANSPORT = 4,
	QUAPSCV_GLOBALINDEX = 5,
	QUAPSCV_ENVIRON = 6
};

class QuaPort;

/*
 * an entity that can represent error messages
 * the default implementation prints to the console
 */
class ErrorHandler {
public:
	virtual void parseErrorViewClear();
	virtual void parseErrorViewAddLine(std::string, int severity);
	virtual void parseErrorViewShow();
	virtual void tragicError(const char *str, ...);
	virtual void reportError(const char *str, ...);
};


/*
 * the basic requirements that a sequencer has or would like to have from a display or display-like entity
 * it's ok if these are all stubs. This is the minimum functional display required by the constructor
 */
class QuaReflection: public ErrorHandler {
public:
	virtual bool setup(Qua *) /** basic setup called on creation */{
		return true;
	}
	virtual bool spawn() { /** called when local init and setup is complete*/
		return true;
	}
	virtual bool cleanup() { /** called as we're being deleted */
		return true;
	}

	virtual void AddSchedulableRepresentation(Schedulable *) = 0;
	virtual void AddLambdaRepresentation(Lambda *) = 0;

	virtual void AddDestinationRepresentations(Channel *) = 0;
	virtual void RemoveDestinationRepresentation(Channel *, Input *) = 0;
	virtual void RemoveDestinationRepresentation(Channel *, Output *) = 0;

	virtual void RemoveSchedulableRepresentations(StabEnt *) = 0;
	virtual void RemoveMethodRepresentations(StabEnt *) = 0;
	virtual void RemoveChannelRepresentations(StabEnt *) = 0;
	virtual void RemoveInstanceRepresentations(StabEnt *) = 0;

	virtual void UpdateTakeIndexDisplay(StabEnt *sym) = 0;
	virtual void updateClipIndexDisplay(StabEnt *sym) = 0;
	virtual void UpdateVariableIndexDisplay(StabEnt *sym) = 0;

	virtual bool HasDisplayParameters(StabEnt *) = 0;
	virtual char *DisplayParameterId() = 0;
	virtual string GetDisplayParameters(StabEnt *) = 0;
	virtual void* ReadDisplayParameters(FILE *) = 0;
	virtual void SetDisplayParameters(StabEnt *, void*) = 0;

	virtual void displayArrangementTitle(const char *) = 0;

	// display the current master tempo/master time ... 
	virtual void DisplayTempo(float tempo, bool async) = 0;
	virtual void DisplayGlobalTime(Time &t, bool async) = 0;

	virtual void DisplayDuration(Instance *) = 0;
	virtual void DisplayChannel(Instance *) = 0;
	virtual void DisplayStartTime(Instance *) = 0;
	virtual void DisplayWake(Instance *) = 0;
	virtual void DisplaySleep(Instance *) = 0;

	virtual void RemoveHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent) = 0;
#ifdef Q_FRAME_MAP
#else
	virtual void PopHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent) = 0;
#endif

	virtual void UpdateControllerDisplay(StabEnt *stackerSym, QuasiStack *stack, StabEnt *sym,
		qua_perpective_type srctype = QUAPSCV_NOT, QuaPerspective *src = nullptr) = 0;
	virtual void ControllerChanged(StabEnt *sym, QuasiStack *, TypedValue &t,
		qua_perpective_type srctype = QUAPSCV_NOT, QuaPerspective *src = nullptr) = 0;

	virtual void DisplayStatus(Instance *, qua_status) = 0;
	virtual void DisplayStatus(QuasiStack *, qua_status) = 0;

	virtual void RemoveQuaNexion(QuaInsert *) = 0;
	virtual void AddQuaNexion(QuaInsert *, QuaInsert *) = 0;

	virtual void RemoveControlVariables(QuasiStack *) = 0;
	virtual void AddControlVariables(QuasiStack *) = 0;
};


class PerspectiveHolder {
public:
	virtual void AddChannelRack(QuaChannelRackPerspective *i) = 0;
	virtual void RemoveChannelRack(long i) = 0;
	virtual int NChannelRack() = 0;
	virtual QuaChannelRackPerspective *ChannelRack(long i) = 0;
	virtual void AddArranger(QuaArrangerPerspective *i) = 0;
	virtual void RemoveArranger(long i) = 0;
	virtual int NArranger() = 0;
	virtual QuaArrangerPerspective *Arranger(long i) = 0;
	virtual void AddIndexer(QuaIndexPerspective *i) = 0;
	virtual void RemoveIndexer(long i) = 0;
	virtual int NIndexer() = 0;
	virtual QuaIndexPerspective *Indexer(long i) = 0;
	virtual void AddObjectRack(QuaObjectRackPerspective *i) = 0;
	virtual void RemoveObjectRack(long i) = 0;
	virtual int NObjectRack() = 0;
	virtual QuaObjectRackPerspective *ObjectRack(long i) = 0;
	virtual void AddTransporter(QuaTransportPerspective *i) = 0;
	virtual void RemoveTransporter(long i) = 0;
	virtual int NTransporter() = 0;
	virtual QuaTransportPerspective *Transporter(long i) = 0;
};

/*
 * attempt to create a generic interface layer so I can jump to something
 * a bit more baroque like gl from standard widget or a http network interface
 *  perceptual set .... some way of grouping a set of perspectives and their
 *  bits and pieces for appropriate hot plugging e.g. a particular QuaPerceptualSet
 * might correspond to a local display, or a remote quaLink to a distant qua, or even a simultaneous 3d  view
 * 
 * - core display hook for a running sequencer to do things to an interface
 * - an idealized set of methods for the interface to manipulate the sequencer
 * - a collection of different perspectives for doing more detailed display
 */
class QuaPerceptualSet: public PerspectiveHolder, public QuaReflection
{
public:
	virtual void AddChannelRepresentations(QuaChannelRackPerspective *i) = 0;
	virtual void AddChannelRepresentations() = 0;

	virtual void ShowObjectRepresentation(StabEnt *) = 0;
	virtual void HideObjectRepresentation(StabEnt *) = 0;
	virtual void RequestPopFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *rootSym, QuasiStack *parent, QuasiStack *stack, bool show, bool add_children) = 0;
	virtual void RequestRemoveFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *stackerSym, QuasiStack *stack) = 0;
#ifdef Q_FRAME_MAP
	virtual void PopFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *, long)=0;
	virtual void HideFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack)=0;
	virtual void RemoveFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack)=0;
#else
	virtual void PopFrameRepresentation(StabEnt *rootSym, QuasiStack *stack)=0;
//	void PopFrameRepresentations(StabEnt *, QuasiStack *);
#endif
	virtual void HideFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) = 0;
	virtual void RemoveFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) = 0;

	virtual void Rename(StabEnt *, const string &)=0;

	//////////////////////////////////////////////////////////////////
	// calls from this display to interrogate the source sequence
	//////////////////////////////////////////////////////////////////
	virtual long NChannel() = 0;
	virtual StabEnt *QuaSym() = 0;
	virtual StabEnt *ChannelSym(short i) = 0;
	virtual std::vector<StabEnt*> SchedulableSyms() = 0;
	virtual std::vector<StabEnt*> MethodSyms(StabEnt *) = 0;
	virtual std::vector<StabEnt*> InstanceSyms(StabEnt *) = 0;
	virtual std::vector<StabEnt*> ClipSyms(short) = 0;

	virtual std::vector<Envelope*> ListEnvelopesFor(StabEnt *stacker) = 0;

	virtual Metric *QMetric() = 0;

	//////////////////////////////////////////////////////////////////
	// calls from this display to make changes to the source sequencer
	//////////////////////////////////////////////////////////////////
	virtual void GotoStartOfClip(StabEnt *) = 0;
	virtual void GotoEndOfClip(StabEnt *) = 0;
	virtual void SelectRegion(StabEnt *) = 0;

	virtual StabEnt * CreateSample(const std::string &nm, const std::vector<std::string> & p1) = 0;
	virtual StabEnt * CreateVoice(const std::string &nm, const std::vector<std::string> & pl) = 0;
	virtual StabEnt * CreatePool(const std::string &nm, const std::vector<std::string> & pl) = 0;
	virtual StabEnt * CreateChannel(
		char *nm = nullptr, short c = -1,
		uchar nin = 2, uchar nout = 2,
		bool add_dflt_au_in = false,
		bool add_dflt_au_out = false,
		bool add_dflt_str_in = false,
		bool add_dflt_str_out = false
	) = 0;
	virtual StabEnt	* CreateMethod(const std::string &nm, StabEnt * const p = nullptr) = 0;
	virtual StabEnt	* CreateClip(const std::string &nm, const Time &st, const Time &dur) = 0;

	// callbacks from the interface to create and manipulate instances of schedulables
	virtual StabEnt * CreateInstance(StabEnt * const, const short, const Time &, const Time &) = 0;
	virtual void MoveInstance(StabEnt * const, const short, const Time &, const Time &) = 0;
	virtual void DeleteInstance(StabEnt *) = 0;

	virtual void CreateStreamTake(StabEnt *) = 0;
	virtual void LoadSampleTake(StabEnt *, std::string) = 0;

	virtual void DeleteObject(StabEnt *) = 0;

	virtual short CompileBlock(StabEnt *, char *srcnm, char *txt, long textlen) = 0;
	virtual short ParseBlock(StabEnt *, char *srcnm, char *txt, long textlen) = 0;

	// tempo or time is changed from the interface ... updates these in the interface ... also calls the respective DisplayXXX function
	virtual void UpdateTempo(float tempo) = 0;
	virtual void UpdateGlobalTime(Time &t) = 0;
};

//////////////////////////////////////////////////////////////////////////

/**
* QuaDisplayStub
* a default, do nothing display. absolute minimal instantiable base
*/
class QuaDisplayStub : public QuaReflection
{
public:
//////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void AddSchedulableRepresentation(Schedulable *) override { };
	virtual void AddLambdaRepresentation(Lambda *) override { };

	virtual void AddDestinationRepresentations(Channel *) override { };
	virtual void RemoveDestinationRepresentation(Channel *, Input *) override { };
	virtual void RemoveDestinationRepresentation(Channel *, Output *) override { };

	virtual void UpdateControllerDisplay(StabEnt *stackerSym, QuasiStack *stack, StabEnt *sym,
		qua_perpective_type srctype = QUAPSCV_NOT, QuaPerspective *src = nullptr) override { };
	virtual void ControllerChanged(StabEnt *sym, QuasiStack *, TypedValue &t,
		qua_perpective_type srctype = QUAPSCV_NOT, QuaPerspective *src = nullptr) override { };

	virtual void DisplayStatus(Instance *, qua_status) override {};
	virtual void DisplayStatus(QuasiStack *, qua_status) override {};

	virtual void UpdateTakeIndexDisplay(StabEnt *sym) override { };
	virtual void updateClipIndexDisplay(StabEnt *sym) override { };
	virtual void UpdateVariableIndexDisplay(StabEnt *sym) override { };

	// display the current master tempo/master time ... 
	virtual void DisplayTempo(float tempo, bool async) override {};
	virtual void DisplayGlobalTime(Time &t, bool async) override {};

#ifdef Q_FRAME_MAP
	virtual void PopFrameRepresentations(StabEnt *, QuasiStack *, frame_map_hdr *, long) override {}
#else
	virtual void PopHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent)override  { };
#endif
	virtual void RemoveHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent)override { };


	virtual void RemoveSchedulableRepresentations(StabEnt *) override { };
	virtual void RemoveMethodRepresentations(StabEnt *) override { };
	virtual void RemoveChannelRepresentations(StabEnt *) override { };
	virtual void RemoveInstanceRepresentations(StabEnt *) override { };

	virtual void displayArrangementTitle(const char *) override { };

	virtual void DisplayDuration(Instance *) override { }
	virtual void DisplayChannel(Instance *) override { }
	virtual void DisplayStartTime(Instance *) override { }
	virtual void DisplayWake(Instance *) override { }
	virtual void DisplaySleep(Instance *) override { }

	virtual bool HasDisplayParameters(StabEnt *) override { return false; }
	virtual char *DisplayParameterId() override { return ""; }
	virtual string GetDisplayParameters(StabEnt *) override { return string(); }
	virtual void* ReadDisplayParameters(FILE *) override { return nullptr; }
	virtual void SetDisplayParameters(StabEnt *, void*) override { };

	virtual void RemoveQuaNexion(QuaInsert *) override { };
	virtual void AddQuaNexion(QuaInsert *, QuaInsert *) override { };

	virtual void RemoveControlVariables(QuasiStack *) override { };
	virtual void AddControlVariables(QuasiStack *) override { };

};

extern QuaDisplayStub defaultDisplay;

class QSParser;

//////////////////////////////////////////////////////////////////////////

/**
 * QuaDisplay
 * local display with different windows
 */
class QuaDisplay: public QuaPerceptualSet
{
public:
	QuaDisplay();
	~QuaDisplay();

	////////////////////////////////////////////////////////////////////
	// collections of perspectives belonging to this representation
	////////////////////////////////////////////////////////////////////
	virtual void AddChannelRack(QuaChannelRackPerspective *i) { channelRacks.push_back(i); }
	virtual void RemoveChannelRack(long i) { channelRacks.erase(channelRacks.begin() + i); }
	virtual int NChannelRack() { return channelRacks.size(); }
	virtual QuaChannelRackPerspective *ChannelRack(long i) {
		return i >= 0 && ((size_t)i)<channelRacks.size() ? channelRacks[i] : nullptr;
	}

	virtual void AddArranger(QuaArrangerPerspective *i) { arrangers.push_back(i); }
	virtual void RemoveArranger(long i) { arrangers.erase(arrangers.begin() + i); }
	virtual int NArranger() { return arrangers.size(); }
	virtual QuaArrangerPerspective *Arranger(long i) {
		return i >= 0 && ((size_t)i) < arrangers.size() ? arrangers[i] : nullptr;
	}

	virtual void AddIndexer(QuaIndexPerspective *i) { indexers.push_back(i); }
	virtual void RemoveIndexer(long i) { indexers.erase(indexers.begin() + i); }
	virtual int NIndexer() { return indexers.size(); }
	virtual QuaIndexPerspective *Indexer(long i) {
		return i >= 0 && ((size_t)i) < indexers.size() ? indexers[i] : nullptr;
	}

	virtual void AddObjectRack(QuaObjectRackPerspective *i) { objectRacks.push_back(i); }
	virtual void RemoveObjectRack(long i) {
		objectRacks.erase(objectRacks.begin() + i);
	}
	virtual int NObjectRack() { return objectRacks.size(); }
	virtual QuaObjectRackPerspective *ObjectRack(long i) {
		return i >= 0 && ((size_t)i) < objectRacks.size() ? objectRacks[i] : nullptr;
	}

	virtual void AddTransporter(QuaTransportPerspective *i) { transporters.push_back(i); }
	virtual void RemoveTransporter(long i) { objectRacks.erase(objectRacks.begin() + i); }
	virtual int NTransporter() { return transporters.size(); }
	virtual QuaTransportPerspective *Transporter(long i) {
		return i >= 0 && ((size_t)i) < transporters.size() ? transporters[i] : nullptr;
	}

	std::vector<QuaChannelRackPerspective *> channelRacks;
	std::vector<QuaArrangerPerspective *> arrangers;
	std::vector<QuaIndexPerspective *> indexers;
	std::vector<QuaObjectRackPerspective *> objectRacks;
	std::vector<QuaTransportPerspective *> transporters;

//////////////////////////////////////////////////////////////////////////////
// variouos virutals
/////////////////////////////////////////////////////////////////////////////

	virtual void displayArrangementTitle(const char *) override;
	virtual bool setup(Qua *q) override;
	virtual bool spawn() override;
	virtual bool cleanup() override;

//	QuaSymbolBridge			*FindExecutableRepresentation(StabEnt *S);

	virtual void AddChannelRepresentations(QuaChannelRackPerspective *i);
	virtual void AddChannelRepresentations();


	virtual void UpdateTakeIndexDisplay(StabEnt *sym) override;
	virtual void updateClipIndexDisplay(StabEnt *sym) override;
	virtual void UpdateVariableIndexDisplay(StabEnt *sym) override;

	// display the current master tempo/master time ... 
	virtual void DisplayTempo(float tempo, bool async) override;
	virtual void DisplayGlobalTime(Time &t, bool async) override;

	virtual void UpdateControllerDisplay(StabEnt *stackersym, QuasiStack *stack, StabEnt *sym,
				qua_perpective_type srctype=QUAPSCV_NOT, QuaPerspective *src = nullptr) override;
	virtual void ControllerChanged(StabEnt *sym, QuasiStack *stack, TypedValue &t,
				qua_perpective_type srctype=QUAPSCV_NOT, QuaPerspective *src = nullptr) override;

	virtual void DisplayStatus(Instance *, qua_status) override;
	virtual void DisplayStatus(QuasiStack *, qua_status) override;
	
	// called by interface to pop the stack frame, given a map of it at a particular level
#ifdef Q_FRAME_MAP
	virtual void RequestPopFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *rootSym, QuasiStack *parent, QuasiStack *stack, bool show, bool add_children);
	virtual void PopFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *, long) override;
#else
	virtual void RequestPopFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *rootSym,
		QuasiStack *parent, QuasiStack *stack, bool show, bool add_children) override;
	virtual void PopFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) override;
#endif
	virtual void RemoveFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack);
	virtual void RequestRemoveFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *stackerSym, QuasiStack *stack) override;
	virtual void HideFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) override;
	virtual void RemoveHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent) override;
	virtual void PopHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent) override;

	virtual void AddSchedulableRepresentation(Schedulable *) override;
	virtual void AddLambdaRepresentation(Lambda *) override;

	virtual void AddDestinationRepresentations(Channel *) override;
	virtual void RemoveDestinationRepresentation(Channel *, Input *) override;
	virtual void RemoveDestinationRepresentation(Channel *, Output *) override;

	virtual void RemoveSchedulableRepresentations(StabEnt *) override;
	virtual void RemoveMethodRepresentations(StabEnt *) override;
	virtual void RemoveChannelRepresentations(StabEnt *) override;
	virtual void RemoveInstanceRepresentations(StabEnt *) override;

	virtual bool HasDisplayParameters(StabEnt *sym) override;
	virtual char *DisplayParameterId() override;
	virtual string GetDisplayParameters(StabEnt *) override;
	virtual void* ReadDisplayParameters(FILE *) override;
	virtual void SetDisplayParameters(StabEnt *, void*) override;

	virtual void DisplayDuration(Instance *) override;
	virtual void DisplayChannel(Instance *) override;
	virtual void DisplayStartTime(Instance *) override;
	virtual void DisplayWake(Instance *) override;
	virtual void DisplaySleep(Instance *) override;

	virtual void RemoveQuaNexion(QuaInsert *) override;
	virtual void AddQuaNexion(QuaInsert *, QuaInsert *) override;

	virtual void RemoveControlVariables(QuasiStack *) override;
	virtual void AddControlVariables(QuasiStack *) override;

/////////////////////////////////////////////////////////////////////////////////

	virtual long NChannel() override;
	virtual StabEnt *QuaSym() override;
	virtual StabEnt *ChannelSym(short i) override;
	virtual std::vector<StabEnt*> SchedulableSyms() override;
	virtual std::vector<StabEnt*> MethodSyms(StabEnt *) override;
	virtual std::vector<StabEnt*> InstanceSyms(StabEnt *) override;
	virtual std::vector<StabEnt*> ClipSyms(short) override;

	virtual Metric *QMetric() override;

	virtual std::vector<Envelope*> ListEnvelopesFor(StabEnt *stacker) override;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void GotoStartOfClip(StabEnt *) override;
	virtual void GotoEndOfClip(StabEnt *) override;
	virtual void SelectRegion(StabEnt *) override;

	virtual StabEnt * CreateSample(const std::string &nm, const std::vector<std::string> &p1) override;
	virtual StabEnt * CreateVoice(const std::string &nm, const std::vector<std::string> &pl) override;
	virtual StabEnt * CreatePool(const std::string &nm, const std::vector<std::string> & pl) override;
	virtual StabEnt * CreateChannel(
		char *nm = nullptr, short c = -1,
		uchar nin = 2, uchar nout = 2,
		bool add_dflt_au_in = false,
		bool add_dflt_au_out = false,
		bool add_dflt_str_in = false,
		bool add_dflt_str_out = false
	) override;
	virtual StabEnt * CreateMethod(const std::string &nm, StabEnt * const p = nullptr) override;
	virtual StabEnt	* CreateClip(const std::string &nm, const Time &st, const Time &dur) override;
	virtual StabEnt * CreateInstance(StabEnt * const, const short, const Time &, const Time &) override;
	virtual void MoveInstance(StabEnt *const, const short, const Time &, const Time &) override;
	virtual void DeleteInstance(StabEnt *) override;
	virtual void DeleteObject(StabEnt *) override;
	virtual void ShowObjectRepresentation(StabEnt *) override;
	virtual void HideObjectRepresentation(StabEnt *) override;
	virtual short CompileBlock(StabEnt *, char *srcnm, char *txt, long textlen) override;
	virtual short ParseBlock(StabEnt *, char *srcnm, char *txt, long textlen) override;
	virtual void CreateStreamTake(StabEnt *) override;
	virtual void LoadSampleTake(StabEnt *, std::string) override;

	virtual void Rename(StabEnt *sym, const string &nm) override;

	virtual void UpdateTempo(float tempo) override;
	virtual void UpdateGlobalTime(Time &t) override;

	///////////////////////////////////////////////////////////////////////////
	virtual void parseErrorViewClear() override;
	virtual void parseErrorViewAddLine(std::string, int severity) override;
	virtual void parseErrorViewShow() override;
	virtual void tragicError(const char *str, ...) override;
	virtual void reportError(const char *str, ...) override;

	///////////////////////////////////////////////////////////////////////////
	Qua *qua=nullptr;
};


class QuaEnvironmentDisplay: public ErrorHandler {
public:
	QuaEnvironmentDisplay();
	virtual ~QuaEnvironmentDisplay();

	bool			SpawnGlobalDisplay();
	bool			CleanupGlobalDisplay();

	void			RefreshVstPluginList();

	inline void	AddGlobalIndexer(QuaGlobalIndexPerspective *i) { globindexers.push_back(i); }
	inline void	RemoveGlobalIndexer(long i) { globindexers.erase(globindexers.begin()+i); }
	inline int NGlobalIndexer() { return globindexers.size(); }
	inline QuaGlobalIndexPerspective *GlobalIndexer(long i) {
		return i >= 0 && ((size_t)i) < globindexers.size() ? globindexers[i] : nullptr;
	}

	inline void	AddEnvironment(QuaEnvironmentPerspective *i) { environments.push_back(i); }
	inline void	RemoveEnvironment(long i) { environments.erase(environments.begin()+i); }
	inline int NEnvironment() { return environments.size(); }
	inline QuaEnvironmentPerspective *Environment(long i) {
		return i >= 0 && ((size_t)i) < environments.size() ? environments[i] : nullptr;
	}

	std::vector<QuaGlobalIndexPerspective*> globindexers;
	std::vector<QuaEnvironmentPerspective*>	environments;

	void CreatePortBridge(QuaPort *);
	void RemovePortBridge(QuaPort *);
	void CreateTemplateBridge(Template *);
	void RemoveTemplateBridge(Template *);
	void CreateMethodBridge(Lambda *);
	void RemoveMethodBridge(Lambda *);
	void CreateVstPluginBridge(VstPlugin *);
	void RemoveVstPluginBridge(VstPlugin *);

};


/**
 * QuaBridge: this bridge between the main qua object and the display, which is the View in an MVC sense
 * possibly a networked app will have lots of interfaces, 
 *  [a local display] or not? and a set of networked interfaces
 */
class QuaBridge: public QuaReflection
{
public:
	QuaBridge(Qua &q, QuaPerceptualSet &d);
};


////////////////////////////////////////////////////////////////////////////////////
// representations of "viewports"/"control panels"
///////////////////////////////////////////////////////////////////////////////////

// maybe there are several of these
class QuaPerspective
{
public:
	QuaPerspective();
	virtual ~QuaPerspective();

	void				SetLinkage(QuaPerceptualSet *);

	QuaPerceptualSet	*quaLink;
};

class QuaArrangerPerspective: public QuaPerspective
{
public:
	QuaArrangerPerspective();
	virtual ~QuaArrangerPerspective();

	std::vector<QuaInstanceRepresentation*> instanceRepresentations;
	inline long NIR() {
		return instanceRepresentations.size();
	}

	inline QuaInstanceRepresentation *IR(long i) {
		return (i >= 0 && ((size_t)i) < instanceRepresentations.size()? instanceRepresentations[i]: nullptr);
	}

	inline void	AddI(QuaInstanceRepresentation *i) { 
		auto ci = std::find(instanceRepresentations.begin(), instanceRepresentations.end(), i);
		if (ci == instanceRepresentations.end()) {
			instanceRepresentations.push_back(i);
		}
		
	}

	inline bool	RemI(QuaInstanceRepresentation *i) {
		auto ci = std::find(instanceRepresentations.begin(), instanceRepresentations.end(), i);
		if (ci != instanceRepresentations.end()) {
			instanceRepresentations.erase(ci);
			return true;
		}
		return false;
	}

	QuaInstanceRepresentation	*InstanceRepresentationFor(Instance *c);

	virtual void displayArrangementTitle(const char *)=0;
	virtual void updateClipIndexDisplay()=0;

	virtual void AddInstanceRepresentation(Instance *i)=0;
	virtual void MoveInstanceRepresentation(Instance *i)=0;
	virtual void RemoveInstanceRepresentation(Instance *i)=0;

	virtual void symbolNameChanged(StabEnt *s) = 0;

	virtual void UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *)=0;


	virtual void DisplayCurrentTime(Time &t, bool async)=0;	// ie update time cursor
	virtual void DisplayTempo(float t, bool async)=0;
	virtual void DisplayMetric(Metric *m, bool async)=0;	// ie update time cursor
	virtual void DisplayChannelMetric(Channel *c, Metric *m, bool async)=0;

	void GlobalTimeSetTo(Time &t); // global cursor changed by user event
	void ChannelMetricSetTo(Channel *c, Metric *m);

	void SelectRegion();
};

class QuaChannelRackPerspective: public QuaPerspective
{
public:
	QuaChannelRackPerspective();
	virtual ~QuaChannelRackPerspective();

	std::vector<QuaChannelRepresentation*> channelRepresentations;
	inline long		NCR() { 
		return channelRepresentations.size(); 
	}
	inline QuaChannelRepresentation	*CR(long i) { 
		return i >= 0 && ((size_t)i) < channelRepresentations.size()? channelRepresentations[i]:nullptr; 
	}
	inline void	AddCR(QuaChannelRepresentation *i) {
		auto ci = std::find(channelRepresentations.begin(), channelRepresentations.end(), i);
		if (ci == channelRepresentations.end()) {
			channelRepresentations.push_back(i);
		}

	}

	inline bool	RemCR(QuaChannelRepresentation *i) {
		auto ci = std::find(channelRepresentations.begin(), channelRepresentations.end(), i);
		if (ci != channelRepresentations.end()) {
			channelRepresentations.erase(ci);
			return true;
		}
		return false;
	}

	QuaChannelRepresentation	*ChannelRepresentationFor(Channel *c);
	QuaChannelRepresentation	*ChannelRepresentationFor(StabEnt *s);

	virtual void				displayArrangementTitle(const char *)=0;

	virtual void				AddChannelRepresentation(Channel *c)=0;
	virtual void				RemoveChannelRepresentation(Channel *c)=0;
};

// transport bar
class QuaTransportPerspective: public QuaPerspective
{
public:
	QuaTransportPerspective();
	virtual ~QuaTransportPerspective();

	virtual void				displayArrangementTitle(const char *)=0;

	void						Start();
	void						Stop();
	void						Pause();
	void						Record();

	void						UpdateGlobalTime(Time &t); // global time changed by user event
	void						UpdateTempo(float);

	virtual void				displayCurrentTime(Time &t, bool async)=0;	// ie update time cursor
	virtual void				displayTempo(float t, bool async)=0;
	virtual void				displayMetric(Metric *m, bool async)=0;	// ie update time cursor

	void						DisplayMetric();
	void						SetDisplayMetric();
};



// roster of devices, ports, browser file/symbols, connections
class QuaIndexPerspective: public QuaPerspective
{
public:
	QuaIndexPerspective();
	virtual ~QuaIndexPerspective();

	virtual void displayArrangementTitle(const char *)=0;

	virtual void addToSymbolIndex(StabEnt *s)=0;
	virtual void removeFromSymbolIndex(StabEnt *s)=0;
	virtual void symbolNameChanged(StabEnt *s)=0;

	virtual void updateClipIndexDisplay()=0;
};

// close up look at the object. 
// - container for display
// - object is a schedulable, channel, instance, port ... top level symbol
// - display and access components and symbols
// - container for multi-var/multi-axis display controllers
class QuaObjectRackPerspective: public QuaPerspective
{
public:
	QuaObjectRackPerspective();
	virtual ~QuaObjectRackPerspective();

	std::vector<QuaObjectRepresentation*> objectRepresentations;
	inline long		NOR() { return objectRepresentations.size(); }
	inline QuaObjectRepresentation	*OR(long i) {
		return i >= 0 && ((size_t)i) < objectRepresentations.size() ? objectRepresentations[i] : nullptr;
	}
	inline void	AddOR(QuaObjectRepresentation *i) {
		auto ci = std::find(objectRepresentations.begin(), objectRepresentations.end(), i);
		if (ci == objectRepresentations.end()) {
			objectRepresentations.push_back(i);
		}

	}

	inline bool	RemOR(QuaObjectRepresentation *i) {
		auto ci = std::find(objectRepresentations.begin(), objectRepresentations.end(), i);
		if (ci != objectRepresentations.end()) {
			objectRepresentations.erase(ci);
			return true;
		}
		return false;
	}
	inline void *RemOR(long i) {
		QuaObjectRepresentation *qi = OR(i);
		if (qi != nullptr) RemOR(qi);
		return qi;
	}

	QuaObjectRepresentation	*				RepresentationFor(StabEnt *c);
	QuaSchedulableObjectRepresentation	*	RepresentationForSchedulable(StabEnt *c);
	QuaInstanceObjectRepresentation	*		RepresentationForInstance(StabEnt *c);

	void						updateClipIndexDisplay(StabEnt *s);
	void						UpdateChannelIndexDisplay(StabEnt *ds);

	virtual void				displayArrangementTitle(const char *)=0;

	virtual bool				CanShow(StabEnt *)=0;
	virtual bool				AddObjectRepresentation(StabEnt *c)=0;
	virtual void				RemoveObjectRepresentation(StabEnt *c)=0;
};

/////////////////////////////////////////////////////////////////
// global displays
// roster of devices, ports, browser file/symbols, connections
class QuaGlobalIndexPerspective
{
public:
	QuaGlobalIndexPerspective();
	~QuaGlobalIndexPerspective();

	virtual void				displayArrangementTitle(const char *)=0;

	virtual void				addToSymbolIndex(StabEnt *s)=0;
	virtual void				removeFromSymbolIndex(StabEnt *s)=0;
	virtual void				symbolNameChanged(StabEnt *s)=0;
};

class QuaEnvironmentPerspective
{
public:
	QuaEnvironmentPerspective();
	virtual ~QuaEnvironmentPerspective();

	virtual void AddToEnvironment(StabEnt *s)=0;
	virtual void RemoveFromEnvironment(StabEnt *s)=0;
};

/////////////////////////////////////////////////////////////////////////////////////
// Access to symbols, and thence most other ops.
//  The representations for symbols are stacked packed and embedded but cover everything
// from controllers to editors ...
/////////////////////////////////////////////////////////////////////////////////////

// - objects dealt with through their symbols
//		- schedulable
//			- instance
//			- method
//			- input, output
//			- event?
//		- channel
//			- method
//			- input, output
//			- event?
// - edit code and events associated with the object
// - edit data associated the object
// - container for controllers/display variables etc
// - container for multi-var/multi-axis display controllers
// a particular visual representation of a symbol manipulated within 
// a particular persective 
class QuaFrameRepresentation;

class QuaControllerRepresentation
{
public:
	QuaControllerRepresentation();
	virtual ~QuaControllerRepresentation();

	void						Set(StabEnt *, QuaFrameRepresentation *);
	virtual void				UpdateDisplay(TypedValue &)=0;
	virtual void				SetSymValue()=0;
	virtual TypedValue			Value()=0;

	StabEnt						*symbol;
	QuaFrameRepresentation		*parent;
};

// child frames of a frame are collected in the parent
class QuaFrameRepresentation
{
public:
	QuaFrameRepresentation();
	virtual ~QuaFrameRepresentation();

	std::vector<QuaControllerRepresentation*> controllerRepresentations;
	inline long	NCR() { return controllerRepresentations.size(); }
	inline QuaControllerRepresentation	*CR(long i) {
		return i >= 0 && ((size_t)i) < controllerRepresentations.size() ? controllerRepresentations[i] : nullptr;
	}
	inline void	AddCR(QuaControllerRepresentation *i) {
		auto ci = std::find(controllerRepresentations.begin(), controllerRepresentations.end(), i);
		if (ci == controllerRepresentations.end()) {
			controllerRepresentations.push_back(i);
		}
	}

	inline bool	RemCR(QuaControllerRepresentation *i) {
		auto ci = std::find(controllerRepresentations.begin(), controllerRepresentations.end(), i);
		if (ci != controllerRepresentations.end()) {
			controllerRepresentations.erase(ci);
			return true;
		}
		return false;
	}

	inline void *RemCR(long i) {
		QuaControllerRepresentation *qi = CR(i);
		if (qi != nullptr) RemCR(qi);
		return qi;
	}
	QuaControllerRepresentation		*RepresentationFor(StabEnt *);

#ifdef Q_FRAME_MAP
	virtual void					RefreshFrame(frame_map_hdr *, bool)=0;
	virtual void					PopFrame(frame_map_hdr *, bool)=0;
#else
	virtual void					RefreshFrame(QuasiStack *, bool)=0;
	virtual void					PopFrame(QuasiStack *, bool)=0;
#endif
	virtual void					HideFrame()=0;
	virtual void					ArrangeChildren()=0;
	virtual void					AddChildFrame(QuasiStack *)=0;
	virtual void					AddController(StabEnt *)=0;
	virtual void					updateClipIndexDisplay()=0;
	virtual void					UpdateChannelIndexDisplay(StabEnt *ds)=0;

	void							Set(QuaInstanceObjectRepresentation	*, QuaFrameRepresentation *, QuasiStack *, bool);
	QuaInstanceObjectRepresentation	*root;
	QuaFrameRepresentation			*parent;
	QuasiStack						*frame;
	bool							hasChildren;
	bool							isHidden;

};

class QuaObjectRepresentation
{
public:
	QuaObjectRepresentation();
	virtual ~QuaObjectRepresentation();

	void SetSymbol(StabEnt *s);

	void Rename(char *nm); // calls to symbol table rename
	virtual void SetName()=0;
	virtual void ChildNameChanged(StabEnt *)=0;
	void Delete();	// calls to symbol table delete

	void Populate();
	virtual void ChildPopulate(StabEnt *sym)=0;
	virtual void AttributePopulate()=0;
	virtual QuaObjectRepresentation	* AddChildRepresentation(StabEnt *c)=0;

	std::vector<QuaObjectRepresentation*> childRepresentations;
	inline long	NCOR() { return childRepresentations.size(); }
	inline QuaObjectRepresentation	*COR(long i) { 
		return i >= 0 && ((size_t)i)<childRepresentations.size() ? childRepresentations[i] : nullptr;
	}
	inline void	AddCOR(QuaObjectRepresentation *i) {
		auto ci = std::find(childRepresentations.begin(), childRepresentations.end(), i);
		if (ci == childRepresentations.end()) {
			childRepresentations.push_back(i);
		}
	}

	inline bool	RemCOR(QuaObjectRepresentation *i) {
		auto ci = std::find(childRepresentations.begin(), childRepresentations.end(), i);
		if (ci != childRepresentations.end()) {
			childRepresentations.erase(ci);
			return true;
		}
		return false;
	}
	inline QuaObjectRepresentation *RemCOR(long i) {
		QuaObjectRepresentation *qi = COR(i);
		if (qi != nullptr) RemCOR(qi);
		return qi;
	}


	QuaObjectRepresentation	*	RepresentationFor(StabEnt *c);


	void						SetLinkage(QuaPerceptualSet *);
	QuaPerceptualSet			*quaLink;
	virtual void				UpdateControllerDisplay(StabEnt *, QuasiStack *, StabEnt *)=0;

	StabEnt						*symbol;
};


class QuaInstanceObjectRepresentation: public QuaObjectRepresentation
{
public:
	QuaInstanceObjectRepresentation();
	virtual ~QuaInstanceObjectRepresentation();

	std::vector<QuaFrameRepresentation*> frameRepresentations;
	inline long	NFR() { return frameRepresentations.size(); }
	inline QuaFrameRepresentation	*FR(long i) { 
		return i >= 0 && ((size_t)i) < frameRepresentations.size() ? frameRepresentations[i] : nullptr;
	}
	inline void	AddFR(QuaFrameRepresentation *i) {
		auto ci = std::find(frameRepresentations.begin(), frameRepresentations.end(), i);
		if (ci == frameRepresentations.end()) {
			frameRepresentations.push_back(i);
		}
	}

	inline bool	RemFR(QuaFrameRepresentation *i) {
		auto ci = std::find(frameRepresentations.begin(), frameRepresentations.end(), i);
		if (ci != frameRepresentations.end()) {
			frameRepresentations.erase(ci);
			return true;
		}
		return false;
	}
	inline QuaFrameRepresentation *RemFR(long i) {
		QuaFrameRepresentation *qi = FR(i);
		if (qi != nullptr) RemFR(qi);
		return qi;
	}
	QuaFrameRepresentation		*RepresentationFor(QuasiStack *);

#ifdef Q_FRAME_MAP
	virtual void PopFrameRepresentation(StabEnt *stackerSym, QuasiStack *parent, QuasiStack *stack, frame_map_hdr *, bool)=0;
	virtual void RefreshFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *)=0;
#else
	virtual void PopFrameRepresentation(StabEnt *stackerSym, QuasiStack *parent, QuasiStack *stack, bool)=0;
	virtual void RefreshFrameRepresentation(StabEnt *, QuasiStack *)=0;
#endif
	virtual void HideFrameRepresentation(StabEnt *, QuasiStack *)=0;
	virtual void DeleteFrameRepresentation(StabEnt *, QuasiStack *)=0;

	virtual void updateClipIndexDisplay();
	virtual void UpdateChannelIndexDisplay(StabEnt *ds);
};


class QuaSchedulableObjectRepresentation: public QuaObjectRepresentation
{
public:
	QuaSchedulableObjectRepresentation();
	virtual ~QuaSchedulableObjectRepresentation();

	virtual void UpdateTakeIndexDisplay()=0;
	virtual void updateClipIndexDisplay()=0;
	virtual void UpdateVariableIndexDisplay()=0;
};

class QuaSampleObjectRepresentation: public QuaSchedulableObjectRepresentation
{
public:
	QuaSampleObjectRepresentation();
	virtual ~QuaSampleObjectRepresentation();
};

class QuaVoiceObjectRepresentation: public QuaSchedulableObjectRepresentation
{
public:
	QuaVoiceObjectRepresentation();
	virtual ~QuaVoiceObjectRepresentation();
};

class QuaChannelObjectRepresentation: public QuaInstanceObjectRepresentation
{
public:
	QuaChannelObjectRepresentation();
	virtual ~QuaChannelObjectRepresentation();
	virtual void UpdateVariableIndexDisplay()=0;
};

class QuaMethodObjectRepresentation: public QuaObjectRepresentation
{
public:
	QuaMethodObjectRepresentation();
	virtual ~QuaMethodObjectRepresentation();
	virtual void UpdateVariableIndexDisplay()=0;
};

// an overview representation of a channel, popped into a channel rack
//	- a main executable block 
//	- child methods
//	- child inserts
//  - child base types (numeric, string)
//  - controllers for executable code (eg effects and filters)
class QuaChannelRepresentation
{
public:
	QuaChannelRepresentation();
	virtual ~QuaChannelRepresentation();

	std::vector<QuaInputRepresentation*> inputRepresentations;
	std::vector<QuaOutputRepresentation*> outputRepresentations;
	inline long NInR() { return inputRepresentations.size(); }
	inline long NOutR() { return outputRepresentations.size(); }
	inline QuaInputRepresentation	*InR(long i) {
		return i >= 0 && ((size_t)i) < inputRepresentations.size() ? inputRepresentations[i] : nullptr;
	}
	inline QuaOutputRepresentation	*OutR(long i) {
		return i >= 0 && ((size_t)i) < outputRepresentations.size() ? outputRepresentations[i] : nullptr;
	}
	inline void	AddInR(QuaInputRepresentation *i) {
		auto ci = std::find(inputRepresentations.begin(), inputRepresentations.end(), i);
		if (ci == inputRepresentations.end()) {
			inputRepresentations.push_back(i);
		}
	}

	inline bool	RemoveInR(QuaInputRepresentation *i) {
		auto ci = std::find(inputRepresentations.begin(), inputRepresentations.end(), i);
		if (ci != inputRepresentations.end()) {
			inputRepresentations.erase(ci);
			return true;
		}
		return false;
	}
	inline void	AddOutR(QuaOutputRepresentation *i) {
		auto ci = std::find(outputRepresentations.begin(), outputRepresentations.end(), i);
		if (ci == outputRepresentations.end()) {
			outputRepresentations.push_back(i);
		}
	}

	inline bool	RemoveOutR(QuaOutputRepresentation *i) {
		auto ci = std::find(outputRepresentations.begin(), outputRepresentations.end(), i);
		if (ci != outputRepresentations.end()) {
			outputRepresentations.erase(ci);
			return true;
		}
		return false;
	}

	void						AddDestinationRepresentations();

	QuaInputRepresentation		*InputRepresentationFor(Input *d);
	QuaOutputRepresentation		*OutputRepresentationFor(Output *d);

	virtual void				AddInputRepresentation(Input *c)=0;
	virtual void				RemoveInputRepresentation(Input *c)=0;
	virtual void				AddOutputRepresentation(Output *c)=0;
	virtual void				RemoveOutputRepresentation(Output *c)=0;

	virtual void				NameChanged()=0;

	void						SetChannel(Channel *c);

	Channel						*channel;
};

// representation of an input and output
//	- controllers for levels, depend on type of insert: pot, vu
//	- children (ie controllers) are direct consequences of insert type and properties
//	- controller/display of source (eg menu)
//	- enable/disable/status of thruput
//	- record enable/disable/status
class QuaDestinationRepresentation
{
public:
	virtual void				DisplayDestination()=0;
	virtual void				DisplayEnable()=0;
	virtual void				DisplayPan()=0;
	virtual void				DisplayGain()=0;

	virtual const char *		DeviceName(uchar nfmt, uchar cfmt)=0;
	virtual long				DeviceChannel()=0;
	virtual long				DeviceType()=0;

	virtual void				NameChanged()=0;

	virtual void				UpdateDestination(QuaPort *, port_chan_id ch, short flg=0)=0;
};

class QuaInputRepresentation: public QuaDestinationRepresentation
{
public:
	QuaInputRepresentation();
	~QuaInputRepresentation();

	Input						*input;
	void						SetInput(Input *d);

	virtual const char *		DeviceName(uchar nfmt, uchar cfmt);
	virtual long				DeviceChannel();
	virtual long				DeviceType();

	virtual void				UpdateDestination(QuaPort *, port_chan_id ch, short flg=0);
	void						UpdateEnable(bool);
	void						UpdatePan(float);
	void						UpdateGain(float);

	void						SetRecordStatus();
	void						RecordEnable();
	void						RecordDisable();
};

class QuaOutputRepresentation: public QuaDestinationRepresentation
{
public:
	QuaOutputRepresentation();
	~QuaOutputRepresentation();

	Output						*output;
	void						SetOutput(Output *d);

	virtual const char *		DeviceName(uchar nfmt, uchar cfmt);
	virtual long				DeviceChannel();
	virtual long				DeviceType();

	virtual void				UpdateDestination(QuaPort *, port_chan_id ch, short flg=0);
	void						UpdateEnable(bool);
	void						UpdatePan(float);
	void						UpdateGain(float);

	void						SetRecordStatus();
	void						RecordEnable();
	void						RecordDisable();
};


// an instance of a voice or sample:
//  - controllers representing data/executable elements
//  - structures, variables, code configured through schedulable
class QuaInstanceRepresentation
{
public:
	QuaInstanceRepresentation();
	~QuaInstanceRepresentation();

	void SetInstance(Instance *i);
	Instance *instance;
};

// representation of a value type symbol, eg string, numeric, list
//	- controller for this value
//	- display of this value
// could be a pot, &/or text field, or an envelope
class QuaSymbolController: public QuaObjectRepresentation
{
public:
	QuaSymbolController();
	~QuaSymbolController();

	void SetDisplayValue();		// sets the display value
	void DisplayValue();			// return current value of display
	void DisplayValueChanged(); // called to change the actual value
};

// representation of a value type symbol, eg numeric which has an enveloped value
//	- the envelope for display and edit
//		- add points
//		- delete points
//		- move points
//		- modify point property/type
//		- modify/display envelope properties/type
//	- display cursor for current part of envelope
class QuaSymbolEnvelopper: public QuaObjectRepresentation
{
public:
	QuaSymbolEnvelopper();
	~QuaSymbolEnvelopper();

	void AddPoint();		// sets the display value
	void DeletePoint();			// return current value of display
	void MovePoint(); // called to change the actual value
	void ModifyPoint();
	void ModifyProperty();
};



#endif
