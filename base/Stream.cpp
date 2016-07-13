#include "qua_version.h"

#include <stdio.h>

#include <vector>
#include <string>

#include "tinyxml2.h"
#include "StdDefs.h"
#include "Stream.h"
#include "Block.h"
#include "Qua.h"
#include "Parse.h"
#include "MidiDefs.h"
#include "OSCMessage.h"

#include "Dictionary.h"

flag debug_stream=0;
Stream::Stream()
{
	head = tail = nullptr;
	nItems = 0;
	condom = nullptr;
}

Stream::Stream(Stream *S)
{
	StreamItem	*p = S->head, *q = nullptr,
				**pp = &head;
	*pp = nullptr;
	nItems = 0;
	while (p != nullptr) {
		q = p->Clone();
		*pp = q;
		pp = &q->next;
		nItems++;
		p = p->next;
	}
	tail = q;
	condom = nullptr;
}

status_t
Stream::SaveSnapshot(FILE *fp)
{
	fprintf(fp, "<stream>\n");
	for (StreamItem *p = head; p!= nullptr; p=p->next) {
		p->SaveSnapshot(fp);
	}
	fprintf(fp, "</stream>\n");
	return B_OK;
}


status_t
Stream::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	bool		hasCommand = false;
	bool		hasValueType = false;
	cmd_t		cmd = 0;
	pitch_t		pitch = 36;
	ctrl_t		controller = 0;
	vel_t		velocity = 80;
	uint8		amount = 80;
	dur_t		duration = 6;
	bend_t		bendamt = 0;
	prg_t		program = 0;
	bank_t		bank = 0;
	bank_t		subBank = 0;
	base_type_t	vType=TypedValue::S_INT;
	sysc_data_t	data1=0;
	sysc_data_t	data2=0;
	std::string vValueStr;
	j_axis_t	jAxis=0;
	j_button_t	jButton=false;
	j_hat_t		jHat=QUA_JOY_HAT_CENTRED;
	j_stick_t	jStick=0;
	j_which_t	jWhich=0;
	j_type_t	jType=QUA_JOY_AXIS;
	Time		atTime = Time::zero;
	int			encoding=0;

	const char *timeVal = element->Attribute("time");
	const char *cmdVal = element->Attribute("cmd");
	const char *pitchVal = element->Attribute("pitch");
	const char *durationVal = element->Attribute("duration");
	const char *velocityVal = element->Attribute("velocity");
	const char *amountVal = element->Attribute("amount");
	const char *controllerVal = element->Attribute("controller");
	const char *programVal = element->Attribute("program");
	const char *bankVal = element->Attribute("bank");
	const char *subBankVal = element->Attribute("subBank");
	const char *data1Val = element->Attribute("data1");
	const char *data2Val = element->Attribute("data2");
	const char *stickVal = element->Attribute("stick");
	const char *joyWhichVal = element->Attribute("which");
	const char *joyTypeVal = element->Attribute("type");
	const char *axisVal = element->Attribute("axis");
	const char *buttonVal = element->Attribute("button");
	const char *hatVal = element->Attribute("hat");
	const char *valTypeVal = element->Attribute("valType");
	const char *valVal = element->Attribute("valValue");
	const char *encodingStr = element->Attribute("encoding");
	
	if (timeVal != nullptr) {
		atTime.Set(timeVal);
	}
	if (cmdVal != nullptr) {
		cmd = atoi(cmdVal);
		hasCommand = true;
	}
	if (timeVal != nullptr) {
		pitch = atoi(pitchVal);
	}
	if (timeVal != nullptr) {
		duration = atoi(durationVal);
	}
	if (velocityVal != nullptr) {
		velocity = atoi(velocityVal);
	}
	if (controllerVal != nullptr) {
		controller = atoi(controllerVal);
	}
	if (amountVal != nullptr) {
		amount = atoi(amountVal);
	}
	if (programVal != nullptr) {
		program = atoi(programVal);
	}
	if (bankVal != nullptr) {
		bank = atoi(bankVal);
	}
	if (subBankVal != nullptr) {
		subBank = atoi(subBankVal);
	}
	if (data1Val != nullptr) {
		data1 = atoi(data1Val);
	}
	if (data2Val != nullptr) {
		data2 = atoi(data2Val);
	}
	if (stickVal != nullptr) {
		jStick = atoi(stickVal);
	}
	if (joyWhichVal != nullptr) {
		jWhich = atoi(joyWhichVal);
	}
	if (joyTypeVal != nullptr) {
		jType = atoi(joyTypeVal);
	}
	if (axisVal != nullptr) {
		jAxis = atoi(axisVal);
	}
	if (buttonVal != nullptr) {
		jButton = atoi(buttonVal);
	}
	if (hatVal != nullptr) {
		jHat = atoi(hatVal);
	}
	if (valTypeVal != nullptr) {
		auto ci =findType(valTypeVal);
		if (ci != TypedValue::S_UNKNOWN) {
			vType = ci;
			hasValueType = true;
		}
	}
	if (valVal != nullptr) {
		vValueStr = valVal;
	}
	if (encodingStr != nullptr) {
		if (encoding == 0) {
			if (strcmp(encodingStr, "base64") == 0) {
				encoding = 1;
			}
		}
	}

	const char *namep = element->Value();
	std::string namestr = namep;
	std::vector<std::string> textFragments;

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element, textFragments);
	} else if (namestr == "stream") {
		LoadSnapshotChildren(element, textFragments);
	} else if (namestr == "note") {
		Note	note;
		note.Set(pitch, velocity, duration, hasCommand?cmd:MIDI_NOTE_ON);
		AddToStream(&note, &atTime);
	} else if (namestr == "ctrl") {
		Ctrl	ctrl;
		ctrl.Set(controller, amount, hasCommand?cmd:MIDI_CTRL);
		AddToStream(&ctrl, &atTime);
	} else if (namestr == "bend") {
		Bend	bend;
		bend.Set(bendamt, hasCommand?cmd:MIDI_BEND);
		AddToStream(&bend, &atTime);
	} else if (namestr == "mesg") {
		Note	note;
		AddToStream(&note, &atTime);
	} else if (namestr == "prog") {
		Prog	prog;
		prog.Set(program, bank, subBank, hasCommand?cmd:MIDI_PROG);
		AddToStream(&prog, &atTime);
	} else if (namestr == "sysc") {
		if (hasCommand) {
			SysC	sysc;
			sysc.Set(cmd, data1, data2);
			AddToStream(&sysc, &atTime);
		}
	} else if (namestr == "sysx") {
		LoadSnapshotChildren(element, textFragments);
		short	cnt=textFragments.size(); // TODO this looked wrong in original version don't quite get what i was on or if i was wrong
	} else if (namestr == "joy") {
		QuaJoy	joy;
		AddToStream(&joy, &atTime);
	} else if (namestr == "value") {
		if (hasValueType) {
			TypedValue	val;
			AddToStream(&val, &atTime);
		}
	}

	return B_OK;
}

