#include "qua_version.h"


#include <stdio.h>
#include <algorithm>


#include "StdDefs.h"

#include "Stream.h"
#include "Block.h"
#include "Parse.h"
#include "MidiDefs.h"

#include "Dictionary.h"

// danger. exterminate the enemy

StreamItemCache		FreeNote(sizeof(StreamNote));
#ifdef QUA_V_APP_HANDLER
StreamItemCache		FreeMesg(sizeof(StreamMesg));
#endif
StreamItemCache		FreeValue(sizeof(StreamValue));
StreamItemCache		FreeCtrl(sizeof(StreamCtrl));
StreamItemCache		FreeSysX(sizeof(StreamSysX));
StreamItemCache		FreeSysC(sizeof(StreamSysC));
StreamItemCache		FreeBend(sizeof(StreamBend));
StreamItemCache		FreeProg(sizeof(StreamProg));
StreamItemCache		FreeLogEntry(sizeof(StreamLogEntry));
StreamItemCache		FreeJoy(sizeof(StreamJoy));

StreamItemCache::StreamItemCache(size_t sz)
{
	free = nullptr;
	size = sz;
}

StreamItemCache::~StreamItemCache()
{
}

void *
StreamItemCache::Alloc()
{
	void *p;
	mutex.lock();
    if (free != nullptr) {
		p = (void *)free;
		free = free->next;
    } else {
		p = malloc(size);
    }
	mutex.unlock();
//	fprintf(stderr, "Alloc %d %x\n", Size, p);
	return p;
}

void
StreamItemCache::Dealloc(void *p)
{
	mutex.lock();
//	fprintf(stderr, "Dealloc %x\n", p);
	StreamItem	*q = (StreamItem *)p;
    q->next = free;
    free = q;
	mutex.unlock();
}

StreamItem *
StreamItem::Clone()
{
	internalError("Clone: unexpected stream object %d",type);
	return nullptr;
}

void
StreamItem::operator delete(void *q)
{
	switch(((StreamItem *)q)->type) {
	case TypedValue::S_NOTE: 		delete (StreamNote *)q; break;
	case TypedValue::S_BEND: 		delete (StreamBend *)q; break;
#ifdef QUA_V_APP_HANDLER
	case TypedValue::S_MESSAGE:	delete (StreamMesg *)q; break;
#endif
	case TypedValue::S_VALUE: 		delete (StreamValue *)q; break;
	case TypedValue::S_CTRL: 		delete (StreamCtrl *)q; break;
	case TypedValue::S_PROG: 		delete (StreamProg *)q; break;
	case TypedValue::S_SYSX: 		delete (StreamSysX *)q; break;
	case TypedValue::S_SYSC: 		delete (StreamSysC *)q; break;
	case TypedValue::S_LOG_ENTRY: 	delete (StreamLogEntry *)q; break;
	case TypedValue::S_JOY:		 	delete (StreamJoy *)q; break;
	default:
		internalError("delete: Unexpected stream object %d",
			((StreamItem *)q)->type);
	}
}

// values will pretty much depend on what this item is.
//  for a note,
//		- cmd is channel/ cmd (ie note on, note off, keypress)
//		- data1 is pitch
//		- data2 is velocity
//  for a control,
//		- cmd is channel/ 
//		- data1 is controller
//		- data2 is amount
//  for a program,
//		- cmd is channel/ 
//		- data1 is program no
bool
StreamItem::SetMidiParams(int8 cmd, int8 data1, int8 data2, bool force)
{
	if (type == TypedValue::S_NOTE) {
		StreamNote *me = (StreamNote *)this;
		me->note.pitch = data1;
		me->note.dynamic = data2;
	}
	return true;
}

/*
 * StreamNote
 */
void *
StreamNote::operator new(size_t sz)
{
//	fprintf(stderr, "new tone %d\n", sz);
	return FreeNote.Alloc();
}

