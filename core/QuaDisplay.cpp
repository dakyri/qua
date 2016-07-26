#include "qua_version.h"

/**
 * collections of classes that abstract the idea of a display for the sequencer so that we migh encompass
 * anything from a command line to a few different interface styles on the same platform and of course
 * multiple platforms
 * original Beos interface was entangled with the operational code ... I've captured the functionality of that original
 * interface so it;s conceivable that it could be backported ... though afics the platform is consigned to the garbage dump of history
 */
#include <vector>
#include <string>
#include <iostream>
#include <stdarg.h>
using namespace std;

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#include "StdDefs.h"

#include "Sym.h"
#include "Qua.h"
#include "Executable.h"
#include "Lambda.h"
#include "Channel.h"
#include "Schedulable.h"
#include "Instance.h"
#include "QuaDisplay.h"
#include "QuaPort.h"
#include "Sample.h"
#include "Voice.h"
#include "Parse.h"
#include "Block.h"

#ifdef Q_FRAME_MAP
void
QuaPerceptualSet::PopFrameRepresentations(StabEnt *stackerSym, QuasiStack *qs, frame_map_hdr *map, long maplen)
{
	;
}
#else
/* TODO XXXX FIXME this and its companion may well be superfluous slate for deprectation */
/*

void
QuaPerceptualSet::PopFrameRepresentations(StabEnt *stackerSym, QuasiStack *qs)
{
	;
}
*/
#endif

/**
 * QuaDisplay
 * the base class of a full cubase/ableton style local interface. not so much the display per se, though
 * some components of it will map onto interface components directly .. more like a protocol bridge between the sequencer and the components
 *
 * the current MFC interface uses this largely as-is. but makes a lot of use of the public 'qua' reference ... it would be great to 
 */
void
QuaDisplay::DisplayGlobalTime(Time &t, bool async)
{
	for (short i=0; i<NArranger(); i++) {
		if (Arranger(i) != nullptr) {
			Arranger(i)->DisplayCurrentTime(t, async);
		}
	}
	for (short j=0; j<NTransporter(); j++) {
		if (Transporter(j) != nullptr) {
			Transporter(j)->displayCurrentTime(t, async);
		}
	}
}

void
QuaDisplay::DisplayTempo(float t, bool async)
{
	for (short i=0; i<NArranger(); i++) {
		if (Arranger(i) != nullptr) {
			Arranger(i)->DisplayTempo(t, async);
		}
	}
	for (short j=0; j<NTransporter(); j++) {
		if (Transporter(j) != nullptr) {
			Transporter(j)->displayTempo(t, async);
		}
	}
}

void
QuaDisplay::RemoveHigherFrameRepresentations(StabEnt *stkSym, QuasiStack *parent)
{
	for (short i=0; i<NObjectRack(); i++) {
		QuaObjectRackPerspective	*ov = ObjectRack(i);
		QuaInstanceObjectRepresentation * ir=ov->RepresentationForInstance(stkSym);
		if (ir && ir->symbol) {
			Instance	*in = ir->symbol->InstanceValue();
			if (in) {
				std::vector<QuasiStack*> hfs;
				short	j;
				for (j=0; j<ir->NFR(); j++) {
					QuaFrameRepresentation	*fr = ir->FR(j);
					if (fr) {
						if (fr->frame && fr->frame->lowerFrame == parent) {
							hfs.push_back(fr->frame);
						}
					}
				}
				for (j=0; ((size_t)j)<hfs.size(); j++) {
					ir->DeleteFrameRepresentation(stkSym, hfs[j]);
				}
			}
		}
	}
}

void
QuaDisplay::PopHigherFrameRepresentations(StabEnt *stkSym, QuasiStack *parent)
{
	if (parent) {
		for (short i=0; i<NObjectRack(); i++) {
			QuaObjectRackPerspective	*ov = ObjectRack(i);
			QuaInstanceObjectRepresentation * ir=ov->RepresentationForInstance(stkSym);
			if (ir && ir->symbol) {
				QuaFrameRepresentation *pfr = ir->RepresentationFor(parent);
				if (pfr) {
					for (QuasiStack *s: parent->higherFrame) {
						pfr->AddChildFrame(s);
					}
				} else {
					reportError("frame for stack %x of %s not found", parent, stkSym->name.c_str());
				}
			}
		}
	}
}

void
QuaDisplay::displayArrangementTitle(const char *nm)
{
	int i;
	for (i=0; i<NArranger(); i++) {
		Arranger(i)->displayArrangementTitle(nm);
	}
	for (i=0; i<NObjectRack(); i++) {
		ObjectRack(i)->displayArrangementTitle(nm);
	}
	for (i=0; i<NChannelRack(); i++) {
		ChannelRack(i)->displayArrangementTitle(nm);
	}
	for (i=0; i<NTransporter(); i++) {
		Transporter(i)->displayArrangementTitle(nm);
	}
	for (i=0; i<NArranger(); i++) {
		Arranger(i)->displayArrangementTitle(nm);
	}
	for (i=0; i<NIndexer(); i++) {
		Indexer(i)->displayArrangementTitle(nm);
	}
}

QuaDisplay::QuaDisplay()
{
}

QuaDisplay::~QuaDisplay()
{
	;
}

bool
QuaDisplay::HasDisplayParameters(StabEnt *)
{
	return false;
}

char *
QuaDisplay::DisplayParameterId()
{
	return "MFC";
}

status_t
QuaDisplay::WriteDisplayParameters(FILE *fp, StabEnt *)
{
	return B_OK;
}
void*
QuaDisplay::ReadDisplayParameters(FILE *) {
	return nullptr;
}

void
QuaDisplay::SetDisplayParameters(StabEnt *, void*) {
}

/*
void
QuaDisplay::RemoveSchedulableBridge(Schedulable *Q)
{
	if (Q->sym->type == TypedValue::S_LAMBDA) {
		RemoveMethod(Q->sym->LambdaValue(), true);
		return;
	}
	Schedulable		*S;
	Lambda			*M;
	if (S = Q->schedulable) {
		sequencerWindow->arrange->RemoveArrangerObject(Q, true);
		if (S && S->controlPanel) {
			mixerView->DeleteSchedulablePanel(S->controlPanel);
		}
		RemoveSchedulable(S, true);
	} else if ((M=Q->sym->LambdaValue()) != nullptr) {
		RemoveMethod(M, true);
	}
	Q->RemoveSelf();
	delete Q;
}
*/
	/*
QuaSchedulableBridge *
QuaDisplay::CreateSchedulableBridge(Schedulable *S)
{
	QuaObject				*QO = nullptr;
	
	BRect		Qrect;
	BRect		r;
	if (where) {
		Qrect.Set(where->x, where->y, 50+where->x, where->y+20);
	}

	SchedulablePanel		*SC = nullptr;

	if (mixerView->Window())
		mixerView->Window()->Lock();
	r = mixerView->back->Frame();
	if (mixerView->Window())
		mixerView->Window()->Unlock();

	BRect		CRect(0,0,r.right-r.left,2*MIXER_MARGIN);

	fprintf(stderr, "add qua_obj @ %g %g in %x\n", where->x, where->y, sequencerWindow->aquarium);

	BBitmap		bicon(BRect(0,0,31,31), B_CMAP8);
	bicon.SetBits(quapp->quaBigIcon->Bits(),
				 quapp->quaBigIcon->BitsLength(),
				 0,
				 B_CMAP8);
	BBitmap		sicon(BRect(0,0,15,15), B_CMAP8);
	sicon.SetBits(quapp->quaSmallIcon->Bits(),
				 quapp->quaSmallIcon->BitsLength(),
				 0,
				 B_CMAP8);
	
	switch (S->type) {

	case TypedValue::S_POOL: {
    	QO = new QuaObject(S, Qrect, &sicon,  &bicon, sequencerWindow->aquarium, this, red);
    	SC = new SchedulablePanel(CRect, S->PoolValue(), true);
		break;
	}	

	case TypedValue::S_SAMPLE: {
		if (S->SampleValue()->selectedFilePathName) {
			BFile		theFile(
							S->SampleValue()->selectedFilePathName,
							B_READ_ONLY);
			BNodeInfo	nodeInfo(&theFile);
			if ((err=nodeInfo.GetTrackerIcon(&bicon, B_LARGE_ICON)) != B_NO_ERROR) {
				reportError("Qua: can't find sample icon, %s\n", ErrorStr(err));
			}
			if ((err=nodeInfo.GetTrackerIcon(&sicon, B_MINI_ICON)) != B_NO_ERROR) {
				reportError("Qua: can't find sample icon, %s\n", ErrorStr(err));
			}
		}
    	QO = new QuaObject(S, Qrect, &sicon,  &bicon, sequencerWindow->aquarium, this, yellow);
    	SC = new SchedulablePanel(CRect, S->SampleValue(), true);
		break;
	}

	case TypedValue::S_VOICE: {
		QO = new QuaObject(S, Qrect,  &sicon, &bicon, sequencerWindow->aquarium, this, lavender);
    	SC = new SchedulablePanel(CRect, S->VoiceValue(), true);
		break;
	}

	case TypedValue::S_LAMBDA: {
		if (S->context == sym) {
   		 	QO = new QuaObject(S, Qrect, &sicon,  &bicon, sequencerWindow->aquarium, this, ltGray);
		}
		break;
	}
 		

	case TypedValue::S_PORT: {
    	SC = new SchedulablePanel(CRect, S->PortValue(), S->PortValue()->IsMultiSchedulable());
		break;
	}
	
	default: {
		break;
	}
	
	}

	if (QO) {
		QO->MoveTo(where.x, where.y);
//		where += BPoint(0, QO->Bounds().bottom + 2);
//		if (where.y >= sequencerWindow->aquarium->Frame().bottom - 5) {
//			where.y = 3;
//			where.x += 60;
//		}
	}
	if (SC) {
		mixerView->AddSchedulablePanel(SC);
	}
	return QO;
return &S->interfaceBridge;
}*/