status_t
Stream::LoadSnapshotChildren(tinyxml2::XMLElement *element, std::vector<std::string> &textFragments)
{
	tinyxml2::XMLNode*childNode = element->FirstChildElement();
	while (childNode != nullptr) {
		tinyxml2::XMLText *textNode;
		tinyxml2::XMLElement *elementNode;
		if ((textNode = childNode->ToText()) != nullptr) {
			textFragments.push_back(textNode->Value());
		} else if ((elementNode = childNode->ToElement()) != nullptr) {
			if (LoadSnapshotElement(elementNode) == B_ERROR) {
				return B_ERROR;
			}
		}

		childNode = childNode->NextSibling();
	}

	return B_OK;
}



void
Stream::SetProtection(RWLock *durex)
{
	condom = durex;
}

// remove infinite durations, and midi note offs, so
// tones in a stream have if possible a sane, fized
// duration
void
Stream::Coalesce()
{
	StreamItem	*p, **qp;
	p = head;
	qp = &head;

	if (debug_stream) {
		fprintf(stderr, "in coalesce\n");
		PrintStream(stderr);
	}
	while (p != nullptr) {
		if (p->type == TypedValue::S_NOTE) {
			StreamNote *pt = (StreamNote *)p;
			if (pt->note.cmd == MIDI_NOTE_OFF) {
				StreamItem		*z;
				StreamNote		*zt;
				
				for (z = head; z != p; z = z->next) {
					if (z->type == TypedValue::S_NOTE) {
						zt = (StreamNote *)z;
						if ( (zt->note.cmd == MIDI_NOTE_ON ||
							  zt->note.cmd == MIDI_KEY_PRESS)
							 && (zt->note.duration == INFINITE_TICKS)
							 && zt->note.pitch == pt->note.pitch) {
							zt->note.duration = (float)(
								(pt->time - zt->time).ticks);
						}
					}								
				}
				*qp = p->next;
				nItems--;
				delete pt;
				p = *qp;
			} else {
				qp = &p->next;
				p = p->next;
			}
		} else {
			qp = &p->next;
			p = p->next;
		}
	}
	
	if (debug_stream) {
		fprintf(stderr, "after coalesce\n");
		PrintStream(stderr);
	}
}

void
Stream::PrintStream(FILE *fp)
{
	StreamItem	*p;
	
	fprintf(fp, "contents of stream %x (%d items)\n", (unsigned)this, nItems);
	for (p=head; p!=nullptr; p=p->next) {
		switch (p->type) {
		case TypedValue::S_NOTE: {
			StreamNote	*pt = (StreamNote *)p;
			fprintf(fp, "note %x cmd %x pitch %d dyn %d dur %g @ %d\n",
				(unsigned)p, pt->note.cmd, pt->note.pitch, pt->note.dynamic, pt->note.duration, p->time.ticks);
			break;
		}
		case TypedValue::S_JOY: {
			StreamJoy	*pt = (StreamJoy *)p;
			break;
		}
		case TypedValue::S_LOG_ENTRY: {
			StreamLogEntry *pt = (StreamLogEntry *)p;
			fprintf(fp, "log entry %x cmd %d\n", (unsigned)p, pt->logEntry.type);
			break;
		}
		default:
			fprintf(fp, "item %x, type %d @ %d\n", (unsigned) p, p->type, p->time.ticks);
		}
	}
}	
	

void
Stream::SetGranularity(ulong gr)
{
}

Time
Stream::Duration()
{
	Time	t(0); // if list is empty return zero in default metric
	
	if (head != nullptr && tail != nullptr) {
		t = (tail->time - head->time); // should now have the right metric
		if (tail->type == TypedValue::S_NOTE) {
			t.ticks +=	((StreamNote *)tail)->note.duration;
		}
	}
	
	return t;
}

Time
Stream::EndTime()
{
	Time	t(0);
	
	if (head != nullptr && tail != nullptr) {
		t = tail->time + (tail->type == TypedValue::S_NOTE?
						(int32)((StreamNote *)tail)->note.duration:0);
	}
	
	return t;
}

