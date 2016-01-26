#ifndef _APP_CTL
#define _APP_CTL

#include <stdio.h>

class Application;
class Block;
class Method;
class QuaControllerBridge;
class Qua;
class StabEnt;
class LocusList;
class Stream;

#include "Time.h"
#include "Metric.h"
#include "Schedulable.h"

class Application: public Schedulable
{
public:
						Application(char *nm, BPath *path, Qua *Q, char *mimet);
						~Application();
						
	long				SetAppPath(BPath *path);
						
	virtual bool		Init();
	
	virtual void		Cue(Time &t);
	virtual status_t	Wake(Instance *i);
	virtual	status_t	Sleep(Instance *i);
	virtual status_t	Run(Instance *i);
	virtual status_t	Recv(Stream &stream);
	virtual status_t	QuaStop();
	virtual status_t	QuaStart();
	virtual status_t	QuaRecord();
	
	virtual void		Notification(Block *b, Instance *i, QuasiStack *S);
	
	void				OutMess(Stream *S);
	void				OutMess(StreamItem *S);
	void				GetStreamItems(Stream *S);
	bool				HasStreamItems();
	status_t			Save(FILE *fp, short indent);
	
	virtual bool		ExtraneousMessage(class ArrangerObject *a, BMessage *inMsg);
	virtual bool		ExtraneousMouse(class ArrangerObject *a, BPoint where, ulong, ulong);
	virtual void		DrawExtraneous(class ArrangerObject *a, BRect r);
	bool				MouseDownLogItem(class ArrangerObject *a, BPoint where, StreamItem *p);
	void				DrawLogItem(class ArrangerObject *a, class StreamItem *p);

	team_id				appTeamId;
	team_info			appInfo;

	BPath				appFilePath;
	BMimeType			appMimeType;

	BMessenger			txConnect;
};

#endif
