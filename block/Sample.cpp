#include "qua_version.h"

#include "StdDefs.h"

#if defined(WIN32)
#include <io.h>

#define WIN32_LEAN_AND_MEAN
#include <stdafx.h>
#endif

#include "tinyxml2.h"
#include <math.h>

#include "SampleFile.h"

#include "Sample.h"
#include "QuaAudio.h"
#include "Qua.h"
#include "Block.h"
#include "QuaFX.h"
#include "Envelope.h"
#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif
#include "Parse.h"
#include "Channel.h"
#include "SampleBuffer.h"
#include "State.h"

#if defined(QUA_V_ARRANGER_INTERFACE)
#if defined(WIN32)
#include "QuaDisplay.h"
#endif
#endif

Sample::Sample(std::string nm, std::string path, Qua *uq, short maxbuf, short maxreq):
	Schedulable(
		DefineSymbol(nm, TypedValue::S_SAMPLE, 0,
					this, uq->sym,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT),
		uq,
		uq->metric)
{
	uberQua = uq;

	maxBuffers = maxbuf;
	maxRequests = maxreq;
	fileBufferLength = QUA_DFLT_SAMPLE_READ_BUF_BYTES;
	buffer = new SampleBuffer*[maxbuf];
	request = new long[maxreq];
	requestedTake = new SampleTake*[maxreq];
	fileBuffer = (char *)malloc(fileBufferLength);
	
//	AddClip("clip");

	recordTake = nullptr;
	
	
	nBuffers = 0;
	for (short j=0; j<maxBuffers; j++)
		buffer[j] = nullptr;
	ampSym = panSym = sfrSym = efrSym = nullptr;
	
	if (path.size() > 0) {
		AddSampleTake(nm, path, true);
	}
	
	SampleInstance	*i = 0;
//	StabEnt		*ams, *lcs, *pms, *sms, *ems;
//
//	DefineSymbol("Mute", S_BOOL, 0,
//				&i->mute, sym,
//				REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_CTL);
//	ampSym=DefineSymbol("Amp", S_FLOAT, 0,
//				&i->amp, sym,
//				REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
//	panSym=DefineSymbol("Pan", S_FLOAT, 0,
//				&i->pan, sym,
//				REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_ENV);
//	sfrSym=DefineSymbol("StartFrame", S_TIME, 0,
//				&i->startFrame, sym,
//				REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_CTL);
//	efrSym=DefineSymbol("EndFrame", S_TIME, 0,
//				&i->endFrame, sym,
//				REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_CTL);
////	sms=DefineSymbol("StartFrame", S_EXPRESSION, &i->sFrameExp, REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_CTL);
////	ems=DefineSymbol("EndFrame", S_EXPRESSION, &i->eFrameExp, REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_CTL);
//	lcs=DefineSymbol("Loop", S_EXPRESSION, 0,
//				&i->loopCondition, sym,
//				REF_INSTANCE, StabEnt::ATTR_NONE, true, StabEnt::DISPLAY_CTL);

	TypedValue v1, v2;
//	v1 = TypedValue::Float(0.0); v2 = TypedValue::Float(1.0);
//	ampSym->SetBounds(v1, v2);
//	v1 = TypedValue::Float(-1.0); v2 = TypedValue::Float(1.0);
//	panSym->SetBounds(v1, v2);
//	v1 = TypedValue::Int(0); v2 = TypedValue::Int(selectedNFrames);
//	sfrSym->SetBounds(v1, v2);
//	v1 = TypedValue::Int(0); v2 = TypedValue::Int(selectedNFrames);
//	efrSym->SetBounds(v1, v2);
//	ampSym->SetInit(TypedValue::Float(0.5));
//	panSym->SetInit(TypedValue::Float(0.0));
//	sfrSym->SetInit(TypedValue::Int(0));
//	efrSym->SetInit(TypedValue::Int(nFrames));
	
// totally stupid hack to pass necasary context dependent stuff to parser
//	sms->SetBounds(TypedValue::FloatBaseVal(0.0),
//				TypedValue::StrangePointer(&i->TxLock));
//	ems->SetBounds(TypedValue::FloatBaseVal(0.0),
//				TypedValue::StrangePointer(&i->TxLock));
//	v1 = TypedValue::Float(0.0); v2 = TypedValue::Pointer(S_STRANGE_POINTER, &i->stackLock);
//	lcs->SetBounds(v1, v2);

//	sms->SetInit(TypedValue::StrangePointer(sym));
//	ems->SetInit(TypedValue::StrangePointer(sym));
//	lcs->SetInit(TypedValue::StrangePointer(sym));

// reference to the default clip selection of this sample

	DefineSymbol("loopCount", TypedValue::S_INT, 0,
					&i->loopCount, sym,
					TypedValue::REF_INSTANCE, false, false, StabEnt::DISPLAY_NOT);
	status = STATUS_RUNNING;
	for (short i=0; i<maxRequests; i++) {
		request[i] = SAMPLE_DATA_REQUEST_NOTHING;
	}
	SynchronizeBuffers();
#ifdef QUA_V_AUDIO
	context.quaAudio->AddSample(this);
#endif
}

Clip *
Sample::AddClip(std::string nm, SampleTake *take, Time&start, Time&duration, bool disp)
{
	Clip	*c = new Clip(nm, sym);
	clips.push_back(c);
	c->Set(take, start, duration);
	if (disp) {
		uberQua->bridge.updateClipIndexDisplay(sym);
	}
	return c;
}

void
Sample::RemoveClip(Clip *c, bool disp)
{
	if (clips.size() > 1) { // always keep at least one clip handy
		qut::del(clips, c);
	}
	if (disp) {
		uberQua->bridge.updateClipIndexDisplay(sym);
	}
}


Sample::~Sample()
{
#ifdef QUA_V_AUDIO
	context.quaAudio->RemoveSample(this);
#endif
	delete fileBuffer;
//	for (short i=0; i<CountTakes(); i++) {
//		SampleTake	*t = Take(i);
//		delete t;
//	}
}

SampleFile *
Sample::selectedFile()
{
	return sampleClip(0)->media? ((SampleTake*)sampleClip(0)->media)->file :nullptr;
}

std::string
Sample::selectedPath()
{
	return (sampleClip(0)&&sampleClip(0)->media)?
			((SampleTake*)sampleClip(0)->media)->path : "";
}



SampleTake *
Sample::AddRecordTake(long fileType, short nChan, short sampleSize, float sampleRate)
{
	if (recordTake != nullptr) {
		return nullptr;
	}

	long takeno = 0;
	char *extension = "wav";
	std::string	path;
	std::string filenm;
	std::string takenm;
	status_t err;

	recordFrame = 0;
	pendingRecordBuffers = nullptr;
	currentRecordBuffer = nullptr;
	freeRecordBuffers = nullptr;

	do {
		takeno++;
		char buf[100];
		sprintf(buf, "%s_%d.%s", sym->name, takeno, extension);
		filenm = buf;
		path == uberQua->sampleDirectoryPath + "/" + filenm;
	} while (!_access(path.c_str(), 04));
	takenm = glob.MakeUniqueName(sym, "record", 1);
	fprintf(stderr, "new take %s file %s path %s\n", filenm, takenm, path);

	SampleFile	*file = new SampleFile(fileType, nChan, sampleSize, sampleRate);
	if ((err = file->SetTo(path.c_str(), O_RDWR|O_CREAT)) != B_NO_ERROR) {
		reportError("Can't set sample file... %s", errorStr(err));
		delete file;
		return nullptr;
	}

	recordTake = AddSampleTake(takenm,	nullptr, true);
	recordTake->SetSampleFile(path, file);
	return recordTake;
}