void
Stream::ClearStream()
{
    StreamItem	*p, *q;

	if (nItems > 0) {
	    for (p=head; p!=nullptr; ) {
			if (debug_stream)
			    fprintf(stderr, "disposing %d %x\n", nItems, (unsigned)p);
			nItems --;
			q = p->next;
			delete p;
			p = q;
	    }
	    head = tail = nullptr;
	}
}

//Stream::~Stream()
//{
//	ClearStream();
//}

StreamNote *
Stream::AddToStream(Note *tp, Time *tag)
{
	StreamNote *i=new StreamNote(tag, tp);
    AppendItem(i);
	return i;
}

StreamCtrl *
Stream::AddToStream(Ctrl *tp, Time *tag)
{
	StreamCtrl *i=new StreamCtrl(tag, tp);
    AppendItem(i);
	return i;
}

StreamBend *
Stream::AddToStream(Bend *tp, Time *tag)
{
	StreamBend *i=new StreamBend(tag, tp);
    AppendItem(i);
	return i;
}

StreamSysX *
Stream::AddToStream(SysX *tp, Time *tag)
{
	StreamSysX *i=new StreamSysX(tag, tp);
    AppendItem(i);
	return i;
}

StreamSysC *
Stream::AddToStream(SysC *tp, Time *tag)
{
	StreamSysC *i = new StreamSysC(tag, tp);
    AppendItem(i);
	return i;
}

StreamProg *
Stream::AddToStream(Prog *tp, Time *tag)
{
	StreamProg * i = new StreamProg(tag, tp);
	AppendItem(i);
	return i;
}

StreamValue *
Stream::AddToStream(class TypedValue *tp, Time *tag)
{
	StreamValue *i= new StreamValue(tag, tp);
	AppendItem(i);
	return i;
}

StreamJoy *
Stream::AddJoyAxisToStream(uchar stick, uchar which, float v,  Time *tag)
{
	StreamJoy *i= new StreamJoy(tag, stick, which, QUA_JOY_AXIS);
	i->joy.value.axis = v;
	AppendItem(i);
	return i;
}

StreamJoy *
Stream::AddJoyHatToStream(uchar stick, uchar which, uchar v,  Time *tag)
{
	StreamJoy *i= new StreamJoy(tag, stick, which, QUA_JOY_HAT);
	i->joy.value.hat = v;
	AppendItem(i);
	return i;
}

StreamJoy *
Stream::AddJoyButtonToStream(uchar stick, uchar which, bool v,  Time *tag)
{
	StreamJoy *i= new StreamJoy(tag, stick, which, QUA_JOY_BUTTON);
	i->joy.value.button = v;
	AppendItem(i);
	return i;
}

StreamJoy *
Stream::AddToStream(class QuaJoy *tp, Time *tag)
{
	StreamJoy *i= new StreamJoy(tag, tp);
	AppendItem(i);
	return i;
}

StreamMesg *
Stream::AddToStream(OSCMessage *mp, Time *tag)
{
	StreamMesg *i = new StreamMesg(tag, mp);
	AppendItem(i);
	return i;
}

StreamLogEntry *
Stream::AddToStream(LogEntry *sp, Time *tag)
{
	StreamLogEntry *i = new StreamLogEntry(tag, sp);
	InsertItem(i);
	return i;
}

StreamItem *
Stream::AddToStream(StreamItem *p, Time *tag)
{
    switch (p->type) {
	case TypedValue::S_NOTE: {
		return AddToStream(&((StreamNote*)p)->note,tag);
	}
	
	case TypedValue::S_JOY: {
		return AddToStream(&((StreamJoy*)p)->joy,tag);
	}

	case TypedValue::S_MESSAGE: return AddToStream(new OSCMessage(*((StreamMesg*)p)->mesg),tag);
	case TypedValue::S_PROG: return AddToStream(&((StreamProg*)p)->prog,tag);
	case TypedValue::S_BEND: return AddToStream(&((StreamBend*)p)->bend,tag);
	case TypedValue::S_SYSX: return AddToStream(&((StreamSysX*)p)->sysX,tag);
	case TypedValue::S_SYSC: return AddToStream(&((StreamSysC*)p)->sysC,tag);
	case TypedValue::S_CTRL: return AddToStream(&((StreamCtrl*)p)->ctrl,tag);
	case TypedValue::S_STREAM_ITEM: {
		internalError("AddToStream: unexpected stream_item stream item");
		return nullptr;
	}
	case TypedValue::S_VALUE: {
		StreamValue *q = (StreamValue *) p;
		if (q->value.type == TypedValue::S_STREAM_ITEM) {
			StreamItem	*i = q->value.StreamItemValue();
			StreamItem	*j = i->Clone();
			AddStreamItems(j, 1, nullptr);
			return j;
		} else {
			return AddToStream(&q->value,	tag);
		}
	}
	
	default: {
		internalError("unknown stream_item type");
	}}
	return nullptr;
}

void
Stream::AddToStream(Stream *S, Time *offt)
{
	StreamItem	*p;
    for (p = S->head; p != nullptr; p=p->next) {
    	Time	timeat = p->time;
    	if (offt) timeat = timeat - *offt;
    	if (debug_stream)
    		fprintf(stderr, "chan:p = %x %d\n", (unsigned) p, p->type);
		AddToStream(p, &timeat);

    }
	if (debug_stream)
		PrintStream(stderr);
}

void
Stream::AppendItem(StreamItem *l)
{
    if (nItems < MAX_STREAM) {
		if (debug_stream)
			fprintf(stderr, "adding l = %x\n", (unsigned)l);
		if (head == nullptr) {
		    head = l;
		} else {
		    tail->next = l;
		}
		tail = l;
		nItems++;
		if (debug_stream)
		    fprintf(stderr, "add to stream %x %d\n", (unsigned)l, nItems);
    } else {
		internalError("Stream buffer overflow in stream %x, %d\n", (long)this, nItems);
		delete l;
    }
}

