#include "qua_version.h"

#ifdef QUA_V_AUDIO

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "StdDefs.h"

#include "QuaAudio.h"
#include "QuaPort.h"
#include "Instance.h"
#include "Qua.h"
#include "ValList.h"
#include "Channel.h"
#include "Block.h"
#include "Schedulable.h"
//#include "messid.h"
#include "Envelope.h"
#include "Sample.h"
#include "FloatQueue.h"

#if defined(_BEOS)
#include "Sampler.h"
#elif defined(WIN32)

#endif
#if defined(QUA_V_ARRANGER_INTERFACE)
#if defined(WIN32)
#include "QuaDisplay.h"
#elif defined(_BEOS)
#include "Toolbox.h"
#include "RosterView.h"
#include "ArrangerObject.h"
#include "AQuarium.h"
#include "SequencerWindow.h"
#endif
#endif
#ifdef QUA_V_AUDIO_ASIO
QuaAsio		QuaAudioManager::asio;
#endif

QuaAudioManager		*audioManager = nullptr;

QuaAudioStreamIO::QuaAudioStreamIO(Qua *q, QuaAudioPort *p,
								   long chid, long gid, char *nm,
								   short sf, short nac):
	QuaStreamIO(q, p)
{
	nChannel = nac;
	sampleFormat = sf;
	strcpy(insertName, nm);
	insertId = chid;
	groupId = gid;
}

QuaAudioOut::QuaAudioOut(Qua *q, QuaAudioPort *p,
								   long chid, long gid, char *nm,
								   short sf, short nac):
	QuaAudioStreamIO(q, p, chid, gid, nm, sf, nac)
{
	offset = 0;
	outbuf = nullptr;
}


QuaAudioIn::QuaAudioIn(Qua *q, QuaAudioPort *p,
								   long chid, long gid, char *nm,
								   short sf, short nac):
	QuaAudioStreamIO(q, p, chid, gid, nm, sf, nac)
{
	data = nullptr;
}

QuaAudioIn::~QuaAudioIn()
{
	if (data)
		delete data;
}

QuaAudioOut::~QuaAudioOut()
{
	if (outbuf)
		delete outbuf;
}

QuaAudioPort::QuaAudioPort(char *nm, QuaAudioManager *qa, short subt
#if defined(_BEOS) && defined(NEW_MEDIA)
							,media_node *ni
#endif
							)
	: QuaPort(nm, QUA_DEV_AUDIO, subt, QUA_PORT_IO
#ifdef QUA_V_ARRANGER_INTERFACE
#if defined(_BEOS)&& defined(NEW_MEDIA)
			,((ni->kind & B_PHYSICAL_INPUT) == B_PHYSICAL_INPUT)?
					quapp->audioInSmallIcon:
			((ni->kind & B_PHYSICAL_OUTPUT) == B_PHYSICAL_OUTPUT)?
					quapp->audioOutSmallIcon:
			((ni->kind & (B_BUFFER_PRODUCER|B_BUFFER_CONSUMER))
						 == (B_BUFFER_PRODUCER|B_BUFFER_CONSUMER))?
					quapp->audioSmallIcon:	 
			((ni->kind & B_BUFFER_PRODUCER) == B_BUFFER_PRODUCER)?
					quapp->audioSmallIcon:	 
			((ni->kind & B_BUFFER_CONSUMER) == B_BUFFER_CONSUMER)?
					quapp->audioSmallIcon:quapp->audioSmallIcon,
					
			((ni->kind & B_PHYSICAL_INPUT) == B_PHYSICAL_INPUT)?
					quapp->audioInBigIcon:
			((ni->kind & B_PHYSICAL_OUTPUT) == B_PHYSICAL_OUTPUT)?
					quapp->audioOutBigIcon:
			((ni->kind & (B_BUFFER_PRODUCER|B_BUFFER_CONSUMER))
						 == (B_BUFFER_PRODUCER|B_BUFFER_CONSUMER))?
					quapp->audioBigIcon:	 
			((ni->kind & B_BUFFER_PRODUCER) == B_BUFFER_PRODUCER)?
					quapp->audioBigIcon:	 
			((ni->kind & B_BUFFER_CONSUMER) == B_BUFFER_CONSUMER)?
					quapp->audioBigIcon:quapp->audioBigIcon
#else
#endif
#endif
		)
{
	quaAudio = qa;
	
#if defined(_BEOS)
#if NEW_MEDIA
	if ((ni->kind & B_PHYSICAL_INPUT) == B_PHYSICAL_INPUT) {
	} else if ((ni->kind & B_PHYSICAL_OUTPUT) == B_PHYSICAL_OUTPUT) {
	} else if ((ni->kind & (B_BUFFER_PRODUCER|B_BUFFER_CONSUMER))
					 == (B_BUFFER_PRODUCER|B_BUFFER_CONSUMER)) {
	} else if ((ni->kind & B_BUFFER_PRODUCER) == B_BUFFER_PRODUCER) {
	} else if ((ni->kind & B_BUFFER_CONSUMER) == B_BUFFER_CONSUMER) {
	} else {
	}

	if (ni->kind & B_CONTROLLABLE) {
//			fprintf(stderr, "controllable\n");
	}
	if (ni->kind & B_SYSTEM_MIXER) {
//			fprintf(stderr, "mixer\n");
	}
	if (ni->kind & B_TIME_SOURCE) {
//			fprintf(stderr, "time source\n");
	}
	
	mediaNode = ni?*ni:media_node::null;
	BMediaRoster	*r = BMediaRoster::Roster();
	if (mediaNode.kind & B_CONTROLLABLE) {
		err = r->GetParameterWebFor(mediaNode, &parameterWeb);
		if (err != B_OK) {
			reportError("Can't build parameter web for %s, %s\n", sym->UniqueName(), ErrorStr(err));
			parameterWeb = nullptr;
		} else {
			fprintf(stderr, "creating groups %s\n", sym->name);
			for (short i=0; i<parameterWeb->CountGroups(); i++) {
				BParameterGroup	*p = parameterWeb->GroupAt(i);
				ZotParameterGroup(p, sym);
			}
		}
	} else {
	}
	fprintf(stderr, "insert check\n");
	ZotInsertCheck(true, true, true, false);
	fprintf(stderr, "insert check done\n");
	
#else
	AddInsert("AudioIn", 0, INPUT_INSERT, 1, 0);
	AddInsert("Mixer", 1, OUTPUT_INSERT, 1, 0);
#endif
#elif defined(WIN32)

#endif
}


char *
QuaAudioPort::Name(uchar dfmt)
{
	if (dfmt == NMFMT_NAME) {
		if (deviceSubType == QUA_AUDIO_ASIO) {
			if (quaAudio->asio.loaded) {
				return quaAudio->asio.CurrentDriver();
			} else if (quaAudio->asio.preferredDriver) {
				return quaAudio->asio.preferredDriver;
			} else {
				return "asio";
			}
		}
	}
	return sym->name;
}


short
QuaAudioPort::NInputs()
{
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.nInputChannels;
		}
	}
	return 0;
}

short
QuaAudioPort::NOutputs()
{
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.nOutputChannels;
		}
	}
	return 0;
}

short
QuaAudioPort::NInputChannels(short port)
{
	if (deviceSubType == QUA_AUDIO_ASIO) {
		return 1;
	}
	return 0;
}

short
QuaAudioPort::NOutputChannels(short port)
{
	if (deviceSubType == QUA_AUDIO_ASIO) {
		return 1;
	}
	return 0;
}

const char *
QuaAudioPort::InputName(port_chan_id ch_id)
{
	if (ch_id < 0 || ch_id >= NInputs()) {
		return "invalid";
	}
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.input[ch_id]->insertName;
		}
	}
	static char buf[32];
	sprintf(buf, "In %d", ch_id);
	return buf;
}

const char *
QuaAudioPort::OutputName(port_chan_id ch_id)
{
	if (ch_id < 0 || ch_id >= NOutputs()) {
		return "invalid";
	}
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.output[ch_id]->insertName;
		}
	}
	static char buf[32];
	sprintf(buf, "Out %d", ch_id);
	return buf;
}