SampleTake	*
Sample::AddSampleTake(std::string nm, std::string path, bool disp)
{
	fprintf(stderr, "sample %s: adding sample take %s at %s\n", sym->name, nm, path);
	SampleTake	*take;
	take = new SampleTake(this, nm, path);

	takes.push_back(take);
	if (take->status == STATUS_LOADED &&
			(sampleClip(0) != nullptr &&
				(sampleClip(0)->media == nullptr ||
				 sampleClip(0)->media->status == STATUS_UNLOADED)
			) ) {
		SelectTake(take, false);
	}
	if (disp) {
		uberQua->bridge.UpdateTakeIndexDisplay(sym);
	}
	fprintf(stderr, "sample %s: added sample take %s at %s\n", sym->name, nm, path);
	return take;
}


status_t
Sample::SelectTake(SampleTake *take, bool disp)
{
	fprintf(stderr, "selecting take %x\n", take);
	if (take) {
		if (take != recordTake && take != sampleClip(0)->media) {
//			selectedTake = take;
//			selectedFile = take->file;
			sampleClip(0)->media = take;
			if (take->file) {
				sampleRate = take->file->sampleRate;
//				for (short i=0; i<maxBuffersPerSample; i++) {
//					if (buffer[i]) {
//						buffer[i]->nFrames = 0;
//						buffer[i]->fromFrame = -1;
//					}
//				}
				sampleClip(0)->start = Time(0, &Metric::sampleRate);
				sampleClip(0)->duration = Time(take->file->nFrames, &Metric::sampleRate);
				if (efrSym && sfrSym) {
					TypedValue v1, v2;
					v1 = TypedValue::Int(0); v2 = TypedValue::Int(take->file->nFrames);
					sfrSym->SetBounds(v1, v2);
					v1 = TypedValue::Int(0); v2 = TypedValue::Int(take->file->nFrames);
					efrSym->SetBounds(v1, v2);
					sfrSym->SetInit(TypedValue::Int(0));
					efrSym->SetInit(TypedValue::Int(take->file->nFrames));
				}
			} else {
				return B_ERROR;
			}
		}
	} else {
		sampleClip(0)->media = nullptr;
	}
	return B_NO_ERROR;
}

status_t
Sample::DeleteTake(SampleTake *take, bool disp)
{
	bool del = false;
	if (countTakes() <= 1)
		return B_ERROR;
	if (take == sampleClip(0)->media) {
		if (qut::del(takes, take)) {
			del = true;
		}
		SelectTake((SampleTake*)takes[0], false);
	} else {
		if (qut::del(takes, take)) {
			del = true;
		}
	}
	if (del) {
		delete take;
	}
	return B_NO_ERROR;
}

Instance *
Sample::AddInstance(std::string nm, short chan_id, Time *t, Time *d, bool disp)
{
	Time	at_t;
	Time	dur_t;
	Channel	*c;
	if (t == nullptr) {
		return nullptr;
	}
	at_t = *t;
	if (d == nullptr) {
		Clip	*dfc = sampleClip(0);
		SampleTake	*dft = take(0);
		if (dfc != nullptr) {
			dur_t = dfc->duration;
		} else if (dft != nullptr && dft->status == STATUS_LOADED && dft->file != nullptr) {
			dur_t.Set(dft->file->nFrames, &Metric::sampleRate);
		} else {
			dur_t = Time::infinity;
		}
	} else {
		dur_t = *d;
	}
	if (chan_id >= uberQua->nChannel || chan_id < 0) {
		return nullptr;
	}
	c = uberQua->channel[chan_id];
	return AddInstance(nm, at_t, dur_t, c);
}

Instance *
Sample::AddInstance(std::string nm, Time t, Time d, Channel * chan)
{
// duration in sec: ((float)sample->selectedNFrames)/kSamplingRate,
//		Time(((float)nFrames)/(kSamplingRate*tv->uberQua->timeQuanta), tv->metric),
	SampleInstance *i = new SampleInstance(this, nm, t, d, chan);
	instanceLock.lock();
	addInstToList(i);
	instanceLock.unlock();
	fprintf(stderr, "added sample instance\n");
	if (uberQua && i) {
//		b = b->Sibling(3);
//		i->SetValue(b);
		i->Init();
		uberQua->AddToSchedule(i);
#if defined(QUA_V_ARRANGER_INTERFACE)
//		uberQua->display.CreateInstanceBridge(i);
#endif
	} else {
		fprintf(stderr, "Sample: unexpected null while committing to schedule");
	}
	return i;
}

void
Sample::RemoveInstance(Instance *i, bool display)
{
	SampleInstance	*s = (SampleInstance *)i;
	if (s->status == STATUS_RUNNING) {
#ifdef QUA_V_AUDIO
		context.quaAudio->StopInstance(s);
#endif
	}
	instanceLock.lock();
	removeInstFromList(i);
	instanceLock.unlock();
#if defined(QUA_V_ARRANGER_INTERFACE)
	if (display) {
		uberQua->bridge.RemoveInstanceRepresentations(s->sym);
	}
#endif
}



void
Sample::Cue(Time &t)
{
	;
}


// called from:
//	* inside Qua main thread
status_t
Sample::Wake(Instance *i)
{
	if (i && i->status == STATUS_SLEEPING) {
#ifdef LOTSALOX
		stackableLock.Lock();
#endif
		if (wake.block) {
			Stream	mainStream;
			Time Now = uberQua->theTime;
		    flag ua = UpdateActiveBlock(
		    	uberQua, &mainStream,
		    	wake.block,
				&Now,
				i,
				i->sym,
		    	i->wakeStack,
				1,
				true);
			if (i->channel)
				i->channel->OutputStream(&mainStream);
		    mainStream.ClearStream();
		}
#ifdef LOTSALOX
	    stackableLock.Unlock();
#endif
		
#ifdef QUA_V_AUDIO
		context.quaAudio->StartInstance(i);
#endif
		return Schedulable::Wake(i);
	}
	return B_NO_ERROR;
}


// called from:
//	* inside Qua main thread
status_t
Sample::Sleep(Instance *i)
{
	if (i && i->status != STATUS_SLEEPING) {
		SampleInstance	*inst = (SampleInstance *)i;
#ifdef QUA_V_AUDIO
		context.quaAudio->StopInstance(inst);
#endif
//		fprintf(stderr, "to sleep...\n");
#ifdef LOTSALOX
		stackableLock.Lock();
#endif
		if (sleep.block) {
			Stream	mainStream;
			Time Now = uberQua->theTime;
		    flag ua = UpdateActiveBlock(
		    	uberQua,
				&mainStream,
		    	sleep.block,
				&Now,
				i,
				i->sym,
		    	i->sleepStack,
				1,
				true);
			if (i->channel)
				i->channel->OutputStream(&mainStream);
		    mainStream.ClearStream();
		}
#ifdef LOTSALOX
	    stackableLock.Unlock();
#endif
		return Schedulable::Sleep(i);
	}
	return B_NO_ERROR;
}