/*
QuaSymbolBridge *
QuaDisplay::FindExecutableRepresentation(StabEnt *S)
{
	Schedulable *p = S->SchedulableValue();
	if (p) {
		return &p->interfaceBridge;
	}
	Lambda *q = S->LambdaValue();
	if (q) {
		return &q->interfaceBridge;
	}
	Channel *r = S->ChannelValue();
	if (r) {
		return &r->interfaceBridge;
	}
	return nullptr;
}
*/


// should also check for presence in object lists
void
QuaDisplay::AddSchedulableRepresentation(Schedulable *s)
{
	for (short i = 0; i<NIndexer(); i++) {
		QuaIndexPerspective	*crv = Indexer(i);
		crv->addToSymbolIndex(s->sym);
	}
}

void
QuaDisplay::AddLambdaRepresentation(Lambda *m)
{
	for (short i = 0; i<NIndexer(); i++) {
		QuaIndexPerspective	*crv = Indexer(i);
		crv->addToSymbolIndex(m->sym);
	}
}

void
QuaDisplay::AddDestinationRepresentations(Channel *c)
{
	for (short i = 0; i<NChannelRack(); i++) {
		QuaChannelRackPerspective	*crv = ChannelRack(i);
		if (crv != nullptr) {
			QuaChannelRepresentation	*cv = crv->ChannelRepresentationFor(c);
			if (cv != nullptr) {
				cv->AddDestinationRepresentations();
			}
		}
	}
}

void
QuaDisplay::RemoveDestinationRepresentation(Channel *c, Input *s)
{
	for (short i = 0; i<NChannelRack(); i++) {
		QuaChannelRackPerspective	*crv = ChannelRack(i);
		if (crv != nullptr) {
			QuaChannelRepresentation	*cv = crv->ChannelRepresentationFor(c);
			if (cv != nullptr) {
				cv->RemoveInputRepresentation(s);
			}
		}
	}
}

void
QuaDisplay::RemoveDestinationRepresentation(Channel *c, Output *s)
{
	for (short i = 0; i<NChannelRack(); i++) {
		QuaChannelRackPerspective	*crv = ChannelRack(i);
		if (crv != nullptr) {
			QuaChannelRepresentation	*cv = crv->ChannelRepresentationFor(c);
			if (cv != nullptr) {
				cv->RemoveOutputRepresentation(s);
			}
		}
	}
}


bool
QuaDisplay::setup(Qua *q)
{
	qua = q;
	return true;
}

bool
QuaDisplay::spawn()
{
	return true;
}


bool
QuaDisplay::cleanup()
{
	return true;
}

void
QuaDisplay::AddChannelRepresentations(QuaChannelRackPerspective *cr)
{
	if (cr == nullptr) {
		return;
	}
	for (short i=0; i<qua->nChannel; i++) {
		if (cr->ChannelRepresentationFor(qua->channel[i]) == nullptr) {
			cr->AddChannelRepresentation(qua->channel[i]);
		}
	}
}

void
QuaDisplay::AddChannelRepresentations()
{
	for (short i=0; i<NChannelRack(); i++) {
		AddChannelRepresentations(ChannelRack(i));
	}
}

long
QuaDisplay::NChannel()
{
	return qua? qua->nChannel: 0;
}

StabEnt *
QuaDisplay::QuaSym()
{
	return qua?qua->sym:nullptr;
}

void
QuaDisplay::GotoStartOfClip(StabEnt *clipSym)
{
	if (qua) {
		qua->GotoStartOfClip(clipSym);
	}
}

void
QuaDisplay::GotoEndOfClip(StabEnt *clipSym)
{
	if (qua) {
		qua->GotoEndOfClip(clipSym);
	}
}

void
QuaDisplay::SelectRegion(StabEnt *clipSym)
{
	if (qua) {
		qua->SelectRegion(clipSym);
	}
}

std::vector<StabEnt*>
QuaDisplay::SchedulableSyms()
{
	Schedulable	*schds = qua->schedulees;
	std::vector<StabEnt*> ss;
	while (schds != nullptr) {
		ss.push_back(schds->sym);
		schds = schds->next;
	}
	return ss;
}


std::vector<StabEnt*>
QuaDisplay::ClipSyms(short typ)
{
	std::vector<StabEnt*> ss;
	if (!qua || !qua->sym) {
		return ss;
	}
	StabEnt	*schds = qua->sym->children;
	while (schds != nullptr) {
		if (schds->type == TypedValue::S_CLIP) {
			Clip	*c = schds->ClipValue(nullptr);
			if (typ == 0 || (typ > 0 && c->duration.ticks > 0) || (typ < 0 && c->duration.ticks <= 0)) {
				ss.push_back(schds);
			}
		}
		schds = schds->sibling;
	}
	return ss;
}

std::vector<StabEnt*>
QuaDisplay::MethodSyms(StabEnt *parent)
{
	std::vector<StabEnt*> ss;
	if (!parent) {
		return ss;
	}
	StabEnt	*chlds = parent->children;
	while (chlds != nullptr) {
		if (chlds->type == TypedValue::S_LAMBDA) {
			ss.push_back(chlds);
		}
		chlds = chlds->sibling;
	}
	return ss;
}

std::vector<StabEnt*>
QuaDisplay::InstanceSyms(StabEnt *parent)
{
	std::vector<StabEnt*> ss;
	if (!parent) {
		return ss;
	}
	StabEnt	*chlds = parent->children;
	while (chlds != nullptr) {
		if (chlds->type == TypedValue::S_INSTANCE) {
			ss.push_back(chlds);
		}
		chlds = chlds->sibling;
	}
	return ss;
}

StabEnt *
QuaDisplay::ChannelSym(short i)
{
	return qua?qua->channel[i]->sym:nullptr;
}

Metric *
QuaDisplay::QMetric()
{
	return qua?qua->metric:nullptr;
}

