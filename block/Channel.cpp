#include "qua_version.h"

#include "tinyxml2.h"

#include "StdDefs.h"

#include "Channel.h"
#include "ControllerBridge.h"
#include "Stream.h"
#include "QuaMidi.h"
#include "QuaOSC.h"
#include "Qua.h"
#include "QuaEnvironment.h"
#include "Destination.h"
#include "Sym.h"
#include "Block.h"
#include "Sample.h"
#include "Sampler.h"
#include "Pool.h"
#include "FloatQueue.h"
#include "QuaAudio.h"
#include "QuasiStack.h"
#include "QuaFX.h"
#include "SampleBuffer.h"

#ifdef QUA_V_JOYSTICK
#include "QuaJoystick.h"
#endif

#ifdef QUA_V_ARRANGER_INTERFACE

#include "QuaDisplay.h"

#endif

flag	debug_chan = 0;

//static float	qmo_temp[8192];
//	 	audioRX(this, ain, 0, false),
//		audioTX(this, aout, 0, true),
//	 	streamRX(this, sin, mdi_chan, true),
//		streamTX(this, sout, mdi_chan, true),
//	AddChild(&audioRX);
//	AddChild(&audioTX);
//	AddChild(&streamRX);
//	AddChild(&streamTX);
//	ArrangeChildren();
//					QuaPort *sin, QuaPort *sout,
//					QuaPort *ain, QuaPort *aout,


