#include "qua_version.h"


#include <stdio.h>
#include <algorithm>


#include "StdDefs.h"

#include "Stream.h"
#include "Block.h"
#include "Parse.h"
#include "MidiDefs.h"
#include "OSCMessage.h"

#include "Dictionary.h"

#ifndef NEW_STREAM_ITEM
StreamItemCache		FreeNote(sizeof(StreamNote));
StreamItemCache		FreeMesg(sizeof(StreamMesg));
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
	return p;
}

void
StreamItemCache::Dealloc(void *p)
{
	mutex.lock();
	StreamItem	*q = (StreamItem *)p;
    q->next = free;
    free = q;
	mutex.unlock();
}
#endif
#include <iostream>
StreamItem::StreamItem(Time &_time, short _type)
	: type(_type)
	, time(_time)
	, next(nullptr)
	, prev(nullptr)
{
	cerr << "time in " << time.metric << " v " << _time.metric << endl;
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
	case TypedValue::S_MESSAGE:		delete (StreamMesg *)q; break;
	case TypedValue::S_VALUE: 		delete (StreamValue *)q; break;
	case TypedValue::S_CTRL: 		delete (StreamCtrl *)q; break;
	case TypedValue::S_PROG: 		delete (StreamProg *)q; break;
	case TypedValue::S_SYSX: 		delete (StreamSysX *)q; break;
	case TypedValue::S_SYSC: 		delete (StreamSysC *)q; break;
	case TypedValue::S_LOG_ENTRY: 	delete (StreamLogEntry *)q; break;
	case TypedValue::S_JOY:		 	delete (StreamJoy *)q; break;
	default:
		internalError("delete: Unexpected stream object %d", ((StreamItem *)q)->type);
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

dur_t
StreamItem::Duration()
{
	return 0;
}

StreamItem *
StreamItem::Subsequent(short typ, short cmd, short data)
{
	StreamItem	*p = next;
	while (p) {
		if (p->type == typ) {
			if (typ == TypedValue::S_NOTE) {
				StreamNote	*sub = (StreamNote *)p;
				if (sub->note.cmd == cmd && sub->note.pitch == data) {
					return sub;
				}
			} else if (p->type == TypedValue::S_CTRL) {
				StreamCtrl	*sub = (StreamCtrl *)p;
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

bool
StreamItem::setAttributes(AttributeList &attribs) {
	return false;
}

/*
 * StreamNote
 */
#ifndef NEW_STREAM_ITEM
void *
StreamNote::operator new(size_t sz)
{
	return FreeNote.Alloc();
}

void
StreamNote::operator delete(void *q)
{
	FreeNote.Dealloc(q);
}
#endif
StreamNote::StreamNote(Time &t, Note &tp)
	: StreamItemImpl<StreamNote>(t, TypedValue::S_NOTE) {
	note = tp;
}

StreamNote::StreamNote(Time &tag, cmd_t cmd, pitch_t pitch, vel_t vel, dur_t dur)
	: StreamItemImpl<StreamNote>(tag, TypedValue::S_NOTE) {
	cerr << "tag " << (unsigned)tag.metric << endl;
	note.cmd = cmd;
	note.duration = dur;
	note.dynamic = vel;
	note.pitch = pitch;
}

StreamCtrl::StreamCtrl(Time &tag, cmd_t cmd, ctrl_t ctrlr, amt_t amt)
	: StreamItemImpl<StreamCtrl>(tag, TypedValue::S_NOTE) {
	ctrl.cmd = cmd;
	ctrl.controller = ctrlr;
	ctrl.amount = amt;
}

StreamBend::StreamBend(Time &tag, cmd_t chan, bend_t bendamt)
	: StreamItemImpl<StreamBend>(tag, TypedValue::S_BEND) {
	bend.bend = bendamt;
	bend.cmd = chan;
}


StreamProg::StreamProg(Time &tag, cmd_t chan, prg_t program, prg_t bank, prg_t subbank)
	: StreamItemImpl<StreamProg>(tag, TypedValue::S_PROG) {
	prog.cmd = chan;
	prog.program = program;
	prog.bank = bank;
	prog.subbank = subbank;
}

StreamSysC::StreamSysC(Time &tag, int8 cmd, int8 data1, int8 data2)
	: StreamItemImpl<StreamSysC>(tag, TypedValue::S_SYSC) {
	sysC.cmd = cmd;
	sysC.data1 = data1;
	sysC.data2 = data2;
}


/*
* todo xxxx now that the stream item holds the Attributes field, this clone is not going to clone those
* will that be an issue? currently debating the value of this call anyway
*/
StreamItem *
StreamNote::Clone()
{
	return new StreamNote(time, note);
}

status_t
StreamNote::SaveSnapshot(ostream &out)
{
	out << "<note time=\"" << time.StringValue()
		<< "\" cmd=\"" << note.cmd << "\" pitch=\"" << note.pitch << "\" duration=\"" << note.duration << "\" velocity=\"" << note.dynamic << "\"/>" << endl;
	return B_OK;
}
dur_t
StreamNote::Duration()
{
	if (note.duration == INFINITE_TICKS) {
		StreamNote	*sub = (StreamNote *)Subsequent(TypedValue::S_NOTE, MIDI_NOTE_OFF, note.pitch);
		if (sub) {
			return (dur_t)(sub->time.ticks - time.ticks);
		}
	}
	return note.duration;
}

bool
StreamNote::SetMidiParams(int8 cmd, int8 data1, int8 data2, bool force)
{
	note.pitch = data1;
	note.dynamic = data2;
	note.cmd = cmd;
	return true;
}


bool
StreamNote::setAttributes(AttributeList &attribs) {
	attributes = attribs;
	return true;
}

/*
 * StreamMesg
 */
#ifndef NEW_STREAM_ITEM
void *
StreamMesg::operator new(size_t sz)
{
	return FreeMesg.Alloc();
}

void
StreamMesg::operator delete(void *q)
{
	FreeMesg.Dealloc(q);
}

#endif
StreamMesg::StreamMesg(Time &tag, OSCMessage *mp)
	: StreamItemImpl<StreamMesg>(tag, TypedValue::S_MESSAGE) {
    mesg = mp;
}

StreamItem *
StreamMesg::Clone()
{
	return new StreamMesg(time, mesg?new OSCMessage(*mesg):nullptr);
}


status_t
StreamMesg::SaveSnapshot(ostream &out)
{
	out << "<mesg time=\""<< time.StringValue() <<"\">"<<endl;
	return B_OK;
}

StreamMesg::~StreamMesg()
{
	if (mesg)
		delete mesg;
}

/*
 * StreamValue
 */
#ifndef NEW_STREAM_ITEM
void *
StreamValue::operator new(size_t sz)
{
	return FreeValue.Alloc();
}

void
StreamValue::operator delete(void *q)
{
	FreeValue.Dealloc(q);
}
#endif

StreamValue::StreamValue(Time &tag, TypedValue &vp)
	: StreamItemImpl<StreamValue>(tag, TypedValue::S_VALUE) {
    value = vp;
}

StreamItem *
StreamValue::Clone()
{
	return new StreamValue(time, value);
}

status_t
StreamValue::SaveSnapshot(ostream &out)
{
	string nm = findTypeName(value.type);
	if (nm.size()) {
		out <<  "<value time=\""<< time.StringValue() <<"\" valType=\"" << nm << "\" valValue=\"" << value.StringValue() << "\"/>"<< endl;
	}
	
	return B_OK;
}

/*
 * StreamCtrl
 */
#ifndef NEW_STREAM_ITEM
void *
StreamCtrl::operator new(size_t sz)
{
	return FreeCtrl.Alloc();
}

void
StreamCtrl::operator delete(void *q)
{
	FreeCtrl.Dealloc(q);
}

#endif
StreamCtrl::StreamCtrl(Time &tag, Ctrl &cp)
	: StreamItemImpl<StreamCtrl>(tag, TypedValue::S_CTRL) {
	ctrl = cp;
}


StreamItem *
StreamCtrl::Clone()
{
	return new StreamCtrl(time, ctrl);
}


status_t
StreamCtrl::SaveSnapshot(ostream & out)
{
	out << "<ctrl time=\"" << time.StringValue()
		<< "\" cmd=\"" << ctrl.cmd << "\" controller=\"" << ctrl.controller << "\" amount=\"" << ctrl.amount << "\"/>" << endl;
	return B_OK;
}

bool
StreamCtrl::SetMidiParams(int8 cmd, int8 data1, int8 data2, bool force)
{
	ctrl.controller = data1;
	ctrl.amount = data2;
	ctrl.cmd = cmd;
	return true;
}

/*
 * StreamSysX
 * this owns the sysx data ... we can't be certain of anything else because our SysX wrapper is in a union
 */
#ifndef NEW_STREAM_ITEM
void *
StreamSysX::operator new(size_t sz)
{
	return FreeSysX.Alloc();
}

void
StreamSysX::operator delete(void *q)
{
	FreeSysX.Dealloc(q);
}
#endif

StreamSysX::StreamSysX(Time &tag, SysX &cp)
	: StreamItemImpl<StreamSysX>(tag, TypedValue::S_SYSX) {
	sysX = cp;
}

StreamItem *
StreamSysX::Clone()
{
	return new StreamSysX(time, sysX.clone());
}


status_t
StreamSysX::SaveSnapshot(ostream &out)
{
	out << "<sysx time=\""<< time.StringValue() <<"\">"<<endl;
	out <<"</sysx>"<<endl;
	return B_OK;
}

/*
 */
StreamSysX::~StreamSysX()
{
	sysX.clear();
}


/*
 * StreamSysC
 */
#ifndef NEW_STREAM_ITEM
void *
StreamSysC::operator new(size_t sz)
{
	return FreeSysC.Alloc();
}
void
StreamSysC::operator delete(void *q)
{
	FreeSysC.Dealloc(q);
}
#endif

StreamSysC::StreamSysC(Time &tag, SysC &cp)
	: StreamItemImpl<StreamSysC>(tag, TypedValue::S_SYSC) {
	sysC = cp;
}


StreamItem *
StreamSysC::Clone()
{
	return new StreamSysC(time, sysC);
}


status_t
StreamSysC::SaveSnapshot(ostream &out)
{
	out << "<sysc time=\"" << time.StringValue() << "\" cmd=\"" << sysC.cmd << "\"  data1=\"" << sysC.data1 << "\"  data2=\"" << sysC.data2 << "\"/>" << endl;
	return B_OK;
}

bool
StreamSysC::SetMidiParams(int8 cmd, int8 data1, int8 data2, bool force)
{
	sysC.data1 = data1;
	sysC.data2 = data2;
	sysC.cmd = cmd;
	return true;
}

/*
 * StreamBend
 */
#ifndef NEW_STREAM_ITEM
void *
StreamBend::operator new(size_t sz)
{
	return FreeBend.Alloc();
}

void
StreamBend::operator delete(void *q)
{
	FreeBend.Dealloc(q);
}
#endif
StreamBend::StreamBend(Time &tag, Bend &cp)
	: StreamItemImpl<StreamBend>(tag, TypedValue::S_BEND) {
	bend = cp;

}


StreamItem *
StreamBend::Clone()
{
	return new StreamBend(time, bend);
}


status_t
StreamBend::SaveSnapshot(ostream &out)
{
	out << "<bend time=\"" << time.StringValue() << "\" cmd=\"" << bend.cmd << "\" amount=\"" << bend.bend << "\"/>" << endl;
	return B_OK;
}


/*
 * StreamProg
 */
#ifndef NEW_STREAM_ITEM
void *
StreamProg::operator new(size_t sz)
{
	return FreeProg.Alloc();
}

void
StreamProg::operator delete(void *q)
{
	FreeProg.Dealloc(q);
}
#endif
StreamProg::StreamProg(Time &tag, Prog &cp)
	: StreamItemImpl<StreamProg>(tag, TypedValue::S_PROG) {
	prog = cp;

}



StreamItem *
StreamProg::Clone()
{
	return new StreamProg(time, prog);
}


status_t
StreamProg::SaveSnapshot(ostream &out)
{
	out << "<prog time=\"" << time.StringValue() << "\" cmd=\"" << prog.cmd << "\" program=\"" << prog.program << "\"  bank=\"" << prog.bank << "\"  subBank=\"" << prog.subbank << "\"/>" << endl;
	return B_OK;
}

bool
StreamProg::SetMidiParams(int8 cmd, int8 data1, int8 data2, bool force)
{
	prog.bank = data2;
	prog.program = data1;
	prog.cmd = cmd;
	return true;
}
/*
 * StreamLogEntry
 */
#ifndef NEW_STREAM_ITEM
void *
StreamLogEntry::operator new(size_t sz)
{
	return FreeLogEntry.Alloc();
}
void
StreamLogEntry::operator delete(void *q)
{
	FreeLogEntry.Dealloc(q);
}
#endif

StreamLogEntry::StreamLogEntry(Time &tag, class LogEntry *cp)
	: StreamItemImpl<StreamLogEntry>(tag, TypedValue::S_LOG_ENTRY) {
	if (cp) logEntry = *cp;
}


StreamItem *
StreamLogEntry::Clone()
{
	return new StreamLogEntry(time, &logEntry);
}


status_t
StreamLogEntry::SaveSnapshot(ostream &out)
{
	out << "<logentry time=\"" << time.StringValue() << "\"/>" << endl;
	return B_OK;
}

/*
 * StreamJoy
 */
#ifndef NEW_STREAM_ITEM
void *
StreamJoy::operator new(size_t sz)
{
	return FreeJoy.Alloc();
}

void
StreamJoy::operator delete(void *q)
{
	FreeJoy.Dealloc(q);
}
#endif
StreamJoy::StreamJoy(Time &tag, QuaJoy &cp)
	: StreamItemImpl<StreamJoy>(tag, TypedValue::S_JOY) {
	joy = cp;
}

StreamJoy::StreamJoy(Time &tag, uchar stick, uchar which, uchar type)
	: StreamItemImpl<StreamJoy>(tag, TypedValue::S_JOY) {
	joy.stick = stick;
	joy.which = which;
	joy.type = type;
}


StreamItem *
StreamJoy::Clone()
{
	return new StreamJoy(time, joy);
}


status_t
StreamJoy::SaveSnapshot(ostream &out)
{
	switch (joy.type) {
		case QUA_JOY_AXIS:
			out << "<joy time=\""<< time.StringValue() <<"\" stick=\"" << joy.stick << "\"  joyType=\"" << joy.type << "\"  joyWhich=\"" << joy.which << "\"  axis=\"" << joy.value.axis << "\"/>\n" << endl;
			break;
		case QUA_JOY_BUTTON:
			out << "<joy time=\"" << time.StringValue() << "\" stick=\"" << joy.stick << "\"  joyType=\"" << joy.type << "\"  joyWhich=\"" << joy.which << "\"  button=\"" << joy.value.button << "\"/>\n" << endl;
			break;
		case QUA_JOY_HAT:
			out << "<joy time=\"" << time.StringValue() << "\" stick=\"" << joy.stick << "\"  joyType=\"" << joy.type << "\"  joyWhich=\"" << joy.which << "\"  hat=\"" << joy.value.hat << "\"/>" << endl;
			break;
	}
	return B_OK;
}