// called by interface routines to remove an object
void
QuaDisplay::DeleteObject(StabEnt *sym)
{
	switch (sym->type) {
		case TypedValue::S_CHANNEL: {
			Channel	*c = sym->ChannelValue();
			if (c) {
				qua->RemoveChannel(c->chanId, true);
			}
			break;
		}
		case TypedValue::S_VOICE: {
			Voice	*c = sym->VoiceValue();
			if (c) {
				qua->RemoveSchedulable(c, true, true);
			}
			break;
		}
		case TypedValue::S_SAMPLE: {
			Sample	*c = sym->SampleValue();
			if (c) {
				qua->RemoveSchedulable(c, true, true);
			}
			break;
		}
		case TypedValue::S_LAMBDA: {
			Lambda	*c = sym->LambdaValue();
			if (c) {
				qua->RemoveMethod(c, true, true);
			}
			break;
		}
		case TypedValue::S_INSTANCE: {
			DeleteInstance(sym);
			break;
		}
	}
}

void
QuaDisplay::RemoveSchedulableRepresentations(StabEnt *sym)
{
	for (short i=0; i<NIndexer(); i++) {
		if (Indexer(i)) {
			Indexer(i)->removeFromSymbolIndex(sym);
		}
	}
	for (short i=0; i<NObjectRack(); i++) {
		if (ObjectRack(i)) {
			ObjectRack(i)->RemoveObjectRepresentation(sym);
		}
	}
}

void
QuaDisplay::RemoveChannelRepresentations(StabEnt *sym)
{
	Channel	*c = sym->ChannelValue();
	if (c) {
		for (short i=0; i<NChannelRack(); i++) {
			if (ChannelRack(i)) {
				ChannelRack(i)->RemoveChannelRepresentation(c);
			}
		}
	}
	for (short i=0; i<NObjectRack(); i++) {
		if (ObjectRack(i)) {
			ObjectRack(i)->RemoveObjectRepresentation(sym);
		}
	}
	for (short i=0; i<NObjectRack(); i++) {
		if (ObjectRack(i)) {
			ObjectRack(i)->UpdateChannelIndexDisplay(sym);
		}
	}
	for (short i=0; i<NIndexer(); i++) {
		if (Indexer(i)) {
			Indexer(i)->removeFromSymbolIndex(sym);
		}
	}
}

void
QuaDisplay::RemoveMethodRepresentations(StabEnt *sym)
{
	for (short i=0; i<NIndexer(); i++) {
		if (Indexer(i)) {
			Indexer(i)->removeFromSymbolIndex(sym);
		}
	}
	for (short i=0; i<NObjectRack(); i++) {
		if (ObjectRack(i)) {
			ObjectRack(i)->RemoveObjectRepresentation(sym);
		}
	}
}

void
QuaDisplay::RemoveInstanceRepresentations(StabEnt *sym)
{
	Instance *inst = sym->InstanceValue();
	short i;
	for (i=0; i<NIndexer(); i++) {
		Indexer(i)->removeFromSymbolIndex(sym);
	}
	if (inst) {
		for (i=0; i<NArranger(); i++) {
			Arranger(i)->RemoveInstanceRepresentation(inst);
		}
	}
	for (i=0; i<NObjectRack(); i++) {
		ObjectRack(i)->RemoveObjectRepresentation(sym);
	}
}

void
QuaDisplay::ShowObjectRepresentation(StabEnt *sym)
{
	short i;
	for (i=0; i<NObjectRack(); i++) {
		if (ObjectRack(i)->CanShow(sym) && ObjectRack(i)->RepresentationFor(sym)==nullptr) {
			ObjectRack(i)->AddObjectRepresentation(sym);
			break;
		}
	}
}



void
QuaDisplay::HideObjectRepresentation(StabEnt *sym)
{
	;
}
/////////////////////////////////////////////////////////////////////////////////////
// called by an interface to make dynamic changes to a potentially running sequencer
/////////////////////////////////////////////////////////////////////////////////////
/**
 * Change an instances time and duration ... may also update several screen objects
 * TODO FIXME XXXX perhaps split this into 2 or 3 separate functions for time, duration and/or channel
 */
void
QuaDisplay::MoveInstance(StabEnt *instSym, short chan, const Time &at_t, const Time &dur_t)
{
	short	i;
	Instance	*inst = instSym->InstanceValue();
	if (inst) {
		inst->Move(chan, at_t, dur_t, false);
		for (i=0; i<NArranger(); i++) {
			Arranger(i)->MoveInstanceRepresentation(inst);
		}
		UpdateControllerDisplay(instSym, inst->mainStack, inst->schedulable.chanSym);
		UpdateControllerDisplay(instSym, inst->mainStack, inst->schedulable.starttSym);
		UpdateControllerDisplay(instSym, inst->mainStack, inst->schedulable.durSym);
	}
}

void
QuaDisplay::DeleteInstance(StabEnt *instSym)
{
	Instance	*inst = instSym->InstanceValue();
	if (inst) {
		RemoveInstanceRepresentations(instSym);
		glob.DeleteSymbol(instSym, true); 
		// which deletes the symbol, which then deletes it from
		// elsewhere, the schedulables list, and the main schedule
	}
}

std::vector<Envelope *>
QuaDisplay::ListEnvelopesFor(StabEnt *stacker)
{
	return std::vector<Envelope*>();
}

// called by an interface to make dynamic changes to a potentially running sequencer
StabEnt *
QuaDisplay::CreateInstance(StabEnt * const schSym, const short chan, const Time &t, const Time &d)
{
	short	i;
	Schedulable	*sch = schSym->SchedulableValue();
	if (sch) {
		Instance	*instance = sch->addInstance(sch->sym->name, chan, t, d, false);
		cerr << "QuaDisplay::CreateInstance() duraction " << instance->duration.ticks << endl;
		if (instance) {
			for (i=0; i<NIndexer(); i++) {
				Indexer(i)->addToSymbolIndex(instance->sym);
			}
			for (i=0; i<NArranger(); i++) {
				Arranger(i)->AddInstanceRepresentation(instance);
			}
			return instance->sym;
		}
	}
	return nullptr;
}

// called by an interface to make dynamic changes to a potentially running sequencer
StabEnt *
QuaDisplay::CreateSample(const std::string & nm, const std::vector<std::string> & pathList, short chan, Time *tp, Time *dp)
{
	// create sample
	std::string	sample_nm;

	if (nm != "") {
		sample_nm = nm;
	} else if (pathList.size() > 0) {
		sample_nm = Qua::nameFromLeaf(pathList[0]);
	} else {
		sample_nm = "";
	}

	std::string nmbuf = glob.makeUniqueName(qua->sym, sample_nm, 0);

	Sample	*sample = qua->CreateSample(nmbuf, false);
	if (sample) {
		cerr << "created sample " << sample->sym->name<< endl;
		for (std::string p: pathList) {
			std::string b = Qua::nameFromLeaf(p);
			sample->addSampleTake(b, p, false);
		}
		// update index displays
		Instance	*instance = nullptr;
		if (tp != nullptr && dp != nullptr) {
			Time t = *tp, d = *dp;
			instance = sample->addInstance(nm, chan, t, d, false);
		}
		for (short i = 0; i < NIndexer(); i++) {
			Indexer(i)->addToSymbolIndex(sample->sym);
			if (instance) {
				Indexer(i)->addToSymbolIndex(instance->sym);
			}
		}
		// update arranger displays;
		/*
		Time t = sample->SampleClip(0)->duration;
		int	bar;
		int	beat;
		int	q;
		t.GetBBQValue(bar, beat, q);
		fprintf(stderr, "selection duration %d:%d.%d\n", bar, beat, q);
		*/
		if (instance) {
			for (short i=0; i<NArranger(); i++) {
				Arranger(i)->AddInstanceRepresentation(instance);
			}
		}
		ShowObjectRepresentation(sample->sym);
		return sample->sym;
	}
	return nullptr;
}

