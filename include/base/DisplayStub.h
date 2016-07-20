#pragma once

#include "QuaDisplay.h"
/**
* QuaDisplayStub
* a default, do nothing display. absolute minimal instantiable base
*/
class QuaDisplayStub : public QuaPerceptualSet
{
public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void AddSchedulableRepresentation(Schedulable *) override { };
	virtual void AddLambdaRepresentation(Lambda *) override { };

	virtual void AddDestinationRepresentations(Channel *) override { };
	virtual void RemoveDestinationRepresentation(Channel *, Input *) override { };
	virtual void RemoveDestinationRepresentation(Channel *, Output *) override { };

	virtual void AddChannelRepresentations(QuaChannelRackPerspective *i) override { };
	virtual void AddChannelRepresentations() override { };

	virtual StabEnt *		CreateSample(std::string nm, std::vector<std::string> p1, short c = -1, Time *att = nullptr, Time *ctp = nullptr) override { return nullptr; }
	virtual StabEnt *		CreateVoice(std::string nm, std::vector<std::string> pl, short c = -1, Time *att = nullptr, Time *ctp = nullptr) override { return nullptr; }
	virtual StabEnt *		CreateChannel(
		char *nm = nullptr, short c = -1,
		uchar nin = 2, uchar nout = 2,
		bool add_dflt_au_in = false,
		bool add_dflt_au_out = false,
		bool add_dflt_str_in = false,
		bool add_dflt_str_out = false
	) override {
		return nullptr;
	}
	virtual StabEnt	* CreateMethod(std::string nm, StabEnt *p = nullptr) override { return nullptr; }
	virtual StabEnt	* CreateClip(std::string nm, Time *st = nullptr, Time *dur = nullptr)override { return nullptr; }
	virtual StabEnt * CreateInstance(StabEnt *, const short, const Time &, const Time &) override {};
	virtual void MoveInstance(StabEnt *const, const short, const Time &, const Time &) override {};
	virtual void DeleteInstance(StabEnt *) override { };
	virtual void DeleteObject(StabEnt *)override { };
	virtual void ShowObjectRepresentation(StabEnt *) override { };
	virtual void HideObjectRepresentation(StabEnt *)override { };
	virtual short CompileBlock(StabEnt *, char *srcnm, char *txt, long textlen) override { return 0; }
	virtual short ParseBlock(StabEnt *, char *srcnm, char *txt, long textlen) override { return 0; }

	virtual void			UpdateControllerDisplay(StabEnt *stackerSym, QuasiStack *stack, StabEnt *sym,
		qua_perpective_type srctype = QUAPSCV_NOT, QuaPerspective *src = nullptr) override { };
	virtual void			ControllerChanged(StabEnt *sym, QuasiStack *, TypedValue &t,
		qua_perpective_type srctype = QUAPSCV_NOT, QuaPerspective *src = nullptr) override { };

	virtual void DisplayStatus(Instance *, qua_status) override {};
	virtual void DisplayStatus(QuasiStack *, qua_status) override {};

	virtual std::vector<Envelope*> ListEnvelopesFor(StabEnt *stacker) override { return std::vector<Envelope *>(); }

	virtual void			GotoStartOfClip(StabEnt *) override { };
	virtual void			GotoEndOfClip(StabEnt *) override { };
	virtual void			SelectRegion(StabEnt *) override { };

	virtual void			CreateStreamTake(StabEnt *) override { };
	virtual void			LoadSampleTake(StabEnt *, std::string) override { };
	virtual void			UpdateTakeIndexDisplay(StabEnt *sym) override { };
	virtual void			updateClipIndexDisplay(StabEnt *sym) override { };
	virtual void			UpdateVariableIndexDisplay(StabEnt *sym) override { };

	// tempo or time is changed from the interface
	virtual void			UpdateTempo(float tempo) override { };
	virtual void			UpdateGlobalTime(Time &t) override { };

	// display the current master tempo/master time ... 
	virtual void DisplayTempo(float tempo, bool async) override {};
	virtual void DisplayGlobalTime(Time &t, bool async) override {};

	virtual void			RequestPopFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *rootSym,
		QuasiStack *parent, QuasiStack *stack, bool show, bool add_children) override { };
#ifdef Q_FRAME_MAP
	virtual void			PopFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *, long) override { };
	virtual void			HideFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) override { };
	virtual void			RemoveFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack)override {}
	virtual void			RequestRemoveFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) override {}

	void					PopFrameRepresentations(StabEnt *, QuasiStack *, frame_map_hdr *, long) override {}
#else
	virtual void			PopFrameRepresentation(StabEnt *rootSym, QuasiStack *stack) override { };
	virtual void			HideFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack) override { };
	virtual void			RemoveFrameRepresentation(StabEnt *stackerSym, QuasiStack *stack)override { };
	virtual void			RequestRemoveFrameRepresentation(QuaInstanceObjectRepresentation *ir, StabEnt *stackerSym, QuasiStack *stack)override { };
	virtual void			PopHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent)override { };

//	void					PopFrameRepresentations(StabEnt *, QuasiStack *) override {}
#endif
	virtual void			RemoveHigherFrameRepresentations(StabEnt *frame, QuasiStack *parent)override { };
	virtual void			Rename(StabEnt *, const string &)override { };

	virtual void			RemoveSchedulableRepresentations(StabEnt *) override { };
	virtual void			RemoveMethodRepresentations(StabEnt *) override { };
	virtual void			RemoveChannelRepresentations(StabEnt *) override { };
	virtual void			RemoveInstanceRepresentations(StabEnt *) override { };

	virtual void			displayArrangementTitle(const char *) override { };

	virtual void DisplayDuration(Instance *) override { }
	virtual void DisplayChannel(Instance *) override { }
	virtual void DisplayStartTime(Instance *) override { }
	virtual void DisplayWake(Instance *) override { }
	virtual void DisplaySleep(Instance *) override { }

	virtual bool HasDisplayParameters(StabEnt *) override { return false; }
	virtual char *DisplayParameterId() override { return ""; }
	virtual status_t WriteDisplayParameters(FILE *, StabEnt *)override { return B_OK; }
	virtual void* ReadDisplayParameters(FILE *) override { return nullptr; }
	virtual void SetDisplayParameters(StabEnt *, void*) override { };

	virtual void RemoveQuaNexion(QuaInsert *) override { };
	virtual void AddQuaNexion(QuaInsert *, QuaInsert *) override { };

	virtual void RemoveControlVariables(QuasiStack *) override { };
	virtual void AddControlVariables(QuasiStack *) override { };

	//////////////////////////////////////////////////////////////////////////////////////////////////
	virtual long NChannel() override { return 0; }
	virtual StabEnt *QuaSym()override { return nullptr; }
	virtual StabEnt *ChannelSym(short i) { return nullptr; }
	virtual std::vector<StabEnt*> SchedulableSyms() override { return vector<StabEnt *>(); };
	virtual std::vector<StabEnt*> MethodSyms(StabEnt *) override { return vector<StabEnt *>(); };
	virtual std::vector<StabEnt*> InstanceSyms(StabEnt *) override { return vector<StabEnt *>(); };
	virtual std::vector<StabEnt*> ClipSyms(short)override { return vector<StabEnt *>(); };

	virtual Metric *QMetric() override { return nullptr; }

};