void
QuaAudioPort::ZotInsertCheck(bool check_input, bool check_output, bool check_connection, bool do_squish)
{
#ifdef NEW_MEDIA
	if (1) { // somethins so this won't deadlock
#define MAX_MEDIA_OUTPUT	32
#define MAX_MEDIA_INPUT		32
		BList			squishlist;
		bool	squished = false;

		BMediaRoster	*r = BMediaRoster::Roster();
		QuaInsert	*ins, *insd;
		status_t	err;
		if (check_output) {
			media_output	outlist[MAX_MEDIA_OUTPUT];
			int32			nConnOut=0,nFreeOut=0;
			err = r->GetConnectedOutputsFor(mediaNode,
							outlist,
							MAX_MEDIA_OUTPUT,
							&nConnOut);
			fprintf(stderr, "%d conn outs %s\n", nConnOut, ErrorStr(err));
			
			for (short i=0; i<nConnOut; i++) {
				if ((ins=FindInsert(nullptr/*outlist[i].name*/, outlist[i].source.id, OUTPUT_INSERT)) == nullptr) {
					ins = AddInsert(outlist[i].name,
							outlist[i].source.id,
							OUTPUT_INSERT,
							outlist[i].format.u.raw_audio.channel_count,
							outlist[i].format.u.raw_audio.format
							);
				}
				if (do_squish)
					squishlist.AddItem(ins);
				if (strcmp(outlist[i].name, ins->name) != 0) {
					ins->SetName(outlist[i].name);
				}
				QuaAudioPort	*p=nullptr;
				if (check_connection) {
					p = quaAudio->AudioPortForNodePort(outlist[i].destination.port);
					if (p) {
						insd = p->FindInsert(nullptr, outlist[i].destination.id, INPUT_INSERT);
						p->ZotInsertCheck(true, false, false, do_squish);
						if (insd) {
							BMessage	conMsg(B_MEDIA_CONNECTION_MADE);
							conMsg.AddPointer("input object", p->PortObjectView());
							conMsg.AddPointer("output object", PortObjectView());
							conMsg.AddPointer("input insert", insd);
							conMsg.AddPointer("output insert", ins);
							conMsg.AddInt32("channels", outlist[i].format.u.raw_audio.channel_count);
							quaAudio->Looper()->PostMessage(&conMsg, quaAudio);
						}
					}
				}
				fprintf(stderr, "out node <%s> outname <%s> dest <%s> node %d %d port %d s.port %d d.port %d\n",
							sym->name, outlist[i].name, p?p->sym->name:"", outlist[i].node.node,
							mediaNode.node, mediaNode.port,
							outlist[i].source.port, outlist[i].destination.port);
			}
			err = r->GetFreeOutputsFor(mediaNode,
							&outlist[nConnOut],
							MAX_MEDIA_OUTPUT-nConnOut,
							&nFreeOut);
			fprintf(stderr, "%d free outs %s\n", nConnOut, ErrorStr(err));
			for (short i=nConnOut; i<nFreeOut+nConnOut; i++) {
				if ((ins = FindInsert(nullptr/*outlist[i].name*/, outlist[i].source.id, OUTPUT_INSERT)) == nullptr) {
					ins = AddInsert(outlist[i].name,
							outlist[i].source.id,
							OUTPUT_INSERT,
							outlist[i].format.u.raw_audio.channel_count,
							outlist[i].format.u.raw_audio.format
							);
				}
				if (do_squish) {
					squishlist.AddItem(ins);
				}
				if (strcmp(outlist[i].name, ins->name) != 0) {
					ins->SetName(outlist[i].name);
				}
				if (check_connection && ins->quanexion) {
					Insertable	*p;
					QuaAudioPort	*q;
					
					p = ins->quanexion->to->object;
					if (p) {
						q = (QuaAudioPort *) p->executable;
						if (q) {
							q->ZotInsertCheck(true, false, false, do_squish);
						}
					}
					BMessage	conMsg(B_MEDIA_CONNECTION_BROKEN);
					conMsg.AddPointer("input insert", ins->quanexion?ins->quanexion->to:nullptr);
					conMsg.AddPointer("output insert", ins);
					quaAudio->Looper()->PostMessage(&conMsg, quaAudio);
				}
				fprintf(stderr, "fout %s %s node %d %d port %d s.port %d d.port %d\n", sym->name, outlist[i].name, outlist[i].node.node, mediaNode.node, mediaNode.port, outlist[i].source.port, outlist[i].destination.port);
			}
			if (do_squish) {
						// look for and remove duds...
				short 	i=0;
				QuaInsert	*ins;
				while(ins = (QuaInsert *)inserts.ItemAt(i)) {
					if (ins->type == OUTPUT_INSERT) {
						if (squishlist.HasItem(ins)) {
							i++;
						} else {
							fprintf(stderr, "%s removed unregistered output %d: %x %s\n", sym->name, i, ins, ins->name);
							squished = true;
							RemoveInsert(i);
//							i++;
						}
					} else {
						i++;
					}
				}
			}
		}
		
		if (check_input) {
			media_input		inlist[MAX_MEDIA_INPUT];
			int32			nConnIn=0,nFreeIn=0;
	
			err = r->GetConnectedInputsFor(mediaNode,
							inlist,
							MAX_MEDIA_INPUT,
							&nConnIn);
			fprintf(stderr, "%d conn ins %s\n", nConnIn, ErrorStr(err));
			for (short i=0; i<nConnIn; i++) {
				if ((ins = FindInsert(nullptr/*inlist[i].name*/, inlist[i].destination.id, INPUT_INSERT)) == nullptr) {
					ins = AddInsert(inlist[i].name,
							inlist[i].destination.id,INPUT_INSERT,
							inlist[i].format.u.raw_audio.channel_count,
							inlist[i].format.u.raw_audio.format
						);
				}
				if (do_squish)
					squishlist.AddItem(ins);
				if (strcmp(inlist[i].name, ins->name) != 0) {
					ins->SetName(inlist[i].name);
				}
				QuaAudioPort	*p=nullptr;
				if (check_connection) {
					p = quaAudio->AudioPortForNodePort(inlist[i].source.port);
					if (p) {
						insd = p->FindInsert(nullptr, inlist[i].source.id, OUTPUT_INSERT);
						p->ZotInsertCheck(false, true, false, do_squish);
						if (insd) {
							BMessage	conMsg(B_MEDIA_CONNECTION_MADE);
							conMsg.AddPointer("input object", PortObjectView());
							conMsg.AddPointer("output object", p->PortObjectView());
							conMsg.AddPointer("input insert", ins);
							conMsg.AddPointer("output insert", insd);
							conMsg.AddInt32("channels", inlist[i].format.u.raw_audio.channel_count);
							quaAudio->Looper()->PostMessage(&conMsg, quaAudio);
						}
					}
				}
				fprintf(stderr, "in %s %s node %d %d port %d s.port %d d.port %d\n", sym->name, inlist[i].name, inlist[i].node.node, mediaNode.node, mediaNode.port, inlist[i].source.port, inlist[i].destination.port);
			}
			err = r->GetFreeInputsFor(mediaNode,
							&inlist[nConnIn],
							MAX_MEDIA_INPUT-nConnIn,
							&nFreeIn);
			fprintf(stderr, "%d free ins %s\n", nConnIn, ErrorStr(err));
			for (short i=nConnIn; i<nFreeIn+nConnIn; i++) {
				if ((ins = FindInsert(nullptr /*inlist[i].name*/, inlist[i].destination.id, INPUT_INSERT)) == nullptr) {
					ins = AddInsert(
								inlist[i].name,
								inlist[i].destination.id,INPUT_INSERT,
								inlist[i].format.u.raw_audio.channel_count,
								inlist[i].format.u.raw_audio.format
								);
				}
				if (do_squish)
					squishlist.AddItem(ins);
				if (strcmp(inlist[i].name, ins->name) != 0) {
					ins->SetName(inlist[i].name);
				}
				if (check_connection && ins->quanexion) {
					Insertable	*p;
					QuaAudioPort	*q;
					
					p = ins->quanexion->from->object;
					if (p) {
						q = (QuaAudioPort *) p->executable;
						if (q) {
							q->ZotInsertCheck(false, true, false, do_squish);
						}
					}
					BMessage	conMsg(B_MEDIA_CONNECTION_BROKEN);
					conMsg.AddPointer("input insert", ins->quanexion?ins->quanexion->from:nullptr);
					conMsg.AddPointer("output insert", ins);
					quaAudio->Looper()->PostMessage(&conMsg, quaAudio);
				}
				fprintf(stderr, "fin %s %s node %d %d port %d s.port %d d.port %d (%d %d)\n",
						sym->name, inlist[i].name, inlist[i].node.node,
						mediaNode.node, mediaNode.port,
						inlist[i].source.port, inlist[i].destination.port,
						inlist[i].source.id, inlist[i].destination.id);
						
			}
			if (do_squish) {
						// look for and remove duds...
				short i=0;
				QuaInsert	*ins;
				while(ins = (QuaInsert *)inserts.ItemAt(i)) {
					if (ins->type == INPUT_INSERT) {
						if (squishlist.HasItem(ins)) {
							i++;
						} else {
							fprintf(stderr, "%s removed unregistered input %d: %x %s\n", sym->name, i, ins, ins->name);
							squished = true;
							RemoveInsert(i);
//							i++;
						}
					} else {
						i++;
					}
				}
			}
		}
#ifdef QUA_V_ARRANGER_INTERFACE
		if (squished) {
			SetupInsertDisplay();
		}
#endif
	}
#endif
}

QuaAudioPort::~QuaAudioPort()
{
#ifdef NEW_MEDIA
	BMediaRoster	*r = BMediaRoster::Roster();
	r->ReleaseNode(mediaNode);
	BMessage	delMsg(B_MEDIA_NODE_DELETED);
	delMsg.AddPointer("port object", PortObjectView());
	if (representation->Window())
		representation->Window()->PostMessage(&delMsg, representation->Parent());
#endif
}	


#ifdef _BEOS
void
QuaAudioPort::DrawExtras()
{
#ifdef NEW_MEDIA
	if (mediaNode.kind & B_CONTROLLABLE) {
		representation->SetDrawingMode(B_OP_COPY);
		representation->DrawBitmap(
					quapp->audioCtrlIcon,
					representation->iconPoint+BPoint(32,0));
	}
#endif
}
#endif

