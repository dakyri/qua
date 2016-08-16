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
#define NEW_STREAM_ITEM
#ifdef NEW_STREAM_ITEM

/*
 * simple thread safe cache for allocating objects of type C ...
 * in general, this version won't quite be useful on subclasses of C ... only allocations < sizeof C are pulled from cache
 * but I'm confident that the various StreamItem subclasses won't themselves be subclassed
 */
template <typename C>
class StreamItemCache {
public:
	StreamItemCache<typename C>() {
		freeList = nullptr;
	}
	~StreamItemCache<typename C>() {
		C* p = freeList;
		void* q;
		while (p != nullptr) {
			q = p;
			p = static_cast<C*>(p->next);
			free(q);
		}
	}
	void *alloc(size_t s) {
		void *p;
		mutex.lock();
		if (freeList != nullptr && s <= sizeof(C)) {
			p = (void *)freeList;
			freeList = static_cast<C *>(freeList->next);
		} else {
			p = malloc(s/*sizeof(C)*/);
//			cerr << "malloc " << s << " of " << sizeof(C) << ", " << sizeof(StreamItem) << ", " << sizeof(Note) << ", " << sizeof(Time) << ", " << sizeof(StreamItem*) << endl;
		}
		mutex.unlock();
		return p;
	}
	void dealloc(void *p) {
		mutex.lock();
		C *q = static_cast<C *>(p);
		q->next = freeList;
		freeList = q;
		mutex.unlock();
	}
	std::mutex mutex;
	C *freeList;
};

/**
 * StreamItem
 * base class for the various Items, provides default implementation of virtuals and general purpose functions
 * provides an override delete operator so caching on subclasses works ok
 * streams are in general intermixed with various types of stream item
 */
class StreamItem
{
public:
	StreamItem(Time &_time, short _type = TypedValue::S_UNKNOWN);
	virtual ~StreamItem() {}

	Time time;
	StreamItem *next, *prev;
	short type;

	StreamItem *Subsequent(short typ, short cmd, short data);
	//    StreamItem			*Previous(short typ, short cmd, short data); // currently next is ignored?
	virtual dur_t Duration();
	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual bool setAttributes(AttributeList &attribs);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out) = 0;

	//	void *operator new(size_t);
	void operator delete(void *);
};

/*
 * StreamItemImpl<C>
 * base for the actual implementations of StreamItem ... maintains a cache for items of type C which are used for re-allocations
 */
template <typename C>
class StreamItemImpl : public StreamItem {
public:
	StreamItemImpl<C>(Time &_time, short _type = TypedValue::S_UNKNOWN)
		: StreamItem(_time, _type)
	{	}
protected:
	static StreamItemCache<C> cache;
public:
	void *operator new(size_t s) {
		return cache.alloc(s);
	}
	void operator delete(void *q) {
		cache.dealloc(q);
	}
};
template <typename C>
StreamItemCache<C> StreamItemImpl<C>::cache;

/**
 * stream item for note objects
 */
class StreamNote : public StreamItemImpl<StreamNote>
{
public:
	StreamNote(Time &tag, Note &tp);
	StreamNote(Time &tag, cmd_t cmd, pitch_t pitch, vel_t vel, dur_t dur);

	virtual dur_t Duration() override;
	virtual bool SetMidiParams(int8, int8, int8, bool) override;
	virtual bool setAttributes(AttributeList &attribs) override;
	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	Note note;
	AttributeList attributes;
};

/**
* stream item for ctrl objects
*/
class StreamCtrl : public StreamItemImpl<StreamCtrl>
{
public:
	StreamCtrl(class Time &tag, Ctrl &cp);
	StreamCtrl(class Time &tag, cmd_t cmd, ctrl_t ct, amt_t amt);

	virtual bool SetMidiParams(int8, int8, int8, bool) override;
	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	Ctrl ctrl;
};

/**
* stream item for bend objects
*/
class StreamBend : public StreamItemImpl<StreamBend>
{
public:
	StreamBend(Time &tag, class Bend &cp);
	StreamBend(Time &tag, cmd_t, bend_t);

	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	Bend bend;
};

/**
* stream item for prog objects
*/
class StreamProg : public StreamItemImpl<StreamProg>
{
public:
	StreamProg(Time &tag, Prog &cp);
	StreamProg(Time &tag, cmd_t, prg_t, prg_t, prg_t);

	virtual bool SetMidiParams(int8, int8, int8, bool) override;
	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	Prog prog;
};

/**
* stream item for sys common objects
*/
class StreamSysC : public StreamItemImpl<StreamSysC>
{
public:
	StreamSysC(Time &tag, SysC &cp);
	StreamSysC(Time &tag, int8, int8, int8);

	virtual bool SetMidiParams(int8, int8, int8, bool) override;
	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	SysC sysC;
};


/**
* stream item for note objects
*/
class StreamMesg : public StreamItemImpl<StreamMesg>
{
public:
	StreamMesg(Time &tag, OSCMessage *mp);
	~StreamMesg();

	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	OSCMessage *mesg;
};