void
StreamNote::operator delete(void *q)
{
//	fprintf(stderr, "delete tone %x\n", q);
	FreeNote.Dealloc(q);
}

StreamNote::StreamNote(class Time *t, class Note *tp)
{
    type = TypedValue::S_NOTE;
    if (tp) note = *tp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

StreamNote::StreamNote(class Time *tag, cmd_t cmd, pitch_t pitch, vel_t vel, dur_t dur)
{
    type = TypedValue::S_NOTE;
	note.cmd = cmd;
	note.duration = dur;
	note.dynamic = vel;
	note.pitch = pitch;
    time = *tag;
    next = nullptr;
    prev = nullptr;
}

StreamCtrl::StreamCtrl(class Time *tag, cmd_t cmd, ctrl_t ctrlr, amt_t amt)
{
    type = TypedValue::S_CTRL;
	ctrl.cmd = cmd;
	ctrl.controller = ctrlr;
	ctrl.amount = amt;
    time = *tag;
    next = nullptr;
    prev = nullptr;
}

StreamBend::StreamBend(class Time *tag, cmd_t chan, bend_t bendamt)
{
    type = TypedValue::S_BEND;
	bend.bend = bendamt;
	bend.cmd = chan;
    time = *tag;
    next = nullptr;
    prev = nullptr;
}

StreamProg::StreamProg(class Time *tag, cmd_t chan, prg_t program, prg_t bank, prg_t subbank)
{
    type = TypedValue::S_PROG;
	prog.cmd = chan;
	prog.program = program;
	prog.bank = bank;
	prog.subbank = subbank;
    time = *tag;
    next = nullptr;
    prev = nullptr;
}

StreamSysC::StreamSysC(class Time *tag, int8 cmd, int8 data1, int8 data2)
{
    type = TypedValue::S_SYSC;
	sysC.cmd = cmd;
	sysC.data1 = data1;
	sysC.data2 = data2;
    time = *tag;
    next = nullptr;
    prev = nullptr;
}


StreamItem *
StreamNote::Clone()
{
	return new StreamNote(&time, &note);
}

status_t
StreamNote::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<note time=\"%s\" cmd=\"%x\" pitch=\"%d\" duration=\"%d\" velocity=\"%d\"/>\n",
		time.StringValue(), note.cmd, note.pitch, note.duration, note.dynamic);
	return B_OK;
}

StreamItem *
StreamItem::Subsequent(short typ, short cmd, short data)
{
	StreamItem	*p = next;
	while (p) {
		if (p->type == typ) {
			if (typ == TypedValue::S_NOTE) {
				StreamNote	*sub = (StreamNote *) p;
				if (sub->note.cmd == cmd && sub->note.pitch == data) {
					return sub;
				}
			} else if (p->type == TypedValue::S_CTRL) {
				StreamCtrl	*sub = (StreamCtrl *) p;
				if ((cmd < 0 || sub->ctrl.cmd == cmd) && data == sub->ctrl.controller) {
					return sub;
				}
			} else {
				return p;
			}
		} else {
		}
		p = p->next;
	}
	return nullptr;
}

dur_t
StreamItem::Duration()
{
	if (type == TypedValue::S_NOTE) {
		StreamNote	*me = (StreamNote *)this;
		if (me->note.duration == INFINITE_TICKS) {
			StreamNote	*sub= (StreamNote *)Subsequent(TypedValue::S_NOTE, MIDI_NOTE_OFF, me->note.pitch);
			if (sub) {
				return (dur_t)(sub->time.ticks-time.ticks);
			}
		}
		return me->note.duration;
	}
	return 0;
}

/*
 * StreamMesg
 */
#ifdef QUA_V_APP_HANDLER
void *
StreamMesg::operator new(size_t sz)
{
	return FreeMesg.Alloc();
}