StabEnt *
QuaDisplay::CreateVoice(const std::string &nm, const std::vector<std::string> & pathList, short chan, Time *tp, Time *dp)
{
	// create voice
	short	i;
	std::string voice_nm;

	if (nm.size() > 0) {
		voice_nm = nm;
	} else if (pathList.size() > 0) {
		voice_nm = Qua::nameFromLeaf(pathList[0]);
	}

	voice_nm = glob.makeUniqueName(qua->sym, voice_nm, 0);

	Voice	*voice = qua->CreateVoice(voice_nm, false);
	if (voice) {
//		fprintf(stderr, "created voice %s\n", voice->sym->name);
		for (i=0; ((size_t)i)<pathList.size(); i++) {
			std::string nm = Qua::nameFromLeaf(pathList[i]);
			voice->addStreamTake(nm, pathList[i], true);
		}
		// update index displays
		Instance	*instance=nullptr;
		if (chan >= 0 && tp != nullptr && dp != nullptr) {
			Time t = *tp, d = *dp;
			instance = voice->addInstance(voice->sym->name, chan, t, d, false);
		}
		for (short i=0; i<NIndexer(); i++) {
			Indexer(i)->addToSymbolIndex(voice->sym);
			if (instance) {
				Indexer(i)->addToSymbolIndex(instance->sym);
			}
		}
		// update arranger displays;
		/*
		Time t = voice->StreamClip(0)->duration;
		int	bar;
		int	beat;
		int	q;
		t.GetBBQValue(bar, beat, q);
		fprintf(stderr, "selection duration %d:%d.%d\n", bar, beat, q);*/
		if (instance) {
			for (short i=0; i<NArranger(); i++) {
				Arranger(i)->AddInstanceRepresentation(instance);
			}
		}
		ShowObjectRepresentation(voice->sym);
		return voice->sym;
	}
	return nullptr;
}


StabEnt *
QuaDisplay::CreateChannel(char *nm, short chan,
						uchar nAuIn,
						uchar nAuOut,
						bool add_dflt_au_in,
						bool add_dflt_au_out,
						bool add_dflt_str_in,
						bool add_dflt_str_out
				)
{
	// create voice
	string channel_nm;
	short	i;

	if (nm != nullptr  && *nm != '\0') {
		channel_nm = nm;
	} else {
		channel_nm = "channel" + (chan>=0?chan+1:qua->nChannel+1);
	}

	std::string nmbuf = glob.makeUniqueName(qua->sym, channel_nm, 0);
	// stereo in and out at id, 'chan', midi and audio thru enabled ... no adding destingations
	Channel	*channel = qua->AddChannel(
						nmbuf, chan, nAuIn, nAuOut,
						true, true,
						add_dflt_au_in,
						add_dflt_au_out,
						add_dflt_str_in,
						add_dflt_str_out,
						-1);
	if (channel) {
		fprintf(stderr, "created channel %s\n", channel->sym->name.c_str());
		// update index displays
		for (i=0; i<NIndexer(); i++) {
			Indexer(i)->addToSymbolIndex(channel->sym);
		}
		for (i=0; i<NChannelRack(); i++) {
			ChannelRack(i)->AddChannelRepresentation(channel);
		}
		for (i=0; i<NObjectRack(); i++) {
			ObjectRack(i)->UpdateChannelIndexDisplay(nullptr);
		}
		return channel->sym;
	}
	return nullptr;
}

StabEnt *
QuaDisplay::CreateMethod(const std::string &nm, StabEnt *parent)
{
	// create voice
	short	i;

	std::string nmbuf = glob.makeUniqueName(parent, nm.size()>0?nm:"action", 0);

	Lambda	*lambda = qua->CreateMethod(nmbuf, parent, false);
	fprintf(stderr, "created lambda %s\n", lambda->sym->name.c_str());
	if (lambda) {
		// update index displays
		for (i=0; i<NIndexer(); i++) {
			Indexer(i)->addToSymbolIndex(lambda->sym);
		}
		// update object view displays
		if (parent != nullptr && parent->type != TypedValue::S_QUA) {
			// do squat if it's a global show it only when asked
			for (i=0; i<NObjectRack(); i++) {
				QuaObjectRepresentation	*OR = ObjectRack(i)->RepresentationFor(parent);
				if (OR != nullptr) {
					OR->AddChildRepresentation(lambda->sym);
				}
			}
		}
		return lambda->sym;
	}
	return nullptr;
}


StabEnt *
QuaDisplay::CreateClip(const std::string &nm, const Time &at, const Time &dur)
{
	std::string clip_nm = glob.makeUniqueName(qua->sym, nm.size() > 0? nm:"clip", 1);

	Clip *clip = qua->addClip(clip_nm, at, dur, false);
	if (clip) {
		fprintf(stderr, "created clip %s\n", clip->sym->name.c_str());
		for (short i=0; i<NIndexer(); i++) {
			Indexer(i)->updateClipIndexDisplay();
		}
		for (short i=0; i<NArranger(); i++) {
			Arranger(i)->updateClipIndexDisplay();
		}
		for (short i=0; i<NObjectRack(); i++) {
			ObjectRack(i)->updateClipIndexDisplay(clip->sym->context);
		}

		return clip->sym;
	}
	return nullptr;
}

void
QuaDisplay::Rename(StabEnt *sym, const string &nm)
{
	glob.rename(sym, nm);
	for (short i=0; i<NObjectRack(); i++) {
		QuaObjectRepresentation *or = ObjectRack(i)->RepresentationFor(sym);
		if (or) {
			or->SetName();
		}
		or = ObjectRack(i)->RepresentationFor(sym->context);
		if (or) {
			or->ChildNameChanged(sym);
		}
	}
	for (short j=0; j<NIndexer(); j++) {
		Indexer(j)->symbolNameChanged(sym);
	}
	for (short j = 0; j < NArranger(); j++) {
		Arranger(j)->symbolNameChanged(sym);
	}
	if (sym->type == TypedValue::S_CHANNEL) {
		for (short k=0; k<NChannelRack(); k++) {
			QuaChannelRepresentation *cr = ChannelRack(k)->ChannelRepresentationFor(sym);
			if (cr) {
				cr->NameChanged();
			}
		}
	}
	if (sym->type == TypedValue::S_INPUT) {
		Input		*inp = sym->InputValue();
		StabEnt		*ps = sym->context;

		for (short k=0; k<NChannelRack(); k++) {
			QuaChannelRepresentation *cr = ChannelRack(k)->ChannelRepresentationFor(ps);
			QuaInputRepresentation	*ir = cr->InputRepresentationFor(inp);
			if (ir) {
				ir->NameChanged();
			}
		}
	}
	if (sym->type == TypedValue::S_OUTPUT) {
		Output		*inp = sym->OutputValue();
		StabEnt		*ps = sym->context;

		for (short k=0; k<NChannelRack(); k++) {
			QuaChannelRepresentation *cr = ChannelRack(k)->ChannelRepresentationFor(ps);
			QuaOutputRepresentation	*ir = cr->OutputRepresentationFor(inp);
			if (ir) {
				ir->NameChanged();
			}
		}
	}
}

// called from within interface to add a sample take to a sample object
void
QuaDisplay::LoadSampleTake(StabEnt *sampleSym, std::string path)
{
	Sample	*sample = sampleSym->SampleValue();
	if (sample == nullptr) {
		return;
	}
	std::string name = glob.makeUniqueName(sampleSym, getBase(path),  1);
	sample->addSampleTake(name, path, true);
}

void
QuaDisplay::CreateStreamTake(StabEnt *voiceSym)
{
	Voice	*voice = voiceSym->VoiceValue();
	if (voice == nullptr) {
		return;
	}
	Time	duration;
	duration.Set("1:0.0");

	std::string nmbuf = glob.makeUniqueName(voiceSym, "stream", 1);
	voice->addStreamTake(nmbuf, duration, true);
}


void
QuaDisplay::updateClipIndexDisplay(StabEnt *sym)
{
	if (sym->type == TypedValue::S_QUA) {
		for (short i=0; i<NArranger(); i++) {
			Arranger(i)->updateClipIndexDisplay();
		}
		for (short i=0; i<NIndexer(); i++) {
			Indexer(i)->updateClipIndexDisplay();
		}
	} else {
		for (short i=0; i<NObjectRack(); i++) {
			ObjectRack(i)->updateClipIndexDisplay(sym);
		}
	}
}