#ifndef QUA_V_BETTER_SCHEDULER
status_t
Sample::Run(Instance *i)
{
	SampleInstance	*inst = (SampleInstance *)i;
	// recalc params... this should be
	// somehow rate dependent
	inst->wakeDuration = uberQua->theTime - inst->startTime;
	inst->UpdateEnvelopes(inst->wakeDuration);
	
	schlock.Lock();
	if (mainBlock) {
		Stream	mainStream;
		Time	tag_time = uberQua->theTime;
		inst->stackLock.ReadLock();

		bool	uac = UpdateActiveBlock(
						uberQua,
						&mainStream,
						mainBlock,
						1,
						&tag_time,
						TRUE, inst,
						inst->mainStack);
		inst->stackLock.ReadUnlock();
		uberQua->channel[i->channel]->OutputStream(&mainStream);
		mainStream.ClearStream();
	}
	schlock.Unlock();
	return B_NO_ERROR;
}
#endif

// called from:
//	* from Channel::CheckInBuffer(), inside Qua main thread
status_t
Sample::Recv(Stream &s)
{
	Time	tag_time = uberQua->theTime;
	for (Instance *inst: instances) {
#ifdef LOTSALOX
		stackableLock.Lock();
#endif
		
		flag	uac = UpdateActiveBlock(
						uberQua,
						&s,
						rx.block,
						&tag_time,
						inst,
						inst->sym,
						inst->rxStack,
						1,
						true);
#ifdef LOTSALOX
		stackableLock.Unlock();
#endif
	}
	s.ClearStream();
	return B_NO_ERROR;
}


/*
// ?????????????? obsolete. delete soon
status_t
Sample::SetTakeList(Block *b)
{
	fprintf(stderr, "settakelist b %d\n", b->type);
	if (b->type == C_LIST) {
		for (Block *p = b->crap.list.block; p!=nullptr; p=p->next) {
			fprintf(stderr, "settakelist p %d\n", p->type);
			if (p->type == C_LIST) {
				Block		*tb = p->crap.list.block;
				if (	tb && tb->type == C_VALUE &&
						tb->crap.constant.value.type == TypedValue::S_STRING &&
						tb->next && tb->next->type == C_VALUE &&
						tb->next->crap.constant.value.type == TypedValue::S_STRING) {
					char	*pathnm = tb->crap.constant.value.StringValue();
					char	*takenm = tb->next->crap.constant.value.StringValue();
					fprintf(stderr, "add take %s %s\n", pathnm, takenm);
					AddSampleTake(takenm, pathnm, true, O_RDWR);
				}
			}
		}
	}
	return B_OK;
}*/

status_t
Sample::Save(FILE *fp, short indent)
{
	status_t	err=B_NO_ERROR;
	short		i=0;
	tab(fp, indent);
	fprintf(fp,	"sample");
	
	fprintf(fp,	" %s", sym->PrintableName());

	if (countControllers() > 0) {
		fprintf(fp, "(");
		err = saveControllers(fp, indent+2);
		fprintf(fp, ")");
	}
/* should now be taken care of as normal sym child of the sample
	if (CountTakes() > 0) {
		for (i=0; i<CountTakes(); i++) {
			SampleTake	*t = Take(i);
			if (selectedFile == t->file) {
				tab(fp, indent+1);
				fprintf(fp, "take \"%s\" %s\n", t->path, t->sym->name);
				break;
			}
		}
		for (i=0; i<takes.CountItems(); i++) {
			SampleTake	*t = (SampleTake *)takes.ItemAt(i);
			if (selectedFile != t->file) {
				tab(fp, indent+1);
				fprintf(fp, "take \"%s\" %s\n", t->path, t->sym->name);
			}
		}
	}	
*/
	SaveMainBlock(mainBlock, fp, indent, sym, true, false, nullptr, nullptr); 
	return err;
}

status_t
Sample::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *takeAttrVal = element->Attribute("take");
	const char *startAttrVal = element->Attribute("start");
	const char *durAttrVal = element->Attribute("duration");
	const char *nameAttrVal = element->Attribute("name");
	const char *typeAttrVal = element->Attribute("type");
	const char *scriptAttrVal = element->Attribute("script");

	std::string valAttr;
	std::string	nameAttr;
	std::string	typeAttr;

	bool	hasNameAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTypeAttr = false;

	int		encoding = 0;
	int		position = 0;


	if (nameAttrVal != nullptr) {
		nameAttr = nameAttrVal;
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		typeAttr = typeAttrVal;
		hasTypeAttr = true;
	}

	std::string namestr = element->Value();

	bool	hasNameAttr = false;
	bool	hasTypeAttr = false;
	bool	hasScriptAttr = false;
	bool	hasTakeAttr = false;
	Time	startt = Time::zero;
	Time	durt(12);

	if (scriptAttrVal != nullptr) {
		hasScriptAttr = true;
	}
	if (nameAttrVal != nullptr) {
		hasNameAttr = true;
	}
	if (typeAttrVal != nullptr) {
		hasTypeAttr = true;
	}
	if (takeAttrVal != nullptr) {
		hasTakeAttr = true;
	}
	if (startAttrVal != nullptr) {
		startt.Set(startAttrVal);
	}
	if (durAttrVal != nullptr) {
		durt.Set(durAttrVal);
	} 

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "qua") {
		LoadSnapshotChildren(element);
	} else if (namestr == "sample") {
		LoadSnapshotChildren(element);
	} else if (namestr == "instance") {
		if (hasNameAttr) {
			Instance *i = FindInstance(sym, nameAttr);
			if (i != nullptr) {
				i->LoadSnapshotElement(element);
			}
		}
	} else if (namestr == "stack") {
		LoadSnapshotChildren(element);
	} else if ((namestr == "fixed") || namestr == "envelope") {
		if (hasNameAttr) {
			StabEnt	*childsym = FindSymbolInCtxt(nameAttr, sym);
			if (childsym) {
				childsym->SetAtomicSnapshot(element, nullptr, nullptr, nullptr);	
			}
		}
	} else if (namestr == "take") {
		if (hasNameAttr) {
			StabEnt *takeSym = FindTypedSymbolInCtxt(nameAttr, TypedValue::S_TAKE, sym);
			if (takeSym != nullptr) {
				class Take	*t = takeSym->TakeValue();
				SampleTake	*strt = nullptr;
				if (t->type == Take::SAMPLE) {
					if (!hasTypeAttr || strcmp(typeAttrVal, "sample") == 0) {
						strt = (SampleTake *)t;
						strt->LoadSnapshotElement(element);
					}
				}
			}
		}
	} else if (namestr == "clip") {
		if (hasNameAttr) {
			StabEnt *clipSym = FindTypedSymbolInCtxt(nameAttr, TypedValue::S_CLIP, sym);
			if (clipSym != nullptr) {
				Clip	*c = clipSym->ClipValue(nullptr);
				if (c != nullptr && hasTakeAttr) {
					StabEnt	*takeSym = FindTypedSymbolInCtxt(takeAttrVal, TypedValue::S_TAKE, sym);
					class Take	*take = nullptr;
					if (takeSym && (take=takeSym->TakeValue())!=nullptr) {
						c->Set(take, startt, durt);
					}
				}
			}
		}
	} else {
	}

	return B_OK;
}


status_t
Sample::LoadSnapshotChildren(tinyxml2::XMLElement *element)
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