#ifdef _BEOS
void
QuaAudioPort::ZotParameterGroup(BParameterGroup *group, StabEnt *ctxt)
{
//	for (short k=0; k<depth; k++)
//		fprintf(stderr, "    ");
	fprintf(stderr, "Group %s\n", group->Name());
	
	if (group->CountParameters() == 0) {
		for (short i=0; i<group->CountGroups(); i++) {
			BParameterGroup		*q = group->GroupAt(i);
			ZotParameterGroup(q, ctxt);
		}
		return;
	}
		
//	if (group->CountParameters() == 0 && group->CountGroups() == 1) {
//		ZotParameterGroup(group->GroupAt(0), ctxt);
//		return;
//	}
	
	StabEnt		*grop =
		DefineSymbol((char *)group->Name(), S_PARAMGROUP, 0,
				0, ctxt,
				REF_VALUE, StabEnt::ATTR_NONE, false, StabEnt::DISPLAY_NOT);
	grop->ParamGroupValue()->Set(en);

	for (short i=0; i<group->CountParameters(); i++) {
	
		BParameter	*p = group->ParameterAt(i);
		switch (p->Type()) {
		
		case BParameter::B_nullptr_PARAMETER:

//			for (short k=0; k<depth; k++)
//				fprintf(stderr, "    ");
//			fprintf(stderr, "--->");
//			fprintf(stderr, "\"%s\": null\n", p->Name());

			break;
			
		case BParameter::B_CONTINUOUS_PARAMETER: {
			StabEnt	*pSym=DefineSymbol((char *)p->Name(), S_PORTPARAM, 0,
						p, grop,
						REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
//			for (short k=0; k<depth; k++)
//				fprintf(stderr, "    ");
//			fprintf(stderr, "--->");
//			fprintf(stderr, "\"%s\": continuous.\n", p->Name());
			break;
		}
		
		case BParameter::B_DISCRETE_PARAMETER: {
			BDiscreteParameter	*q = (BDiscreteParameter *)p;
//				if (q->CountInputs())
//					q->MakeItemsFromInputs();
//				if (q->CountOutputs())
//					q->MakeItemsFromOutputs();
//			for (short k=0; k<depth; k++)
//				fprintf(stderr, "    ");
//			fprintf(stderr, "--->");
//			fprintf(stderr, "\"%s\": discrete of %d vals.\n", q->Name(), q->CountItems());
//			for (short j=0; j<q->CountItems(); j++) {
//				for (short k=0; k<depth; k++)
//					fprintf(stderr, "    ");
//				fprintf(stderr, "  -->");
//				fprintf(stderr, "\"%s\" %d\n", q->ItemNameAt(j), q->ItemValueAt(j));
//			}
			break;
		}

		}
	}
	
	for (short i=0; i<group->CountGroups(); i++) {
		BParameterGroup		*q = group->GroupAt(i);
		ZotParameterGroup(q, grop);
	}
}
#endif


#ifdef QUA_V_ARRANGER_INTERFACE
#ifdef _BEOS
void
QuaAudioPort::ZotParameterMenu(BMenu *q, StabEnt *s, ArrangerObject *a)
{
	for (StabEnt *p = s->children; p!= nullptr; p=p->sibling) {
		if (p->type == S_PARAMGROUP) {
			BMenu	*qq = new BMenu(p->name);
			ZotParameterMenu(qq, p, a);
			q->AddItem(qq);
		}
	}
	for (short i=0; i<a->instance->envelopes.CountItems(); i++) {
		Envelope	*e = (Envelope *)a->instance->envelopes.ItemAt(i);
		if (e->sym->context == s) {
				
			BMessage	*msg = new BMessage(SET_ENVELOPE);
			msg->AddPointer("control var", e->controlVar);
		
			BMenuItem	*item = new BMenuItem(e->sym->name, msg);
			q->AddItem(item);
			item->SetTarget(a);
		}
	}
}
#endif
#endif


#ifdef QUA_V_ARRANGER_INTERFACE

#ifdef _BEOS
ArrangerObject *
QuaAudioPort::CloneArrangerObject(int chan, Time start, Time dur,
						TimeArrangeView *tv)
{
	if (uberQua) {
		if (uberQua != tv->uberQua) {
			reportError("This node is already attached to a different sequencer\n");
			return nullptr;
		}
	}

#ifdef NEW_MEDIA
	if (!(mediaNode.kind & B_CONTROLLABLE)) {
		reportError("Not a controllable node, unfortunately");
		return nullptr;
	}
	if (  mediaNode.kind & B_PHYSICAL_INPUT ||
		  mediaNode.kind & B_PHYSICAL_OUTPUT ||
		  mediaNode.kind & B_SYSTEM_MIXER) {
		if (instances.CountItems() > 0) {
			reportError("Can only have one instance of a physical or mixer node");
			return nullptr;
		}
		start.ticks = 0;
	}
	
	uberQua = tv->uberQua;
	metric = tv->uberQua->metric;

	ArrangerObject *p =  new ArrangerObject(
								sym->UniqueName(),
								this,
								chan,
								start,
								dur, tv,
								representation->color);
	return p;
#else
	if (this == quaAudio->dfltInput.device || this == quaAudio->dfltOutput.device) {
		if (instances.CountItems() > 0) {
			reportError("Can only have one instance of a physical or mixer node");
			return nullptr;
		}
		start.ticks = 0;
		uberQua = tv->uberQua;
		metric = tv->uberQua->metric;
		ArrangerObject *p =  new ArrangerObject(
									sym->UniqueName(),
									this,
									chan,
									start,
									dur, tv,
									representation->color);
		return p;
	}
	return nullptr;
#endif
}
#endif

#endif


#if defined(_BEOS)
void
QuaAudioPort::KickStop()
{
#ifdef NEW_MEDIA
	if (!(	mediaNode.kind & (	B_PHYSICAL_INPUT|
								B_PHYSICAL_INPUT|
								B_SYSTEM_MIXER))) {
		status_t		err;
		BMediaRoster	*r = BMediaRoster::Roster();
		BTimeSource* ts = r->MakeTimeSourceFor(mediaNode);
		if (ts == nullptr) {
			fprintf(stderr, "Port snoozer: can't find TimeSource");
			return;
		}
		err = r->StopNode(mediaNode, ts->Now());
								//*upstreamLatency);
		if (err != B_OK) {
			fprintf(stderr, "Port snoozer: starting consumer node error: %s\n", ErrorStr(err));
		}
	}
#endif
}
#endif

#if defined(_BEOS)
void
QuaAudioPort::KickStart()
{
#ifdef NEW_MEDIA
	if (!(	mediaNode.kind & (	B_PHYSICAL_INPUT|
								B_PHYSICAL_INPUT|
								B_SYSTEM_MIXER))) {
		BMediaRoster	*r = BMediaRoster::Roster();
		BTimeSource* ts = r->MakeTimeSourceFor(mediaNode);
		status_t		err;
		if (ts == nullptr) {
			fprintf(stderr, "Port waker: can't find TimeSource");
			return;
		}
		err = r->StartNode(mediaNode, ts->Now());
								//*upstreamLatency);
		if (err != B_OK) {
			fprintf(stderr, "Port waker: starting consumer node error: %s\n", ErrorStr(err));
		}
	}
#endif
}
#endif


#ifdef _BEOS
status_t
QuaAudioPort::QuaStart()
{
	Schedulable::QuaStart();
	instanceLock.ReadLock();
	bool	kikkit = false;
	for (short i=0; i<CountInstances(); i++) {
		Instance	*inst = InstanceNo(i);
		if (inst->status == STATUS_RUNNING) {
			kikkit = true;
		}
	}
	if (kikkit) {
#ifdef _BEOS
		KickStart();
#endif
	}
	instanceLock.ReadUnlock();
	return B_NO_ERROR;
}

status_t
QuaAudioPort::QuaStop()
{
	Schedulable::QuaStop();
	instanceLock.ReadLock();
	bool	kikkit = false;
	for (short i=0; i<CountInstances(); i++) {
		Instance	*inst = InstanceNo(i);
		if (inst->status == STATUS_RUNNING) {
			kikkit = true;
		}
	}
	if (kikkit) {
#ifdef _BEOS
	 	KickStart();
#endif
	}
	instanceLock.ReadUnlock();
//	for (short i=0; i<activeInstances.CountItems(); i++) {
//		Sleep((Instance *)activeInstances.ItemAt(i));
//	}
	return B_NO_ERROR;
}
#endif

#ifdef _BEOS
au_dest_info_t::au_dest_info_t()
{
	device = nullptr;
	xdevice = nullptr;
#ifdef NEW_MEDIA
	destination = media_destination::null;
	xdestination = media_destination::null;
#endif
}

au_src_info_t::au_src_info_t()
{
	device = nullptr;
	xdevice = nullptr;
#ifdef NEW_MEDIA
	source = media_source::null;
	xsource = media_source::null;
#endif
}
#endif

void
QuasiStack::SetAudio(uchar v)
{
	if (v == AUDIO_NONE) {
		hasAudio = AUDIO_NONE;
	} else {
		hasAudio |= v;
		for (QuasiStack *p=lowerFrame; p != nullptr; p = p->lowerFrame) {
			p->hasAudio |= v;
		}
	}
}

///////////////////////////////////////////////////////////
// AudioManager
///////////////////////////////////////////////////////////
QuaAudioManager::QuaAudioManager()
{
	sampleRate = 44100.0;
	bufferSize = 512;
	audioManager = this;

#if defined(WIN32)
	dfltInput = nullptr;
	dfltOutput = nullptr;

#ifdef QUA_V_AUDIO_ASIO
	asio.audio = this;

	QuaAudioPort *mp = new QuaAudioPort("asio", this, QUA_AUDIO_ASIO);
	if (dfltInput == nullptr) {
		dfltInput = mp;
	}
	if (dfltOutput == nullptr) {
		dfltOutput = mp;
	}
	ports.AddItem(mp);
/*
	int32		na = 0;
	char		**devnm = QuaAudioManager::asio.DeviceNames(&na);

	int32			i, j;

	if (devnm) {
		for (i=0; i<na; i++) {
			QuaAudioPort *mp = new QuaAudioPort(devnm[i], this, QUA_AUDIO_ASIO);
			if (dfltInput == nullptr) {
				dfltInput = mp;
			}
			if (dfltOutput == nullptr) {
				dfltOutput = mp;
			}
			ports.AddItem(mp);
		}

		for (j=0; j<na; j++) {
			delete devnm[j];
		}
		delete devnm;
	}
*/
#endif

#elif defined(_BEOS)
    sampler = new Sampler(uq);

#ifdef NEW_MEDIA
//	for (short i=0; i<5; i++)
//		atY[i] = patchBay->atX.y;
		
	quapp->AddHandler(this);
	
	MapRoster();

	BMediaRoster			*r = BMediaRoster::Roster();
	if (r == nullptr) {
		TragicError("Perhaps you could start the media server first, next time.");
	}
	BMessenger				rightHere(this);
	r->StartWatching(rightHere);
	
	media_node		inputNode;
	status_t		err;
	int32			count;
	live_node_info	ninf;
	media_output	inputOutput[20];
	
	r->GetAudioInput(&inputNode);
	r->GetLiveNodeInfo(inputNode, &ninf);
	
	dfltInput.device = AddPort(ninf.name, &inputNode);
	err = r->GetFreeOutputsFor(inputNode, inputOutput, 20, &count);
	for (short i=0; i<count; i++) {
		if (inputOutput[i].format.u.raw_audio.channel_count == 2) {
			dfltInput.source = inputOutput[i].source;
			dfltInput.format = inputOutput[i].format;
			break;
		}
	}
	if (dfltInput.source == media_source::null) {
		for (short i=0; i<count; i++) {
			if (inputOutput[i].format.u.raw_audio.channel_count == 1) {
				if (dfltInput.source == media_source::null) {
					dfltInput.source = inputOutput[i].source;
					dfltInput.format = inputOutput[i].format;
				} else {
					dfltInput.xdevice = dfltInput.device;
					dfltInput.xsource =  inputOutput[i].source;
					break;
				}
			}
		}
	}
	
	media_node	mixerNode;
	media_input	mixerInput[20];
	
	r->GetAudioMixer(&mixerNode);
	r->GetLiveNodeInfo(mixerNode, &ninf);
	
	dfltOutput.device = AddPort(ninf.name, &mixerNode);
	err = r->GetFreeInputsFor(mixerNode, mixerInput, 20, &count);
	for (short i=0; i<count; i++) {
		if (  mixerInput[i].destination.id != 0 &&
			  mixerInput[i].format.u.raw_audio.channel_count == 2) {
			dfltOutput.destination = mixerInput[i].destination;
			dfltOutput.format = mixerInput[i].format;
			break;
		}
	}
	if (dfltOutput.destination == media_destination::null) {
		// wierd
		fprintf(stderr, "wierd mixer!\n");
	}

#else
	dfltInput.device = new QuaAudioPort(
					"AudioIn",this);
	DefineSymbol("ADC In", S_PORTPARAM, 0,
				(void *)B_ADC_IN, dfltInput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	
	dfltOutput.device = new QuaAudioPort(
					"Mixer",this);
	DefineSymbol("CDThrough", S_PORTPARAM, 0,
				(void *)B_CD_THROUGH, dfltOutput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	DefineSymbol("Line through", S_PORTPARAM, 0,
				(void *)B_LINE_IN_THROUGH, dfltOutput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	DefineSymbol("Loopback", S_PORTPARAM, 0,
				(void *)B_LOOPBACK, dfltOutput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	DefineSymbol("DAC", S_PORTPARAM, 0,
				(void *)B_DAC_OUT, dfltOutput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	DefineSymbol("Master", S_PORTPARAM, 0,
				(void *)B_MASTER_OUT, dfltOutput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	DefineSymbol("Speaker", S_PORTPARAM, 0,
				(void *)B_SPEAKER_OUT, dfltOutput.device->sym,
				REF_POINTER, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
	dacCtrl = new BDACStream();
#endif
   	sampler->StartSampler();
#endif
	status = STATUS_RUNNING;
	buffyThread = spawn_thread(BuffaeratorWrapper, "buffaerator",
								B_DISPLAY_PRIORITY, this);
}

QuaAudioManager::~QuaAudioManager()
{
#if defined(_BEOS)
	delete sampler;
#ifdef NEW_MEDIA
	BMediaRoster		*r = BMediaRoster::Roster();
	BMessenger			rightHere(this);
	r->StopWatching(rightHere);
#endif
#else 
#endif
	status = STATUS_DEAD;

#if defined(_BEOS)&&defined(SEMWAIT)
	release_sem(fluffySem);
#endif
	fprintf(stderr, "~QuaAudioManager: resuming buffer eater\n");
	resume_thread(buffyThread);
	fprintf(stderr, "~QuaAudioManager: resuming writer\n");
	resume_thread(writerThread);
	status_t	err;
	fprintf(stderr, "~QuaAudioManager: waiting on buffer eater\n");
#ifdef WIN32
	if ((err=WaitForSingleObject(buffyThread, 5000)) != WAIT_OBJECT_0) {
		fprintf(stderr, "Qua: erroneous wait for death of buffer eater: %s\n", sem_error_string(err));
	}
#else
	wait_for_thread(buffyThread, &err);
#endif
	fprintf(stderr, "~QuaAudioManager: waiting on writer\n");
#ifdef WIN32
	if ((err=WaitForSingleObject(writerThread, 10000)) != WAIT_OBJECT_0) {
		fprintf(stderr, "Qua: erroneous wait for death of buffer writer: %s\n", sem_error_string(err));
	}
#else
	wait_for_thread(writerThread, &err);
#endif
	fprintf(stderr, "~QuaAudioManager: all done ... phew!\n");
}

status_t
QuaAudioManager::StartAudio()
{
	status_t	err = B_OK;
	status = STATUS_RUNNING;
	resume_thread(buffyThread);
#ifdef QUA_V_AUDIO_ASIO
	err = asio.Start();
#endif
	return err;
}

status_t
QuaAudioManager::StopAudio()
{
	status = STATUS_SLEEPING;
	resume_thread(buffyThread);
	status_t	err = B_OK;
#ifdef QUA_V_AUDIO_ASIO
	err = asio.Stop();
#endif
	return err;
}

char *
QuaAudioManager::SampleFormatName(long sf)
{
	switch(sf) {
	case ASIOSTInt16MSB: return "Int 16 MSB";
	case ASIOSTInt24MSB: return "Int 24 MSB";		// used for 20 bits as well
	case ASIOSTInt32MSB: return "Int 32 MSB";
	case ASIOSTFloat32MSB: return "Float 32 MSB";		// IEEE 754 32 bit float
	case ASIOSTFloat64MSB: return "Float 64 MSB";		// IEEE 754 64 bit double float

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can be more easily used with these
	case ASIOSTInt32MSB16: return "Int 32 MSB16";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32MSB18: return "Int 32 MSB18";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32MSB20: return "Int 32 MSB20";		// 32 bit data with 20 bit alignment
	case ASIOSTInt32MSB24: return "Int 32 MSB24";		// 32 bit data with 24 bit alignment
	
	case ASIOSTInt16LSB: return "Int 16 LSB";
	case ASIOSTInt24LSB: return "Int 24 LSB";		// used for 20 bits as well
	case ASIOSTInt32LSB: return "Int 32 LSB";
	case ASIOSTFloat32LSB: return "Float 32 LSB";		// IEEE 754 32 bit float, as found on Intel x86 architecture
	case ASIOSTFloat64LSB: return "Float 64 LSB"; 		// IEEE 754 64 bit double float, as found on Intel x86 architecture

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can more easily used with these
	case ASIOSTInt32LSB16: return "Int 32 LSB16";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32LSB18: return "Int 32 LSB18";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32LSB20: return "Int 32 LSB20";		// 32 bit data with 20 bit alignment
	case ASIOSTInt32LSB24: return "Int 32 LSB24";		// 32 bit data with 24 bit alignment
	}
	return "Unknown format";
}

int32
QuaAudioManager::BuffaeratorWrapper(void *data)
{
	return ((QuaAudioManager *)data)->Buffaerator();
}

int32
QuaAudioManager::Buffaerator()
{
#ifdef WIN32
	suspend_thread(buffyThread);
#endif
	status_t	err;
	while (status != STATUS_DEAD) {

		if (samples.CountItems() == 0 || status == STATUS_SLEEPING) {
			suspend_thread(buffyThread);
		} else {
			if ((err=sampleLock.ReadLock()) != B_NO_ERROR) {
				reportError("bufferaetor: can't acquire samples lock\n", sem_error_string(err));
				continue;
			}
			for (short i=0; i<samples.CountItems(); i++) {
				Sample	*sam = (Sample *)samples.ItemAt(i);
				if ((err=sam->uberQua->objectsBlockStack.ReadLock()) != B_NO_ERROR) {
					reportError("bufferaetor: can't acquire objects/block/stacks lock: %s\n", sem_error_string(err));
					continue;
				}
				sam->uberQua->objectsBlockStack.ReadUnlock();
				status_t	err = sam->SynchronizeBuffers();
				if (err != B_NO_ERROR) {
					::reportError("sample reader: file error on %s: %s", sam->sym->name, ErrorStr(err));
				}
			}
			sampleLock.ReadUnlock();
		}
#if defined(_BEOS)&&defined(SEMWAIT)
		acquire_sem(fluffySem);
#else
//		snooze(100000.0);
// 1 sample buffer = 2024 frames =~ 50ms
// ??? maybe we can be safely 100ms ahead
#if defined(WIN32)
		Sleep(1);
#elif defined(_BEOS)		
		snooze(1000);
#endif
#endif
	}
	return B_NO_ERROR;
}

long
QuaAudioManager::WriterWrapper(void *data)
{
	return ((QuaAudioManager *)data)->Writer();
}

long
QuaAudioManager::Writer()
{
	while(status != STATUS_DEAD) {
// this semaphore should not be locked by the main thread
		recordInstanceLock.Lock();
		if (recordInstances.CountItems() == 0) {
			recordInstanceLock.Unlock();
			fprintf(stderr, "suspending\n");
			suspend_thread(writerThread);
		} else {
			for (short i=0; i<recordInstances.CountItems(); i++) {
				SampleInstance	*si = ((SampleInstance *)recordInstances.ItemAt(i));
				if (si->QSample()->FlushRecordBuffers(false) != B_NO_ERROR) {
					recordInstances.RemoveItemAt(i);
					break;
				}
			}
			recordInstanceLock.Unlock();
		}
#if defined(WIN32)
		Sleep(2);
#elif defined(_BEOS)		
		snooze(2000);
#endif
	}
	return B_NO_ERROR;
}

bool
QuaAudioManager::Generate(size_t nFrames)
{

#ifdef _BEOS	
// buffer allocation for all necessary! ex-GenerateBBuffer
		
	for (short i=0; i<nOutput; i++) {
		QuaAudioOut	*op = &outPort[output[i]];
		if (op->isConnected) {
			if (op->buffer == nullptr) {
// allocate a buffer if there's no buffer allocated
// should be zeroed after it is filled fully and sent...
// request may return nullptr, but hopefully, we dont need to block
// everything, if were careful
				op->buffer = op->bufferGroup->RequestBuffer(
									op->bufSize,
									BufferDuration());
//				fprintf(stderr, "%d/%d request for %d buf gives %x\n", i, output[i], op->bufSize, op->buffer);
				long	nOutSamples = op->bufSize / op->sampleSize;
				if (op->buffer) {
//					fprintf(stderr, "%d %d %d %d %x\n", nOutSamples, op->bufSize, op->sampleSize, op->frameSize, op->quap_buf);
					for (ulong i=0; i<nOutSamples; i++) {
						// just zero the float tempbuf for here...
						op->quap_buf[i] = 0;
					}
					// fill in the buffer header
					media_header* hdr = op->buffer->Header();
					op->offset = 0;
					hdr->type = B_MEDIA_RAW_AUDIO;
					hdr->size_used = framesPerEvent * op->frameSize;
// we'll update size_used as we fill the buffer. in the bigga
// pitcha, it may take several event cycles to fill this buffer.
// e.g. Gina likes 512 p channel, and the mixer likes 2048 (in bytes),
// so two Gina buffer fill and sends would fill one mixer buffer
					hdr->time_source = TimeSource()->ID();
				
					bigtime_t stamp;
					if (RunMode() == B_RECORDING) {
						// In B_RECORDING mode, we stamp with the capture time.  We're not
						// really a hardware capture node, but we simulate it by using the (precalculated)
						// time at which this buffer "should" have been created.
						stamp = event_time;
					} else {
						// okay, we're in one of the "live" performance run modes.  in these modes, we
						// stamp the buffer with the time at which the buffer should be rendered to the
						// output, not with the capture time.  mStartTime is the cached value of the
						// first buffer's performance time; we calculate this buffer's performance time as
						// an offset from that time, based on the amount of media we've created so far.
						// Recalculating every buffer like this avoids accumulation of error.
						stamp = startTime +
									bigtime_t(
										double(op->frameCount) /
										double(op->output.format.u.raw_audio.frame_rate) * 1000000.0);
					}
					hdr->start_time = stamp;
				} else {
					status_t	err = op->bufferGroup->RequestError();
					
					fprintf(stderr, "Sampler:: buffer rundown on output %d/%d (%s) of %d. Error, %s\n", i, output[i], op->insertName, nOutput, ErrorStr(err));
					return false;
				}
			} else {	// we're in the middle of filling a buf..
				media_header* hdr = op->buffer->Header();
				if (debug_media >= 2) {
					fprintf(stderr, "filling %d/%d of %d @byte %d, off %d sz %d\n", i, output[i], nOutput, hdr->size_used, op->offset, op->bufSize);
				}
				if (hdr->size_used < op->bufSize) {
					op->offset = hdr->size_used/op->sampleSize;
					hdr->size_used += framesPerEvent * op->frameSize;
				}
			}
		}
	}

#endif

// for the moment, we will just generate regardless of whether stopped or not
	for (short i=0; i<channels.CountItems(); i++) {
	// ! must check for SR change ... one day
		Channel	*chan = (Channel *)channels.ItemAt(i);
		int nf = chan->Generate(nFrames);
	}

#ifdef QUA_V_ARRANGER_INTERFACE
#ifdef _BEOS
	editors.Lock();
	for (short i=0; i<editors.CountItems(); i++) {
		SampleEditor	*edit = (SampleEditor *)editors.ItemAt(i);
		int nf = edit->Generate(qmo_temp,
								nFrames,
								samplesPerOutFrame);
	}
	editors.Unlock();
#endif
#endif
	return true;
}

QuaAudioIn *
QuaAudioManager::OpenInput(QuaAudioPort *p, short ch, short nch)
{
	QuaAudioIn	*con;
	con = InputConnectionForPort(p, ch);
	if (con == nullptr) {
//		con = new QuaAudioIn(uberQua, nch, p, ch);
//		if (con->Open(p) != B_OK) {
//			delete con;
//			return nullptr;
//		}
//		inputs.AddItem(con);
	}
	return con;
}

QuaAudioOut *
QuaAudioManager::OpenOutput(QuaAudioPort *p, short ch, short nch)
{
	QuaAudioOut	*con;
	con = OutputConnectionForPort(p, ch);
	if (con == nullptr) {
//		con = new QuaAudioOut(uberQua, nch, p, ch);
//		if (con->Open(p) != B_OK) {
//			delete con;
//			return nullptr;
//		}
//		outputs.AddItem(con);
	}
	return con;
}

status_t
QuaAudioManager::CloseInput(QuaAudioIn *c)
{
//	c->Close();
	return B_OK;
}

status_t
QuaAudioManager::CloseOutput(QuaAudioOut *c)
{
//	c->Close();
	return B_OK;
}

QuaAudioIn *
QuaAudioManager::InputConnectionForPort(QuaAudioPort *, short)
{
	return nullptr;
}

QuaAudioOut *
QuaAudioManager::OutputConnectionForPort(QuaAudioPort *, short)
{
	return nullptr;
}

status_t
QuaAudioManager::Connect(Input *s)
{
	long err = B_OK;
	if (s->device == nullptr) {
		return B_OK;
	}
	s->src.audio.port = nullptr;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		if ((err=asio.Load()) != B_OK) {
			return err;
		}
		s->src.audio.port = asio.EnableInput(s->channel->uberQua, s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			if ((err=asio.Load()) != B_OK) {
				return err;
			}
			s->src.audio.xport = asio.EnableInput(s->channel->uberQua, s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	if (s->src.audio.port == nullptr) {
		s->enabled = false;
		return err;
	} else {
		s->enabled = true;
	}
	return err;
}


char *
QuaAudioManager::ErrorString(status_t err)
{
#ifdef QUA_V_AUDIO_ASIO
	return asio.ErrorString(err);
#else
	return nullptr;
#endif
}

status_t
QuaAudioManager::Connect(Output *s)
{
	s->dst.audio.port = nullptr;
	long err = B_OK;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		if ((err=asio.Load()) != B_OK) {
			return err;
		}
		s->dst.audio.port = asio.EnableOutput(s->channel->uberQua, s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			if ((err=asio.Load()) != B_OK) {
				return err;
			}
			s->dst.audio.xport = asio.EnableOutput(s->channel->uberQua, s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	if (s->dst.audio.port == nullptr) {
		s->enabled = false;
		return err;
	}
	s->enabled = true;
	return err;
}

status_t
QuaAudioManager::Disconnect(Input *s)
{
	long err = B_OK;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		asio.DisableInput(s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			asio.DisableInput(s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	s->src.audio.xport = s->src.audio.port = nullptr;
	s->enabled = false;
	return err;
}

status_t
QuaAudioManager::Disconnect(Output *s)
{
	long err = B_OK;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		asio.DisableOutput(s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			asio.DisableOutput(s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	s->dst.audio.xport = s->dst.audio.port = nullptr;
	s->enabled = false;
	return err;
}

void
QuaAudioManager::AddSample(Sample *s)
{
	sampleLock.WriteLock();
	samples.AddItem(s);
	sampleLock.WriteUnlock();
    resume_thread(buffyThread);
}

/*
 * remove a sample from the buffer scrounging list
 * 
 */
void
QuaAudioManager::RemoveSample(Sample *s)
{
	sampleLock.WriteLock();
	samples.RemoveItem(s);
	sampleLock.WriteUnlock();
}

void
QuaAudioManager::StartInstance(Instance *i)
{
	fprintf(stderr, "start audio instance %s\n", i->sym->name);
	if (i->channel == nullptr)
		return;
	Channel	*c = i->channel;
	channelLock.Lock();
	if (!c->audioInstances.HasItem(i))
		c->audioInstances.AddItem(i);
	if (!channels.HasItem(c)) {
		channels.AddItem(c);
	}
	channelLock.Unlock();
}

void
QuaAudioManager::StopInstance(Instance *i)
{
	if (i->channel == nullptr)
		return;
	Channel	*c = i->channel;
	channelLock.Lock();
	c->audioInstances.RemoveItem(i);
	if (c->audioInstances.CountItems() == 0 && !(c->hasAudio&AUDIO_HAS_PLAYER)) {
		channels.RemoveItem(c);
	}
    channelLock.Unlock();
}

void
QuaAudioManager::StartChannel(Channel *c)
{
	channelLock.Lock();
	if (!channels.HasItem(c)) {
		channels.AddItem(c);
	}
    channelLock.Unlock();
}

status_t
QuaAudioManager::StartRecording(SampleInstance *ri)
{
	status_t	err = B_NO_ERROR;
	if (ri == nullptr || ri->channel == nullptr) {
		return B_ERROR;
	}

	recordInstanceLock.Lock();
	for (short i=0; i<recordInstances.CountItems(); i++) {
		if (ri->QSample() == ((SampleInstance*)recordInstances.ItemAt(i))->QSample()) {
			recordInstanceLock.Unlock();
			return B_ERROR;
		}
	}
	
	Sample	*sam = ri->QSample();

	SampleTake *recdTake = nullptr;
	if (sam != nullptr) {
		recdTake = sam->AddRecordTake(
						SampleFile::WAVE_TYPE,
						ri->channel->nAudioIns,
						4, 44100.0);
		if (recdTake == nullptr) {
			::reportError("Output file not initialised: %s", ErrorStr(err));
		} else {
			sam->recordTake->file->SeekToFrame(0);
			ri->QSample()->status = STATUS_RECORDING;

			recordInstances.AddItem(ri);

			if (recordInstances.CountItems() >= 1) {
				resume_thread(writerThread);
			}
		}
	}
	
	recordInstanceLock.Unlock();

	return B_NO_ERROR;
}

void
QuaAudioManager::StopRecording(SampleInstance *ri)
{
	recordInstanceLock.Lock();
	recordInstances.RemoveItem(ri);		// no more flushing from main writer

	if (recordInstances.CountItems() == 0) {
		;
	}

	Sample	*sam = ri->QSample();
#if defined(WIN32)
	Sleep(10);
#elif defined(_BEOS)		
	snooze(10000);
#endif
	sam->FlushRecordBuffers(true);
	ri->endFrame = sam->recordTake->file->nFrames;
	sam->status = STATUS_RUNNING;
	SampleTake		*newest = sam->recordTake;
	sam->recordTake = nullptr;
	if (sam->SampleClip(0)->media == nullptr) {
		sam->SelectTake(newest, true);
	}
	fprintf(stderr, "Sampler: stopped recording\n");
	recordInstanceLock.Unlock();
}

#ifdef _BEOS
status_t
QuaAudioManager::RemovePort(QuaAudioPort *ap)
{
	if (ap) {
		ports.RemoveItem(ap);
		glob.DeleteSymbol(ap->sym);
//		delete ap;
	}
	
	return B_OK;
}
#endif

#ifdef _BEOS
QuaAudioPort *
QuaAudioManager::AddPort(char *nm, media_node *nip)
{
	QuaAudioPort *ap=nullptr;

//	if (patchBay->Window())
//		patchBay->Looper()->Lock();
	ap = new QuaAudioPort(
				nm, this,
				nip);		
//	patchBay->atX = BPoint(patchBay->lastRB.x+5,
//							patchBay->atX.y);
//	if (patchBay->Window())
//		patchBay->Looper()->Unlock();
	ports.AddItem(ap);
	return ap;
}
#endif

#ifdef _BEOS
#ifdef NEW_MEDIA

QuaAudioPort *
QuaAudioManager::AudioPortForNode(media_node_id ni)
{
	for (short i=0; i<ports.CountItems(); i++) {
		QuaAudioPort	*a = (QuaAudioPort *)ports.ItemAt(i);
		if (a->mediaNode.node == ni)
			return a;
	}
	return nullptr;
}

QuaAudioPort *
QuaAudioManager::AudioPortForNodePort(port_id ni)
{
	for (short i=0; i<ports.CountItems(); i++) {
		QuaAudioPort	*a = (QuaAudioPort *)ports.ItemAt(i);
		if (a->mediaNode.port == ni)
			return a;
	}
	return nullptr;
}


char *
QuaAudioManager::MyKind(BParameter *p)
{
	const char *k = p->Kind();
	if (strcmp(k, B_WEB_PHYSICAL_INPUT) == 0) {		/* a jack on the back of the card */
		return (char *)  B_WEB_PHYSICAL_INPUT;
	} else if (strcmp(k, B_WEB_PHYSICAL_OUTPUT) == 0) {
		return (char *) B_WEB_PHYSICAL_OUTPUT;
	} else if (strcmp(k, B_WEB_ADC_CONVERTER) == 0) {		/* from analog to digital signals */
		return (char *) B_WEB_ADC_CONVERTER;
	} else if (strcmp(k, B_WEB_DAC_CONVERTER) == 0) {		/* from digital to analog signals */
		return (char *) B_WEB_DAC_CONVERTER;
	} else if (strcmp(k, B_WEB_LOGICAL_INPUT) == 0) {		/* an "input" that may not be physical */
		return (char *) B_WEB_LOGICAL_INPUT;
	} else if (strcmp(k, B_WEB_LOGICAL_OUTPUT) == 0) {
		return (char *) B_WEB_LOGICAL_OUTPUT;
	} else if (strcmp(k, B_WEB_BUFFER_INPUT) == 0) {		/* an input that corresponds to a media_input */
		return (char *) B_WEB_BUFFER_INPUT;
	} else if (strcmp(k, B_WEB_BUFFER_OUTPUT) == 0) {
		return (char *) B_WEB_BUFFER_OUTPUT;

	} else if (strcmp(k, B_MASTER_GAIN) == 0) {	/* Main Volume */
		return (char *) B_MASTER_GAIN;
	} else if (strcmp(k, B_GAIN) == 0) {
		return (char *) B_GAIN;
	} else if (strcmp(k, B_BALANCE) == 0) {
		return (char *) B_BALANCE;
	} else if (strcmp(k, B_FREQUENCY) == 0) {	/* like a radio tuner */
		return (char *) B_FREQUENCY;
	} else if (strcmp(k, B_LEVEL) == 0) {		/* like for effects */
		return (char *) B_LEVEL;
	} else if (strcmp(k, B_SHUTTLE_SPEED) == 0) {	/* Play, SloMo, Scan 1.0 == regular */
		return (char *) B_SHUTTLE_SPEED;
	} else if (strcmp(k, B_CROSSFADE) == 0) {		/* 0 == first input, +100 == second input */
		return (char *) B_CROSSFADE;
	} else if (strcmp(k, B_EQUALIZATION) == 0) {	/* depth (dB) */
		return (char *) B_EQUALIZATION;
		
	} else if (strcmp(k, B_COMPRESSION) == 0) {	/* 0% == no compression, 99% == 100:1 compression */
		return (char *) B_COMPRESSION;
	} else if (strcmp(k, B_QUALITY) == 0) {		/* 0% == full compression, 100% == no compression */
		return (char *) B_QUALITY;
	} else if (strcmp(k, B_BITRATE) == 0) {		/* in bits/second */
		return (char *) B_BITRATE;
	} else if (strcmp(k, B_GOP_SIZE) == 0) {	/* Group Of Pictures. a k a "Keyframe every N frames" */
		return (char *) B_GOP_SIZE;
	} else if (strcmp(k, B_GENERIC) == 0) {
		return (char *) B_GENERIC;

	} else if (strcmp(k, B_MUTE) == 0) {		/* 0 == thru, 1 == mute */
		return (char *) B_MUTE;
	} else if (strcmp(k, B_ENABLE) == 0) {		/* 0 == disable, 1 == enable */
		return (char *) B_ENABLE;
	} else if (strcmp(k, B_INPUT_MUX) == 0) {	/* "value" 1-N == input selected */
		return (char *) B_INPUT_MUX;
	} else if (strcmp(k, B_OUTPUT_MUX) == 0) {	/* "value" 1-N == output selected */
		return (char *) B_OUTPUT_MUX;
	} else if (strcmp(k, B_TUNER_CHANNEL) == 0) {		/* like cable TV */
		return (char *) B_TUNER_CHANNEL;
	} else if (strcmp(k, B_TRACK) == 0) {		/* like a CD player; "value" should be 1-N */
		return (char *) B_TRACK;
	} else if (strcmp(k, B_RECSTATE) == 0) {	/* like mutitrack tape deck, 0 == silent, 1 == play, 2 == record */
		return (char *) B_RECSTATE;
	} else if (strcmp(k, B_SHUTTLE_MODE) == 0) {	/* -1 == backwards, 0 == stop, 1 == play, 2 == pause/cue */
		return (char *) B_SHUTTLE_MODE;
	} else if (strcmp(k, B_RESOLUTION) == 0) {
		return (char *) B_RESOLUTION;
	} else if (strcmp(k, B_COLOR_SPACE) == 0) {		/* "value" should be color_space */
		return (char *) B_COLOR_SPACE;
	} else if (strcmp(k, B_FRAME_RATE) == 0) {
		return (char *) B_FRAME_RATE;
	} else if (strcmp(k, B_VIDEO_FORMAT) == 0) {	/* 1 == NTSC-M, 2 == NTSC-J, 3 == PAL-BDGHI, 4 == PAL-M, 5 == PAL-N, 6 == SECAM, 7 == MPEG-1, 8 == MPEG-2 */
		return (char *) B_VIDEO_FORMAT;
	} else {
		return 0;
	}
}


void
QuaAudioManager::MessageReceived(BMessage *inMsg)
{
	status_t	err;
	
	switch (inMsg->what) {

	case B_MEDIA_NODE_CREATED: {		/* "media_node_id" (multiple items) */
		fprintf(stderr, "node created\n");
		BMediaRoster		*r = BMediaRoster::Roster();
		short 				cnt=0;
		media_node_id		ni;

		while ((err=inMsg->FindInt32("media_node_id", cnt, &ni)) == B_NO_ERROR) {
			QuaAudioPort	*ap = AudioPortForNode(ni);
			if (ap == nullptr) {
				media_node		liveOne;
				live_node_info	liveInfo;
				
				if ((err=r->GetNodeFor(ni, &liveOne)) == B_NO_ERROR) {
					media_output			outputs[15];
					media_input				inputs[15];
					media_node_attribute	attribs[40];
					int32					nAttribs;
					int32					nConOuts, nConIns, nFreeOuts, nFreeIns;
					bool					isAudio=false;
					
					if (liveOne.kind != B_TIME_SOURCE) {
						if ((err=r->GetLiveNodeInfo(
									liveOne, &liveInfo)) == B_NO_ERROR) {
							fprintf(stderr, "adding port %s\n", liveInfo.name);
							AddPort(liveInfo.name, &liveOne);
							fprintf(stderr, "node create handled\n");
						} else {
							fprintf(stderr, "node %d: can't get live node info: %s\n", liveOne.node, ErrorStr(err));
							r->ReleaseNode(liveOne);	// not released by port
						}
					}
					
				}
// ... kept and deleted by the port
//				r->ReleaseNode(liveOne);
			}
			cnt++;
		}
		break;
	}
	
	case B_MEDIA_NODE_DELETED: {	/* "media_node_id" (multiple items) */
		BMediaRoster		*r = BMediaRoster::Roster();
		short 				cnt=0;
		media_node_id		ni;

			
		RosterView	*patchBay = uberQua->sequencerWindow->aquarium;
		while ((err=inMsg->FindInt32("media_node_id", cnt, &ni)) == B_NO_ERROR) {
			QuaAudioPort	*ap = AudioPortForNode(ni);
			if (ap != nullptr) {
				PortObject		*p = (PortObject *)ap->representation;
				RemovePort(ap);
				BMessage		delMsg(B_MEDIA_NODE_DELETED);
				delMsg.AddPointer("port object", p);
				patchBay->Window()->PostMessage(&delMsg, patchBay);
			} else {
				fprintf(stderr, "Qua: non-existent node deleted\n");
			}
			cnt++;
		}
		break;
	}

	case B_MEDIA_CONNECTION_MADE: {	/* "output", "input", "format" */
		QuaAudioPort	*op=nullptr, *ip=nullptr;
		QuaInsert		*outins=nullptr, *inins=nullptr;
		if (  inMsg->HasPointer("input insert") &&
			  inMsg->HasPointer("output insert")) {
			fprintf(stderr, "forwarding connection message\n");

			inMsg->FindPointer("output insert", (void **)&outins);
			inMsg->FindPointer("input insert", (void **)&inins);
			if (outins) {
				op = (QuaAudioPort *)outins->object->executable;
			}
			if (inins) {
				ip = (QuaAudioPort *)inins->object->executable;
			}
			
			if (op) {
				fprintf(stderr, "zot out %x...\n", op);
				op->ZotInsertCheck(false, true, false, false);
				fprintf(stderr, "done insert check...\n", op);
			}
			if (ip) {
				fprintf(stderr, "zot in %x..\n", ip);
				ip->ZotInsertCheck(true, false, false, false);
				fprintf(stderr, "done insert check...\n", op);
			}
			
//			inMsg->AddInt32("channels", output.format.u.raw_audio.channel_count);
			RosterView	*patchBay = uberQua->sequencerWindow->aquarium;
			patchBay->Window()->PostMessage(inMsg, patchBay);
		} else {
			media_output	output,*outp;
			media_input		input,*inp;
			media_format	f;
			long			len;
			if ((err=inMsg->FindData("output", B_RAW_TYPE, (const void **)&outp, &len))
						!= B_NO_ERROR)
				break;
			if ((err=inMsg->FindData("input", B_RAW_TYPE, (const void **)&inp, &len))
						!= B_NO_ERROR)
				break;
			if ((err=inMsg->FindData("format", B_RAW_TYPE, (const void **)&f, &len))
						!= B_NO_ERROR)
				break;
			output = *outp;
			input = *inp;
			op = AudioPortForNode(output.node.node);
			ip = AudioPortForNode(input.node.node);
			
			if (op) {
				fprintf(stderr, "zot out of %s...\n", op->sym->name);
				op->ZotInsertCheck(false, true, false, false);
			}
			if (ip) {
				fprintf(stderr, "zot in of %s...\n", ip->sym->name);
				ip->ZotInsertCheck(true, false, false, false);
			}
			
			if (op && ip) {
				inins = ip->FindInsert(
										inp->name, inp->destination.id, INPUT_INSERT);
				outins = op->FindInsert(
										outp->name, outp->source.id, OUTPUT_INSERT);
				if (outins && inins) {
					BMessage	conMsg(B_MEDIA_CONNECTION_MADE);
					conMsg.AddPointer("output object", op->PortObjectView());
					conMsg.AddPointer("output insert", outins);
					conMsg.AddPointer("input object", ip->PortObjectView());
					conMsg.AddPointer("input insert", inins);
					conMsg.AddInt32("channels", output.format.u.raw_audio.channel_count);
					fprintf(stderr, "audio connection made, %d channels\n", conMsg.FindInt32("channels"));
					RosterView	*patchBay = uberQua->sequencerWindow->aquarium;
					patchBay->Window()->PostMessage(&conMsg, patchBay);
				} else {
					fprintf(stderr, "Qua: connection between non inserts\n");
				}
			} else {
				fprintf(stderr, "Qua: connection between non nodes\n");
			}
		}
		break;
	}
	
	case B_MEDIA_CONNECTION_BROKEN:	{/* "source", "destination" */
		fprintf(stderr, "connection broken\n");
		QuaAudioPort	*op=nullptr, *ip=nullptr;
		QuaInsert		*outins=nullptr, *inins=nullptr;
		if (  inMsg->HasPointer("input insert") &&
			  inMsg->HasPointer("output insert")) {
			inMsg->FindPointer("output insert", (void **)&outins);
			inMsg->FindPointer("input insert", (void **)&inins);
			if (outins)
				op = (QuaAudioPort *)outins->object->executable;
			if (inins)
				ip = (QuaAudioPort *)inins->object->executable;
			if (op) {
				fprintf(stderr, "zot out...\n");
				op->ZotInsertCheck(false, true, false, true);
			}
			if (ip) {
				fprintf(stderr, "zot in...\n");
				ip->ZotInsertCheck(true, false, false, true);
			}
			
			fprintf(stderr, "forwarding break message\n");
			RosterView	*patchBay = uberQua->sequencerWindow->aquarium;
			patchBay->Window()->PostMessage(inMsg, patchBay);
		} else {
			media_source		output,*outp;
			media_destination	input,*inp;
			long				len;
			if ((err=inMsg->FindData("source", B_RAW_TYPE, (const void **)&outp, &len))
						!= B_NO_ERROR)
				break;
			if ((err=inMsg->FindData("destination", B_RAW_TYPE, (const void **)&inp, &len))
						!= B_NO_ERROR)
				break;
			output = *outp;
			input = *inp;
			op = AudioPortForNodePort(output.port);
			ip = AudioPortForNodePort(input.port);
			
			if (op) {
				fprintf(stderr, "zot out...\n");
				op->ZotInsertCheck(false, true, false, true);
				outins = op->FindInsert(nullptr, output.id, OUTPUT_INSERT);
			}
			if (ip) {
				fprintf(stderr, "zot in...\n");
				ip->ZotInsertCheck(true, false, false, true);
				inins = ip->FindInsert(nullptr, input.id, INPUT_INSERT);
			}
			if (inins || outins) {
				BMessage	disMsg(B_MEDIA_CONNECTION_BROKEN);
				disMsg.AddPointer("output insert", outins);
				disMsg.AddPointer("input insert", inins);
				RosterView	*patchBay = uberQua->sequencerWindow->aquarium;
				patchBay->Window()->PostMessage(&disMsg, patchBay);
			} else {
				fprintf(stderr, "Qua: disconnect a non connection\n");
			}
		}
		break;
	}
		
	case B_MEDIA_WEB_CHANGED:		/* N "node" */
		fprintf(stderr, "web changed\n");
		break;
	case B_MEDIA_PARAMETER_CHANGED:	/* N "node", "parameter" */
		fprintf(stderr, "media parameter changed\n");
		break;
	case B_MEDIA_NEW_PARAMETER_VALUE:	/* N "node", "parameter", "when", "value" */
		fprintf(stderr, "media parameter value changed\n");
		break;
	case B_MEDIA_FORMAT_CHANGED:		/* N "source", "destination", "format" */
		break;
	case B_MEDIA_TRANSPORT_STATE:	/* "state", "location", "realtime" */
		break;
	case B_MEDIA_DEFAULT_CHANGED:	/* "default", "node" -- handled by BMediaRoster */
		break;
	case B_MEDIA_NODE_STOPPED:	/* N "node", "when" */
		break;
	case B_MEDIA_WILDCARD:		/* used to match any notification in Start/StopWatching */
		break; 
	case B_MEDIA_BUFFER_CREATED:		/* "clone_info" -- handled by BMediaRoster */
		break;
	case B_MEDIA_BUFFER_DELETED:		/* "media_buffer_id" -- handled by BMediaRoster */
		break;
	default:
		BHandler::MessageReceived(inMsg);
	}	
}

#endif

void
QuaAudioManager::SetParameterBounds(BParameter *p, TypedValue *min, TypedValue *ini, TypedValue *max)
{
	fprintf(stderr, "p %x\n", p);
#ifdef NEW_MEDIA
	char	*k = MyKind(p);
	if (	k == B_GAIN
	  ||	k == B_MASTER_GAIN) {
		*min = TypedValue::Float(0);
		*ini = TypedValue::Float(10);
		*max = TypedValue::Float(11);
	} else if (	k == B_CROSSFADE
	  ||		k == B_BALANCE) {
		*min = TypedValue::Float(-100);
		*ini = TypedValue::Float(0);
		*max = TypedValue::Float(100);
	} else if (k == B_EQUALIZATION) {
		*min = TypedValue::Float(-100);
		*ini = TypedValue::Float(0);
		*max = TypedValue::Float(100);
	} else if (	k == B_COMPRESSION
			||  k == B_QUALITY) {
		*min = TypedValue::Float(0);
		*ini = TypedValue::Float(50);
		*max = TypedValue::Float(99);
	} else if (	k == B_BITRATE) {
		*min = TypedValue::Float(1000);
		*ini = TypedValue::Float(50000);
		*max = TypedValue::Float(1000000);
	} else if (	k == B_FREQUENCY) {
		*min = TypedValue::Float(10);
		*ini = TypedValue::Float(256);
		*max = TypedValue::Float(20000);
	} else {
		*min = TypedValue::Float(0);
		*ini = TypedValue::Float(1);
		*max = TypedValue::Float(11);
	}
#endif
}

status_t
QuaAudioManager::MapRoster()
{
	BMediaRoster			*r = BMediaRoster::Roster();
	live_node_info			liveNodes[50];
	int32					nodeCount;
	media_format			audioFormat;
	status_t				err;
	
	BMessage				createMsg(B_MEDIA_NODE_CREATED);
	
	audioFormat.type = B_MEDIA_RAW_AUDIO;
	audioFormat.u.raw_audio.format = media_raw_audio_format::B_AUDIO_FLOAT;
	audioFormat.u.raw_audio.channel_count = 2;
	audioFormat.u.raw_audio.frame_rate = 44100;		// measured in Hertz
	audioFormat.u.raw_audio.byte_order = (B_HOST_IS_BENDIAN) ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN;

	nodeCount = 50;

	err=r->GetLiveNodes(liveNodes, &nodeCount, nullptr, &audioFormat, nullptr, 0);//B_BUFFER_PRODUCER); //);

	for (short i=0; i<nodeCount; i++) {
		fprintf(stdout, "output node %d: node %d, port %d, name %s\n",
			i, liveNodes[i].node.node, liveNodes[i].node.port, liveNodes[i].name);
		createMsg.AddInt32("media_node_id", liveNodes[i].node.node);
	}
	
	nodeCount = 50;

	err=r->GetLiveNodes(liveNodes, &nodeCount, &audioFormat, nullptr, nullptr, 0);//B_BUFFER_PRODUCER); //);
	for (short i=0; i<nodeCount; i++) {
		fprintf(stdout, "input node %d: node %d, port %d, name %s\n",
			i, liveNodes[i].node.node, liveNodes[i].node.port, liveNodes[i].name);
		createMsg.AddInt32("media_node_id", liveNodes[i].node.node);
	}
	
	if (Looper())
		Looper()->PostMessage(&createMsg, this);

	return B_OK;
}


BMenu *
QuaAudioManager::DestinationMenu(
		char *nm,
		short channelCount,
		BHandler *tgt,
		Output *out)
{
	BMenu	*isMenu = new BMenu(nm);
	
#ifdef NEW_MEDIA
	BMediaRoster			*r = BMediaRoster::Roster();
//	int32					nodeCount;
//	media_format			audioFormat;
	status_t				err;
	
	Sampler					*s = uberQua->sampler;
	
	for (short i=0; i<s->nOutput; i++) {	
		fprintf(stderr, "out %d %d\n", i, s->output[i]);
		QuaAudioOut	*op = &s->outPort[s->output[i]];
		if (channelCount < 0 || op->nChannel == channelCount) {
			BMessage	*msg = new BMessage(SET_DESTINATION);
			msg->AddPointer("audio port", op->quaport);
			msg->AddPointer("qua output", op);
			msg->AddInt32("channels", channelCount);
			if (out) {
				msg->AddPointer("output", out);
			}
			BMenuItem	*o = new BMenuItem(op->insertName, msg);
			o->SetTarget(tgt);
			isMenu->AddItem(o);
		}
	}
	if (channelCount == 2) {	// make up paired mono stereo sets
		bool	used[MAX_QUA_AUDIO_OUTPUT];
		for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
			used[i] = false;
		}
		for (short i=0; i<s->nOutput; i++) {
			QuaAudioOut	*op = &s->outPort[s->output[i]];
			if (op->nChannel == 1 && !used[i]) {
				for (short j=i+1; j<s->nOutput; j++) {
					QuaAudioOut	*xp = &s->outPort[s->output[j]];
					if (xp->nChannel == 1
						  && op->output.node == xp->output.node) {
						used[s->output[i]] = used[j] = true;
						BMessage	*msg = new BMessage(SET_DESTINATION);
						msg->AddPointer("audio port", op->quaport);
						msg->AddPointer("qua output", op);
						msg->AddInt32("channels", channelCount);
						msg->AddPointer("qua xoutput", xp);
						if (out) {
							msg->AddPointer("output", out);
						}
						char	buf[128];
						char	*t = op->insertName;
						char	*s = xp->insertName;
						char	*u = t;
						while (s && *s && *u && *s == *u) {
							s++;
							u++;
						}
						sprintf(buf, "%s/%s", t,s);
						BMenuItem	*o =
								new BMenuItem(buf, msg);
						o->SetTarget(tgt);
						isMenu->AddItem(o);
					}
				}
			}
		}
	}
	
	isMenu->AddSeparatorItem();
				
//	audioFormat.type = B_MEDIA_RAW_AUDIO;
//	audioFormat.u.raw_audio.format = media_raw_audio_format::B_AUDIO_FLOAT;
//	audioFormat.u.raw_audio.channel_count = 2;
//	audioFormat.u.raw_audio.frame_rate = 44100;		// measured in Hertz
//	audioFormat.u.raw_audio.byte_order = (B_HOST_IS_BENDIAN) ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN;
//
//	nodeCount = 50;
//
//	err=r->GetLiveNodes(liveNodes, &nodeCount, &audioFormat, nullptr, nullptr, 0);//B_BUFFER_PRODUCER); //);

	for (short i=0; i<ports.CountItems(); i++) {
		QuaAudioPort	*a = (QuaAudioPort *)ports.ItemAt(i);
		media_input	inputs[32];
		long			nIns=0, nMono = 0;
		if ((err=r->GetFreeInputsFor(
						a->mediaNode, inputs,
						32, &nIns)<B_NO_ERROR)) {
			fprintf(stderr, "Can't get inputs to node %d %s\n", i, a->sym->name);
			break;
		}
//		} else {
//			if ((err=r->GetConnectedInputsFor(
//							a->mediaNode, inputs,
//							32, &nIns)<B_NO_ERROR)) {
//				fprintf(stderr, "Can't get outputs of node\n");
//				break;
//			}
//		}
		
		if (nIns > 0) {
			BMenu	*dMenu = nullptr;
			fprintf(stdout, "output %d: node %d, port %d, name %s\n",
				i, a->mediaNode.node, a->mediaNode.port, inputs[i].name);
			for (short j=0; j<nIns; j++) {
				if (inputs[j].format.type == B_MEDIA_RAW_AUDIO &&
					  (channelCount < 0 ||
					   inputs[j].format.u.raw_audio.channel_count
					   						== channelCount)) {
					BMessage	*msg = new BMessage(SET_DESTINATION);
					msg->AddPointer("audio port", a);
					msg->AddData("destination", B_RAW_TYPE, &inputs[j].destination, sizeof(media_destination));
					msg->AddData("format", B_RAW_TYPE, &inputs[j].format, sizeof(media_format));
					msg->AddInt32("channels", channelCount);
					if (out) {
						msg->AddPointer("output", out);
					}
					BMenuItem	*o = new BMenuItem(inputs[j].name, msg);
					o->SetTarget(tgt);
					if (dMenu == nullptr) {
						dMenu = new BMenu(a->sym->name);
					}
					dMenu->AddItem(o);
				}
				if (inputs[j].format.u.raw_audio.channel_count == 1) {
					nMono++;
				}
			}
			if (channelCount == 2 && nMono >= 2) {
				short	l=0, r=0;
				for (short j=0; j<nMono/2; j++) {
					while ( l<nIns &&
							(inputs[l].format.type != B_MEDIA_RAW_AUDIO ||
							 inputs[l].format.u.raw_audio.channel_count != 1)) {
						l++;
					}
					r = l+1;
					while ( r<nIns &&
							(inputs[r].format.type != B_MEDIA_RAW_AUDIO ||
							 inputs[r].format.u.raw_audio.channel_count != 1)) {
						r++;
					}
					BMessage	*msg = new BMessage(SET_DESTINATION);
					msg->AddPointer("audio port", a);
					msg->AddData("format", B_RAW_TYPE, &inputs[l].format, sizeof(media_format));
					msg->AddInt32("channels", channelCount);
					msg->AddData("destination", B_RAW_TYPE, &inputs[l].destination, sizeof(media_destination));
					msg->AddData("xdestination", B_RAW_TYPE, &inputs[r].destination, sizeof(media_destination));
					if (out) {
						msg->AddPointer("output", out);
					}
//					} else {
//						msg->AddData("used destination", B_RAW_TYPE, &inputs[l], sizeof(inputs[l]));
//						msg->AddData("used xdestination", B_RAW_TYPE, &inputs[r], sizeof(inputs[r]));
//					}

					if (dMenu == nullptr) {
						dMenu = new BMenu(a->sym->name);
					}
					char	buf[128];
					char	*s = inputs[l].name;
					char	*u = inputs[r].name;
					while (s && *s && *u && *s == *u) {
						s++;
						u++;
					}
					sprintf(buf, "%s/%s", inputs[l].name,u);
					BMenuItem	*o = new BMenuItem(buf, msg);
					o->SetTarget(tgt);
					dMenu->AddItem(o);
					l = r+1;
				}
			}
			if (dMenu) {
				isMenu->AddItem(dMenu);
			}
		}
	}
#endif
	return isMenu;
}
#endif

#ifdef QUA_V_ARRANGER_INTERFACE
#ifdef _BEOS
BMenu *
QuaAudioManager::SourceMenu(
		char *nm,
		short channelCount,
		BHandler *tgt,
		Input *in)
{
	BMenu	*isMenu = new BMenu(nm);
	
#ifdef NEW_MEDIA
	BMediaRoster			*r = BMediaRoster::Roster();
//	int32					nodeCount;
//	media_format			audioFormat;
	status_t				err;
	
	Sampler					*s = uberQua->sampler;
	
	for (short i=0; i<s->nInput; i++) {
		QuaAudioIn	*op = &s->inPort[s->input[i]];
		if (channelCount < 0 || op->nChannel == channelCount) {
			BMessage	*msg = new BMessage(SET_SOURCE);
			msg->AddPointer("audio port", op->quaport);
			msg->AddPointer("qua input", op);
			msg->AddInt32("channels", channelCount);
			if (in) {
				msg->AddPointer("input", in);
			}
			BMenuItem	*o = new BMenuItem(op->insertName, msg);
			o->SetTarget(tgt);
			isMenu->AddItem(o);
		}
	}
	if (channelCount == 2) {	// make up paired mono stereo sets
		bool	used[MAX_QUA_AUDIO_OUTPUT];
		for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
			used[i] = false;
		}
		for (short i=0; i<s->nInput; i++) {
			QuaAudioIn	*op = &s->inPort[s->input[i]];
			if (op->nChannel == 1 && !used[i]) {
				for (short j=i+1; j<s->nInput; j++) {
					QuaAudioIn	*xp = &s->inPort[s->input[j]];
					if (xp->nChannel == 1
						  && op->input.node == xp->input.node) {
						used[s->input[i]] = used[j] = true;
						BMessage	*msg = new BMessage(SET_SOURCE);
						msg->AddPointer("audio port", op->quaport);
						msg->AddPointer("qua input", op);
						msg->AddPointer("qua xinput", xp);
						msg->AddInt32("channels", channelCount);
						if (in) {
							msg->AddPointer("input", in);
						}
						char	buf[128];
						char	*t = op->insertName;
						char	*s = xp->insertName;
						char	*u = t;
						while (s && *s && *u && *s == *u) {
							s++;
							u++;
						}
						sprintf(buf, "%s/%s", t,s);
						BMenuItem	*o =
								new BMenuItem(buf, msg);
						o->SetTarget(tgt);
						isMenu->AddItem(o);
					}
				}
			}
		}
	}
	
	isMenu->AddSeparatorItem();
				
//	audioFormat.type = B_MEDIA_RAW_AUDIO;
//	audioFormat.u.raw_audio.format = media_raw_audio_format::B_AUDIO_FLOAT;
//	audioFormat.u.raw_audio.channel_count = 2;
//	audioFormat.u.raw_audio.frame_rate = 44100;		// measured in Hertz
//	audioFormat.u.raw_audio.byte_order = (B_HOST_IS_BENDIAN) ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN;
//
//	nodeCount = 50;
//
//	err=r->GetLiveNodes(liveNodes, &nodeCount, &audioFormat, nullptr, nullptr, 0);//B_BUFFER_PRODUCER); //);

	for (short i=0; i<ports.CountItems(); i++) {
		QuaAudioPort	*a = (QuaAudioPort *)ports.ItemAt(i);
		media_output	outputs[32];
		long			nOuts=0, nMono = 0;
		if ((err=r->GetFreeOutputsFor(
						a->mediaNode, outputs,
						32, &nOuts)<B_NO_ERROR)) {
			fprintf(stderr, "Can't get outputs of node\n");
			break;
		}
		
		if (nOuts > 0) {
			BMenu	*dMenu = nullptr;
//			fprintf(stdout, "output %d: node %d, port %d, name %s\n",
//				i, a->mediaNode.node.node, a->mediaNode.node.port, liveNodes[i].name);
			for (short j=0; j<nOuts; j++) {
				if (outputs[j].format.type == B_MEDIA_RAW_AUDIO &&
					  (channelCount < 0 ||
					   outputs[j].format.u.raw_audio.channel_count
					   						== channelCount)) {
					BMessage	*msg = new BMessage(SET_SOURCE);
					msg->AddPointer("audio port", a);
					msg->AddInt32("channels", channelCount);
					msg->AddData("source", B_RAW_TYPE, &outputs[j].source, sizeof(media_source));
					msg->AddData("format", B_RAW_TYPE, &outputs[j].format, sizeof(media_format));
					if (in) {
						msg->AddPointer("input", in);
					}
					BMenuItem	*o = new BMenuItem(outputs[j].name, msg);
					o->SetTarget(tgt);
					if (dMenu == nullptr) {
						dMenu = new BMenu(a->sym->name);
					}
					dMenu->AddItem(o);
				}
				if (outputs[j].format.u.raw_audio.channel_count == 1) {
					nMono++;
				}
			}
			if (channelCount == 2 && nMono >= 2) {
				short	l=0, r=0;
				for (short j=0; j<nMono/2; j++) {
					while ( l<nOuts &&
							(outputs[l].format.type != B_MEDIA_RAW_AUDIO ||
							 outputs[l].format.u.raw_audio.channel_count != 1)) {
						l++;
					}
					r = l+1;
					while ( r<nOuts &&
							(outputs[r].format.type != B_MEDIA_RAW_AUDIO ||
							 outputs[r].format.u.raw_audio.channel_count != 1)) {
						r++;
					}
					BMessage	*msg = new BMessage(SET_SOURCE);
					msg->AddPointer("audio port", a);
					msg->AddData("format", B_RAW_TYPE, &outputs[j].format, sizeof(media_format));
					msg->AddInt32("channels", channelCount);
					msg->AddData("source", B_RAW_TYPE, &outputs[l].source, sizeof(media_source));
					msg->AddData("xsource", B_RAW_TYPE, &outputs[r].source, sizeof(media_source));
					if (in) {
						msg->AddPointer("input", in);
					}

					if (dMenu == nullptr) {
						dMenu = new BMenu(a->sym->name);
					}
					char	buf[128];
					char	*s = outputs[l].name;
					char	*u = outputs[r].name;
					while (s && *s && *u && *s == *u) {
						s++;
						u++;
					}
					sprintf(buf, "%s/%s", outputs[l].name,u);
					BMenuItem	*o = new BMenuItem(buf, msg);
					o->SetTarget(tgt);
					dMenu->AddItem(o);
					l = r+1;
				}
			}
			if (dMenu) {
				isMenu->AddItem(dMenu);
			}
		}
	}
#endif
	return isMenu;
}

#else
bool
DestinationIndex(KeyIndex *, short nc)
{
	return true;
}

bool
SourceIndex(KeyIndex *, short nc)
{
	return true;
}


bool
DestinationChannelIndex(KeyIndex *, QuaAudioPort *, short nc)
{
	return true;
}


bool
SourceChannelIndex(KeyIndex *, QuaAudioPort *, short nc)
{
	return true;
}

#endif
#endif

#endif
//		media_node	liveOne;
//		err=r->GetNodeFor(liveNodes[i].node.node, &liveOne);
//		for (short j=0; j<nIns; j++) {
//			fprintf(stdout, "\tnode %d <= (%d %d)\n",
//				liveOne.node,
//				inCon[j].source.id,
//				inCon[j].source.port);
//		}
//		r->ReleaseNode(liveOne);
//		media_node	liveOne;
//		err=r->GetNodeFor(liveNodes[i].node.node, &liveOne);
//		if (err != B_NO_ERROR)
//			fprintf(stderr, "get node for error, %s\n", ErrorStr(err));
//		for (short j=0; j<nOuts; j++) {
//			fprintf(stdout, "\tnode %d => (%d %d)\n",
//				liveOne.node,
//				outCon[j].destination.id,
//				outCon[j].destination.port
//				);
//		}
//		r->ReleaseNode(liveOne);

//						if (( (err=r->GetConnectedInputsFor(
//									liveOne, inputs,
//									15, &nConIns))< B_NO_ERROR)
//							||(err=r->GetFreeInputsFor(
//									liveOne, inputs+nConIns,
//									15-nConIns, &nFreeIns)< B_NO_ERROR)
//							||(err=r->GetConnectedOutputsFor(
//									liveOne, outputs,
//									15, &nConOuts)< B_NO_ERROR)
//							||(err=r->GetFreeOutputsFor(
//									liveOne, outputs+nConOuts,
//									15-nConIns, &nFreeOuts)<B_NO_ERROR)) {
//							fprintf(stderr, "%s: get input/output connection error, %s\n", liveInfo.name, ErrorStr(err));
//						} else {
//							for (short i=0; i<nConIns+nFreeIns; i++) {
//								if (inputs[i].format.IsAudio()) {
//									isAudio=true;
//								}
//							}					
//							for (short i=0; i<nConOuts+nFreeOuts; i++) {
//								if (outputs[i].format.IsAudio()) {
//									isAudio=true;
//								}
//							}
//						}					
//	
//	//					nAttribs=r->GetNodeAttributesFor(
//	//								liveOne, attribs, 40);
//	//					if (nAttribs < B_NO_ERROR) {
//	//						fprintf(stderr, "%s: get attribute error, %s\n", liveInfo.name, ErrorStr(nAttribs));
//	//					} else if (nAttribs == 0) {
//	//						fprintf(stderr, "%s: no attributes to speak of\n", liveInfo.name);
//	//					} else {
//	//						for (short i=0; i<nAttribs; i++) {
//	//							fprintf(stderr, "%s: at %d %x %Lx\n",
//	//								liveInfo.name, attribs[i].what, attribs[i].flags, attribs[i].data);
//	//						}
//	//					}
//						if (isAudio) {
//						} else {
//							fprintf(stderr, "%s: probably not an audio node\n", liveInfo.name);
//							r->ReleaseNode(liveOne); // not released by port
//						}