void
QuaDisplay::UpdateTakeIndexDisplay(StabEnt *sym)
{
	for (short i=0; i<NObjectRack(); i++) {
		QuaSchedulableObjectRepresentation *or =
			ObjectRack(i)->RepresentationForSchedulable(sym);
		if (or) {
			or->UpdateTakeIndexDisplay();
		}
	}
}

void
QuaDisplay::UpdateVariableIndexDisplay(StabEnt *sym)
{
	for (short i=0; i<NObjectRack(); i++) {
		QuaSchedulableObjectRepresentation *or =
			ObjectRack(i)->RepresentationForSchedulable(sym);
		if (or) {
			or->UpdateVariableIndexDisplay();
		}
	}
}

void
QuaDisplay::UpdateControllerDisplay(StabEnt *stacker, QuasiStack *frame, StabEnt *sym,
			qua_perpective_type srctype, QuaPerspective *src)
{
	fprintf(stderr, "update ctlr disp\n");
	for (short i=0; i<NObjectRack(); i++) {
		QuaObjectRackPerspective *ork=ObjectRack(i);
		if (ork != nullptr && !(srctype==QUAPSCV_OBJECTRACK && ork==src)) {
			QuaObjectRepresentation *or =
				ObjectRack(i)->RepresentationFor(stacker);
			if (or) {
				or->UpdateControllerDisplay(stacker, frame, sym);
			}
		}
	}
	for (short j=0; j<NArranger(); j++) {
		QuaArrangerPerspective	*ar = Arranger(j);
		if (ar!= nullptr && !(srctype==QUAPSCV_ARRANGER && ar==src)) {
			ar->UpdateControllerDisplay(stacker, frame, sym);
		}
	}
}

void
QuaDisplay::UpdateGlobalTime(Time &t)
{
	if (qua != nullptr) {
		qua->theTime = t;
		DisplayGlobalTime(t, true);
	}
}

void
QuaDisplay::UpdateTempo(float tempo)
{
	if (qua != nullptr && qua->theTime.metric != nullptr) {
		qua->theTime.metric->tempo = tempo;
		DisplayTempo(tempo, true);
	}
}

void
QuaDisplay::ControllerChanged(
		StabEnt *sym, QuasiStack *qs, TypedValue &v,
		qua_perpective_type srctype,QuaPerspective *src)
{
	switch (sym->type) {
#ifdef QUA_V_VST_HOST
	case TypedValue::S_VST_PARAM: {
			Block		*b = qs->callingBlock;
			if (b == nullptr) {
				return;
			}
			VstPlugin	*vst = b->crap.call.crap.vstplugin;
			if (!vst || vst->status != VST_PLUG_LOADED) {
				return;
			}
			AEffect	*afx = qs->stk.afx;
			if (afx)
				afx->setParameter(afx, sym->VstParamValue(), v.FloatValue(nullptr));
			break;
		}

		case TypedValue::S_VST_PROGRAM: {
			Block		*b = qs->callingBlock;
			if (b == nullptr) {
				return;
			}
			VstPlugin	*vst = b->crap.call.crap.vstplugin;
			if (!vst || vst->status != VST_PLUG_LOADED) {
				return;
			}
			AEffect	*afx = qs->stk.afx;
			VstPlugin::SetProgram(afx, v.VstProgramValue());
			break;
		}
#endif
		default: {
			LValue	l;
			ResultValue	r;
			sym->SetLValue(l, nullptr, qs->stacker, qs->stackerSym, qs);
			l.StoreValue(&v);
			break;
		}
	}
	// now update every attached interface (but not the one we just 
	// made this change from
	UpdateControllerDisplay(qs->stackerSym, qs, sym, srctype,src);
}

void
QuaDisplay::DisplayStatus(Instance *, qua_status) {
}

void
QuaDisplay::DisplayStatus(QuasiStack *, qua_status) {
};


void
QuaDisplay::AddQuaNexion(QuaInsert *i, QuaInsert *di)
{
	;
}

void
QuaDisplay::RemoveQuaNexion(QuaInsert *di)
{
	;
}

void
QuaDisplay::RemoveControlVariables(QuasiStack *qs)
{
	;
}


void
QuaDisplay::AddControlVariables(QuasiStack *qs)
{
	;
}


// called by interface to do all the appropriate bits and pieces
// compiling a block of code in the given symbol space
short
QuaDisplay::CompileBlock(
	StabEnt *sym, char *srcnm, char *txt, long textlen)
{
	TxtParser	parser(txt, textlen, srcnm, qua);
	parser.rewind();
	parser.GetToken();
	Block	*b = parser.ParseBlockSequenceToList(sym);
	if (parser.err_cnt == 0) {
		StabEnt		*stksym = sym;

		while (stksym != nullptr && stksym->type != TypedValue::S_QUA) {
			glob.PushContext(stksym);
			stksym = stksym->context;
		}
		if (b && !b->Init(nullptr)) {
			b->DeleteAll();
			return Q_PARSE_ERR;
		}
		while (glob.PopContext(nullptr) != sym);
	} else {
		if (b) {
			b->DeleteAll();
		}
		return Q_PARSE_ERR;
	}
	Executable *ex;
	if (sym->type == TypedValue::S_EVENT) {
		Event	*ev = sym->EventValue();
//		ev->ReplaceBlock(qua, ev->block, b);
		ev->ReplaceBlock(ev->block, b);
	} else if ((ex = sym->ExecutableValue()) != nullptr) {
//		ex->ReplaceBlock(qua, ex->mainBlock, b);
		ex->ReplaceBlock(ex->mainBlock, b);
	}
	return Q_PARSE_OK;
}

// called by interface to do all the appropriate bits and pieces
// to check validity of a block of code in the given symbol space
short
QuaDisplay::ParseBlock(StabEnt *sym, char *srcnm, char *txt, long textlen)
{
	TxtParser	parser(txt, textlen, srcnm, qua);
	parser.rewind();
	parser.GetToken();
	Block	*b = parser.ParseBlockSequenceToList(sym);
	parser.ShowErrors();
	if (parser.err_cnt == 0) {
		StabEnt		*stksym = sym;

		while (stksym != nullptr && stksym->type != TypedValue::S_QUA) {
			glob.PushContext(stksym);
			stksym = stksym->context;
		}
		if (b && !b->Init(nullptr)) {
			b->DeleteAll();
			return Q_PARSE_ERR;
		}
		while (glob.PopContext(nullptr) != sym);
	} else {
		if (b) {
			b->DeleteAll();
		}
		return Q_PARSE_ERR;
	}
	if (b) {
		b->DeleteAll();
	}
	return Q_PARSE_OK;
}


// called by instance an instance object to get a qua stack frame popped onto it.
// 
void	
QuaDisplay::RequestPopFrameRepresentation(
	QuaInstanceObjectRepresentation *iv,
	StabEnt *sym,
	QuasiStack *parent, QuasiStack *stack, bool show, bool add_children)
{
	if (iv == nullptr) {
		return;
	}
	if (show) {
#ifdef Q_FRAME_MAP
		frame_map_hdr	*map;
#endif
		if (stack == nullptr) {
			Instance	*i;
			if ((i=sym->InstanceValue()) != nullptr) {
				stack = i->mainStack;
			} else {
				if (sym->type == TypedValue::S_EVENT && sym->context) {
					if (sym->context->type == TypedValue::S_CHANNEL) {
						Channel		*c=sym->context->ChannelValue();
						if (sym == c->rx.sym) {
							stack = c->rxStack;
						} else if (sym == c->tx.sym) {
							stack = c->txStack;
						}
					}
				}
			}
			fprintf(stderr, "request pop: stack is %x\n", ((unsigned)stack));
			if (stack) {
#ifdef Q_FRAME_MAP
				if (stack->GetFrameMap(map)) {
					fprintf(stderr, "popping parent from request %s\n", sym?sym->uniqueName():"(null)"); 
					iv->PopFrameRepresentation(
						sym, parent, stack, map, add_children);
				}
#else
				iv->PopFrameRepresentation(
					sym, parent, stack, add_children);
#endif
			}
		} else {
#ifdef Q_FRAME_MAP
			if (stack->GetFrameMap(map)) {
				fprintf(stderr, "popping from request %s\n", sym?sym->uniqueName():"(null)"); 
				iv->PopFrameRepresentation(
					sym, parent, stack, map, add_children);
			}
#else
			iv->PopFrameRepresentation(
				sym, parent, stack, add_children);
#endif
		}
	} else {
//		iv->HideFrameRepresentation(sym, stack);
	}
}

