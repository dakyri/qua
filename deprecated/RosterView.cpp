#include "qua_version.h"

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "include/RosterView.h"
#include "StdDefs.h"
#include "Colors.h"
#include "include/QuaPort.h"

RosterView::RosterView(BRect rec, char *nm):
	ObjectViewContainer(rec,
						nm,
						B_FOLLOW_ALL,
						B_WILL_DRAW|B_PULSE_NEEDED)
{
}

RosterView::~RosterView()
{
}

void
RosterView::AttachedToWindow()
{
}

void
RosterView::MessageReceived(BMessage *inMsg)
{
	switch (inMsg->what) {

	case B_MEDIA_NODE_CREATED: {	/* "media_node_id" (multiple items) */
		PortObject		*po;
		if (inMsg->HasPointer("port object")) {
			inMsg->FindPointer("port object", (void **)&po);
			fprintf(stderr, "adding port object %x\n", po);
			lastRB = po->Frame().RightBottom();
			po->SetDisplayMode(OBJECT_DISPLAY_SMALL);
			AddChild(po);
		}
		break;
	}
		break;
	case B_MEDIA_NODE_DELETED: {	/* "media_node_id" (multiple items) */
		PortObject		*po;
		if (inMsg->HasPointer("port object")) {
			inMsg->FindPointer("port object", (void **)&po);
			fprintf(stderr, "removing port object %x\n", po);
//			po->RemoveQuaNexion(nullptr);
			RemoveChild(po);
			Invalidate();
//			delete po;
		}
		break;
	}
	case B_MEDIA_CONNECTION_MADE: {	/* "output", "input", "format" */
		PortObject		*ip=nullptr,*op=nullptr;
		inMsg->FindPointer("input object", (void **)&ip);
		inMsg->FindPointer("output object", (void **)&op);
		QuaInsert		*ii=nullptr, *oi=nullptr;
		inMsg->FindPointer("input insert", (void **)&ii);
		inMsg->FindPointer("output insert", (void **)&oi);
		short	nChannels;
		if (inMsg->HasInt32("channels")) {
			nChannels = inMsg->FindInt32("channels");
		} else {
			nChannels = 1;
		}
		rgb_color c = black;
		if (inMsg->HasInt32("device type")) {
			switch(inMsg->FindInt32("device type")) {
				case QUA_DEV_JOYSTICK: c = purple; break;
				case QUA_DEV_MIDI: c = red; break;
				case QUA_DEV_AUDIO: c = blue; break;
				case QUA_DEV_PARALLEL: c = green; break;
			}
		}
		if (ip && op && ii && oi) {
			fprintf(stderr, "qua nexion made\n");
			AddQuaNexion(op, oi, ip, ii, c, QN_UNIDIRECT, nChannels);
		}
		break;
	}
	case B_MEDIA_CONNECTION_BROKEN:	{ /* "source", "destination" */
		QuaInsert		*ip=nullptr,*op=nullptr;
		inMsg->FindPointer("input insert", (void **)&ip);
		inMsg->FindPointer("output insert", (void **)&op);
		if (ip && op) {
			RemoveQuaNexion(op, ip);
		} else if (ip) {
			RemoveQuaNexion(ip);
		} else if (op) {
			RemoveQuaNexion(op);
		}
		break;
	}
		
	case B_MEDIA_BUFFER_CREATED:		/* "clone_info" -- handled by BMediaRoster */
		break;
	case B_MEDIA_BUFFER_DELETED:		/* "media_buffer_id" -- handled by BMediaRoster */
		break;
	case B_MEDIA_TRANSPORT_STATE:	/* "state", "location", "realtime" */
		break;
	case B_MEDIA_PARAMETER_CHANGED:	/* N "node", "parameter" */
		break;
	case B_MEDIA_FORMAT_CHANGED:		/* N "source", "destination", "format" */
		break;
	case B_MEDIA_WEB_CHANGED:		/* N "node" */
		break;
	case B_MEDIA_DEFAULT_CHANGED:	/* "default", "node" -- handled by BMediaRoster */
		break;
	case B_MEDIA_NEW_PARAMETER_VALUE:	/* N "node", "parameter", "when", "value" */
		break;
	case B_MEDIA_NODE_STOPPED:	/* N "node", "when" */
		break;
	case B_MEDIA_WILDCARD:		/* used to match any notification in Start/StopWatching */
		break; 

	default:
		ObjectViewContainer::MessageReceived(inMsg);
	}	


}



void
RosterView::AddChild(BView *v, BView *s)
{
	v->MoveTo(atX);
	BView::AddChild(v, s);
	atX += BPoint(0, v->Bounds().bottom + 2);
	BRect r = Frame();
	if (atX.y >= r.bottom - 5) {
		atX.x += 60;
		atX.y = 3;
	}
}


QuaNexion *
RosterView::AddQuaNexion(
				ObjectView *o,
				QuaInsert *oi,
				ObjectView *i,
				QuaInsert *ii,
				rgb_color c, uchar ty, uchar w)
{
	if (o && i && ii && oi)
		return ObjectViewContainer::AddQuaNexion(o, oi, i, ii, c, ty, w);
	else
		return nullptr;
}