StreamMesg::StreamMesg(class Time *t, BMessage *mp)
{
    type = S_MESSAGE;
    mesg = mp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamMesg::operator delete(void *q)
{
    FreeMesg.Dealloc(q);
}

StreamItem *
StreamMesg::Clone()
{
	return new StreamMesg(&time, mesg?new BMessage((*mesg)):nullptr);
}


status_t
StreamMesg::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<mesg time=\"%s\">\n",
		time.StringValue(), note.pitch, note.duration, note.dynamic, note.cmd);
	return B_OK;
}

StreamMesg::~StreamMesg()
{
	if (mesg)
		delete mesg;
}

#endif

/*
 * StreamValue
 */
void *
StreamValue::operator new(size_t sz)
{
	return FreeValue.Alloc();
}

StreamValue::StreamValue(class Time *t, class TypedValue *vp)
{
    type = TypedValue::S_VALUE;
    if (vp) value.Set(vp);
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamValue::operator delete(void *q)
{
    FreeValue.Dealloc(q);
}


StreamItem *
StreamValue::Clone()
{
	return new StreamValue(&time, &value);
}

status_t
StreamValue::SaveSnapshot(FILE *fp)
{
	string nm = findTypeName(value.type);
	if (nm.size()) {
		fprintf(fp, "<value time=\"%s\" valType=\"%s\" valValue=\"%s\"/>\n", time.StringValue(), nm.c_str(), value.StringValue());
	}
	
	return B_OK;
}

/*
 * StreamCtrl
 */
void *
StreamCtrl::operator new(size_t sz)
{
	return FreeCtrl.Alloc();
}

StreamCtrl::StreamCtrl(class Time *t, class Ctrl *cp)
{
    type = TypedValue::S_CTRL;
	if (cp) ctrl = *cp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamCtrl::operator delete(void *q)
{
    FreeCtrl.Dealloc(q);
}


StreamItem *
StreamCtrl::Clone()
{
	return new StreamCtrl(&time, &ctrl);
}


status_t
StreamCtrl::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<ctrl time=\"%s\" cmd=\"%x\" controller=\"%d\" amount=\"%d\"/>\n",
		time.StringValue(), ctrl.cmd, ctrl.controller, ctrl.amount);
	return B_OK;
}


/*
 * StreamSysX
 */
void *
StreamSysX::operator new(size_t sz)
{
	return FreeSysX.Alloc();
}

StreamSysX::StreamSysX(class Time *t, class SysX *cp)
{
    type = TypedValue::S_SYSX;
    if (cp) {
		sysX.length = cp->length;
		sysX.data = new char[sysX.length];
		memcpy(sysX.data, cp->data, sysX.length);
    }
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamSysX::operator delete(void *q)
{
	FreeSysX.Dealloc(q);
}


StreamItem *
StreamSysX::Clone()
{
	return new StreamSysX(&time, &sysX);
}


status_t
StreamSysX::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<sysx time=\"%s\">\n", time.StringValue());
	fprintf(fp, "</sysx>\n", time.StringValue());
	return B_OK;
}

StreamSysX::~StreamSysX()
{
	if (sysX.data)
		delete [] sysX.data;
}


/*
 * StreamSysC
 */
void *
StreamSysC::operator new(size_t sz)
{
	return FreeSysC.Alloc();
}

StreamSysC::StreamSysC(class Time *t, class SysC *cp)
{
    type = TypedValue::S_SYSC;
	if (cp) sysC = *cp;
	time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamSysC::operator delete(void *q)
{
    FreeSysC.Dealloc(q);
}


StreamItem *
StreamSysC::Clone()
{
	return new StreamSysC(&time, &sysC);
}


status_t
StreamSysC::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<sysc time=\"%s\" cmd=\"%x\"  data1=\"%x\"  data2=\"%x\"/>\n",
		time.StringValue(), sysC.cmd, sysC.data1, sysC.data2);
	return B_OK;
}

/*
 * StreamBend
 */