#ifdef Q_FRAME_MAP
void
QuaDisplay::PopFrameRepresentation(StabEnt *, QuasiStack *, frame_map_hdr *, long)
{
	;
}
#else
void
QuaDisplay::PopFrameRepresentation(StabEnt *stacker, QuasiStack *stack)
{
	for (short i=0; i<NObjectRack(); i++) {
		QuaObjectRackPerspective	*ov = ObjectRack(i);
		QuaInstanceObjectRepresentation * ir=ov->RepresentationForInstance(stacker);
		if (ir && ir->symbol) {
			Instance	*in = ir->symbol->InstanceValue();
			if (in) {
				RequestPopFrameRepresentation(ir, stacker, in->mainStack, stack, true, true);
			}
		}
	}
}
#endif

void	
QuaDisplay::HideFrameRepresentation(StabEnt *, QuasiStack *)
{
	;
}

void	
QuaDisplay::RemoveFrameRepresentation(StabEnt *stacker, QuasiStack *stack)
{
	for (short i=0; i<NObjectRack(); i++) {
		QuaObjectRackPerspective	*ov = ObjectRack(i);
		QuaInstanceObjectRepresentation * ir=ov->RepresentationForInstance(stacker);
		if (ir && ir->symbol) {
			Instance	*in = ir->symbol->InstanceValue();
			if (in) {
				RequestRemoveFrameRepresentation(ir, stacker, stack);
			}
		}
	}
}

void	
QuaDisplay::RequestRemoveFrameRepresentation(QuaInstanceObjectRepresentation * ir, StabEnt *stacker, QuasiStack *stack)
{
	if (ir) {
		ir->DeleteFrameRepresentation(stacker, stack);
	}
}

void
QuaDisplay::parseErrorViewClear() {
}

void
QuaDisplay::parseErrorViewAddLine(string s, int sev) {
}

void
QuaDisplay::parseErrorViewShow() {
}

void
QuaDisplay::tragicError(const char *str, ...) {

}
void
QuaDisplay::reportError(const char *str, ...) {

}

void
QuaDisplay::DisplayDuration(Instance *) {
}

void
QuaDisplay::DisplayChannel(Instance *) {
}

void
QuaDisplay::DisplayStartTime(Instance *) {
}

void
QuaDisplay::DisplayWake(Instance *) {
}

void
QuaDisplay::DisplaySleep(Instance *) {
}



//////////////////////////////////////////////
// QuaEnvironmentDisplay
//////////////////////////////////////////////

QuaEnvironmentDisplay::QuaEnvironmentDisplay()
{
	;
}

QuaEnvironmentDisplay::~QuaEnvironmentDisplay()
{
	;
}

void
QuaEnvironmentDisplay::RefreshVstPluginList()
{
	;
}

void
QuaEnvironmentDisplay::CreateMethodBridge(Lambda *)
{
	;
}

void
QuaEnvironmentDisplay::CreatePortBridge(QuaPort *)
{
	;
}

void
QuaEnvironmentDisplay::CreateTemplateBridge(Template *)
{
	;
}


void
QuaEnvironmentDisplay::CreateVstPluginBridge(VstPlugin *)
{
	;
}

void
QuaEnvironmentDisplay::RemoveMethodBridge(Lambda *)
{
	;
}

void
QuaEnvironmentDisplay::RemovePortBridge(QuaPort *)
{
	;
}



void
QuaEnvironmentDisplay::RemoveTemplateBridge(Template *)
{
	;
}

void
QuaEnvironmentDisplay::RemoveVstPluginBridge(VstPlugin *)
{
	;
}

QuaGlobalIndexPerspective::QuaGlobalIndexPerspective()
{
	;
}

QuaGlobalIndexPerspective::~QuaGlobalIndexPerspective()
{
	;
}


//////////////////////////////////////////////
// QuaRepresentations
//////////////////////////////////////////////

//////////////////////////////////////////////
// QuaChannelRepresentation
//////////////////////////////////////////////
QuaChannelRepresentation::QuaChannelRepresentation()
{
	channel = nullptr;
}

QuaChannelRepresentation::~QuaChannelRepresentation()
{
	channel = nullptr;
}

void
QuaChannelRepresentation::SetChannel(Channel *c)
{
	channel = c;
	AddDestinationRepresentations();
}

void
QuaChannelRepresentation::AddDestinationRepresentations()
{
	if (channel != nullptr) {
		for (short i=0; ((size_t)i)<channel->inputs.size(); i++) {
			if (InputRepresentationFor(channel->inputs.Item(i)) == nullptr) {
				AddInputRepresentation(channel->inputs.Item(i));
			}
		}
		for (short i=0; ((size_t)i)<channel->outputs.size(); i++) {
			if (OutputRepresentationFor(channel->outputs.Item(i)) == nullptr) {
				AddOutputRepresentation(channel->outputs.Item(i));
			}
		}
	}
}


QuaInputRepresentation *
QuaChannelRepresentation::InputRepresentationFor(Input *c)
{
	for (short i=0; i<NInR(); i++) {
		if (InR(i)->input == c) {
			return InR(i);
		}
	}
	return nullptr;
}

QuaOutputRepresentation *
QuaChannelRepresentation::OutputRepresentationFor(Output *c)
{
	for (short i=0; i<NOutR(); i++) {
		if (OutR(i)->output == c) {
			return OutR(i);
		}
	}
	return nullptr;
}

//////////////////////////////////////////////
// QuaOutputRepresentation
//////////////////////////////////////////////
QuaOutputRepresentation::QuaOutputRepresentation()
{
	output = nullptr;
}

QuaOutputRepresentation::~QuaOutputRepresentation()
{
}

void
QuaOutputRepresentation::SetOutput(Output *d)
{
	output = d;
}


const char *
QuaOutputRepresentation::DeviceName(uchar df, uchar cf)
{
	if (output != nullptr) {
		return output->Name(df, cf);
	}
	return "No output";
}

long
QuaOutputRepresentation::DeviceChannel()
{
	if (output != nullptr) {
		return output->deviceChannel;
	}
	return 0;
}

long
QuaOutputRepresentation::DeviceType()
{
	if (output != nullptr) {
		if (output->device != nullptr) {
			return output->device->deviceType;
		}
	}
	return 	QUA_DEV_NOT;
}


void
QuaOutputRepresentation::UpdateGain(float gain)
{
	if (output) {
		output->gain = gain;
	}
}

void
QuaOutputRepresentation::UpdatePan(float pan)
{
	if (output) {
		output->pan = pan;
	}
}

void
QuaOutputRepresentation::UpdateEnable(bool en)
{
	if (output) {
		output->SetEnabled(en);
	}
}

void
QuaOutputRepresentation::UpdateDestination(QuaPort *port, port_chan_id ch, short flg)
{
	if (output) {
		output->setPortInfo(port, ch, flg);
		DisplayDestination();
	}
}

//////////////////////////////////////////////
// QuaInputRepresentation
//////////////////////////////////////////////
QuaInputRepresentation::QuaInputRepresentation()
{
	input = nullptr;
}

QuaInputRepresentation::~QuaInputRepresentation()
{
}

void
QuaInputRepresentation::SetInput(Input *d)
{
	input = d;
}

