#ifndef _STREAM_H
#define _STREAM_H

#include <stdio.h>
#include <functional>

using namespace std;

#include "qua_version.h"

#include <vector>
#include <string>
#include <mutex>

#include "Note.h"
#include "QuaTime.h"
#include "Sym.h"
#include "QuaJoy.h"
#include "Log.h"
#include "BaseVal.h"
#include "Attributes.h"

class ResultValue;
class List;

template <typename C>
class StreamItemCache {
public:
	StreamItemCache<typename C>() {
		free = nullptr;
	}
	~StreamItemCache<typename C>() {
	}
	void *Alloc() {
		void *p;
		mutex.lock();
		if (free != nullptr) {
			p = (void *)free;
			free = free->next;
		}
		else {
			p = malloc(sizeof(C));
		}
		mutex.unlock();
		return p;
	}
	void Dealloc(void *q) {
		mutex.lock();
		C *q = (C *)p;
		q->next = free;
		free = q;
		mutex.unlock();
	}
	std::mutex mutex;
	C *free;
};

class StreamItem
{
public:
	StreamItem(Time &_time, short _type=TypedValue::S_UNKNOWN);
	virtual ~StreamItem() {}

	short				type;
    Time				time;
    StreamItem			*next,
    					*prev;

    StreamItem *Subsequent(short typ, short cmd, short data);
//    StreamItem			*Previous(short typ, short cmd, short data); // currently next is ignored?
    virtual dur_t Duration();
	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual bool setAttributes(AttributeList &attribs);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp)=0;

//	void *operator new(size_t);
	void operator delete(void *);
};

template <typename C>
class StreamItemImpl : public StreamItem {
public:
	StreamItemImpl<C>(Time &_time, short _type = TypedValue::S_UNKNOWN)
		: StreamItem(time, _type)
	{ }
protected:
	static StreamItemCache<C> cache;
public:
	void *operator new(size_t) {
		return cache.Alloc();
	}
	void operator delete(void *q) {
		cache.Dealloc(q);
	}
};
template <typename C>
StreamItemCache<C> StreamItemImpl<C>::cache;

class StreamNote: public StreamItemImpl<StreamNote>
{
public:
	StreamNote(Time &tag, Note &tp);
	StreamNote(Time &tag, cmd_t cmd, pitch_t pitch, vel_t vel, dur_t dur);

	virtual dur_t Duration();
	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual bool setAttributes(AttributeList &attribs);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
    
    Note note;
	AttributeList attributes;
};

class StreamCtrl: public StreamItemImpl<StreamCtrl>
{
public:
	StreamCtrl(class Time &tag, Ctrl &cp);
	StreamCtrl(class Time &tag, cmd_t cmd, ctrl_t ct, amt_t amt);

	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	Ctrl ctrl;
};

class StreamBend: public StreamItemImpl<StreamBend>
{
public:
	StreamBend(Time &tag, class Bend &cp);
	StreamBend(Time &tag, cmd_t, bend_t);

    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	Bend bend;
};

class StreamProg: public StreamItemImpl<StreamProg>
{
public:
	StreamProg(Time &tag, Prog &cp);
	StreamProg(Time &tag, cmd_t, prg_t, prg_t, prg_t);

	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	Prog prog;
};

class StreamSysC: public StreamItemImpl<StreamSysC>
{
public:
	StreamSysC(Time &tag, SysC &cp);
	StreamSysC(Time &tag, int8, int8, int8);

	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	SysC sysC;
};


class StreamMesg: public StreamItemImpl<StreamMesg>
{
public:
	StreamMesg(Time &tag, OSCMessage *mp);
	~StreamMesg();

    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	OSCMessage *mesg;
};

class StreamValue: public StreamItemImpl<StreamValue>
{
public:
	StreamValue(Time &tag, TypedValue &vp);

    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	TypedValue value;
};

class StreamSysX: public StreamItemImpl<StreamSysX>
{
public:
	StreamSysX(Time &tag, SysX &cp);
	~StreamSysX();

    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	SysX sysX;
};

class StreamJoy: public StreamItemImpl<StreamJoy>
{
public:
	StreamJoy(Time &tag, class QuaJoy &cp);
	StreamJoy(Time &tag, uchar st, uchar wh, uchar cmd);

    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	QuaJoy joy;
};

