#ifndef _STREAM_H
#define _STREAM_H

#include <stdio.h>

#include "qua_version.h"

#include <vector>
#include <string>
#include <mutex>

#include "Note.h"
#include "QuaTime.h"
#include "Sym.h"
#include "QuaJoy.h"
#include "Log.h"

class ResultValue;
class List;
class RWLock;

class StreamItem
{
public:
	virtual ~StreamItem() {}

	short				type;
    Time				time;
    StreamItem			*next,
    					*prev;

    StreamItem			*Subsequent(short typ, short cmd, short data);
//    StreamItem			*Previous(short typ, short cmd, short data); // currently next is ignored?
    dur_t				Duration();
	bool				SetMidiParams(int8, int8, int8, bool);
	
	virtual StreamItem	*Clone();
	virtual status_t	SaveSnapshot(FILE *fp)=0;

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
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
    
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
	virtual status_t SaveSnapshot(FILE *fp);
	
 	Bend bend;
};

class StreamProg: public StreamItem
{
public:
	StreamProg(Time &tag, Prog &cp);
	StreamProg(Time &tag, cmd_t, prg_t, prg_t, prg_t);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
 	Prog prog;
};

class StreamSysC: public StreamItem
{
public:
	StreamSysC(Time &tag, SysC &cp);
	StreamSysC(Time &tag, int8, int8, int8);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
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
	virtual status_t SaveSnapshot(FILE *fp);
	
 	OSCMessage *mesg;
};

class StreamValue: public StreamItem
{
public:
	StreamValue(Time &tag, TypedValue &vp);
	void *operator new(size_t);
	void operator delete(void *);
    virtual StreamItem *Clone();
	virtual status_t SaveSnapshot(FILE *fp);
	
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
	virtual status_t SaveSnapshot(FILE *fp);
	
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
	virtual status_t SaveSnapshot(FILE *fp);
	
 	QuaJoy joy;
};

class StreamLogEntry: public StreamItem
{
public:
	StreamLogEntry(Time &tag, class LogEntry *cp);
	void *operator new(size_t);
	void operator delete(void *);
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
//					~Stream();
	void			SetProtection(RWLock *);

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
	void			AddToStream(Stream *S, Time &tag=Time::zero);
	StreamMesg		*AddToStream(OSCMessage *mp, Time &tag);
	void			InsertStream(Stream *B, Time *tag=nullptr);
	void			AppendStream(Stream *B);
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

    StreamItem		*head,
					*tail;
    int				nItems;
    RWLock			*condom;
};

#endif