void
Stream::InsertItem(StreamItem *l)
{
    if (nItems < MAX_STREAM) {
		if (debug_stream)
			fprintf(stderr, "inserting l = %x\n", (unsigned)l);
		StreamItem *p = head, **pp =&head;
		while (p != nullptr) {
			if (p->time > l->time) {
				break;
			}
			pp = &p->next;
			p = p->next;
		}
		l->next = p;
		*pp = l;
		if (p == nullptr) {
		    tail = l;
		}
		
		nItems++;
		if (debug_stream)
		    fprintf(stderr, "insert in stream %x %d\n", (unsigned)l, nItems);
    } else {
		internalError("Stream buffer overflow in stream %x, %d\n", (long)this, nItems);
		delete l;
    }
}

void
Stream::ModifyItemTime(StreamItem *l, Time *tag)
{
	StreamItem *p = head, **pp =&head, *prev=nullptr,
			*insert_after=nullptr, *insert_before = nullptr,
			**take_from=nullptr;
	while (p != nullptr) {
		if (p == l) {
			take_from = pp;
		}
		if (p->time > *tag && insert_before == nullptr) {
			insert_after = prev;
			insert_before = p;
		}
		pp = &p->next;
		prev = p;
		p = p->next;
	}
	l->time = *tag;
	if (take_from == nullptr) {
		internalError("modifying demented stream...not");
		return;
	}
	if (insert_before == nullptr) {
		insert_after = prev;
		insert_before = nullptr;
	}
	if (insert_after != l && insert_before != l) {
		*take_from = l->next;
		if (insert_after == nullptr) head = l;
		else insert_after->next = l;
		l->next = insert_before;
		if (l->next == nullptr)
			tail = l;
	}
}

void
Stream::AddStreamItems(StreamItem *sp, short r, Time *tag)
{

    if (nItems < MAX_STREAM) {
		if (debug_stream)
			fprintf(stderr, "adding list = %x\n", (unsigned)sp);
		if (sp) {
			if (head == nullptr) {
			    head = sp;
			} else {
			    tail->next = sp;
			}
			
			Time startt = sp->time;
			while (sp) {
				if (debug_stream)
				    fprintf(stderr, "add to stream %x %d\n", (unsigned)sp, sp->type);
				tail = sp;
				if (sp->type == TypedValue::S_NOTE) {
					((StreamNote*)sp)->note.duration /= r;
				}
				if (tag)
					sp->time = ((sp->time - startt) / r) + *tag;
				sp = sp->next;
				nItems++;
			}
		}
    } else {
		internalError("Stream buffer overflow in stream %x, %d\n", (long)this, nItems);
    }
}

void
Stream::ItemRefsAt(Time after, Time time, TypedValueList &found)
{
	StreamItem *p = head, *q=nullptr, *l=nullptr, *t=nullptr;

	found.Clear();
	while (p!=nullptr && p->time <= after)
		p = p->next;

	while (p!=nullptr) {
		if (p->time == time) {
			found.AddItem((void *) p, TypedValue::S_STREAM_ITEM);
		} else if (p->time > time) {
			break;
		}
		p = p->next;
	}
	return;
}

void
Stream::ItemRefsBetween(Time after, Time time, TypedValueList &found)
{
	StreamItem *p = head, *q=nullptr, *l=nullptr, *t=nullptr;

	found.Clear();
	while (p!=nullptr && p->time <= after)
		p = p->next;

	while (p!=nullptr) {
		if (p->time > time) {
			break;
		}
		found.AddItem((void *) p, TypedValue::S_STREAM_ITEM);
		p = p->next;
	}
	return;
}

StreamItem *
Stream::IndexTo(Time &t)
{
	StreamItem *p = head;
	while (p!=nullptr && p->time < t)
		p = p->next;

	return p;
}


/*
 * InsertStream:
 *   inserts stream B into stream A, clearing stream B
 */
void
Stream::InsertStream(Stream *B, Time *tag)
{
    if (B == nullptr) {
		internalError("fatal in Insert!\n");
		exit(1);
    }
    if (B->nItems > 0) {
#ifdef SHORT_IS
        B->tail->next = A->head;
        A->head = B->head;
        A->nItems += B->nItems;

        B->head = B->tail = nullptr;
        B->nItems = 0;
#else
		StreamItem		*headorig, **lastinsertref,
						 **lastb, *tailinsert;
		StreamItem		*p, *q;
	
		lastb = &B->head;
		lastinsertref = &head;
		tailinsert = nullptr;
		B->tail = nullptr;
		headorig = head;
		p = B->head;
		
		while (p != nullptr) {
		    if (*tag >= p->time) {
		   		q = p->next;
		   		
		   		*lastb = q;
		
				p->next = headorig;
				*lastinsertref = p;
				lastinsertref = &p->next;
				tailinsert = p;
	
		    	nItems++;
		    	B->nItems--;
				p = q;
		    } else {	/* relink to B */
				*lastb = p;
				lastb = &p->next;
				B->tail = p;
				p = p->next;
		    }
		}
		if (debug_stream)
			fprintf(stderr, "is %x %x\n", (unsigned)head, (unsigned)tail);
		if (tail == nullptr)
			tail = tailinsert;

		*lastb = nullptr;
#endif
    }  
    if (debug_stream)
    	fprintf(stderr, "insert stream %d %d\n", nItems, B->nItems);
}

/*
 * AppendStream:
 *   inserts stream B into stream A, clearing stream B
 */
