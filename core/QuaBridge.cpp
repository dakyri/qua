#include "qua_version.h"

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
#include "QuaDisplay.h"

QuaBridge::QuaBridge(Qua &q, QuaPerceptualSet &d)
	: uberQua(q)
	, display(d)
{
	display.setTo(&q);
}

QuaBridge::~QuaBridge()
{
	;
}

void
QuaBridge::AddControlVariables(QuasiStack *qs)
{
	;
}

void
QuaBridge::AddQuaNexion(QuaInsert *i, QuaInsert *di)
{
	;
}

bool
QuaBridge::HasDisplayParameters(StabEnt *s)
{
	return display.HasDisplayParameters(s);
}


char *
QuaBridge::DisplayParameterId()
{
	return display.DisplayParameterId();
}

status_t
QuaBridge::WriteDisplayParameters(FILE *fp, StabEnt *s)
{
	return display.WriteDisplayParameters(fp, s);
}

bool
QuaBridge::ParseDisplayParameters(QSParser *parser)
{
	return false;
}

void
QuaBridge::SetDisplayParameters(StabEnt *sym)
{
	;
}

bool
QuaBridge::Spawn()
{
	return true;
}

bool
QuaBridge::Cleanup()
{
	return true;
}

void
QuaBridge::DisplayChannel(Instance *inst)
{
}


void
QuaBridge::DisplayDuration(Instance *inst)
{
	;
}

void
QuaBridge::DisplayStartTime(Instance *inst)
{
	;
}


void
QuaBridge::DisplaySleep(Instance *inst)
{
	;
}

void
QuaBridge::DisplayWake(Instance *inst)
{
	;
}

void
QuaBridge::DisplayStatus(QuasiStack *qs)
{
	;
}


void
QuaBridge::DisplayStatus(Instance *inst)
{
	;
}


void
QuaBridge::DisplayTempo(float t, bool async)
{
	display.DisplayTempo(t, async);
}

void
QuaBridge::Rename(StabEnt *s, const string &nm)
{
	display.Rename(s, nm);
}

void
QuaBridge::displayArrangementTitle(const char *nm)
{
	display.displayArrangementTitle(nm);
}

void
QuaBridge::RemoveControlVariables(QuasiStack *qs)
{
	;
}

void
QuaBridge::updateClipIndexDisplay(StabEnt *sym)
{
	display.updateClipIndexDisplay(sym);
}

void
QuaBridge::UpdateTakeIndexDisplay(StabEnt *sym)
{
	display.UpdateTakeIndexDisplay(sym);
}

void
QuaBridge::UpdateVariableIndexDisplay(StabEnt *sym)
{
	display.UpdateVariableIndexDisplay(sym);
}

void
QuaBridge::UpdateSequencerTimeDisplay(bool async)
{
	display.DisplayGlobalTime(uberQua.theTime, async);
}

void
QuaBridge::PopHigherFrameRepresentations(QuasiStack *qs)
{
	StabEnt	*rootSym = qs->stackerSym;
#ifdef Q_FRAME_MAP
	display.PopHigherFrameRepresentations(rootSym, qs, nullptr, 0);
#else
	display.PopHigherFrameRepresentations(rootSym, qs);
#endif
}

void
QuaBridge::RemoveHigherFrameRepresentations(QuasiStack *qs)
{
	StabEnt	*rootSym = qs->stackerSym;
	display.RemoveHigherFrameRepresentations(rootSym, qs);
}


void
QuaBridge::RemoveSchedulableRepresentations(StabEnt *sym)
{
	display.RemoveSchedulableRepresentations(sym);
}

void
QuaBridge::RemoveMethodRepresentations(StabEnt *sym)
{
	display.RemoveMethodRepresentations(sym);
}

void
QuaBridge::RemoveChannelRepresentations(StabEnt *sym)
{
	display.RemoveChannelRepresentations(sym);
}

void
QuaBridge::RemoveInstanceRepresentations(StabEnt *sym)
{
	display.RemoveInstanceRepresentations(sym);
}


// should also check for presence in object lists
void
QuaBridge::AddSchedulableRepresentation(Schedulable *s)
{
	for (short i=0; i<display.NIndexer(); i++) {
		QuaIndexPerspective	*crv = display.Indexer(i);
		crv->addToSymbolIndex(s->sym);
	}
}

void
QuaBridge::AddMethodRepresentation(Lambda *m)
{
	for (short i=0; i<display.NIndexer(); i++) {
		QuaIndexPerspective	*crv = display.Indexer(i);
		crv->addToSymbolIndex(m->sym);
	}
}

void
QuaBridge::AddDestinationRepresentations(Channel *c)
{
	for (short i=0; i<display.NChannelRack(); i++) {
		QuaChannelRackPerspective	*crv = display.ChannelRack(i);
		if (crv != nullptr) {
			QuaChannelRepresentation	*cv = crv->ChannelRepresentationFor(c);
			if (cv != nullptr) {
				cv->AddDestinationRepresentations();
			}
		}
	}
}

void
QuaBridge::RemoveDestinationRepresentation(Channel *c, Input *s)
{
	for (short i=0; i<display.NChannelRack(); i++) {
		QuaChannelRackPerspective	*crv = display.ChannelRack(i);
		if (crv != nullptr) {
			QuaChannelRepresentation	*cv = crv->ChannelRepresentationFor(c);
			if (cv != nullptr) {
				cv->RemoveInputRepresentation(s);
			}
		}
	}
}

void
QuaBridge::RemoveDestinationRepresentation(Channel *c, Output *s)
{
	for (short i=0; i<display.NChannelRack(); i++) {
		QuaChannelRackPerspective	*crv = display.ChannelRack(i);
		if (crv != nullptr) {
			QuaChannelRepresentation	*cv = crv->ChannelRepresentationFor(c);
			if (cv != nullptr) {
				cv->RemoveOutputRepresentation(s);
			}
		}
	}
}

#include <stdarg.h>


void
QuaBridge::parseErrorViewClear() {
	return display.parseErrorViewClear();
}

void
QuaBridge::parseErrorViewAddLine(string s) {
	return display.parseErrorViewAddLine(s);
}

void
QuaBridge::parseErrorViewShow() {
	return display.parseErrorViewShow();
}



void
QuaBridge::tragicError(char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	display.tragicError(buf);
	va_end(args);
}

void
QuaBridge::reportError(char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	display.reportError(buf);
	va_end(args);
}

int
QuaBridge::retryError(char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	display.retryError(buf);
	va_end(args);
	return 0;
}

bool
QuaBridge::abortError(char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	display.abortError(buf);
	va_end(args);
	return 0;
}

bool
QuaBridge::continueError(char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	display.continueError(buf);
	va_end(args);
	return 0;
}

int
QuaBridge::optionWin(int i, char *str, ...) {
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	display.optionWin(i, buf);
	va_end(args);
	return 0;
}