bool
Sample::Init()
{
	if (sym == nullptr) {
	    reportError("Qua: sampler not found");
	}
	glob.PushContext(sym);
	fprintf(stderr, "Initing sample %s\n", sym->name);

	if (!Schedulable::Init())
		goto err_ex;

	glob.PopContext(sym);
	return true;
err_ex:
	glob.PopContext(sym);
	return false;
}
////////////////////////////////////////////////////////////
// files are references to objects in takes ... so they can be
// validly compared as pointers
////////////////////////////////////////////////////////////
SampleBuffer *
Sample::BufferForFrame(SampleFile *f, long fr)
{
	for (short i=0; i<nBuffers; i++) {
		if (buffer[i] && buffer[i]->HasFrame(f, fr)) {
			return buffer[i];
		}
	}
	return nullptr;
}

SampleBuffer *
Sample::BufferForChunk(SampleFile *f, long fr)
{
	for (short i=0; i<nBuffers; i++) {
		if (buffer[i] && buffer[i]->chunk == fr && buffer[i]->file == f) {
			return buffer[i];
		}
	}
	return nullptr;
}
////////////////////////////////////////////////////////////////////////////////////////
// recording of audio data into the buffers of this sample
///////////////////////////////////////////////////////////////////////////////////////
long
Sample::Stash(float **inBuf, short nChannel, long nFrames)
{
	long			stashedFrame = 0;
//fprintf(stderr, "stash %d\n", nFrames);
	recordbufLock.lock();
	while (stashedFrame < nFrames) {
// find somewhere to put stuff!
		if (  currentRecordBuffer &&
			  currentRecordBuffer->nFrames == samplesPerBuffer/nChannel) {
			
			SampleBuffer	**tailp=&pendingRecordBuffers,
							*q = pendingRecordBuffers;
			while (q) {
				tailp = &q->next;
				q = q->next;
			}
			
			*tailp = currentRecordBuffer;
			
			currentRecordBuffer = nullptr;
		}
		
		if (currentRecordBuffer == nullptr) {
			SampleBuffer		*nbuf;
			
			if (freeRecordBuffers) {
				nbuf = freeRecordBuffers;
				freeRecordBuffers = nbuf->next;
			} else {
				nbuf = new SampleBuffer();
			}
			nbuf->Set(nullptr, Sample::ChunkForFrame(nChannel, recordFrame), recordFrame, 0);
			nbuf->next = nullptr;

			currentRecordBuffer = nbuf;
		}
		
// put it there
		long		remainingBufFrames = (samplesPerBuffer/nChannel)-currentRecordBuffer->nFrames;
		long		framesToFit = Min(nFrames-stashedFrame, remainingBufFrames);

//	fprintf(stderr, "stashed %d of %d for %d in %x (%d): %g\r",
//						framesToFit,
//						nFrames,
//						recordFrame,
//						currentRecordBuffer,
//						/*currentRecordBuffer->nFrames */0,
//						inBuf[0]);

		sample_buf_copy_to_interleaved(
			&currentRecordBuffer->data[
						currentRecordBuffer->nFrames*nChannel],
			inBuf, nChannel, stashedFrame,
			framesToFit); 
//		for (short i=0; i< toFit; i++) {
//			fprintf(stderr, "%g\n", inBuf[stashedFrame+i]);
//		}
		stashedFrame += framesToFit;
		recordFrame += framesToFit;
		currentRecordBuffer->nFrames += framesToFit;

	}
	recordbufLock.unlock();
	return nFrames;
}


status_t
Sample::FlushRecordBuffers(bool finalflush)
{
	status_t	err = B_NO_ERROR;

//	fprintf(stderr, "fluush\n");
	bool		flushing = finalflush;
	do {
		recordbufLock.lock();
		SampleBuffer		*wb = pendingRecordBuffers;
		if (wb != nullptr) {
			pendingRecordBuffers = wb->next;
//			fprintf(stderr, "pending %x %x size %d from %d\n", wb, wb->next, wb->nFrames, wb->fromFrame);
		}	
		
		if (wb == nullptr) {
	// nothing pending!
	// check to flush remaining buffers
			if (finalflush) {
				flushing = false;
				wb = currentRecordBuffer;
			}
		}
	
		recordbufLock.unlock();
		if (wb != nullptr) {		// write it!
			long	toWrite = wb->nFrames*recordTake->file->nChannels*recordTake->file->sampleSize;
			recordTake->file->NormalizeOutput(((float *)wb->data), wb->nFrames);
			recordTake->file->SeekToFrame(wb->fromFrame);
			if ((err=recordTake->file->Write(wb->data, toWrite)) != toWrite) {
				reportError("Write error in sampler");
				break;
			}
			recordbufLock.lock();
			wb->next = freeRecordBuffers;
			freeRecordBuffers = wb;
			recordbufLock.unlock();
		}
	} while (flushing);
	
	if (finalflush) {
//		nFrames = recordTake->file->Frame();
		fprintf(stderr, "nf %d\n", recordTake->file->Frame());
		recordbufLock.lock();
		delete freeRecordBuffers;
		freeRecordBuffers = nullptr;
		recordbufLock.unlock();
		recordTake->file->Finalize();
	}
	return B_NO_ERROR;
}

// assume that the sample rate of a sample is identical to the
// output sample rate ?????????????????????????????????????
long
Sample::ResetClip(Clip *clip)
{
	long frame;
	frame = clip->start.ticks;
	if (clip->media) {
		SampleTake	*t = (SampleTake*)clip->media;
		long c = ChunkForFrame(t->file, frame);
		RequestChunk(t, c);
		RequestChunk(t, c+1);
	}
	return frame;
}

long
Sample::ResetTake(SampleTake *take)
{
	RequestChunk(take, 0);
	RequestChunk(take, 1);
	return 0;
}