void
Stream::AppendStream(Stream *B)
{
    if (B == nullptr) {
		internalError("fatal in Append! null stream!\n");
		exit(1);
    }

    if (B->nItems > 0) {
		if (debug_stream)
			fprintf(stderr, "head %x tail %x len %d BHead %x Btail %x Blen %d\n", 
				(unsigned)head, (unsigned)tail, nItems, (unsigned)B->head, (unsigned)B->tail, B->nItems);
        if (nItems > 0) {
            tail->next = B->head;
        } else {
	    	head = B->head;
        }

        tail = B->tail;
        nItems += B->nItems;

        B->head = B->tail = nullptr;
        B->nItems = 0;
    }
    if (debug_stream)
    	fprintf(stderr, "append stream %d %d\n", nItems, B->nItems);
}

int
Stream::DeleteItem(TypedValueList *L)
{
	StreamItem	**qq = &head, *q=head;
	int			cnt = 0;

	TypedValueListItem	*lip = L->head;
	while (lip) {
		if (lip->value.type == TypedValue::S_STREAM_ITEM) {
			StreamItem	*p = lip->value.StreamItemValue();
			while (q) {
				if (*qq == p) {
					*qq = p->next;
					if (p->next == nullptr) {
						tail = q;
					}
					delete p;
					cnt ++;
					break;
				}
				qq = &q->next;
				q = q->next;
			}
		}
		lip = lip->next;
	}

	if (head == nullptr)
		tail = nullptr;
	return cnt;
}


int
Stream::DeleteRefItems(StreamItem *SI)
{
	StreamItem	**qq = &head, *q=head;
	int			cnt = 0;
	
	for (StreamItem *p=SI; p != nullptr; p = p->next) {
		if (p->type == TypedValue::S_VALUE &&
				((StreamValue*)p)->value.type == TypedValue::S_STREAM_ITEM) {
			StreamItem *todel = ((StreamValue*)p)->value.StreamItemValue();
			
			while (q) {
				if (*qq == todel) {
					*qq = todel->next;
					if (p->next == nullptr) {
						tail = q;
					}
					delete todel;
					cnt ++;
					break;
				}
				qq = &q->next;
				q = q->next;
			}
		}
	}
	if (head == nullptr)
		tail = nullptr;
	return cnt;
}

int
Stream::DeleteItem(StreamItem *i)
{

	StreamItem	**qq = &head, *q=head, *p=nullptr;
	while (q) {
		if (q == i) {
			*qq = i->next;
			if (i->next = nullptr)
				tail = p;
			delete i;
			return 1;
		}
		qq = &q->next;
		p = q;
		q = q->next;
	}
	return 0;
}

void
Stream::Agglomerate(StreamItem *p, std::vector<StreamItem*> &agglomerate)
{
	if (p) {
		if (p->type == TypedValue::S_NOTE) {
			StreamNote	*ip = (StreamNote*)p;
	
			while (p!=nullptr) {
				if (p->type == TypedValue::S_NOTE) {
					StreamNote	*nip = (StreamNote*)p;
					if (nip->note.pitch == ip->note.pitch) {
						agglomerate.push_back(p);
						if (nip->note.cmd == MIDI_NOTE_OFF) {
							break;
						}
					}
				}
				p = p->next;
			}
		} else {
			agglomerate.push_back(p);
		}
	}
}

void
Stream::SetDuration(StreamItem *ip, dur_t newdur)
{
	if (ip->type == TypedValue::S_NOTE) {
		StreamNote	*np = (StreamNote *)ip;
		if (newdur == INFINITE_TICKS) {
			np->note.duration = newdur;
		} else {
			if (np->note.duration == INFINITE_TICKS) {
				StreamNote	*nop = (StreamNote *)np->Subsequent(
											TypedValue::S_NOTE,
											MIDI_NOTE_OFF,
											np->note.pitch);
				if (nop) {
					Time	newt = np->time.ticks + ((int)newdur);
					ModifyItemTime(nop, &newt);
				} else {
					np->note.duration = newdur;
				}
			} else {
				np->note.duration = newdur;
			}
		}
	}
}

void
Stream::MoveNoteAgglomerate(
				std::vector<StreamItem*> &agglomerate,
				Time &t,
				pitch_t newpitch)
{
	if (agglomerate.size() == 0)
		return;
	Time	delta = t - agglomerate[0]->time;
	for (auto p: agglomerate) {
		Time		newt = p->time + delta;
		ModifyItemTime(p, &newt);
		if (p->type == TypedValue::S_NOTE)
			((StreamNote *)p)->note.pitch = newpitch;
	}
}