const char *
QuaInputRepresentation::DeviceName(uchar df, uchar cf)
{
	if (input != nullptr) {
		return input->Name(df, cf);
	}
	return "No input";
}

long
QuaInputRepresentation::DeviceChannel()
{
	if (input != nullptr) {
		return input->deviceChannel;
	}
	return 0;
}

long
QuaInputRepresentation::DeviceType()
{
	if (input != nullptr) {
		if (input->device != nullptr) {
			return input->device->deviceType;
		}
	}
	return 	QUA_DEV_NOT;
}

// called by the interface to make changes to parameter values, make any other
// approriate adjustments within the sequencer, and forward these
// changes on to other interface elements
void
QuaInputRepresentation::UpdateGain(float gain)
{
	if (input) {
		input->gain = gain;
	}
}

void
QuaInputRepresentation::UpdatePan(float pan)
{
	if (input) {
		input->pan = pan;
	}
}

void
QuaInputRepresentation::UpdateEnable(bool en)
{
	if (input) {
		input->SetEnabled(en);
	}
}

void
QuaInputRepresentation::UpdateDestination(QuaPort *port, port_chan_id ch, short flg)
{
	if (input) {
		input->setPortInfo(port, ch, flg);
		DisplayDestination();
	}
}


//////////////////////////////////////////////
// QuaInstanceRepresentation
//////////////////////////////////////////////

QuaInstanceRepresentation::QuaInstanceRepresentation()
{
	instance = nullptr;
}

QuaInstanceRepresentation::~QuaInstanceRepresentation()
{
	instance = nullptr;
}

void
QuaInstanceRepresentation::SetInstance(Instance *inst)
{
	instance = inst;
}


//////////////////////////////////////////////
// QuaObjectRepresentation
//////////////////////////////////////////////

QuaObjectRepresentation::QuaObjectRepresentation()
{
	symbol = nullptr;
	quaLink = nullptr;
}

void
QuaObjectRepresentation::SetLinkage(QuaPerceptualSet *linkage)
{
	quaLink = linkage;
}

QuaObjectRepresentation::~QuaObjectRepresentation()
{
	symbol = nullptr;
}

void
QuaObjectRepresentation::SetSymbol(StabEnt *s)
{
	symbol = s;
}

void
QuaObjectRepresentation::Populate()
{
	if (symbol) {
		AttributePopulate();
		StabEnt *p = symbol->children;
		while (p != nullptr) {
			ChildPopulate(p);
			p = p->sibling;
		}
	};
}


QuaObjectRepresentation *
QuaObjectRepresentation::RepresentationFor(StabEnt *sym)
{
	short	i;
	for (i=0; i<NCOR(); i++) {
		if (COR(i)->symbol == sym) {
			return COR(i);
		}
	}
	return nullptr;
}


QuaSchedulableObjectRepresentation::QuaSchedulableObjectRepresentation()
{
}

QuaSchedulableObjectRepresentation::~QuaSchedulableObjectRepresentation()
{
}

QuaSampleObjectRepresentation::QuaSampleObjectRepresentation()
{
}

QuaSampleObjectRepresentation::~QuaSampleObjectRepresentation()
{
}

QuaVoiceObjectRepresentation::QuaVoiceObjectRepresentation()
{
}

QuaVoiceObjectRepresentation::~QuaVoiceObjectRepresentation()
{
}

QuaChannelObjectRepresentation::QuaChannelObjectRepresentation()
{
}

QuaChannelObjectRepresentation::~QuaChannelObjectRepresentation()
{
}

QuaMethodObjectRepresentation::QuaMethodObjectRepresentation()
{
}

QuaMethodObjectRepresentation::~QuaMethodObjectRepresentation()
{
}

QuaControllerRepresentation::QuaControllerRepresentation()
{
}

QuaControllerRepresentation::~QuaControllerRepresentation()
{
}

void
QuaControllerRepresentation::Set(StabEnt *sym, QuaFrameRepresentation *p)
{
	symbol = sym;
	parent = p;
}

QuaFrameRepresentation::QuaFrameRepresentation()
{
	root = nullptr;
	parent = nullptr;
	frame = nullptr;
	isHidden = false;
	hasChildren = false;
}

void
QuaFrameRepresentation::Set(QuaInstanceObjectRepresentation	*rfr, QuaFrameRepresentation *pfr, QuasiStack *fr, bool hc)
{
	root = rfr;
	parent = pfr;
	frame = fr;
	hasChildren = hc;
}

QuaFrameRepresentation::~QuaFrameRepresentation()
{
}

QuaControllerRepresentation *
QuaFrameRepresentation::RepresentationFor(StabEnt *sym)
{
	for (short i=0; i<NCR(); i++) {
		QuaControllerRepresentation *cr = CR(i);
		if (cr != nullptr && cr->symbol == sym) {
			return cr;
		}
	}
	return nullptr;
}

QuaInstanceObjectRepresentation::QuaInstanceObjectRepresentation()
{
}

QuaInstanceObjectRepresentation::~QuaInstanceObjectRepresentation()
{
}

void
QuaInstanceObjectRepresentation::UpdateChannelIndexDisplay(StabEnt *ds)
{
	for (short i=0; i<NFR(); i++) {
		FR(i)->UpdateChannelIndexDisplay(ds);
	}
}

void
QuaInstanceObjectRepresentation::updateClipIndexDisplay()
{
	for (short i=0; i<NFR(); i++) {
		FR(i)->updateClipIndexDisplay();
	}
}

QuaFrameRepresentation *
QuaInstanceObjectRepresentation::RepresentationFor(QuasiStack *s)
{
	for (short i=0; i<NFR(); i++) {
		if (FR(i)->frame == s) {
			return FR(i);
		}
	}
	return nullptr;
}
//////////////////////////////////////////////
// QuaPerspective
//////////////////////////////////////////////
QuaPerspective::QuaPerspective()
{
	quaLink = nullptr;
}

QuaPerspective::~QuaPerspective()
{
	;
}

void
QuaPerspective::SetLinkage(QuaPerceptualSet *linkage)
{
	quaLink = linkage;
}

//////////////////////////////////////////////
// QuaChannelRackPerspective
//////////////////////////////////////////////
QuaChannelRackPerspective::QuaChannelRackPerspective():
	QuaPerspective()
{
	;
}

QuaChannelRackPerspective::~QuaChannelRackPerspective()
{
	;
}

QuaChannelRepresentation *
QuaChannelRackPerspective::ChannelRepresentationFor(Channel *c)
{
	for (short i=0; i<NCR(); i++) {
		if (CR(i)->channel == c) {
			return CR(i);
		}
	}
	return nullptr;
}

QuaChannelRepresentation *
QuaChannelRackPerspective::ChannelRepresentationFor(StabEnt *c)
{
	for (short i=0; i<NCR(); i++) {
		if (CR(i)->channel->sym == c) {
			return CR(i);
		}
	}
	return nullptr;
}

//////////////////////////////////////////////
// QuaArrangerPerspective
//////////////////////////////////////////////
QuaArrangerPerspective::QuaArrangerPerspective():
	QuaPerspective()
{
	;
}

QuaArrangerPerspective::~QuaArrangerPerspective()
{
	;
}


QuaInstanceRepresentation *
QuaArrangerPerspective::InstanceRepresentationFor(Instance *c)
{
	for (short i=0; i<NIR(); i++) {
		if (IR(i)->instance == c) {
			return IR(i);
		}
	}
	return nullptr;
}

//////////////////////////////////////////////
// QuaTransportPerspective
//////////////////////////////////////////////
QuaTransportPerspective::QuaTransportPerspective():
	QuaPerspective()
{
	;
}

QuaTransportPerspective::~QuaTransportPerspective()
{
	;
}

void
QuaTransportPerspective::UpdateGlobalTime(Time &t)
{
	quaLink->UpdateGlobalTime(t);
}

void
QuaTransportPerspective::UpdateTempo(float tempo)
{
	quaLink->UpdateTempo(tempo);
}