/**
* stream item for value objects
*/
class StreamValue : public StreamItemImpl<StreamValue>
{
public:
	StreamValue(Time &tag, TypedValue &vp);

	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	TypedValue value;
};


/**
* stream item for sys x objects
*/
class StreamSysX : public StreamItemImpl<StreamSysX>
{
public:
	StreamSysX(Time &tag, SysX &cp);
	~StreamSysX();

	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	SysX sysX;
};


/**
* stream item for JOY objects
*/
class StreamJoy : public StreamItemImpl<StreamJoy>
{
public:
	StreamJoy(Time &tag, class QuaJoy &cp);
	StreamJoy(Time &tag, uchar st, uchar wh, uchar cmd);

	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	QuaJoy joy;
};


/**
* stream item for log objects
*/
class StreamLogEntry : public StreamItemImpl<StreamLogEntry>
{
public:
	StreamLogEntry(Time &tag, class LogEntry *cp);

	virtual StreamItem *Clone() override;
	virtual status_t SaveSnapshot(ostream &out) override;

	LogEntry logEntry;
};

#else
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
	virtual status_t SaveSnapshot(ostream &out)=0;

//	void *operator new(size_t);
	void operator delete(void *);
};

class StreamItemCache
{
public:
	StreamItemCache(size_t sz);
	~StreamItemCache();
	void				*Alloc();
	void				Dealloc(void *q);
	std::mutex			mutex;
	StreamItem			*free;
	size_t				size;
};

class StreamNote: public StreamItem
{
public:
	StreamNote(Time &tag, Note &tp);
	StreamNote(Time &tag, cmd_t cmd, pitch_t pitch, vel_t vel, dur_t dur);

	void *operator new(size_t);
	void operator delete(void *);

	virtual dur_t Duration();
	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual bool setAttributes(AttributeList &attribs);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
    
    Note note;
	AttributeList attributes;
};

class StreamCtrl: public StreamItem
{
public:
	StreamCtrl(class Time &tag, Ctrl &cp);
	StreamCtrl(class Time &tag, cmd_t cmd, ctrl_t ct, amt_t amt);

	void *operator new(size_t);
	void operator delete(void *);

	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	Ctrl ctrl;
};

class StreamBend: public StreamItem
{
public:
	StreamBend(Time &tag, class Bend &cp);
	StreamBend(Time &tag, cmd_t, bend_t);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	Bend bend;
};

class StreamProg: public StreamItem
{
public:
	StreamProg(Time &tag, Prog &cp);
	StreamProg(Time &tag, cmd_t, prg_t, prg_t, prg_t);

	void *operator new(size_t);
	void operator delete(void *);

	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	Prog prog;
};

class StreamSysC: public StreamItem
{
public:
	StreamSysC(Time &tag, SysC &cp);
	StreamSysC(Time &tag, int8, int8, int8);

	void *operator new(size_t);
	void operator delete(void *);

	virtual bool SetMidiParams(int8, int8, int8, bool);
	virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	SysC sysC;
};


class StreamMesg: public StreamItem
{
public:
	StreamMesg(Time &tag, OSCMessage *mp);
	~StreamMesg();
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	OSCMessage *mesg;
};

class StreamValue: public StreamItem
{
public:
	StreamValue(Time &tag, TypedValue &vp);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	TypedValue value;
};

class StreamSysX: public StreamItem
{
public:
	StreamSysX(Time &tag, SysX &cp);
	~StreamSysX();
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	SysX sysX;
};

class StreamJoy: public StreamItem
{
public:
	StreamJoy(Time &tag, class QuaJoy &cp);
	StreamJoy(Time &tag, uchar st, uchar wh, uchar cmd);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	QuaJoy joy;
};

class StreamLogEntry: public StreamItem
{
public:
	StreamLogEntry(Time &tag, class LogEntry *cp);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(ostream &out);
	
 	LogEntry logEntry;
};
#endif

/**
 * constants for alternate save modes for streams. some streams, input in scripts we'd like to keep as ascii scripts
 * recorded midi, perhaps not
 */
enum {
	STR_FMT_RAW = 0,
	STR_FMT_TEXT = 1
};

struct IXMLDOMElement;

/**
 * Stream
 */
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

	status_t		SaveSnapshot(ostream &out);

	status_t		LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t		LoadSnapshotChildren(tinyxml2::XMLElement *element, std::vector<std::string> &textFrags);


	Time			Duration();
	Time			EndTime();
	void			Coalesce();
	void			SetGranularity(ulong gr);
	void			PrintStream(FILE *fp);
	
	status_t		Load(istream&in, Qua *, short fmt=STR_FMT_RAW);
	status_t		Save(ostream &out, Qua *, short fmt=STR_FMT_RAW);
	status_t		SetValue(Block *b);

	void forEach(function<void(StreamItem *i)> op);
	bool satisfies(function<bool(StreamItem *i)> op);

    StreamItem *head, *tail;
    int nItems;
};

#endif