StreamNote *
Stream::FindNearestNote(Time &time, cmd_t cmd, pitch_t pitch)
{
//	PrintStream(stderr);
	StreamItem	*p, *fnd = nullptr;
	for (p=head; p!=nullptr; p=p->next) {
//		fprintf(stderr, "t %d fnd %x %d\n", time.ticks, fnd, p->time.ticks); 
//		fprintf(stderr, "too early %d %g %d %g...\n",
//					p->time.metric->Granularity, p->time.metric->Tempo,
//					time.metric->Granularity, time.metric->Tempo
//					);
		if (p->time > time) {
//			fprintf(stderr, "breaking\n");
			break;
		} else if (p->type == TypedValue::S_NOTE) {
			StreamNote	*nip = (StreamNote *)p;
//			fprintf(stderr, "%d %d %x %x\n",
//				nip->note.pitch, pitch, nip->note.cmd, cmd);
			if (nip->note.pitch == pitch && nip->note.cmd == cmd) {
				if (cmd == MIDI_NOTE_OFF) {
					fnd = nip;
				} else if (nip->note.duration == INFINITE_TICKS) {
//					fprintf(stderr, "inf...\n");
					StreamNote	*nop = (StreamNote *)
						nip->Subsequent(TypedValue::S_NOTE, MIDI_NOTE_OFF, pitch);
//					fprintf(stderr, "no %d %d\n", time.ticks, nop->time.ticks);
					if (nop == nullptr || time <= nop->time) {
						fnd = nip;
					}
				} else {
					if (time <= p->time + ((int)nip->note.duration)) {
						fnd = nip;
					}
				}
			}
		}
	}
	return (StreamNote*)fnd;
}
/*
StreamItem *
Stream::FindItemPrecedingTime(Time &time, int8 typ, int8 cmd, int8 data1)
{
	StreamItem	*p, *fnd = nullptr;
	for (p=head; p!=nullptr; p=p->next) {
		if (p->time > time) {
			break;
		} else if (p->type == typ) {
			switch (typ) {
				case TypedValue::S_CTRL: {
					if (((StreamCtrl *)p)->ctrl.controller == data1) {
						fnd = p;
					}
					break;
				}
				case TypedValue::S_NOTE: {
					if (((StreamNote *)p)->note.pitch == data1) {
						fnd = p;
					}
					break;
				}

				case TypedValue::S_PROG:
				case TypedValue::S_SYSC:
				case TypedValue::S_VALUE:
				case TypedValue::S_JOY:
				case TypedValue::S_SYSX:
				case TypedValue::S_BEND: {
					fnd = p;
					break;
				}

				default: {
					fnd = p;
					break;
				}

			}	
		}
	}
	return fnd;
}
*/
/*
 * currently,
 *     rel == -2, for <
 *     rel == -1, for <=
 *     rel == 0, for ==
 *  may need to differentiate between matching  first and last elements of '== t'
 */
StreamItem *
Stream::FindItemAtTime(short rel, Time &time, int8 typ, int8 cmd, int8 data1, StreamItem*after_item)
{
	StreamItem	*p, *fnd = nullptr;
	if (after_item != nullptr) {
		p = after_item->next;
	} else {
		p = head;
	}
	while (p!=nullptr) {
		if ((rel==-2 && p->time >= time) ||p->time > time) {
			break;
		} else if (
			((rel==-1) || (rel==-2) ||
				 (rel==0 && p->time == time)) && p->type == typ) {
			switch (typ) {
				case TypedValue::S_CTRL: {
					if (((StreamCtrl *)p)->ctrl.controller == data1) {
						fnd = p;
					}
					break;
				}
				case TypedValue::S_NOTE: {
					if (((StreamNote *)p)->note.pitch == data1) {
						fnd = p;
					}
					break;
				}

				case TypedValue::S_PROG:
				case TypedValue::S_SYSC:
				case TypedValue::S_VALUE:
				case TypedValue::S_JOY:
				case TypedValue::S_SYSX:
				case TypedValue::S_BEND: {
					fnd = p;
					break;
				}

				default: {
					fnd = p;
					break;
				}

			}	
		}

		p=p->next;
	}
	return fnd;
}
StreamItem *
Stream::FindItemAtTime(short rel, Time &time, TypedValue &val, StreamItem*after_item)
{
	StreamItem	*p, *fnd = nullptr;
	if (after_item != nullptr) {
		p = after_item->next;
	} else {
		p = head;
	}
	while (p!=nullptr) {
		if ((rel==-2 && p->time >= time) ||p->time > time) {
			break;
		} else if (
			((rel==-1) || (rel==-2) ||
				 (rel==0 && p->time == time)) && p->type == TypedValue::S_VALUE) {
			TypedValue	rv = (((StreamValue *)p)->value == val);
			if (rv.BoolValue(nullptr)) {
				fnd = p;
			}
		}

		p=p->next;
	}
	return fnd;
}

bool
Stream::DeleteItemsMatching(int8 typ, int8 cmd, int8 data1)
{
	StreamItem	*p=head;
	StreamItem	*prev=nullptr;
	while (p!=nullptr) {
		StreamItem	*nextp = p->next;
		bool	do_del = false;
		if (p->type == typ) {
			switch (typ) {
				case TypedValue::S_CTRL: {
					StreamCtrl	* pc = ((StreamCtrl *)p);
					if (pc->ctrl.controller == data1) {
						do_del = true;
					}
					break;
				}
				case TypedValue::S_NOTE: {
					StreamNote	* pc = ((StreamNote *)p);
					if (pc->note.pitch == data1) {
						do_del = true;
					}
					break;
				}
				case TypedValue::S_BEND: {
					StreamBend	* pc = ((StreamBend *)p);
					do_del = true;
					break;
				}
				case TypedValue::S_SYSC: {
					StreamSysC	* pc = ((StreamSysC *)p);
					do_del = true;
					break;
				}
				case TypedValue::S_SYSX: {
					StreamSysX	* pc = ((StreamSysX *)p);
					do_del = true;
					break;
				}
				case TypedValue::S_VALUE: {
					StreamValue	* pc = ((StreamValue *)p);
					do_del = true;
					break;
				}
				case TypedValue::S_JOY: {
					StreamJoy	* pc = ((StreamJoy *)p);
					do_del = true;
					break;
				}
				case TypedValue::S_PROG: {
					StreamProg	* pc = ((StreamProg *)p);
					do_del = true;
					break;
				}
			}
		}
		if (do_del) {
			if (prev) {
				prev->next = nextp;
			} else {
				head = nextp;
			}
			if (nextp == nullptr) {
				tail = prev;
			}
			delete p;
		} else {
			prev = p;
		}
		p = nextp;
	}
	return true;
}