///////////////////////////////////////////////////////////////////////////
// request a chunk of data
//////////////////////////////////////////
bool
Sample::RequestChunk(SampleTake *take, long chunk)
{
//	fprintf(stderr, "requesting %d %x\n", chunk, take);
	for (short i=0; i<maxRequests; i++) {
		if (request[i] == chunk && requestedTake[i] == take) {
//			fprintf(stderr, "already got %d %x\n", chunk, take);
			return true;
		} else if (request[i] == SAMPLE_DATA_REQUEST_NOTHING) {
//			fprintf(stderr, "placing an order for %d %x\n", chunk, take);
			request[i] = chunk;
			requestedTake[i] = take;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
// synchronise buffers for this sample
// looks for audio players in samplers it runs.
//////////////////////////////////////////////////////////////////////////

long
Sample::SynchronizeBuffers()
{
	if (status != STATUS_SLEEPING && countTakes() > 0) {
		short				nPrimary=0;
		short				k = 0;

// uberQua->objectsBlocksStacks should already have a readlock on
		int		n = takes.size();
		short	i = 0;
		short   j = 0;
		for (j=0; j<maxBuffers; j++) {
			if (buffer[j]) {
				buffer[j]->nRequest = 0;
			}
		}
// count the number of requests for each currently held buffer
// requests are filed by sample playing blocks as they go ...
		for (j=0; j<maxRequests; j++) {
//			fprintf(stderr, "got stuff for %s %d\n", sym->name, j, request[j]);
			if (request[j] != SAMPLE_DATA_REQUEST_NOTHING) {
				SampleBuffer	*bufForRequest = 
						BufferForChunk(requestedTake[j]->file, request[j]);
				if (bufForRequest != nullptr) {
					bufForRequest->nRequest++;
					request[j] = SAMPLE_DATA_REQUEST_NOTHING;
				} else {
//					fill the request later
				}
			}
		}

// request[j] can get asynchronously modified by the audio loop
		for (j=0; j<maxRequests; j++) {
			if (request[j] != SAMPLE_DATA_REQUEST_NOTHING) {
				SampleBuffer	*bufForRequest = BufferForChunk(
					requestedTake[j]->file, request[j]);
				if (bufForRequest == nullptr) {
//					fprintf(stderr, "filling request %d from %s -> buf address %x\n", request[j], requestedTake[j]?requestedTake[j]->path.Path():"null", bufForRequest);
//					fill the request now ...
//					find an empty buffer or a buffer with no requests
					short		newBuf = -1;
					if (nBuffers < maxBuffers) {	// new buffer
						newBuf = nBuffers++;
						buffer[newBuf] = new SampleBuffer();
					} else {	// junk something we don't want
						for (short k=0; k<nBuffers; k++) {
							if (	buffer[k]->nFrames == 0 ||
									buffer[k]->fromFrame == -1 ||
									buffer[k]->nRequest == 0) {
									// ... zap this one
								newBuf = k;
								break;
							}
						}
					}

	//					fill this buffer
					SampleFile	*file = requestedTake[j]->file;
					long		chunkStart = StartFrameOfChunk(file, request[j]);
					long		bytesToRead;
					long		chunkInd = 0;
					long		curFrame = chunkStart;
					status_t	err = file->SeekToFrame(chunkStart);
					if (err != B_NO_ERROR) {
						reportError("qua: bufferaetor seek failed");
						continue;
					}
				// looping not really necessary here, but may
				// be for streamed stuff				
					do {
						long	fileSamplesAvail = (file->nFrames - curFrame) *
										file->nChannels;
						long	bufSamplesAvail = (samplesPerBuffer-chunkInd);
						bytesToRead =
							((fileSamplesAvail<bufSamplesAvail)?
								fileSamplesAvail:bufSamplesAvail)*file->sampleSize;
						if (bytesToRead > fileBufferLength) {
							bytesToRead = fileBufferLength;
						}
						long framesToRead = bytesToRead /
										(file->nChannels * file->sampleSize);
						long	bytesRead;
						long	framesRead;

						if ((bytesRead=file->Read(fileBuffer, bytesToRead)) != bytesToRead) {
							if (bytesRead < 0) {
								reportError("qua: sample %s, error reading %d bytes at frame %d of %Ld: %s\n",
									requestedTake[j]->path,
									bytesToRead,
									curFrame,
									file->nFrames,
									errorStr(bytesRead));
								goto seriousBadCookie;
							} else if (bytesRead == 0) {
								reportError("qua: sample %s, null read, reading %d bytes at frame %d of %Ld\n",
									requestedTake[j]->path,
									bytesToRead,
									curFrame,
									file->nFrames);
								goto seriousBadCookie;
							} else {
								reportError("qua: sample %s, read count error, %d returned while reading %d bytes at frame %d of %Ld\n",
									requestedTake[j]->path,
									bytesRead,
									bytesToRead,
									curFrame,
									file->nFrames);
							}
						}

//						fprintf(stderr, "read %d bytes %d frames got %d bytes from %s buf %d %d\n",
//							bytesToRead, framesToRead, bytesRead, requestedTake[j]->path.Path(), newBuf, chunkInd);
						if (bytesRead > 0) {
							file->NormalizeInputCpy(
								&buffer[newBuf]->data[chunkInd],
								fileBuffer,
								bytesRead);
						
							framesRead = bytesRead /
											(file->nChannels *
											file->sampleSize);

							chunkInd += framesRead * file->nChannels;
							curFrame += framesRead;
						}
					} while (	chunkInd < samplesPerBuffer &&
								bytesToRead == fileBufferLength);
					buffer[newBuf]->Set(file, request[j], chunkStart, chunkInd/file->nChannels);
				}
			}
		}

seriousBadCookie:
		;
	}
	
	return B_NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////
// play sample
// this is a vanilla unpitched player ... the sample_buf_ routines can be optimized
// seriously, and it's a lot easier to guess the next couple of file chunks.
// !! we are doing copy ops so because we've got a seperate gain here
// ?????????????????????????????????????? thread sfety issues if clip gets changed?
////////////////////////////////////////////////////////////////////////////////////////
long
Sample::Play(float **outSig, short nAudioChannels, long nFramesReqd,
			 SampleTake *take, long startFrame, long endFrame, long currentFrame,
			 float gain, float pan, int32 &state, flag loop, flag dir)
{
	long outFrame = 0;
	SampleFile	*file = take->file;
// request that the buffers should be there! hopefully we've requested at least the current
// and the next one.
	long	chunk = ChunkForFrame(file, currentFrame);
	RequestChunk(take, chunk);
	long	nextChunk =
				((dir==0)?
					take->NextChunk(chunk, startFrame, endFrame):
					take->PrevChunk(chunk, startFrame, endFrame));
	RequestChunk(take, nextChunk);

	SampleBuffer	*currentBuffer=nullptr;
	while (outFrame < nFramesReqd) {
		long nFramesAvail;
		if (dir == 0) {
			if (currentFrame < startFrame) {
				currentFrame = startFrame;
			}
			if (currentFrame >= endFrame) {
// big question .. should we finish this buffer looped
// ideally we should check a condition
// perhaps we need another play type component that does it that way
// maybe one with a built in condition
// here we opt for maximum loop smoothness
// UpdateActive will handle the resetting of the state variable after it does its stuff.
//	 for instance, recordkeeping like				loopCount++;
//					LoopEnvelopes();
				if (loop) {	// set a loop end state, and keep chugging
					state = PLAYSTATE_LOOPEND;
					currentFrame = startFrame;
				} else { // bug out and fill with zeros
					state = PLAYSTATE_LOOPEND;
					break;
				}
			}
			if (  currentFrame < 0 ||  currentFrame >= file->nFrames) {	// a frame of nothing
				break;
			} else {
				if (!currentBuffer || !currentBuffer->HasFrame(file, currentFrame)) {
					currentBuffer = BufferForFrame(file, currentFrame);
					if (currentBuffer == nullptr) {
						break;
					}				
				}
//			get crap from current frame
				int bufFrame = (currentFrame - currentBuffer->fromFrame);
				nFramesAvail = min32(nFramesReqd-outFrame,
											currentBuffer->nFrames-bufFrame);
				nFramesAvail = min32(nFramesAvail, endFrame-currentFrame);
	
				if (nAudioChannels == file->nChannels) {
//					fprintf(stderr, "adding %d %d interleaved from %s\n", outFrame, nFramesAvail, take->path.Path());
					sample_buf_copy_interleaved(
						outSig, &currentBuffer->data[bufFrame*file->nChannels],
						nAudioChannels, outFrame, nFramesAvail);
				} else if (nAudioChannels == 2 && file->nChannels == 1) {
					sample_buf_copy_mono(
						outSig, &currentBuffer->data[bufFrame*file->nChannels],
						nAudioChannels, outFrame, nFramesAvail);
				} else if (nAudioChannels == 1 && file->nChannels == 2) {
					sample_buf_copy_centring_interleaved_stereo(
						outSig, &currentBuffer->data[bufFrame*file->nChannels],
						nAudioChannels, outFrame, nFramesAvail);
				} else {	// ??????? have a good think about this and get ready for when it happens
					;
				}

				currentFrame += nFramesAvail;
			}
		} else if (dir == 1) {	// backwards
			if (currentFrame > endFrame) {
				currentFrame = endFrame;
			}
			if (currentFrame <= startFrame) {
// big question .. should we finish this buffer looped
// ideally we should check a condition
// perhaps we need another play type component that does it that way
// maybe one with a built in condition
// here we opt for maximum loop smoothness
// UpdateActive will handle the resetting of the state variable after it does its stuff.
//	 for instance, recordkeeping like				loopCount++;
//					LoopEnvelopes();
				if (loop) {	// set a loop end state, and keep chugging
					state = PLAYSTATE_LOOPEND;
					currentFrame = endFrame;
				} else { // bug out and fill with zeros
					state = PLAYSTATE_LOOPEND;
					break;
				}
			}
			if (  currentFrame < 0 ||  currentFrame >= file->nFrames) {	// a frame of nothing
				break;
			} else {
				if (!currentBuffer || !currentBuffer->HasFrame(file, currentFrame)) {
					currentBuffer = BufferForFrame(file, currentFrame);
					if (currentBuffer == nullptr) {
						break;
					}				
				}
//			get crap from current frame
				int bufFrame = (currentFrame - currentBuffer->fromFrame);
				nFramesAvail = min32(nFramesReqd-outFrame, bufFrame);
				nFramesAvail = min32(nFramesAvail, currentFrame-startFrame);
	
				if (nAudioChannels == file->nChannels) {
//					fprintf(stderr, "adding %d %d interleaved from %s\n", outFrame, nFramesAvail, take->path.Path());
					sample_buf_copy_reversed_interleaved(
						outSig, &currentBuffer->data[bufFrame*file->nChannels],
						nAudioChannels, outFrame, nFramesAvail);
				} else if (nAudioChannels == 2 && file->nChannels == 1) {
					sample_buf_copy_reversed_mono(
						outSig, &currentBuffer->data[bufFrame*file->nChannels],
						nAudioChannels, outFrame, nFramesAvail);
				} else if (nAudioChannels == 1 && file->nChannels == 2) {
					sample_buf_copy_reversed_centring_interleaved_stereo(
						outSig, &currentBuffer->data[bufFrame*file->nChannels],
						nAudioChannels, outFrame, nFramesAvail);
				} else {	// ??????? have a good think about this and get ready for when it happens
					;
				}

				currentFrame -= nFramesAvail;
			}
		}
		outFrame += nFramesAvail;
	}

	if (outFrame != nFramesReqd) {
		sample_buf_zero(outSig, nAudioChannels, outFrame, nFramesReqd);
	}

	if (nAudioChannels == 2) {
		float	gainp[] = {gain * ((1-(pan))/2),gain * ((1+(pan))/2)};
		sample_buf_gain(outSig, gainp, nAudioChannels, nFramesReqd);
	} else if (nAudioChannels == 1) {
		sample_buf_gain(outSig, gain, nAudioChannels, nFramesReqd);
	} else { // not sure what pan means in this context
		sample_buf_gain(outSig, gain, nAudioChannels, nFramesReqd);
	}
	return currentFrame; 
}

////////////////////////////////////////////////////////////////////////
// a tad heavier. pitched player. linear interp
////////////////////////////////////////////////////////////////////////
double
Sample::PlayPitched(float **outSig, short nAudioChannels, long nFramesReqd,
			 SampleTake *take, long startFrame, long endFrame, double position,
			 float gain, float pan, double index, float *lastSig, int32 &state, flag loop, flag dir)
{
	long outFrame = 0;
	long	nRead = 0;
	SampleFile	*file = take->file;
	long	currentFrame = (long)((dir==0)?floor(position):ceil(position));
// request that the buffers should be there! hopefully we've requested at least the current
// and the next one.
	long	chunk = ChunkForFrame(file, currentFrame);
	RequestChunk(take, chunk);
	long	nextChunk=chunk;
	do {
		nextChunk = ((dir==0)?
					take->NextChunk(nextChunk, startFrame, endFrame):
					take->PrevChunk(nextChunk, startFrame, endFrame));
		if (nextChunk == chunk) break;
		RequestChunk(take, nextChunk);
	} while (StartFrameOfChunk(file, abs(chunk-nextChunk))<2*index*nFramesReqd);
	SampleBuffer	*currentBuffer=nullptr;
	while (outFrame < nFramesReqd) {
		long nFramesAvail;
		if (dir == 0) {
			while (position < startFrame) {
				position += index;
			}
			if (position >= endFrame) {
// big question .. should we finish this buffer looped
// ideally we should check a condition
// perhaps we need another play type component that does it that way
// maybe one with a built in condition
// here we opt for maximum loop smoothness
// UpdateActive will handle the resetting of the state variable after it does its stuff.
//	 for instance, recordkeeping like				loopCount++;
//					LoopEnvelopes();
				if (loop) {	// set a loop end state, and keep chugging
					state = PLAYSTATE_LOOPEND;
					position = startFrame;
				} else { // bug out and fill with zeros
					state = PLAYSTATE_LOOPEND;
					break;
				}
			}
			currentFrame = (long)floor(position);
			if (  position < 0 ||  position >= file->nFrames) {	// a frame of nothing
				break;
			} else {
				if (!currentBuffer || !currentBuffer->HasFrame(file, currentFrame)) {
					currentBuffer = BufferForFrame(file, currentFrame);
					if (currentBuffer == nullptr) {
						break;
					}				
				}
//			get crap from current frame
				int bufFrame = (currentFrame - currentBuffer->fromFrame);
				nFramesAvail = min32(nFramesReqd-outFrame,
									currentBuffer->nFrames-bufFrame);
				nFramesAvail = min32(nFramesAvail, endFrame-currentFrame);
				if (nAudioChannels == file->nChannels) {
//					fprintf(stderr, "adding %d %d interleaved from %s\n", outFrame, nFramesAvail, take->path.Path());
					nRead = sample_buf_copy_linearpitch_interleaved(
						outSig, currentBuffer->data,
						nAudioChannels, outFrame, nFramesAvail,
						position, index, lastSig);
				} else if (nAudioChannels == 2 && file->nChannels == 1) {
					nRead = sample_buf_copy_linearpitch_mono(
						outSig, currentBuffer->data,
						nAudioChannels, outFrame, nFramesAvail,
						position, index, lastSig);
				} else if (nAudioChannels == 1 && file->nChannels == 2) {
					nRead = sample_buf_copy_linearpitch_centring_interleaved_stereo(
						outSig, currentBuffer->data,
						nAudioChannels, outFrame, nFramesAvail,
						position, index, lastSig);
				} else {	// ??????? have a good think about this and get ready for when it happens
					;
				}

				position += nRead*index;
			}
		} else if (dir == 1) {	// backwards
			while (position > endFrame) {
				position -= index;
			}
			if (position <= startFrame) {
// big question .. should we finish this buffer looped
// ideally we should check a condition
// perhaps we need another play type component that does it that way
// maybe one with a built in condition
// here we opt for maximum loop smoothness
// UpdateActive will handle the resetting of the state variable after it does its stuff.
//	 for instance, recordkeeping like				loopCount++;
//					LoopEnvelopes();
				if (loop) {	// set a loop end state, and keep chugging
					state = PLAYSTATE_LOOPEND;
					position = endFrame;
				} else { // bug out and fill with zeros
					state = PLAYSTATE_LOOPEND;
					break;
				}
			}
			currentFrame = (long)ceil(position);
			if (  currentFrame < 0 ||  currentFrame >= file->nFrames) {	// a frame of nothing
				break;
			} else {
				if (!currentBuffer || !currentBuffer->HasFrame(file, currentFrame)) {
					currentBuffer = BufferForFrame(file, currentFrame);
					if (currentBuffer == nullptr) {
						break;
					}				
				}
//			get crap from current frame
				int bufFrame = (currentFrame - currentBuffer->fromFrame);
				nFramesAvail = min32(nFramesReqd-outFrame, bufFrame);
				nFramesAvail = min32(nFramesAvail, currentFrame-startFrame);
	
				if (nAudioChannels == file->nChannels) {
//					fprintf(stderr, "adding %d %d interleaved from %s\n", outFrame, nFramesAvail, take->path.Path());
					nRead = sample_buf_copy_linearpitch_interleaved(
						outSig, currentBuffer->data,
						nAudioChannels, outFrame, nFramesAvail,
						position, -index, lastSig);
				} else if (nAudioChannels == 2 && file->nChannels == 1) {
					nRead = sample_buf_copy_linearpitch_mono(
						outSig, currentBuffer->data,
						nAudioChannels, outFrame, nFramesAvail,
						position, -index, lastSig);
				} else if (nAudioChannels == 1 && file->nChannels == 2) {
					nRead = sample_buf_copy_linearpitch_centring_interleaved_stereo(
						outSig, currentBuffer->data,
						nAudioChannels, outFrame, nFramesAvail,
						position, -index, lastSig);
				} else {	// ??????? have a good think about this and get ready for when it happens
					;
				}

				position -= nFramesAvail;
			}
		}
		outFrame += nRead;
	}

	if (outFrame != nFramesReqd) {
		sample_buf_zero(outSig, nAudioChannels, outFrame, nFramesReqd);
	}

	if (nAudioChannels == 2) {
		float	gainp[] = {gain * ((1-(pan))/2),gain * ((1+(pan))/2)};
		sample_buf_gain(outSig, gainp, nAudioChannels, nFramesReqd);
	} else if (nAudioChannels == 1) {
		sample_buf_gain(outSig, gain, nAudioChannels, nFramesReqd);
	} else { // not sure what pan means in this context
		sample_buf_gain(outSig, gain, nAudioChannels, nFramesReqd);
	}
	return position; 
}


//////////////////////////////////////////////////////////////////////////////////
// sample instance
//////////////////////////////////////////////////////////////////////////////////
SampleInstance::SampleInstance(class Sample *s, std::string nm, Time t, Time d, Channel *c):
	Instance(s, nm, t, d, c),
	startFrame(0,&Metric::sampleRate),
	endFrame(0,&Metric::sampleRate)
{
	mute = false;
//	currentFrame = 0;
//	currentBuffer = nullptr;
	startFrame = 0;
//	endFrame = s->selectedNFrames;
	amp = 0.5;
	pan = 0;
	loopCount = 0;
	eFrameExp = sFrameExp = nullptr;

	loopCondition = new Block( Block::C_VALUE);
	loopCondition->crap.constant.value = TypedValue::Bool(1);
}

SampleInstance::~SampleInstance()
{
}


void
SampleInstance::Reset()
{
	Instance::Reset();
//	currentFrame = startFrame.ticks;
}

bool
SampleInstance::Init()
{
	if (!Instance::Init())
		return false;
	glob.PushContext(sym);	
	if (  loopCondition &&
		  !loopCondition->Init(schedulable) &&
		  !loopCondition->Reset(mainStack))
		goto err_ex;
	if (  sFrameExp &&
		  !sFrameExp->Init(schedulable) &&
		  !sFrameExp->Reset(mainStack))
		goto err_ex;
	if (  eFrameExp &&
		  !eFrameExp->Init(schedulable) &&
		  !eFrameExp->Reset(mainStack))
		goto err_ex;
	glob.PopContext(sym);	
	return true;
err_ex:
	glob.PopContext(sym);	
	return false;
}

void
SampleInstance::Cue(Time &t)
{
}


bool
SampleInstance::LoopConditionMet(StreamItem *item, Instance *instance, QuasiStack *stack)
{
	if (loopCondition == nullptr) {
		return true;
	} else {
		ResultValue	v = EvaluateExpression(loopCondition, item, instance, instance->sym, stack);
		return v.BoolValue(nullptr);
	}
}

#ifdef QUA_V_BETTER_SCHEDULER
status_t
SampleInstance::Run()
{
	wakeDuration = uberQua->theTime - startTime;
	UpdateEnvelopes(wakeDuration);
	
	if (schedulable->mainBlock) {
		Stream	mainStream;
		Time	tag_time = uberQua->theTime;

		////////// should be locked on mainBlock !!!
		////////// readwrite lock on stacks
		stackLock.lock();
		flag	uac = UpdateActiveBlock(
						uberQua,
						&mainStream,
						schedulable->mainBlock,
						&tag_time,
						this,
						sym,
						mainStack,
						1,
						true);
		stackLock.unlock();
		////////// readwrite lock on stacks

		if (channel)
			channel->OutputStream(&mainStream);
		mainStream.ClearStream(); // garbage collect!!
	}
	return B_NO_ERROR;
}
#endif

//
// main stream operator.
//      outputs stereo 16 bit 44.1,
//		len is # of samples
#ifdef OLDER_SAMPLE
size_t
SampleInstance::Generate(float **outSig, long nFramesReqd, short nAudioChannels)
{
	if (mute)
		return 0;

	long		outFrame = 0;
	
//	fprintf(stderr, "in %d  cf %d %d %d\n",
//			 nFramesReqd, currentFrame, startFrame.ticks, endFrame.ticks);
	if (startFrame.ticks < endFrame.ticks) {
		while (outFrame < nFramesReqd) {
			if (currentFrame < startFrame.ticks) {
				currentFrame = startFrame.ticks;
			}
			if (currentFrame >= endFrame.ticks) {
				if (LoopConditionMet(0, this, mainStack)) {
					if (sFrameExp) {
						ResultValue	v = EvaluateExpression(sFrameExp);
						startFrame.ticks = v.IntValue(nullptr);
					}
					if (eFrameExp) {
						ResultValue	v = EvaluateExpression(eFrameExp);
						endFrame.ticks = v.IntValue(nullptr);
					}
					loopCount++;
					currentFrame = startFrame.ticks;
					// other loop startish things ...
					LoopEnvelopes();
				} else {
	//				fprintf(stderr, "not ready ...\n");
					break;
				}
			}
			if (  currentFrame < 0 ||
				  currentFrame >= QSample()->nFrames) {	// a frame of nothing
//				fprintf(stderr, "empty\n");
				break;
			} else {
				if (!currentBuffer || !currentBuffer->HasFrame(currentFrame)) {
					currentBuffer = QSample()->BufferForFrame(currentFrame);
					if (currentBuffer == nullptr) {
						fprintf(stderr, "break while looking for buf with frame %d. Got\n\t%d %d %d %d %d %d %d %d\n\t%d %d %d %d %d %d %d %d\n",
								QSample()->ChunkForFrame(currentFrame),
								QSample()->buffer[0]?QSample()->ChunkForFrame(QSample()->buffer[0]->fromFrame):-1,
								QSample()->buffer[1]?QSample()->ChunkForFrame(QSample()->buffer[1]->fromFrame):-1,
								QSample()->buffer[2]?QSample()->ChunkForFrame(QSample()->buffer[2]->fromFrame):-1,
								QSample()->buffer[3]?QSample()->ChunkForFrame(QSample()->buffer[3]->fromFrame):-1,
								QSample()->buffer[4]?QSample()->ChunkForFrame(QSample()->buffer[4]->fromFrame):-1,
								QSample()->buffer[4]?QSample()->ChunkForFrame(QSample()->buffer[5]->fromFrame):-1,
								QSample()->buffer[6]?QSample()->ChunkForFrame(QSample()->buffer[6]->fromFrame):-1,
								QSample()->buffer[7]?QSample()->ChunkForFrame(QSample()->buffer[7]->fromFrame):-1,
								QSample()->buffer[8]?QSample()->ChunkForFrame(QSample()->buffer[8]->fromFrame):-1,
								QSample()->buffer[9]?QSample()->ChunkForFrame(QSample()->buffer[9]->fromFrame):-1,
								QSample()->buffer[10]?QSample()->ChunkForFrame(QSample()->buffer[10]->fromFrame):-1,
								QSample()->buffer[11]?QSample()->ChunkForFrame(QSample()->buffer[11]->fromFrame):-1,
								QSample()->buffer[12]?QSample()->ChunkForFrame(QSample()->buffer[12]->fromFrame):-1,
								QSample()->buffer[13]?QSample()->ChunkForFrame(QSample()->buffer[13]->fromFrame):-1,
								QSample()->buffer[14]?QSample()->ChunkForFrame(QSample()->buffer[14]->fromFrame):-1,
								QSample()->buffer[15]?QSample()->ChunkForFrame(QSample()->buffer[15]->fromFrame):-1
								);
						break;
					}				
				}
//			get crap from current frame
				int bufFrame = (currentFrame - currentBuffer->fromFrame);
				size_t	nFramesAvail = min32(nFramesReqd-outFrame,
											currentBuffer->nFrames-bufFrame);
				nFramesAvail = min32(nFramesAvail, endFrame.ticks-currentFrame);
//				fprintf(stderr, "%d\r", nFramesAvail);
//		fprintf(stderr, "cf %d of %d buffr %d from %d nfa %d bo %d ef %d       \n",
//					currentFrame,
//					outFrame,
//					currentBuffer->nFrames,
//					currentBuffer->fromFrame,
//					nFramesAvail,
//					bufFrame,
//					endFrame.ticks);
	
				memcpy(	&outSig[outFrame*QSample()->nChannels],
						&currentBuffer->data[bufFrame*QSample()->nChannels],
						nFramesAvail*QSample()->nChannels*sizeof(float));
				outFrame += nFramesAvail;
				currentFrame += nFramesAvail;
			}
		}
	}

	sample_buf_zero(outSig, nChan, outFrame, nFramesReqd-outFrame);

// tempSig should now be float, regardless... but stereo or mono.
//	some FX could be in mono ...
//  some FX could change frame count, and could be dealt by the reader.
//  perhaps all could be but this will kill response ...

	float		ampl = amp * ((1-(pan))/2),
				ampr = amp * ((1+(pan))/2);		
	if (QSample()->nChannels == 2) {
		for (short i=0; i<nFramesReqd; i++) {
			outSig[2*i] = ampl*outSig[2*i];
			outSig[2*i+1] = ampr*outSig[2*i+1];
		}
	} else if (QSample()->nChannels == 1) {
		for (short i=nFramesReqd-1; i>=0; i--) {
			outSig[2*i] = ampl*outSig[i];
			outSig[2*i+1] = ampr*outSig[i];
		}
	}
	
//	fprintf(stderr, "!os[0] %g oframe %d\r", outSig[0], outFrame);
#ifdef LOTSALOX
	schedulable->stackableLock.Lock();
#endif
	if (schedulable->mainBlock && mainStack->locusStatus == STATUS_RUNNING) {
		ApplyQuaFX(mainStack, schedulable->mainBlock, outSig, nFramesReqd, nAudioChannels);
	}
#ifdef LOTSALOX
	schedulable->stackableLock.Unlock();
#endif

//	fprintf(stderr, "@os[0] %g oframe %d %d-%d\n", outSig[0], outFrame, startFrame.ticks, endFrame.ticks);
	
// now in stereo

	return nFramesReqd;
}

#else
size_t
SampleInstance::Generate(float **outSig, long nFramesReqd, short nAudioChannels)
{
	if (mute)
		return 0;
	sample_buf_zero(outSig, nAudioChannels, nFramesReqd);
	
//	fprintf(stderr, "!os[0] %g oframe %d\r", outSig[0], outFrame);
#ifdef LOTSALOX
	schedulable->stackableLock.Lock();
#endif
	if (schedulable->mainBlock && mainStack->locusStatus == STATUS_RUNNING) {
		ApplyQuaFX(mainStack, schedulable->mainBlock, outSig, nFramesReqd, nAudioChannels);
	}
#ifdef LOTSALOX
	schedulable->stackableLock.Unlock();
#endif

//	fprintf(stderr, "@os[0] %g oframe %d %d-%d\n", outSig[0], outFrame, startFrame.ticks, endFrame.ticks);
	
// now in stereo

	return nFramesReqd;
}
#endif	

SampleBuffer::SampleBuffer()
{
#ifdef QUA_V_RAM_LOCKED_BUFFERS
#ifdef _BEOS
	dataArea = create_area("sample data", (void **)&data,
	     B_ANY_ADDRESS, bytesPerBuffer, B_NO_LOCK, 
	      B_READ_AREA|B_WRITE_AREA); 
	if (dataArea < B_NO_ERROR) { 
		TragicError("Couldn't allocate sample buffer");
	}
#elif defined(WIN32)
	data = new float[samplesPerBuffer];
	if (data == nullptr) {
		TragicError("SampleBuffer:: can't allocate a %d sample buffer", samplesPerBuffer);
	}
	if (!VirtualLock(data, samplesPerBuffer*sizeof(float))) {
		fprintf(stderr, "virtual lock fails: %s\n", general_error_string(GetLastError()));
	}
#endif
#else
	data = new float[samplesPerBuffer];
//	fprintf(stderr, "alloc buffer %x, %d samples, %d bytes\n", data, samplesPerBuffer, sizeof(float)*samplesPerBuffer);
	if (data == nullptr) {
		tragicError("SampleBuffer:: can't allocate a %d sample buffer", samplesPerBuffer);
	}
#endif
	nFrames = 0;
	fromFrame = -1;	
	chunk = -1;
	file = nullptr;
	next = nullptr;
	nRequest = 0;
}

SampleBuffer::~SampleBuffer()
{
#ifdef QUA_V_RAM_LOCKED_BUFFERS
#ifdef _BEOS
	delete_area(dataArea);
#else
	if (!VirtualUnlock(data, samplesPerBuffer*sizeof(float))) {
		fprintf(stderr, "virtual lock fails: %s\n", general_error_string(GetLastError()));
	}
	delete data;
#endif
#else
	delete data;
#endif
	if (next)
		delete next;
}

void
SampleBuffer::Set(SampleFile *f, long ch, long fr, long nf)
{
	file = f;
	chunk = ch;
	fromFrame = fr;
	nFrames = nf;
}