void *
StreamBend::operator new(size_t sz)
{
	return FreeBend.Alloc();
}

StreamBend::StreamBend(class Time *t, class Bend *cp)
{
    type = TypedValue::S_BEND;
	if (cp) bend = *cp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamBend::operator delete(void *q)
{
    FreeBend.Dealloc(q);
}


StreamItem *
StreamBend::Clone()
{
	return new StreamBend(&time, &bend);
}


status_t
StreamBend::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<bend time=\"%s\" cmd=\"%x\" amount=\"%d\"/>\n",
		time.StringValue(), bend.cmd, bend.bend);
	return B_OK;
}


/*
 * StreamProg
 */
void *
StreamProg::operator new(size_t sz)
{
	return FreeProg.Alloc();
}

StreamProg::StreamProg(class Time *t, class Prog *cp)
{
    type = TypedValue::S_PROG;
	if (cp) prog = *cp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamProg::operator delete(void *q)
{
    FreeProg.Dealloc(q);
}


StreamItem *
StreamProg::Clone()
{
	return new StreamProg(&time, &prog);
}


status_t
StreamProg::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<prog time=\"%s\" cmd=\"%x\" program=\"%d\"  bank=\"%d\"  subBank=\"%d\"/>\n",
		time.StringValue(), prog.cmd, prog.program, prog.bank, prog.subbank);
	return B_OK;
}

/*
 * StreamLogEntry
 */
void *
StreamLogEntry::operator new(size_t sz)
{
	return FreeLogEntry.Alloc();
}

StreamLogEntry::StreamLogEntry(class Time *t, class LogEntry *cp)
{
    type = TypedValue::S_LOG_ENTRY;
	if (cp) logEntry = *cp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamLogEntry::operator delete(void *q)
{
    FreeLogEntry.Dealloc(q);
}


StreamItem *
StreamLogEntry::Clone()
{
	return new StreamLogEntry(&time, &logEntry);
}


status_t
StreamLogEntry::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<logentry time=\"%s\"/>\n",
		time.StringValue());
	return B_OK;
}

/*
 * StreamJoy
 */
void *
StreamJoy::operator new(size_t sz)
{
	return FreeJoy.Alloc();
}

StreamJoy::StreamJoy(class Time *t, class QuaJoy *cp)
{
    type = TypedValue::S_JOY;
	if (cp)
		joy = *cp;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

StreamJoy::StreamJoy(class Time *t, uchar stick, uchar which, uchar type)
{
    type = TypedValue::S_JOY;
	joy.stick = stick;
	joy.which = which;
	joy.type = type;
    time = *t;
    next = nullptr;
    prev = nullptr;
}

void
StreamJoy::operator delete(void *q)
{
    FreeJoy.Dealloc(q);
}


StreamItem *
StreamJoy::Clone()
{
	return new StreamJoy(&time, &joy);
}


status_t
StreamJoy::SaveSnapshot(FILE *fp)
{
	switch (joy.type) {
		case QUA_JOY_AXIS:
			fprintf(fp, "<joy time=\"%s\" stick=\"%d\"  joyType=\"%d\"  joyWhich=\"%d\"  axis=\"%g\"/>\n",
				time.StringValue(), joy.stick, joy.type, joy.which, joy.value.axis);
			break;
		case QUA_JOY_BUTTON:
			fprintf(fp, "<joy time=\"%s\" stick=\"%d\"  joyType=\"%d\"  joyWhich=\"%d\"  button=\"%d\"/>\n",
				time.StringValue(), joy.stick, joy.type, joy.which, joy.value.button);
			break;
		case QUA_JOY_HAT:
			fprintf(fp, "<joy time=\"%s\" stick=\"%d\"  joyType=\"%d\"  joyWhich=\"%d\"  hat=\"%d\"/>\n",
				time.StringValue(), joy.stick, joy.type, joy.which, joy.value.hat);
			break;
	}
	return B_OK;
}