void
Stream::DuplicateController(ctrl_t ctl_from, ctrl_t ctl_to)
{
	// delete current destination
	DeleteItemsMatching(TypedValue::S_CTRL, -1, ctl_to);
	StreamItem	*p = head;
	while (p!=nullptr) {
		if (p->type == TypedValue::S_CTRL) {
			StreamCtrl	*pc = (StreamCtrl *)p;
			if (pc->ctrl.controller == ctl_from) {
				StreamCtrl	*npc = new StreamCtrl(&p->time, pc->ctrl.cmd, ctl_to, pc->ctrl.amount);
				npc->next = p->next;
				p->next = npc;
				if (npc->next == nullptr) {
					tail = npc;
				}
			}
		}
		p = p->next;
	}
}

status_t
Stream::Merge(Stream *stream)
{
	if (nItems == 0) {
		head = stream->head;
		tail = stream->tail;
		nItems = stream->nItems;
		return B_OK;
	}
	StreamItem	*p = head, **pp = &head, *q = stream->head;
	for (short i=0; i<stream->nItems; i++) {
		while (p && q->time > p->time) {
			pp = &p->next;
			p = p->next;
		}
		*pp = q;
		if (p==nullptr) {
			tail = stream->tail;
			break;
		}
		StreamItem *nq = q->next;
		q->next = p;
		pp = &q->next;
		q = nq;
	}

	nItems += stream->nItems;
	stream->nItems = 0;
	stream->head = stream->tail = nullptr;
	
	return B_OK;
}

status_t
Stream::Split(cmd_t type, Block *condition, Stacker *stacker,
				StabEnt *stackCtxt,
				QuasiStack *stack, Stream *elseStream)
{
	StreamItem	*p = head, **pp = &head, **qp = &elseStream->head;
	short nits = nItems;
	tail = nullptr;
	for (short i=0; i<nits; i++) {
		bool keep;
		if (type == -1 || p->type == type) {
			keep = true;
			if (condition) {
				ResultValue v = EvaluateExpression(
								condition,
				 				p,
				 				stacker, stackCtxt, stack);
				keep = (!v.Blocked() && v.BoolValue(nullptr));
			}
		} else {
			keep = false;
		}
				
		if (!keep) {	// remove and add to else stream
			*pp = p->next;	// remove
			nItems--;
			
			elseStream->nItems++;
			*qp = elseStream->tail = p;
			qp = &p->next;
			p->next = nullptr;
			
			p = *pp;
		} else {
			pp = &p->next;	// keep this one & skip to next item
			tail = p;
			p = p->next;
		}
	}
	return B_OK;
}

int
Stream::InsertItem(Time *time, TypedValue *val)
{
	StreamItem *p, *q = nullptr, *r;
	
	if (val->type == TypedValue::S_NOTE) {
		r = new StreamNote(time, val->NoteValue());
	} else if (val->type == TypedValue::S_MESSAGE) {
		r = new StreamMesg(time, val->MessageValue());
	} else
		return 0;
		
	for (p=head; p!=nullptr; p=p->next) {
		if (p->time > *time)
			break;
		q = p;
	}
	
	if (p == nullptr)
		tail = r;
	r->next = p;
	if (q == nullptr)
		head = r;
	else
		q->next = r;
	nItems++;
	return 1;
}

status_t
Stream::Save(FILE *fp, Qua *uberQua, short fmt)
{
	
	switch (fmt) {
	case STR_FMT_TEXT: {
		fprintf(fp, "{");
		for (StreamItem *p=head; p!=nullptr; p=p->next) {
		}
		fprintf(fp, "}");
		break;
	}

	case STR_FMT_RAW: {
//#define	WriteVar(X)	if ((err=fp->Write(&X, (ulong)sizeof(X))) < B_NO_ERROR) return err
#define	WriteVar(X)	if (fwrite(&X, (ulong)sizeof(X), 1, fp) != 1) return B_ERROR;
	
		WriteVar(nItems);
		for (StreamItem *p=head; p!=nullptr; p=p->next) {
			WriteVar(p->type);
			Time	t = p->time;
			t.metric = (Metric *)0;
			WriteVar(t);
			switch (p->type) {
			case TypedValue::S_NOTE: {
				StreamNote	*q = (StreamNote *)p;
				WriteVar(q->note.cmd);
				WriteVar(q->note.pitch);
				WriteVar(q->note.dynamic);
				WriteVar(q->note.duration);
				break;
			}

			case TypedValue::S_BEND: {
				StreamBend *q = (StreamBend *)p;
				WriteVar(q->bend.cmd);
				WriteVar(q->bend.bend);
				break;
			}

			case TypedValue::S_MESSAGE: {
				StreamMesg *q = (StreamMesg *)p;
				/* todo xxxx save of S_MESSAGE
				size_t	s = q->mesg->FlattenedSize();
				char	*buf = new char[s];

				q->mesg->Flatten(buf, s);
				if (fwrite(&s, sizeof(s), 1, fp) !=  1) {
					reportError("Couldn't write flattened message size");
					return B_ERROR;
				}
				if (fwrite(buf, s, 1, fp) !=  1) {
					reportError("Couldn't write flattened message");
					return B_ERROR;
				}
				delete buf;
				*/
				break;
			}
	//		case TypedValue::S_VALUE: {
	//			StreamValue *q = (StreamValue *)p;
	//			break;
	//		}

			case TypedValue::S_CTRL: {
				StreamCtrl *q = (StreamCtrl *)p;
				WriteVar(q->ctrl.cmd);
				WriteVar(q->ctrl.controller);
				WriteVar(q->ctrl.amount);
				break;
			}

			case TypedValue::S_PROG: {
				StreamProg *q = (StreamProg *)p;
				WriteVar(q->prog.cmd);
				WriteVar(q->prog.program);
				WriteVar(q->prog.bank);
				WriteVar(q->prog.subbank);
				break;
			}

			case TypedValue::S_SYSX: {
				StreamSysX *q = (StreamSysX *)p;
				WriteVar(q->sysX.length);
//				if ((err=fp->Write(q->sysX.data, q->sysX.length))<B_NO_ERROR) {
//					return err;
//				}
				if (fwrite(q->sysX.data, q->sysX.length, 1, fp) != 1) {
					return B_ERROR;
				}
				break;
			}

			case TypedValue::S_SYSC: {
				StreamSysC *q = (StreamSysC *)p;
				WriteVar(q->sysC.cmd);
				WriteVar(q->sysC.data1);
				WriteVar(q->sysC.data2);
				break;
			}

	//		case TypedValue::S_LOG_ENTRY: {
	//			StreamLogEntry *q = (StreamLogEntry *)p;
	//			break;
	//		}
			case TypedValue::S_JOY: {
				StreamJoy *q = (StreamJoy *)p;
//				WriteVar(q->joy.x);
//				WriteVar(q->joy.y);
//				WriteVar(q->joy.deltax);
//				WriteVar(q->joy.deltay);
//				WriteVar(q->joy.button1);
//				WriteVar(q->joy.button2);
				break;
			}

			default:
				internalError("wierd stream item");
			}
		}
		break;
	}}
	return B_NO_ERROR;
}