//////////////////////////////////////////////
// QuaObjectPerspective
//////////////////////////////////////////////
QuaObjectRackPerspective::QuaObjectRackPerspective():
	QuaPerspective()
{
}

QuaObjectRackPerspective::~QuaObjectRackPerspective()
{
	;
}

QuaObjectRepresentation *
QuaObjectRackPerspective::RepresentationFor(StabEnt *sym)
{
	short	i;
	if (sym == nullptr) {
		return nullptr;
	}
	for (i=0; i<NOR(); i++) {
		if (OR(i)) {
			if (OR(i)->symbol == sym) {
				return OR(i);
			}
		}
	}
	if (sym->context) {
		QuaObjectRepresentation	*por = RepresentationFor(sym->context);
		if (por) {
			QuaObjectRepresentation	*qcor = por->RepresentationFor(sym);
			if (qcor) {
				return qcor;
			}
		}
	}
	return nullptr;
}

QuaInstanceObjectRepresentation *
QuaObjectRackPerspective::RepresentationForInstance(StabEnt *sym)
{
	if (sym->type == TypedValue::S_INSTANCE || sym->type == TypedValue::S_CHANNEL) {
		return (QuaInstanceObjectRepresentation *)RepresentationFor(sym);
	}
	return nullptr;
}

QuaSchedulableObjectRepresentation *
QuaObjectRackPerspective::RepresentationForSchedulable(StabEnt *sym)
{
	short	i;
	if (sym->type == TypedValue::S_SAMPLE || sym->type == TypedValue::S_VOICE) {
		for (i=0; i<NOR(); i++) {
			if (OR(i)->symbol == sym) {
				return (QuaSchedulableObjectRepresentation *)OR(i);
			}
		}
	}
	return nullptr;
}

void
QuaObjectRackPerspective::updateClipIndexDisplay(StabEnt *s)
{
	for (int i=0; i<NOR(); i++) {
		QuaObjectRepresentation *or = OR(i);
		if (or && or->symbol) {
			switch (or->symbol->type) {
				case TypedValue::S_VOICE:
				case TypedValue::S_SAMPLE:
					if (or->symbol == s) {
						QuaSchedulableObjectRepresentation *sor=(QuaSchedulableObjectRepresentation*)or;
						sor->updateClipIndexDisplay();
						for (int j=0; j<sor->NCOR(); j++) {
							QuaObjectRepresentation *cor = sor->COR(j);
							if (cor && cor->symbol && cor->symbol->type == TypedValue::S_INSTANCE) {
								QuaInstanceObjectRepresentation *ior=(QuaInstanceObjectRepresentation*)cor;
								ior->updateClipIndexDisplay();
							}
						}
					}
					break;
				case TypedValue::S_INSTANCE:
					if (or->symbol->context == s) {
						QuaInstanceObjectRepresentation *ior=(QuaInstanceObjectRepresentation*)or;
						ior->updateClipIndexDisplay();
					}
					break;
			}
		}
	}
}

void
QuaObjectRackPerspective::UpdateChannelIndexDisplay(StabEnt *ds)
{
	for (int i=0; i<NOR(); i++) {
		QuaObjectRepresentation *or = OR(i);
		if (or && or->symbol) {
			switch (or->symbol->type) {
				case TypedValue::S_VOICE:
				case TypedValue::S_SAMPLE: {
					for (int j=0; j<or->NCOR(); j++) {
						QuaObjectRepresentation *cor = or->COR(j);
						if (cor->symbol->type == TypedValue::S_INSTANCE) {
							QuaInstanceObjectRepresentation *ior=(QuaInstanceObjectRepresentation*)cor;
							ior->UpdateChannelIndexDisplay(ds);
						}
					}
					break;
				}
				case TypedValue::S_INSTANCE: {
					QuaInstanceObjectRepresentation *ior=(QuaInstanceObjectRepresentation*)or;
					ior->UpdateChannelIndexDisplay(ds);
					break;
				}
				case TypedValue::S_CHANNEL: {
					QuaChannelObjectRepresentation *cor=(QuaChannelObjectRepresentation*)or;
					cor->UpdateChannelIndexDisplay(ds);
					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////
// QuaIndexPerspective
//////////////////////////////////////////////
QuaIndexPerspective::QuaIndexPerspective():
	QuaPerspective()
{
	;
}

QuaIndexPerspective::~QuaIndexPerspective()
{
	;
}

//////////////////////////////////////////////
// default error handler implementation 
/////////////////////////////////////////////
void
ErrorHandler::parseErrorViewClear() {
}

void
ErrorHandler::parseErrorViewAddLine(string s, int sev) {
	cout << "Parse error, severity " << sev << ", " << s << endl;
}

void
ErrorHandler::parseErrorViewShow() {
}

void
ErrorHandler::tragicError(const char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	va_end(args);

	cout << buf << endl;
}
void
ErrorHandler::reportError(const char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	va_end(args);

	cout << buf << endl;
}


/*
 remnant beos code ...
*/
#ifdef XXXXX

status_t
Executable::SetDisplayInfo(Block *b)
{
	displayInfo = b;
	if (representation && b->type == C_LIST) {
		Block	*p = b->crap.list.block;
		if (!p) {
			return B_ERROR;
		}
		if (p->type == C_VALUE) {
			uint32		v = p->crap.constant.value.IntValue(nullptr);
			rgb_color	c = *((rgb_color*)&v);
			representation->SetColor(c);
//			fprintf(stderr, "%x %x %x %x %x\n",
//					v, c.red, c.green, c.blue, c.alpha);
		}
		Block	*pt = p->next;
		if (pt && pt->type == C_LIST) {
			Block	*par = pt->crap.list.block;
			if (par && par->next) {
				BPoint	p;
				p.x = par->crap.constant.value.FloatValue(nullptr);
				p.y = par->next->crap.constant.value.FloatValue(nullptr);
				representation->MoveTo(p);
				fprintf(stderr, "move %g %g\n", p.x, p.y);
			}
		}
	}
	return B_OK;
}


	virtual bool			ExtraneousMessage(class ArrangerObject *a, BMessage *inMsg);
	virtual bool			ExtraneousMouse(class ArrangerObject *a, BPoint *where, ulong, ulong);
	virtual void			DrawExtraneous(class ArrangerObject *a, BRect *r);

bool
QuaAudioPort::ExtraneousMouse(class ArrangerObject *a, BPoint where, ulong buts, ulong mods)
{
	if (	(mods & B_COMMAND_KEY) &&
			!(mods & B_SHIFT_KEY) &&
			(buts & B_PRIMARY_MOUSE_BUTTON)) {
			
		BPopUpMenu	*qMenu = new BPopUpMenu("env sel", true, FALSE);
		ZotParameterMenu(qMenu, sym, a);

		BPoint			orig = where;
		a->ConvertToScreen(&where);
		
		qMenu->SetAsyncAutoDestruct(true);
		qMenu->Go(where, true, false, true);
		return true;
	} else if (buts & B_SECONDARY_MOUSE_BUTTON) {
		if (IsMultiSchedulable()) {
			return false;
		} else {
			return true;
		}
							 
	}
	return false;
}


bool
Schedulable::ExtraneousMessage(class ArrangerObject *a, BMessage *inMsg)
{
	switch (inMsg->what) {
	case SET_ENVELOPE:{
		QuaControllerBridge		*cv = nullptr;
		status_t err=inMsg->FindPointer("control var", (void **)&cv);
		if (err == B_NO_ERROR) {
			a->displayedControllerBridge = cv;
			if (cv && cv->envelope) {
				a->SetEnvelope(cv->envelope);
			}
			a->Invalidate();
			return false;
		}
		return true;
	}
	}
	
	return false;
}

bool
Schedulable::ExtraneousMouse(class ArrangerObject *a, BPoint *where, ulong buts, ulong mods)
{
	return false;
}

void
Schedulable::DrawExtraneous(class ArrangerObject *a, BRect *r)
{
	if (a->displayedControlVariable) {
		a->DrawDisplayedControlVariable(r);
	}
}


#endif