class StreamLogEntry: public StreamItemImpl<StreamLogEntry>
{
public:
	StreamLogEntry(Time &tag, class LogEntry *cp);

    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	LogEntry logEntry;
};

enum {
	STR_FMT_RAW = 0,
	STR_FMT_TEXT = 1
};

struct IXMLDOMElement;

class Stream
{
public:
	Stream();
	Stream(Stream *S);
	~Stream();

	StreamJoy		*AddJoyAxisToStream(uchar stick, uchar which, float v,  Time &tag);
	StreamJoy		*AddJoyHatToStream(uchar stick, uchar which, uchar v,  Time &tag);
	StreamJoy		*AddJoyButtonToStream(uchar stick, uchar which, bool v,  Time &tag);
	StreamJoy		*AddToStream(QuaJoy &sp, Time &tag);

	void			AddStreamItems(StreamItem *sp, short r, Time &tag);
	StreamItem		*AddToStream(StreamItem *sp, Time &tag);
	StreamNote		*AddToStream(Note &tp, Time &tag);
	StreamSysX		*AddToStream(SysX &sp, Time &tag);
	StreamCtrl		*AddToStream(Ctrl &sp, Time &tag);
	StreamBend		*AddToStream(Bend &sp, Time &tag);
	StreamProg		*AddToStream(Prog &sp, Time &tag);
	StreamSysC		*AddToStream(SysC &sp, Time &tag);
	StreamValue		*AddToStream(TypedValue &sp, Time &tag);
	StreamLogEntry	*AddToStream(LogEntry *sp, Time &tag);
	void			AddToStream(Stream &S, Time &tag=Time::zero);
	StreamMesg		*AddToStream(OSCMessage *mp, Time &tag);
	void			InsertStream(Stream &B, Time &tag= Time::zero);
	void			AppendStream(Stream &B);
	void			AppendItem(StreamItem *I);
	void			InsertItem(StreamItem *I);
	void			ClearStream();
	int				DeleteItem(TypedValueList *L);
	int				DeleteRefItems(StreamItem *SI);
    int				DeleteItem(StreamItem *SI);
	int				InsertItem(Time &time, TypedValue &val);
	StreamItem		*IndexTo(Time &t);
	
	void			ItemRefsAt(Time &t1, Time &t2, class TypedValueList &l);
	void			ItemRefsBetween(Time &t1, Time &t2, class TypedValueList &l);
	
	status_t		Split(cmd_t type, Block *condition, Stacker *instance, StabEnt *,
							QuasiStack *stack, Stream *elseStream);
	status_t		Merge(Stream *elseStream);
	
	StreamNote		*FindNearestNote(Time &time, cmd_t, pitch_t);
//	StreamItem		*FindItemPrecedingTime(Time &time, int8 typ, int8 cmd, int8 data1);
	StreamItem		*FindItemAtTime(short, Time &time, int8 typ, int8 cmd, int8 data1, StreamItem*prec=nullptr);
	StreamItem		*FindItemAtTime(short, Time &time, TypedValue &v, StreamItem*prec=nullptr);
	bool			DeleteItemsMatching(int8 typ, int8 cmd, int8 data1);
	void			DuplicateController(ctrl_t from, ctrl_t to);
	void			Agglomerate(StreamItem *, std::vector<StreamItem*> &);
	void			MoveNoteAgglomerate(std::vector<StreamItem*> &, Time &t, pitch_t);

	void			SetDuration(StreamItem *, dur_t);
	void			ModifyItemTime(StreamItem *I, Time &tag);

	status_t		SaveSnapshot(FILE *fp);

	status_t		LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t		LoadSnapshotChildren(tinyxml2::XMLElement *element, std::vector<std::string> &textFrags);


	Time			Duration();
	Time			EndTime();
	void			Coalesce();
	void			SetGranularity(ulong gr);
	void			PrintStream(FILE *fp);
	
	status_t		Load(FILE *, Qua *, short fmt=STR_FMT_RAW);
	status_t		Save(FILE *, Qua *, short fmt=STR_FMT_RAW);
	status_t		SetValue(Block *b);

	void forEach(function<void(StreamItem *i)> op);
	bool satisfies(function<bool(StreamItem *i)> op);

    StreamItem *head, *tail;
    int nItems;
};

#endif