status_t
Stream::Load(FILE *fp, Qua *uberQua, short fmt)
{

#define	ReadVar(X)	if (fread(&X, sizeof(X), 1, fp) != 1) return B_ERROR
	int			sc;
	ReadVar(sc);
	for (int i=0; i<sc; i++) {
		Time		t;
		short		type;
		ReadVar(type);
		ReadVar(t);
		if (uberQua)
			t.metric = uberQua->metric;
		else
			t.metric = &Metric::std;
		switch (type) {
		case TypedValue::S_NOTE: {
			StreamNote	*q = new StreamNote(&t, nullptr);
			ReadVar(q->note.cmd);
			ReadVar(q->note.pitch);
			ReadVar(q->note.dynamic);
			ReadVar(q->note.duration);
			
			AppendItem(q);
			break;
		}
		case TypedValue::S_BEND: {
			StreamBend *q = new StreamBend(&t,nullptr);
			ReadVar(q->bend.cmd);
			ReadVar(q->bend.bend);
			
			AppendItem(q);
			break;
		}

		case TypedValue::S_MESSAGE: {
			/* todo xxxx read back osc message daya
			StreamMesg *q = new StreamMesg(&t, nullptr);
			q->mesg = new OSCMessage();
			size_t	s;
			if (fread(&s, sizeof(s), 1, fp) !=  1) {
				//reportError("Couldn't read flattened message size");
				return B_ERROR;
			}
			char	*buf = new char[s];

			if (fread(buf, s, 1, fp) !=  1) {
				//reportError("Couldn't read flattened message");
				return B_ERROR;
			}
			q->mesg->Unflatten(buf);
			
			delete buf;
			
			AppendItem(q);
			*/
			break;
		}

//		case TypedValue::S_VALUE: {
//			StreamValue *q = (StreamValue *)p;
//			break;
//		}
		case TypedValue::S_CTRL: {
			StreamCtrl *q = new StreamCtrl(&t, nullptr);
			ReadVar(q->ctrl.cmd);
			ReadVar(q->ctrl.controller);
			ReadVar(q->ctrl.amount);
			
			AppendItem(q);
			break;
		}
		case TypedValue::S_PROG: {
			StreamProg *q = new StreamProg(&t, nullptr);
			ReadVar(q->prog.cmd);
			ReadVar(q->prog.program);
			ReadVar(q->prog.bank);
			ReadVar(q->prog.subbank);
			
			AppendItem(q);
			break;
		}
		case TypedValue::S_SYSX: {
			StreamSysX *q = new StreamSysX(&t, nullptr);
			ReadVar(q->sysX.length);
			q->sysX.data = new char[q->sysX.length];
//			if ((err=fp->Read(q->sysX.data, q->sysX.length))<B_NO_ERROR) {
//				return err;
//			}
			if (fread(q->sysX.data, q->sysX.length, 1, fp) != 1) {
				return B_ERROR;
			}
			
			AppendItem(q);
			break;
		}
		case TypedValue::S_SYSC: {
			StreamSysC *q = new StreamSysC(&t, nullptr);
			ReadVar(q->sysC.cmd);
			ReadVar(q->sysC.data1);
			ReadVar(q->sysC.data2);
			
			AppendItem(q);
			break;
		}
//		case TypedValue::S_LOG_ENTRY: {
//			StreamLogEntry *q = (StreamLogEntry *)p;
//			break;
//		}
		case TypedValue::S_JOY: {
			StreamJoy *q = new StreamJoy(&t, nullptr);
//			ReadVar(q->joy.x);
//			ReadVar(q->joy.y);
//			ReadVar(q->joy.deltax);
//			ReadVar(q->joy.deltay);
//			ReadVar(q->joy.button1);
//			ReadVar(q->joy.button2);
			AppendItem(q);
			break;
		}
		default:
			internalError("wierd stream item");
		}
	}
	return B_NO_ERROR;
}

status_t
Stream::SetValue(Block *b)
{
	status_t	err=B_NO_ERROR;
	return err;
}