Channel::Channel(std::string nm, short ch_id,
					bool athr, bool mthr, short nic, short noc, Qua *uq):	
		Stackable(DefineSymbol(nm, TypedValue::S_CHANNEL, 0,
					this, uq->sym,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT)),
		Stacker(uq),
		chanId(ch_id),
		rx("rx", sym),
		tx("tx", sym)
{
	status = STATUS_SLEEPING;
//	interfaceBridge.SetSymbol(sym);	
    audioThru = athr;
	midiThru = mthr;
    inEnabled = true;
    outEnabled = true;

	streamRecordInstance = nullptr;
	audioRecordInstance = nullptr;
	recordState = RECORD_NOT;
	reRecord = false;
	nAudioOuts = noc;
	nAudioIns = nic;
	initBlock = nullptr;

#ifdef QUA_V_AUDIO
	gain = 1;
	DefineSymbol("gain", TypedValue::S_FLOAT, 0,
					&gain, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	pan = 0;
	if (nAudioOuts == 2) {
		DefineSymbol("pan", TypedValue::S_FLOAT, 0,
					&pan, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	}
	DefineSymbol("audioThru", TypedValue::S_BOOL, audioThru,
					&audioThru, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
#endif
	DefineSymbol("midiThru", TypedValue::S_BOOL, midiThru,
					&midiThru, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	DefineSymbol("inEnable", TypedValue::S_BOOL, inEnabled,
					&inEnabled, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	DefineSymbol("outEnable", TypedValue::S_BOOL, outEnabled,
					&outEnabled, sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);

// ????? should pull this from the audio subsystem, though we may not
// ????? yet have instantiated it.
	inSig = sample_buf_alloc(nAudioIns, QUA_MAX_AUDIO_BUFSIZE);
	instSig = sample_buf_alloc(nAudioOuts, QUA_MAX_AUDIO_BUFSIZE);
	outSig = sample_buf_alloc(nAudioOuts, QUA_MAX_AUDIO_BUFSIZE);
	
//	RxSym = DefineSymbol("rx", TypedValue::S_DESTINATION, &Receive, sym, 0,
//					false, false, false, true, StabEnt::DISPLAY_CTL);
//	TxSym = DefineSymbol("tx", TypedValue::S_DESTINATION, &Transmit, sym, 0,
//					false, false, false, true, StabEnt::DISPLAY_CTL);
//	ThruSym = DefineSymbol("thru", TypedValue::S_BOOL, &Thru, sym, -1,
//					false, false, false, true, StabEnt::DISPLAY_CTL);

	

	parameters = nullptr;
//	if (controlInfoList) {
//		parameters = new QuaControllerBridge(uberQua, 0, nullptr, controlInfoList);
//	}
	rx.block = nullptr;
	tx.block = nullptr;
	
	txStack = new QuasiStack(tx.sym, this, sym, nullptr, nullptr, uberQua->theStack, uberQua, "Input");
	rxStack = new QuasiStack(rx.sym, this, sym, nullptr, nullptr, uberQua->theStack, uberQua, "Output");
// setting a block to tx or rx will force the actual data area of this
// to be checked ... CheckMulchSize() called by parser within ParseDefine()
// note that if the channel has local variables, and this is conceivable and untested

}



// initialise the basic state of the channel, maybe open up a few
// devices by enabling input/output
bool
Channel::Init()
{
	fprintf(stderr, "Channel::initialise %s\n", sym->name.c_str());
	hasAudio = txStack->hasAudio|rxStack->hasAudio;
#ifdef QUA_V_AUDIO
	if (hasAudio&AUDIO_HAS_PLAYER) {
		fprintf(stderr, "Channel::initialise starting audio for player channel\n");

		getAudioManager()->startChannel(this);
	}
#endif
	if (initBlock) {
		;
	}
	for (Input *s: inputs) {
		s->Init();
		Enable(s, s->enabled!=0);
	}
	for (Output *s: outputs) {
		s->Init();
		Enable(s, s->enabled!=0);
	}
	if (initBlock) {
		;
	}
	if (tx.block && !tx.block->StackOMatic(txStack, 3)) {
		uberQua->bridge.reportError("can't build stack for channel");
	}
	if (rx.block && !rx.block->StackOMatic(rxStack, 3)) {
		uberQua->bridge.reportError("can't build stack for channel");
	}
	status = STATUS_RUNNING;
	return true;
}

Channel::~Channel()
{
//	if (Window()) {
//		Window()->Lock();
//		RemoveSelf();
//		Window()->Unlock();
//	}
	
	fprintf(stderr, "deleting channel %s\n", sym->name.c_str());
	if (rx.block) {
		rx.block->DeleteAll();
		rx.block = nullptr;
	}

	if (tx.block) {
		tx.block->DeleteAll();
		tx.block = nullptr;
	}
	if (rxStack) {
		delete rxStack;
		rxStack = nullptr;
	}
	
	if (txStack) {
		delete txStack;
		txStack = nullptr;
	}

	if (initBlock) {
		initBlock->DeleteAll();
		initBlock = nullptr;
	}
	
	sample_buf_free(inSig, nAudioIns);
	sample_buf_free(instSig, nAudioOuts);
	sample_buf_free(outSig, nAudioOuts);

}



//long
//Channel::Control()
//{
//	if (controlWindow == nullptr) {
//   		BRect		rect(200, 200, 400, 400);
//   		controlWindow = new ChannelPanel(rect, this);
//   	} else {
//   		controlWindow->Activate();
//   	}
//	return B_NO_ERROR;
//}

void
Channel::EnableIn(bool in)
{
	if (inEnabled != in) {
		inEnabled = in;
	}
}

void
Channel::EnableOut(bool in)
{
	if (outEnabled != in) {
		outEnabled = in;
	}
}

void
Channel::EnableAudioThru(bool in)
{
	if (audioThru != in) {
		audioThru = in;
		fprintf(stderr, "audio thru = %d\n", audioThru);
	}
}

void
Channel::EnableMidiThru(bool in)
{
	if (midiThru != in) {
		midiThru = in;
		fprintf(stderr, "midi thru = %d\n", midiThru);
	}
}



void
Channel::Stop()
{
	status = STATUS_SLEEPING;
}

void
Channel::Start()
{
	status = STATUS_RUNNING;
}


void
Channel::OutputStream(Stream *S)
{
	if (S->nItems > 0 && status == STATUS_RUNNING) {

#ifdef ASYNC_CHAN_OUT
		outMutex.Lock();
#endif
	    outStream.AddToStream(S);
#ifdef ASYNC_CHAN_OUT
		outMutex.Unlock();
#endif
	}
	
//	CheckBuffers();

}

void
Channel::InputStream(Stream *S)
{
	if (status == STATUS_RUNNING) {
#ifdef ASYNC_CHAN_IN
		inMutex.Lock();
#endif
	    S->AddToStream(&inStream);
#ifdef ASYNC_CHAN_IN
		inMutex.Unlock();
#endif
	}
}

void
Channel::CheckOutBuffers()
{
#ifdef ASYNC_CHAN_OUT
	outMutex.Lock();
#endif
	
	if (outStream.nItems > 0 && status == STATUS_RUNNING) {
// need to make sure the processing stacks and blocks don't get fucked with
//		uberQua->objectsBlockStack.ReadLock();
		if (tx.block) {
			Time	tag_time = uberQua->theTime;
			flag	uac = UpdateActiveBlock(
							uberQua,
							&outStream,
							tx.block,
							&tag_time,
							this,
							sym,
							txStack,
							1,
							true);
		}
//		uberQua->objectsBlockStack.ReadUnlock();
		
		for (Output *d: activeStreamOutputs) {
			d->OutputStream(
						uberQua->theTime,
//						uberQua->secsPerTick,
						&outStream);
		}
		outStream.ClearStream();
	}
#ifdef ASYNC_CHAN_OUT
	outMutex.Unlock();
#endif
}

void
Channel::ClearOutBuffers(bool Clr)
{
	if (outStream.nItems > 0 && status == STATUS_RUNNING) {
#ifdef ASYNC_CHAN_OUT
		outMutex.Lock();
#endif
		for (Output *o: activeStreamOutputs) {
			o->ClearStream(&outStream);
		}
		outStream.ClearStream();
#ifdef ASYNC_CHAN_OUT
		outMutex.Unlock();
#endif
	}
}

bool
Channel::ItemsAvailable()
{
	for (Input*i: activeStreamInputs) {
		if (i->HasStreamItems())
			return true;
	}
	return	false;
}

void
Channel::CheckInBuffers()
{
	if (ItemsAvailable() && status == STATUS_RUNNING) {
#ifdef ASYNC_CHAN_IN
		inMutex.Lock();
#endif
		Stream		recvStream;
		
		for (Input *i: activeStreamInputs) {
			// get stream items
			i->GetStreamItems(&recvStream);
			fprintf(stderr, "Channel %s, receiving %d\n", sym->name.c_str(), recvStream.nItems);
		}
		
		if (recvStream.nItems > 0) {
// just need to make sure that the processing units don't get modified.
//			uberQua->objectsBlockStack.ReadLock();
			if (rx.block) {
				Time	tag_time = uberQua->theTime;
				flag	uac = UpdateActiveBlock(
								uberQua,
								&recvStream,
								rx.block,
								&tag_time,
								this,
								sym,
								rxStack,
								1,
								true);
			}
//			uberQua->objectsBlockStack.ReadUnlock();
//
			bool	outalready=false;
			if (receivingInstances.size() > 0) {
				receivingLock.lock();
				for (Instance *inst: receivingInstances) {
					Stream	inthru;
					inthru.AddToStream(&recvStream, nullptr);
					inst->Recv(inthru);
					if (inst->thruEnabled) {
						OutputStream(&inthru);
						outalready = true;
					} else {
						inthru.ClearStream();
					}
				}
				receivingLock.unlock();
			}
			if (midiThru && ! outalready) {	// copy to output stream
				OutputStream(&recvStream);
			}
			CheckRecord(&recvStream);

			inStream.AppendStream(&recvStream);
		}
#ifdef ASYNC_CHAN_IN
		inMutex.Unlock();
#endif
	}
}


#ifdef QUA_V_ARRANGER_INTERFACE
void
Channel::UpdateRecordDisplay()
{
	if (  recordState == RECORD_ING) {
		if (audioRecordInstance ) {
			Time dur = uberQua->theTime - recordStarted;
			audioRecordInstance->SetDuration(dur, true);
		}
		if (streamRecordInstance) {
			Time dur = uberQua->theTime - recordStarted;
			streamRecordInstance->SetDuration(dur, true);
		}
	}
}
#endif

void
Channel::EnableStreamRecord(Instance *i)
{
	if (i) {
		reRecord = true;
		streamRecordInstance = i;
		if (i->channel != this) {
			i->SetChannel(this, true);
		}
	} else {
		streamRecordInstance = nullptr;
		reRecord = false;
	}
	recordState = RECORD_ENABLE;
}

void
Channel::EnableAudioRecord(SampleInstance *i)
{
	if (i) {
		reRecord = true;
		audioRecordInstance = i;
		if (i->channel != this) {
			i->SetChannel(this, true);
		}
	} else {
		audioRecordInstance = nullptr;
		reRecord = false;
	}
	recordState = RECORD_ENABLE;
}

void
Channel::DisableRecord()
{
	audioRecordInstance = nullptr;
	streamRecordInstance = nullptr;
	reRecord = false;
	recordState = RECORD_NOT;
}

status_t
Channel::StopRecording()
{
	recordState = RECORD_NOT;
	reRecord = false;
	if (audioRecordInstance) {
		audioRecordInstance->StopRecording();
#ifdef QUA_V_AUDIO
		getAudioManager()->stopRecording(audioRecordInstance);
#endif
		audioRecordInstance = nullptr;
	}
	if (streamRecordInstance) {
		streamRecordInstance->StopRecording();
		streamRecordInstance = nullptr;
	}
	
	return B_OK;
}


status_t
Channel::StartRecording()
{
//	if () {
//		audioRX.StartRecording();
//		streamRX.StartRecording();
//	}

//	if (status == STATUS_RUNNING && recordState == RECORD_ENABLE) {
//		ArrangerObject	*SO;
//		class Time		startt((long)0, channel->uberQua->metric),
//						dur((long)50, channel->uberQua->metric);
//		recordStarted = uberQua->theTime;
//		
//		if (device->deviceType == QUA_DEV_AUDIO) {
//			if (reRecord) {
//				recordInstance->StartRecording();
//				recordInstance->SetStartTime(recordStarted);
//				SO = recordInstance->arrangerObject;
//			} else {
//				Sample *S = channel->uberQua->NewSample(channel->chanId, -1);	// save message
//				SO = S->CloneArrangerObject(
//					channel->chanId,recordStarted,dur,
//					channel->uberQua->sequencerWindow->arrange);
//				recordInstance = SO->instance;
//				recordInstance->StartRecording();
//			}
//			channel->uberQua->sampler->StartRecording(
//							(SampleInstance *)recordInstance);
//			Sample	*S = (Sample*)recordInstance->schedulable;
//			if (S && S->representation && S->representation->editor) {
//				SampleEditor	*E =
//					(SampleEditor*)S->representation->editor->dataEdit;
//				E->takeView->AddTake(S->recordTake);
//			}
//		} else {
//			Pool			*P;
//			if (reRecord) {
//				recordInstance->StartRecording();
//				recordInstance->SetStartTime(recordStarted);
//				SO = recordInstance->arrangerObject;
//				P = (Pool*)recordInstance->schedulable;
//			} else {
//				P = channel->uberQua->NewPool(channel->chanId);
//				SO = P->CloneArrangerObject(
//					channel->chanId,recordStarted,dur,
//					channel->uberQua->sequencerWindow->arrange);
//				recordInstance = SO->instance;
//				recordInstance->StartRecording();
//			}
//
//			bool	freshTake = P->SetRecordTake();
//			
//			if (device->deviceType == QUA_DEV_MIDI) {
////				FlushInput();
//			}
//			
//			if (freshTake && P && P->representation && P->representation->editor) {
//				PoolEditor	*E = (PoolEditor*)P->representation->editor->dataEdit;
//				E->takeView->AddTake(P->recordTake);
//			}
//		}
//		recordState = RECORD_ING;
//
//		Window()->Lock();
//		Invalidate();
//		Window()->Unlock();
//	}
//}
	return B_NO_ERROR;
}

void
Channel::CheckRecord(Stream *s)
{
	if (recordState == RECORD_ING && streamRecordInstance) {	// dump into rec stream
						// let's hope this is always a
						// pool .......
		Pool *pool = (Pool *) streamRecordInstance->schedulable;
		pool->recordTake->stream.AddToStream(s, &recordStarted);
	}
}



void
Channel::AddReceivingInstance(Instance *i)
{
	receivingLock.lock();
	receivingInstances.push_back(i);
	receivingLock.unlock();
}

void
Channel::RemoveReceivingInstance(Instance *i)
{
	receivingLock.lock();
	auto ci = qut::find(receivingInstances, i);
	if (ci != receivingInstances.end()) {
		receivingInstances.erase(ci);
	}
	receivingLock.unlock();
}

void
Channel::ClearInBuffers(bool Clr)
{
#ifdef ASYNC_CHAN_IN
	inMutex.Lock();
#endif
	inStream.ClearStream();
#ifdef ASYNC_CHAN_IN
	inMutex.Unlock();
#endif
}

				
status_t
Channel::SaveSnapshot(FILE *fp)
{
	status_t	err = B_OK;
	fprintf(fp, "<channel name=\"%s\">\n", sym->name.c_str());
	if (txStack) {
		txStack->SaveSnapshot(fp, "tx");
	}
	if (rxStack) {
		rxStack->SaveSnapshot(fp, "rx");
	}
	StabEnt	*p = sym->children;
	while (p != nullptr) {
		if (p->refType == TypedValue::REF_POINTER || p->refType == TypedValue::REF_VALUE) {
			p->SaveSnapshot(fp);
		}
		p = p->sibling;
	}
	fprintf(fp, "</channel>\n");
	return err;
}

status_t
Channel::Save(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	tab(fp, indent);
		fprintf(fp,	"channel %d", chanId);
		if (nAudioIns >= 0) {
			fprintf(fp,	" \\ins %d", nAudioIns);
		}
		if (nAudioOuts >= 0) {
			fprintf(fp,	" \\outs %d", nAudioOuts);
		}
//		if (midiThru) {
//			fprintf(fp,	" \\midithru");
//		} else {
//			fprintf(fp,	" \\noMidithru");
//		}
//		if (audioThru) {
//			fprintf(fp,	" \\audiothru");
//		} else {
//			fprintf(fp,	" \\noAudiothru");
//		}
		fprintf(fp,	" %s", sym->name.c_str());
	SaveMainBlock(nullptr, fp, indent, sym, true, true, this, nullptr); 
//	tab(fp, indent+1);
//	fprintf(fp, "input {%s %s} {}\n",
//			streamRX.Name(),
//			audioRX.Name());
//	tab(fp, indent+1);
//	fprintf(fp, "output {%s %s} {}\n",
//			streamTX.Name(),
//			audioTX.Name());
//	tab(fp, indent); fprintf(fp, "}\n");
	return err;
}



status_t
Channel::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *nameAttrVal = element->Attribute("name");
	const char *typeAttrVal = element->Attribute("type");
	const char *encodingAttrVal = element->Attribute("encoding");

	std::string	nameAttr;
	std::string	typeAttr;

	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTypeAttr = false;

	int		encoding = 0;

	if (nameAttrVal != nullptr) {
		nameAttr = nameAttrVal;
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		typeAttr = typeAttrVal;
		hasTypeAttr = true;
	}
	if (encodingAttrVal != nullptr) {
		if (std::string(encodingAttrVal) == "base64") {
			encoding = 1;
		}
	}
	const char *namep = element->Value();
	std::string namestr = namep;

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "channel") {
		LoadSnapshotChildren(element);
	} else if (namestr == "stack") {
		if (hasNameAttr) {
			if (nameAttr ==  "rx") {
				if (rxStack) {
					rxStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr ==  "tx") {
				if (txStack) {
					txStack->LoadSnapshotElement(element);
				}
			} else if (nameAttr ==  "rec") {
//				if (recordStack) {
//					recordStack->LoadSnapshotElement(element);
//				}
			} else if (nameAttr == "cue") {
//				if (cueStack) {
//					cueStack->LoadSnapshotElement(element);
//				}
			} else if (nameAttr == "start") {
//				if (startStack) {
//					startStack->LoadSnapshotElement(element);
//				}
			} else if (nameAttr == "stop") {
//				if (stopStack) {
//					stopStack->LoadSnapshotElement(element);
//				}
			} else {
				;
			}
		}
	} else if (namestr == "fixed" || namestr == "envelope") {
		if (hasNameAttr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, this, sym, nullptr);	
			}
		}
	} else if (namestr == "input") {
		if (hasNameAttr) {
			StabEnt *inSym = FindTypedSymbolInCtxt(nameAttr, TypedValue::S_INPUT, sym);
			if (inSym != nullptr) {
				Input	*c = inSym->InputValue();
				if (c != nullptr) {
					c->LoadSnapshotElement(element);
				}
			}
		}
	} else if (namestr == "output") {
		if (hasNameAttr) {
			StabEnt *outSym = FindTypedSymbolInCtxt(nameAttr, TypedValue::S_OUTPUT, sym);
			if (outSym != nullptr) {
				Output	*c = outSym->OutputValue();
				if (c != nullptr) {
					c->LoadSnapshotElement(element);
				}
			}
		}
	} else {
	}

	return B_OK;
}

status_t
Channel::LoadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (LoadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
		childElement = childElement->NextSiblingElement();
	}
	return B_OK;
}


Channel *
LoadChannel(FILE *fp, Qua *u)
{
	return nullptr;
}

// Called
//  * through the audio input infrastructure
size_t
Channel::Receive(size_t nFrames)
{
	long	nsamp = nAudioIns * nFrames;
	sample_buf_zero(inSig, nAudioIns, nFrames);

#ifdef QUA_V_AUDIO
	for (Input *inp: activeAudioInputs) {
		QuaAudioIn	*ip = inp->src.audio.port;
		QuaAudioIn	*xp=nullptr;
		if (ip && ip->enabled) {
			float			*ibuf = ip->data->OutPtr();

			if (nAudioIns == ip->nChannel) {
				sample_buf_copy_interleaved(inSig, ibuf, nAudioIns, nFrames);
			} else if (nAudioIns == 2 &&
					(xp = inp->src.audio.xport) != nullptr &&
					ip->nChannel == 1 && xp->nChannel == 1) {
				float	*xbuf = xp->data->OutPtr();
				float   *tbuf[] = {ibuf, xbuf };
				sample_buf_add(inSig, tbuf, nAudioIns, nFrames);
			} else {
				fprintf(stderr, "Qua channel panic: don't know how to reformat input\n");
			}
		} else {	// skip dud port data
		}
	}

// initially data received was q'd as there was no degree of synchronisation guaranteed between
// receive and generate. will assume from now on that i can. data/cbuf => inSig
//	data->Add(nsamp);
	
// apply channel effects.
#ifdef LOTSALOX
	stackableLock.Lock();
#endif
	if (rx.block && rxStack->locusStatus == STATUS_RUNNING) {
		ApplyQuaFX(rxStack, rx.block, inSig, nFrames, nAudioIns);	
	}
#ifdef LOTSALOX
	stackableLock.Unlock();
#endif

// save shit
	
	if (recordState == RECORD_ING && audioRecordInstance) {
		audioRecordInstance->QSample()->Stash(inSig, nAudioIns, nFrames);
	}
#endif
	return nFrames;
}

// Called
//  * through the audio output infrastructure
//
//  read lock activeAudioOutputs, activeAudioInputs, activeAudioInstances
size_t
Channel::Generate(size_t nFrames)
{
	if (debug_chan >= 2) {
		fprintf(stderr, "Channel %d: generate(%d)\n", chanId, nFrames);
	}
// used to be a simple q ... now a sample_buf.
// sample data received is in inSig
//	float	*bufp = data->OutPtr();
//	short nsample = nFrames*nAudioChannel;
// if not thru enabled clear chan buffer, else set it at input data
	if (audioThru) {
		if (nAudioIns == nAudioOuts) { // or no valid inputs...
			sample_buf_copy(outSig, inSig, nAudioOuts, nFrames);
		} else {
		}
	} else {
		sample_buf_zero(outSig, nAudioOuts, nFrames);
	}
//	data->Take(nsample);
// add sample buffers to channel queue	
	for (Instance *inst: audioInstances) {
		int nf = inst->Generate(
							instSig,
							nFrames,
							nAudioOuts
					);
		sample_buf_add(outSig, instSig, nAudioOuts, nFrames);
	}
#ifdef LOTSALOX
	stackableLock.Lock();
#endif
	if (tx.block && txStack->locusStatus == STATUS_RUNNING) {
		ApplyQuaFX(txStack, tx.block,
					outSig,
					nFrames, nAudioOuts);
	}	
#ifdef LOTSALOX
	stackableLock.Unlock();
#endif

	activePortsLock.lock();
#ifdef QUA_V_AUDIO
	for (Output *dp: activeAudioOutputs) {
		QuaAudioOut	*op = dp->dst.audio.port;
		float	*sp = op->outbuf+op->offset;
		if (nAudioOuts == 2) {
			QuaAudioOut	*xp = dp->dst.audio.xport;
			float	*olp = outSig[0];
			float	*orp = outSig[1];
			if (op->nChannel == 1 && xp && xp->nChannel == 1) {
				float	*xsp = xp->outbuf+xp->offset;
				for (short i=0; ((unsigned)i)<nFrames; i++) {
					*sp++ = *olp++;
					*xsp++ = *orp++;
				}
			} else {
				fprintf(stderr, "Channel::Generate() error: missing audio buffer in stereo channel generation\n");
			}
//	if (chanId == 0) {
//		for (short i=0; i<nFrames; i++) {
//			fprintf(stderr, "%d %x\n", op->nChannel, xp, xp?xp->nChannel:0);
//		}fprintf(stderr, "\n");
//	}
		} else if (nAudioOuts == 1) {
			if (op->nChannel == 1) {
				float	*tp = outSig[0];
				for (short i=0; ((unsigned)i)<nFrames; i++) {
					*sp++ = *tp++;
				}
			}
		} else {	// what a wonderful wierd world this might be!
		}
	}
#endif
	activePortsLock.unlock();
	
	return nFrames;
}

Input *
Channel::AddInput(const string &nm, QuaPort *p, short c, bool en)
{
	fprintf(stderr, "Channel %s adding input %s\n", sym->name.c_str(), nm.c_str());
	Input	*s=nullptr;
	activePortsLock.lock();
	std::string inputName = glob.makeUniqueName(sym, nm, 0);
	inputs.Add(s=new Input(inputName, this, p, c, en));
	DefineSymbol("gain", TypedValue::S_FLOAT, 0,
					&s->gain, s->sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	DefineSymbol("pan", TypedValue::S_FLOAT, 0,
					&s->pan, s->sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);

	if (p && s->enabled) {
		switch (p->deviceType) {
			case QUA_DEV_AUDIO: {
				activeAudioInputs.Add(s);
				break;
			}
			
			case QUA_DEV_JOYSTICK:
			case QUA_DEV_MIDI:
			default: {
				activeStreamInputs.Add(s);
				break;
			}
		}
	}
	activePortsLock.unlock();
	uberQua->bridge.AddDestinationRepresentations(this);

	return s;
}

Output *
Channel::AddOutput(const string &nm, QuaPort *p, short c, bool en)
{
	fprintf(stderr, "Channel %s adding output %s\n", sym->name.c_str(), nm.c_str());

	activePortsLock.lock();
	Output		*d=nullptr;
	std::string outputName = glob.makeUniqueName(sym, nm, 0);

	outputs.Add(d=new Output(outputName, this, p, c, en));
	DefineSymbol("gain", TypedValue::S_FLOAT, 0,
					&d->gain, d->sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	DefineSymbol("pan", TypedValue::S_FLOAT, 0,
					&d->pan, d->sym,
					TypedValue::REF_POINTER, false, false, StabEnt::DISPLAY_CTL);
	if (p && d->enabled) {
		switch (p->deviceType) {
			case QUA_DEV_AUDIO: {
				activeAudioOutputs.Add(d);
				break;
			}
			
			case QUA_DEV_JOYSTICK:
			case QUA_DEV_MIDI:
			default: {
				activeStreamOutputs.Add(d);
				break;
			}
		}
	}
	activePortsLock.unlock();
	uberQua->bridge.AddDestinationRepresentations(this);

	return d;
}

void
Channel::RemoveInput(Input *s)
{
	activePortsLock.lock();
	Enable(s, false);
	uberQua->bridge.RemoveDestinationRepresentation(this, s);
	inputs.Del(s);
	glob.DeleteSymbol(s->sym, true);
	delete s;
	activePortsLock.unlock();
}

void
Channel::RemoveOutput(Output *s)
{
	activePortsLock.lock();
	Enable(s, false);
	uberQua->bridge.RemoveDestinationRepresentation(this, s);
	outputs.Del(s);
	glob.DeleteSymbol(s->sym, true);
	delete s;
	activePortsLock.unlock();
}


bool
Channel::Enable(Input *s, bool en)
{
	status_t	err;
	if (s && s->device) {
		s->enabled = en;
		switch(s->device->deviceType) {
		case QUA_DEV_MIDI: {
			if (en) {
				fprintf(stderr, "Channel %s, enable midi in (%s)\n", sym->name.c_str(), s->Name(NMFMT_NAME,NMFMT_NUM));
				if ((err=getMidiManager()->connect(s)) == B_OK) {
					activeStreamInputs.Add(s);
				} else {
					uberQua->bridge.reportError("failed to open midi input %s\n", s->Name(NMFMT_NAME,NMFMT_NUM));
					return false;
				}
			} else {
				activeStreamInputs.Del(s);
				getMidiManager()->disconnect(s);
			}
			break;
		}

#ifdef QUA_V_JOYSTICK
		case QUA_DEV_JOYSTICK: {
			if (en) {
				if ((err=getJoystickManager()->Connect(s)) == B_OK) {
					activeStreamInputs.Add(s);
				} else {
					reportError("failed to open joy input %s\n", s->Name(NMFMT_NAME,NMFMT_NUM));
					return false;
				}
			} else {
				activeStreamInputs.Del(s);
				getJoystickManager()->Disconnect(s);
			}
			break;
		}
#endif
#ifdef QUA_V_AUDIO
		case QUA_DEV_AUDIO: {
			if (en) {
				fprintf(stderr, "Channel %s, enable aud (%s)\n", sym->name.c_str(), s->Name(NMFMT_NAME,NMFMT_NUM));
				if ((err=getAudioManager()->connect(s)) == B_OK) {
					activeAudioInputs.Add(s);
				} else {
					uberQua->bridge.reportError("failed to open audio input %s\n%s", s->Name(NMFMT_NAME,NMFMT_NUM), getAudioManager()->ErrorString(err));
					return false;
				}
			} else {
				activeAudioInputs.Del(s);
				getAudioManager()->disconnect(s);
			}
			break;
		}
#endif
		}
	}



	return true;
}

bool
Channel::Enable(Output *s, bool en)
{
	status_t	err=B_OK;
	if (s && s->device) {
		s->enabled = en;
		switch(s->device->deviceType) {
		case QUA_DEV_MIDI: {
			if (en) {
				fprintf(stderr, "Channel %s, enable midi out (%s)\n", sym->name.c_str(), s->Name(NMFMT_NAME,NMFMT_NAME));
				if ((err=getMidiManager()->connect(s)) == B_OK) {
					activeStreamOutputs.Add(s);
				} else {
					uberQua->bridge.reportError("failed to open midi output %s\n", s->Name(NMFMT_NAME,NMFMT_NAME));
					return false;
				}
			} else {
				activeStreamOutputs.Del(s);
				getMidiManager()->disconnect(s);
			}
			break;
		}

#ifdef QUA_V_JOYSTICK
		case QUA_DEV_JOYSTICK: {
			if (en) {
				if ((err=getJoystickManager()->Connect(s)) == B_OK) {
					activeStreamOutputs.Add(s);
				} else {
					reportError("failed to open joy output %s\n", s->Name(NMFMT_NAME,NMFMT_NUM));
					return false;
				}
			} else {
				activeStreamOutputs.Del(s);
				getJoystickManager()->Disconnect(s);
			}
			break;
		}
#endif
#ifdef QUA_V_AUDIO
		case QUA_DEV_AUDIO: {
			if (en) {
				fprintf(stderr, "Channel %s, enable audio out (%s)\n", sym->name.c_str(), s->Name(NMFMT_NAME,NMFMT_NUM));
				if ((err=getAudioManager()->connect(s)) == B_OK) {
					activeAudioOutputs.Add(s);
				} else {
					uberQua->bridge.reportError("failed to open audio output %s\n%s", s->Name(NMFMT_NAME,NMFMT_NUM), getAudioManager()->ErrorString(err));
					return false;
				}
			} else {
				activeAudioOutputs.Del(s);
				getAudioManager()->disconnect(s);
			}
			break;
		}
#endif
		}
	}

	return true;
}

void
Channel::SetInput(Input *d, QuaPort *p, short c)
{
	uberQua->bridge.reportError("unimp");
}

void
Channel::SetOutput(Output *d, QuaPort *p, short c)
{
	uberQua->bridge.reportError("unimp");
}

QuaAudioManager *getAudioManager() {
	return environment.quaAudio;
}

QuaMidiManager *getMidiManager() {
	return environment.quaMidi;
}

#ifdef QUA_V_JOYSTICK
QuaJoystickManager *getJoyManager() {
	return environment.quaJoystick;
}
#endif

QuaParallelManager *getParallelManager() {
	return environment.quaParallel;
}

QuaOSCManager *getOSCManager() {
	return nullptr; //  environment.quaAudio;
}


#ifdef XXX
//////////////////////////////////////////////////
// deprecated not yet eliminated				//
//////////////////////////////////////////////////
status_t
Input::NoodleEnable(bool en)
{
	status_t	err=B_OK;
	if (device) {
		switch(device->deviceType) {
		
		case QUA_DEV_MIDI: {
			if (en) {
				src.midi = channel->uberQua->quaMidi->OpenInput((QuaMidiPort *)device);
				if (src.midi == nullptr) {
					reportError("Can't open midi port %s", device->sym->name);
					err = B_ERROR;
				}
				channel->activeStreamInputs.Add(this);
			} else {
				bool	anyEnabled=false;
				channel->activeStreamInputs.Del(this);
				for (short i=0;i<channel->uberQua->nChannel; i++) {
					if (i != channel->chanId) {
						for (short j=0;
							Input *d = channel->uberQua->channel[i]->activeStreamInputs.Item(j);
							if (d->enabled && d->device == device) {
								anyEnabled = true;
								break;
							}
						}
						if (anyEnabled)
							break;
					}
				}
				if (!anyEnabled && !channel->uberQua->quaMidi->CloseInput(src.midi)) {
					reportError("Can't close input midi port %s", device->sym->name);
					err = B_ERROR;
				}
			}
			break;
		}
			
#ifdef QUA_V_JOYSTICK
		case QUA_DEV_JOYSTICK: {
			if (en) {
				if (((QuaJoystickPort*)device)->CheckPortOpen()) {
					reportError("Can't open joystick %s", device->sym->name);
					return B_ERROR;
				}
				channel->activeStreamInputs.Add(this);
			} else {
				channel->activeStreamInputs.Del(this);
				if (!((QuaJoystickPort*)device)->CheckPortClose()) {
					reportError("Can't close joystick %s", device->sym->name);
					err = B_ERROR;
				}
			}

			break;
		}
#endif
			
		case QUA_DEV_AUDIO: {
							
			bool	anyEnabled = false;
			if (!en) {
				channel->activeAudioInputs.Del(this);
				for (short i=0;i<channel->uberQua->nChannel; i++) {
					if (i != channel->chanId) {
						for (short j=0;
								j<channel->uberQua->channel[i]->activeAudioInputs.CountItems();
								j++) {
							Input *d = channel->uberQua->channel[i]->activeAudioInputs.Item(j);
							if (d->enabled) {
								anyEnabled = true;
								break;
							}
						}
						if (anyEnabled)
							break;
					}
				}
			} else {
				channel->activeAudioInputs.Add(this);
				anyEnabled = true;
			}
			if (anyEnabled) {
				err=channel->uberQua->sampler->OpenInput((QuaAudioPort *)device, deviceChannel);
			} else {
				channel->uberQua->sampler->CloseInput((QuaAudioPort *)device, deviceChannel);
			}
		}
		
		}
	}
	
	if (err==B_OK) {
		enabled = en;
	} else {
		enabled = false;
	}
	return err;
}


status_t
Output::NoodleEnable(bool en)
{
	status_t	err=B_OK;

	if (device) {
		fprintf(stderr, "Noodle enable(%d) devt = %d\n",
					en, device->deviceType);
					
		switch(device->deviceType) {
		
		case QUA_DEV_MIDI: {
			if (en) {
				dst.midi = channel->uberQua->quaMidi->OpenOutput((QuaMidiPort *)device);
				if (dst.midi == nullptr) {
					reportError("Can't open midi output port %s", device->sym->name);
					err = B_ERROR;
				}
				channel->activeStreamOutputs.Add(this);
			} else {
				channel->activeStreamOutputs.Del(this);
				bool	anyEnabled=false;
				for (short i=0;i<channel->uberQua->nChannel; i++) {
					if (i != channel->chanId) {
						for (short j=0; j<channel->uberQua->channel[i]->activeStreamOutputs.CountItems(); j++) {
							Output *d = channel->uberQua->channel[i]->activeStreamOutputs.Item(j);
							if (d->device == device && d->enabled) {
								anyEnabled = true;
								break;
							}
						}
						if (anyEnabled)
							break;
					}
				}
				if (!anyEnabled && !channel->uberQua->quaMidi->CloseOutput(dst.midi)) {
					reportError("Can't close output midi port %s", device->sym->name);
					err = B_ERROR;
				}
			}
			break;
		}
			
		case QUA_DEV_JOYSTICK: {
			err = B_ERROR;
			break;
		}
		
		case QUA_DEV_AUDIO: {
			if (en) {
				channel->uberQua->sampler->OpenOutput((QuaAudioPort *)device, deviceChannel);
				channel->activeAudioOutputs.Add(this);
			} else {
				bool	anyEnabled=false;
				channel->activeAudioOutputs.Del(this);
				for (short i=0;i<channel->uberQua->nChannel; i++) {
					if (i != channel->chanId) {
						for (short j=0;
								j<channel->uberQua->channel[i]->activeAudioOutputs.CountItems();
								j++) {
							Output *d = channel->uberQua->channel[i]->activeAudioOutputs.Item(j);
							if (	d->device == device &&
									d->deviceChannel == deviceChannel &&
									d->enabled) {
								anyEnabled = true;
								break;
							}
						}
						if (anyEnabled)
							break;
					}
				}
				if (!anyEnabled) {
					channel->uberQua->sampler->CloseOutput((QuaAudioPort *)device, deviceChannel);

				}
			}
		}
		
		}
	}
	
	if (err == B_OK) {
		enabled = en;
	}
	return err;
}


#endif

QuaAudioManager * Channel::getAudioManager() { return environment.quaAudio; }
QuaMidiManager *Channel::getMidiManager() { return environment.quaMidi; }
QuaJoystickManager *Channel::getJoyManager() { return nullptr; }
QuaParallelManager *Channel::getParallelManager() { return environment.quaParallel; }
QuaOSCManager *Channel::getOSCManager() { return nullptr; }
