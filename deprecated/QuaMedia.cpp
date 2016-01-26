#include "qua_version.h"

// Be specific audio codes

#ifdef QUA_V_AUDIO

#ifdef _BEOS

#ifndef NEW_MEDIA
#include "OldMediaKit.h"
#endif

#include "StdDefs.h"
#include "SampleFile.h"

#include "include/Qua.h"
#include "include/QuaAudio.h"
#include "include/Sampler.h"
#include "include/Sample.h"
#include "include/Channel.h"
#include "include/FloatQueue.h"

#ifdef QUA_V_ARRANGER_INTERFACE
#include "include/SampleEditor.h"
#endif

short debug_media = 1;

#define	samplesPerOutFrame	2

static float		qmo_temp[8192];
static float		qmi_temp[8192];

#ifdef NEW_MEDIA

#define	EXTRA_BUFS	4		// just being greedy for security

#define AUDIO_8BIT_RANGE	256
#define AUDIO_16BIT_RANGE	32767
#define AUDIO_32BIT_RANGE	0x1fffffff

//void 
//Sampler::SetRunMode(run_mode mode)
//{
//	// The BMediaEventLooper tracks our run mode for us
//	BMediaEventLooper::SetRunMode(mode);
//}

void 
Sampler::Preroll()
{
	BMediaEventLooper::Preroll();
}

void 
Sampler::SetTimeSource(BTimeSource* time_source)
{
	BMediaNode::SetTimeSource(time_source);
}

status_t 
Sampler::RequestCompleted(const media_request_info &info)
{
	return BMediaNode::RequestCompleted(info);
}


///////////////////////
// BMediaNode methods
//////////////////////
BMediaAddOn*
Sampler::AddOn(int32 *) const
{
	return nullptr;
}

///////////////////////////////
// BBufferProducer methods ////
///////////////////////////////
status_t 
Sampler::FormatSuggestionRequested(media_type type, int32 /*quality*/, media_format* format)
{
	// FormatSuggestionRequested() is not necessarily part of the format negotiation
	// process; it's simply an interrogation -- the caller wants to see what the node's
	// preferred data format is, given a suggestion by the caller.

	if (!format) {
		::reportError("Sampler::FormatSuggestionRequested() nullptr format pointer passed.");
		return B_BAD_VALUE;
	}

	// this is the format we'll be returning (our preferred format)
	*format = preferredFormat;

	// a wildcard type is okay; we can specialize it
	if (type == B_MEDIA_UNKNOWN_TYPE)
		type = B_MEDIA_RAW_AUDIO;

	// we only support raw audio
	if (type != B_MEDIA_RAW_AUDIO)
		return B_MEDIA_BAD_FORMAT;
	else
		return B_OK;
}

status_t 
Sampler::FormatProposal(const media_source& floutput, media_format* format)
{
	// FormatProposal() is the first stage in the BMediaRoster::Connect() process.  We hand
	// out a suggested format, with wildcards for any variations we support.
	fprintf(stderr, "Sampler::FormatProposal()\n");

	QuaAudioOut	*op=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (floutput == outPort[i].output.source) {
			op = &outPort[i];
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::FormatProposal(), bad source %d/%d\n", floutput.id, floutput.port);
		return B_MEDIA_BAD_SOURCE;
	}

// we accept just about anything now... and don't try to force itbut
// supply an error code depending on whether we found the proposal acceptable.
	media_type requestedType = format->type;

	if (  (requestedType != B_MEDIA_UNKNOWN_TYPE) &&
		  (requestedType != B_MEDIA_RAW_AUDIO)) {
		fprintf(stderr, "Sampler::FormatProposal(): non audio format request\n");
		return B_MEDIA_BAD_FORMAT;
	}	
	if (format->u.raw_audio.buffer_size == media_raw_audio_format::wildcard.buffer_size) {
		format->u.raw_audio.buffer_size = 2048;		// pick something comfortable to suggest
		fprintf(stderr, "Sampler::FormatProposal(): no buffer size provided, suggesting %lu\n", format->u.raw_audio.buffer_size);
	} else {
		fprintf(stderr, "Sampler::FormatProposal(): consumer suggested buffer_size %lu\n", format->u.raw_audio.buffer_size);
	}
	
	return B_OK;		// raw audio or wildcard type, either is okay by us
}

// If the format isn't good, put a good format into *io_format and return error
// If format has wildcard, specialize to what you can do (and change).
// If you can change the format, return OK.
// The request comes from your destination sychronously, so you cannot ask it
// whether it likes it -- you should assume it will since it asked.
status_t 
Sampler::FormatChangeRequested(const media_source& source, const media_destination& destination, media_format* io_format, int32* _deprecated_)
{
	// we don't support any other formats, so we just reject any format changes.
	fprintf(stderr, "Sampler::FormatChangeRequested()\n");
	return B_ERROR;
}

status_t 
Sampler::GetNextOutput(int32* cookie, media_output* out_output)
{
//	fprintf(stderr, "Sampler::GetNextOutput(%d)\n", *cookie);
	// we have only a single output; if we supported multiple outputs, we'd
	// iterate over whatever data structure we were using to keep track of
	// them.
	if (*cookie < MAX_QUA_AUDIO_OUTPUT && *cookie < nOutput+4) {
		*out_output = outPort[*cookie].output;
		*cookie = *cookie + 1;
		return B_OK;
	}
	else
		return B_BAD_INDEX;
}

status_t 
Sampler::DisposeOutputCookie(int32 cookie)
{
	// do nothing because we don't use the cookie for anything special
	return B_OK;
}


// In this function, you should either pass on the group to your upstream guy, 
// or delete your current group and hang on to this group. Deleting the previous 
// group (unless you passed it on with the reclaim flag set to false) is very
// important, else you will 1) leak memory and 2) block someone who may want
// to reclaim the buffers living in that group.
status_t 
Sampler::SetBufferGroup(const media_source& source, BBufferGroup* newGroup)
{
	fprintf(stderr, "Sampler::SetBufferGroup(), source %d/%d\n", source.id, source.port);
	QuaAudioOut	*op=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (source == outPort[i].output.source) {
			op = &outPort[i];
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::SetBufferGroup(), bad source %d/%d\n", source.id, source.port);
		return B_MEDIA_BAD_SOURCE;
	}

	// Are we being passed the buffer group we're already using?
	if (newGroup == op->bufferGroup) {
		return B_OK;
	}

// Ahh, someone wants us to use a different buffer group.  At this point we delete
// the one we are using and use the specified one instead.  If the specified group is
// nullptr, we need to recreate one ourselves, and use *that*.  Note that if we're
// caching a BBuffer that we requested earlier, we have to Recycle() that buffer
// *before* deleting the buffer group, otherwise we'll deadlock waiting for that
// buffer to be recycled!

	delete op->bufferGroup;		// waits for all buffers to recycle
	if (newGroup != nullptr) {
		// we were given a valid group; just use that one from now on
		op->bufferGroup = newGroup;
		op->buffer = nullptr;
	} else	{
			// we were passed a nullptr group pointer; that means we construct
			// our own buffer group to use from now on
		int32 count = int32((op->downstreamLatency+internalLatency) / BufferDuration() + 1 + EXTRA_BUFS);
		op->bufferGroup = new BBufferGroup(op->bufSize, count);
		op->buffer = nullptr;
	}

	return B_OK;
}

status_t 
Sampler::GetLatency(bigtime_t* out_latency)
{
	*out_latency = EventLatency() + SchedulingLatency();
	return B_OK;
}

status_t 
Sampler::PrepareToConnect(const media_source& what, const media_destination& where, media_format* format, media_source* out_source, char* out_name)
{
	// PrepareToConnect() is the second stage of format negotiations that happens
	// inside BMediaRoster::Connect().  At this point, the consumer's AcceptFormat()
	// method has been called, and that node has potentially changed the proposed
	// format.  It may also have left wildcards in the format.  PrepareToConnect()
	// *must* fully specialize the format before returning!
	fprintf(stderr, "Sampler::PrepareToConnect()\n");

	QuaAudioOut	*op=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (what == outPort[i].output.source) {
			op = &outPort[i];
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::PrepareToConnect(), bad source %d/%d\n", what.id, what.port);
		return B_MEDIA_BAD_SOURCE;
	}

	fprintf(stderr, "Sampler::PrepareToConnect() to %d/%d\n", where.port, where.id);
	
	// are we already connected?
	if (op->output.destination != media_destination::null) {
		fprintf(stderr, "Sampler: already connected.\n");
		return B_MEDIA_ALREADY_CONNECTED;
	}
	
	// the format may not yet be fully specialized (the consumer might have
	// passed back some wildcards).  Finish specializing it now, and return an
	// error if we don't support the requested format.
	
	if (format->type != B_MEDIA_RAW_AUDIO)	{
		fprintf(stderr, "Sampler::PrepareToConnect(): bad format, non-raw-audio format?!\n");
		return B_MEDIA_BAD_FORMAT;
	}

// we'll try to accept just about anything, now
//	} else if (format->u.raw_audio.format != media_raw_audio_format::B_AUDIO_FLOAT) {
//		fprintf(stderr, "\tnon-float-audio format?!\n");
//		return B_MEDIA_BAD_FORMAT;
//	}

	 // !!! validate all other fields except for buffer_size here, because the consumer might have
	// supplied different values from AcceptFormat()?

	// check the buffer size, which may still be wildcarded
	if (format->u.raw_audio.buffer_size == media_raw_audio_format::wildcard.buffer_size) {
		format->u.raw_audio.buffer_size = 2048;		// pick something comfortable to suggest
		fprintf(stderr, "Sampler::PrepareToConnect(): no buffer size provided, suggesting %lu\n", format->u.raw_audio.buffer_size);
	} else {
		fprintf(stderr, "Sampler::PrepareToConnect(): consumer suggested buffer_size %lu\n", format->u.raw_audio.buffer_size);
	}

	// Now reserve the connection, and return information about it
	op->output.destination = where;
	op->output.format = *format;
	op->sampleSize = op->output.format.u.raw_audio.format &
							media_raw_audio_format::B_AUDIO_SIZE_MASK;
	op->frameSize =	format->u.raw_audio.channel_count * op->sampleSize;
	op->bufSize = format->u.raw_audio.buffer_size;
	op->nChannel = format->u.raw_audio.channel_count;

	fprintf(stderr, "Sampler::PrepareToConnect(): acceptable format: %d byte buffer, %d channels, byte order %d, format %x\n",
					format->u.raw_audio.buffer_size,
					format->u.raw_audio.channel_count,
					format->u.raw_audio.byte_order,
					format->u.raw_audio.format);

	*out_source = op->output.source;
	strncpy(out_name, op->output.name, B_MEDIA_NAME_LENGTH);
	return B_OK;
}

void 
Sampler::Connect(
			status_t error,
			const media_source& source,
			const media_destination& destination,
			const media_format& format,
			char* io_name)
{
	fprintf(stderr, "Sampler::Connect(%d %d - %d %d)\n", source.port, source.id, destination.port, destination.id);

// If something earlier failed, Connect() might still be called, but with a non-zero
// error code.  When that happens we simply unreserve the connection and do
// nothing else.
	QuaAudioOut	*op=nullptr;
	short outi = -1;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (source == outPort[i].output.source) {
			op = &outPort[i];
			outi = i;
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::Connect(): bad source %d/%d\n", source.id, source.port);
		return;
	}
	if (error) {
		op->output.destination = media_destination::null;
		op->output.format = preferredFormat;
		return;
	}

// Okay, the connection has been confirmed.  Record the destination and format
// that we agreed on, and report our connection name again.
	op->output.destination = destination;
	op->output.format = format;
	strncpy(io_name, op->output.name, B_MEDIA_NAME_LENGTH);

// Now that we're connected, we can determine our downstream latency.
// Do so, then make sure we get our events early enough.
	media_node_id id;
	FindLatencyFor(op->output.destination, &op->downstreamLatency, &id);

// Use a dry run to see how long it takes me to fill a buffer of data
	bigtime_t start, endt;
	size_t samplesPerBBuffer = op->bufSize / op->sampleSize;
	if (samplesPerBBuffer == 0) {
		fprintf(stderr, "Sampler::Connect(): no samples per buffer\n");
		return;
	}

	// reset our buffer duration, etc. to avoid later calculations
	bigtime_t duration = bigtime_t(1000000) * samplesPerBuffer /
							bigtime_t(op->output.format.u.raw_audio.frame_rate);
	SetBufferDuration(duration);

// Set up the buffer group for our connection, as long as nobody handed us a
// buffer group (via SetBufferGroup()) prior to this.  That can happen, for example,
// if the consumer calls SetOutputBuffersFor() on us from within its Connected()
// method.
	if (op->bufferGroup == nullptr) {
		// allocate enough buffers to span our downstream latency, plus one
		int32 count = int32(
						(internalLatency+op->downstreamLatency) / BufferDuration() + 1 + EXTRA_BUFS);

		fprintf(stderr, "Creating %ld %lu byte buffers.\n", count, op->bufSize);
		fprintf(stderr, "Total latency = %Ldusec, buffer duration = %Ld\n",
						op->downstreamLatency+internalLatency,
						BufferDuration());
		op->bufferGroup = new BBufferGroup(op->bufSize, count);
		op->buffer = nullptr;
		fprintf(stderr, "bg %x op %x\n", op->bufferGroup, op);
	}
// the inputs and outputs might have been reassigned during the
// nodes' negotiation of the Connect().
// all necessary renegotiation should be done by now, and these won't be
// set properly if the connect is called externally.

	op->source = source;		
	op->destination = destination;
	
// check to see if we've been called up by another node, so the Qua
// can do its own futzing. if connect is initiated by Qua, it will
// already be in the output's list.

	op->externConnection = true;
	for (short i=0; i<nOutput; i++) {
		if (output[i] == outi) {
			op->externConnection = false;
			break;
		}
	}
	
	if (op->externConnection) {
		fprintf(stderr, "added external connection\n");
//		BMessage	conMsg(CONNECT_OUTPUT);
//		conMsg.AddInt32("externally", outi);
//		conMsg.AddString("name", io_name);
//		theApp->PostMessage(&conMsg);
	}

	float* data = new float[samplesPerBBuffer];

	start = ::system_time();

// current load for rack is: sg 0 is paused with one kit part + synth...
//	Generate(data, samplesPerBBuffer);
		
	endt = ::system_time();
	internalLatency = 100000.0; //endt - start;
	delete [] data;
	
	if (op->quap_buf == nullptr) {
		op->quap_buf = new float[op->bufSize/op->sampleSize];
	}
	
	SetEventLatency(op->downstreamLatency + internalLatency);
	fprintf(stderr, "Sampler::Connect(): filling %d sample buffer took %Ld usec. Downstream latency %Ld usec\n",
					samplesPerBuffer, internalLatency, op->downstreamLatency);

	op->isConnected = true;
	op->enabled = true;

	output[nOutput++] = outi;
}

void 
Sampler::Disconnect(const media_source& what, const media_destination& where)
{
	QuaAudioOut	*op=nullptr;
	short outi = -1;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (what == outPort[i].output.source) {
			op = &outPort[i];
			outi = i;
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::Disconnect(): bad source %d/%d\n", what.id, what.port);
		return;
	}

	fprintf(stderr, "Sampler::Disconnect() (%d/%d -> %d/%d). Recognised as (%d/%d -> %d/%d)\n",
		what.port, what.id, where.port, where.id,
		op->output.source.port, op->output.source.id,
		op->output.destination.port, op->output.destination.id);

	if (where == op->output.destination) {
		op->output.destination = media_destination::null;
		op->output.format = preferredFormat;
		
		fprintf(stderr, "noodling current buffer %x\n", op->buffer);
		if (op->buffer) {
			op->buffer->Recycle();
			op->buffer = nullptr;
		}
#ifdef MANUALLYRECLAIMBUFFS
		int32 cnt;
		BBuffer	*bl[10];
		op->bufferGroup->CountBuffers(&cnt);
		op->bufferGroup->GetBufferList(cnt, bl);
		for (short i=0; i<cnt; i++) {
			fprintf(stderr, "%d %x %d\n", i, bl[i], bl[i]->_mRefCount);
			bl[i]->Recycle();
		}
#endif
		
		fprintf(stderr, "noodling buffer group %x\n", op->bufferGroup);
		delete op->bufferGroup;
		op->bufferGroup = nullptr;

		fprintf(stderr, "noodling other bits\n");
		op->isConnected = false;
		delete op->quap_buf;
		op->quap_buf = nullptr;

		if (op->externConnection) {
//// dangerous to do this for connections
//// made by rack in the normal course of things
//			DeleteOutput(outi);
		}
		op->quaport = nullptr;
		
//		BMessage	dispMsg(DISCONNECT_OUTPUT);
//		dispMsg.AddInt32("output", outi);
//		theApp->PostMessage(&dispMsg);

	} else	{
		fprintf(stderr, "Sampler::Disconnect() called with wrong source/destination (%ld/%ld), ours is (%ld/%ld)\n",
			what.id, where.id, op->output.source.id, op->output.destination.id);
		return;
	}
	fprintf(stderr, "Sampler::Disconnect() completed\n");
}

void 
Sampler::LateNoticeReceived(const media_source& what, bigtime_t how_much, bigtime_t performance_time)
{
	QuaAudioOut	*op=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (what == outPort[i].output.source) {
			op = &outPort[i];
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::LateNoticeReceived(): bad source %d/%d\n", what.id, what.port);
		return;
	}

	fprintf(stderr, "Sampler::LateNoticeReceived(), on source %s.\n", op->insertName);
// If we're late, we need to catch up.  Respond in a manner appropriate to our
// current run mode.
	if (RunMode() == B_RECORDING) {
// A hardware capture node can't adjust; it simply emits buffers at
// appropriate points.  We (partially) simulate this by not adjusting
// our behavior upon receiving late notices -- after all, the hardware
// can't choose to capture "sooner"....
	} else if (RunMode() == B_INCREASE_LATENCY)	{
// We're late, and our run mode dictates that we try to produce buffers
// earlier in order to catch up.  This argues that the downstream nodes are
// not properly reporting their latency, but there's not much we can do about
// that at the moment, so we try to start producing buffers earlier to
// compensate.
		internalLatency += how_much;
		SetEventLatency(op->downstreamLatency + internalLatency);
	} else	{
// The other run modes dictate various strategies for sacrificing data quality
// in the interests of timely data delivery.  The way *we* do this is to skip
// a buffer, which catches us up in time by one buffer duration.
		size_t nSamples = op->bufSize / op->sampleSize;
		op->frameCount += nSamples;
	}
}

void 
Sampler::EnableOutput(const media_source& what, bool enabled, int32* _deprecated_)
{
	QuaAudioOut	*op=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (what == outPort[i].output.source) {
			op = &outPort[i];
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::EnableOutput(): bad source %d/%d\n", what.id, what.port);
		return;
	}
	op->enabled = enabled;
}

status_t 
Sampler::SetPlayRate(int32 numer, int32 denom)
{
// Play rates are weird.  We don't support them.  Maybe we will in a
// later newsletter article....
	return B_ERROR;
}

status_t 
Sampler::HandleMessage(int32 message, const void* data, size_t size)
{
// HandleMessage() is where you implement any private message protocols
// that you want to use.  When messages are written to your node's control
// port that are not recognized by any of the node superclasses, they'll be
// passed to this method in your node's implementation for handling.  The
// ToneProducer node doesn't support any private messages, so we just
// return an error, indicating that the message wasn't handled.
	return B_ERROR;
}

void 
Sampler::AdditionalBufferRequested(const media_source& source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag* prev_tag)
{
// we don't support offline mode (yet...)
// .... and if they don't I don't...
	return;
}

void 
Sampler::LatencyChanged(const media_source& what, const media_destination& destination, bigtime_t new_latency, uint32 flags)
{
// something downstream changed latency, so we need to start producing
// buffers earlier (or later) than we were previously.  Make sure that the
// connection that changed is ours, and adjust to the new downstream
// latency if so.
	QuaAudioOut	*op=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (what == outPort[i].output.source) {
			op = &outPort[i];
			break;
		}
	}
	if (op == nullptr) {
		fprintf(stderr, "Sampler::LatencyChanged(): bad source %d/%d\n", what.id, what.port);
		return;
	}
	if (destination == op->output.destination)	{
		fprintf(stderr, "Sampler:: latency changed\n");
		op->downstreamLatency = new_latency;
		SetEventLatency(op->downstreamLatency + internalLatency);
	}
}

//////////////////////////////
// BMediaEventLooper methods /
//////////////////////////////

void 
Sampler::NodeRegistered()
{
// Start the BMediaEventLooper thread
	SetPriority(B_REAL_TIME_PRIORITY);
	Run();

	// set up as much information about our output as we can
	
// set up as much information about our output as we can
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
//		outPort[i].format = preferredFormat;
		outPort[i].lineStatus = B_OK;
		outPort[i].isConnected = false;
		outPort[i].enabled = true;
		outPort[i].quap_buf = nullptr;
		outPort[i].output.source.port = ControlPort();
		outPort[i].output.source.id = i+1;
		outPort[i].output.node = Node();
		outPort[i].output.destination = media_destination::null;
		char	buf[30];
		sprintf(buf, "Qua Output %d", i+1); 
		::strcpy(outPort[i].output.name, buf);
	}
	
	
	fprintf(stderr, "Registerring output source port %d\n", ControlPort());

	// Initialize as much of our input as we can, now that the Media Kit really "knows" about us
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		inPort[i].input.destination.port = ControlPort();
		inPort[i].input.destination.id = i+1;
		inPort[i].input.node = Node();
		inPort[i].lineStatus = B_OK;
		inPort[i].isConnected = false;
		inPort[i].enabled = false;
		char	buf[30];
		sprintf(buf, "Qua Input %d", i+1); 
		::strcpy(inPort[i].input.name, buf);
	}
	fprintf(stderr, "Sampler:: node registered...\n");

//	// Set up our parameter web
//	mWeb = make_parameter_web();
//	SetParameterWeb(mWeb);

//	BMediaEventLooper::NodeRegistered();
}

void 
Sampler::Start(bigtime_t performance_time)
{
	BMediaEventLooper::Start(performance_time);
}

void 
Sampler::Stop(bigtime_t performance_time, bool immediate)
{
	BMediaEventLooper::Stop(performance_time, immediate);
}

void 
Sampler::Seek(bigtime_t media_time, bigtime_t performance_time)
{
	BMediaEventLooper::Seek(media_time, performance_time);
}

void 
Sampler::TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time)
{
	BMediaEventLooper::TimeWarp(at_real_time, to_performance_time);
}

status_t 
Sampler::AddTimer(bigtime_t at_performance_time, int32 cookie)
{
	return BMediaEventLooper::AddTimer(at_performance_time, cookie);
}

void 
Sampler::SetRunMode(run_mode mode)
{
// We don't support offline run mode, so broadcast an error if we're set to
// B_OFFLINE.  Unfortunately, we can't actually reject the mode change...
	if (B_OFFLINE == mode) {
		reportError(B_NODE_FAILED_SET_RUN_MODE);
	}
}

// in this single input/output case, we're either generating
// and timing off that, or receiving input from up the chain, and then
// shoveling it along...
void 
Sampler::HandleEvent(const media_timed_event* event, bigtime_t lateness, bool realTimeEvent)
{
	status_t err = B_NO_ERROR;
	switch (event->type) {

	case BTimedEventQueue::B_START:
		fprintf(stderr, "Sampler::HandleEvent(): starting qua node...\n");
// if running as a filter, we don't need to do anything particularly, as we're
// just passing on the buffers we get. otherwise, we need to generate handle
// events to make it happen. Otherwise...
// We want to start sending buffers now, so we set up the buffer-sending bookkeeping
// and fire off the first "produce a buffer" event.
		for (short i=0; i<nInput; i++) {
			inPort[input[i]].frameCount = 0;
		}
		for (short i=0; i<nOutput; i++) {
			outPort[output[i]].frameCount = 0;
		}
		
		startTime = event->event_time;
		framesSent = 0;
		if (RunState() != B_STARTED) {
			media_timed_event 	firstBufferEvent(
									startTime,
									BTimedEventQueue::B_HANDLE_BUFFER);

	
// Alternatively, we could call HandleEvent() directly with this event, to avoid a trip through
// the event queue, like this:
//
//		this->HandleEvent(&firstBufferEvent, 0, false);
//

// BufferReceived will send a request when all the necessary input is racked up. It
// will also have to watch out for late things that may have died, etc.
			if (nInput == 0 && nOutput > 0)
				EventQueue()->AddEvent(firstBufferEvent);
		}
		break;

	case BTimedEventQueue::B_STOP:
		fprintf(stderr, "HandelEvent(): stopping qua node...\n");
// stopping implies not handling any more buffers.  So, we flush all pending
// buffers out of the event queue before returning to the event loop.
// When we handle a stop, we must ensure that downstream consumers don't
// get any more buffers from us.  This means we have to flush any pending
// buffer-producing events from the queue.
//		fprintf(stderr, "stop event time %Ld\n", event->event_time);
		EventQueue()->FlushEvents(
							0,
							BTimedEventQueue::B_ALWAYS,
							true,
							BTimedEventQueue::B_HANDLE_BUFFER);
		break;

	case BTimedEventQueue::B_PARAMETER:	{
			size_t dataSize = size_t(event->data);
			int32 param = int32(event->bigdata);
		}
		break;

	
	case BTimedEventQueue::B_HANDLE_BUFFER:	{
// make sure we're both started *and* connected before delivering a buffer
		if (RunState() == BMediaEventLooper::B_STARTED) {
//			BBuffer* buffer = const_cast<BBuffer*>((BBuffer*) event->pointer);
			media_header* 	hdr;
			float			*data;
			size_t			numSamples;
#ifdef SEMWAIT
			static int cnt = 0;
			extern sem_id fluffySem;
			if (++cnt % SEMWAIT == 0)
				release_sem_etc(fluffySem, 1, B_DO_NOT_RESCHEDULE);
#endif
			if (debug_media >= 2) {
				fprintf(stderr, "HandleBuffer() for %Ld\n", event->event_time);
			}
			Generate(event->event_time, framesPerEvent);
//			data = (float*) buffer->Data();
//			Generate(data, numSamples/2, 2);	// assume stereo
			for (short i=0; i<nOutput; i++) {
				QuaAudioOut *op=&outPort[output[i]];
				if (  op->output.destination != media_destination::null &&
					  op->buffer &&
					  op->buffer->Header()->size_used == op->bufSize) {
	
					if (op->enabled) {
		// send the buffer downstream if and only if output is enabled
#ifdef NOSEND		
						op->buffer->Recycle();
#else
						err = SendBuffer(op->buffer, op->output.destination);
						if (err) {
							fprintf(stderr, "failed sending\n");
							// we need to recycle the buffer ourselves if output is disabled or
							// if the call to SendBuffer() fails
							op->buffer->Recycle();
						}
#endif		
						op->buffer = nullptr;
					} else {
						op->buffer->Recycle();
						op->buffer = nullptr;
					}
					// track how much media we've delivered so far
					op->frameCount += op->bufSize / op->frameSize;
				}
			}
			
			framesSent += framesPerEvent;			
// The buffer is on its way; now schedule the next one to go
			if (nInput == 0) {
				bigtime_t nextEvent =
						startTime +
							bigtime_t((double(framesSent) / frameRate) * 1000000.0);
#undef UNREALITY_CHECK
#ifdef UNREALITY_CHECK
				fprintf(stderr, "start %Ld, event @ %Ld, next @ %Ld, frames sent %d rate %g duration %Ld\n",
								startTime,
								event->event_time,
								nextEvent,
								framesPerEvent,
								frameRate,
								bigtime_t((double(framesPerEvent) / frameRate) * 1000000.0));
#endif

				media_timed_event 	nextBufferEvent(
											nextEvent,
											BTimedEventQueue::B_HANDLE_BUFFER);
				EventQueue()->AddEvent(nextBufferEvent);
			}
		}
	}
	break;

	case BTimedEventQueue::B_SEEK: {
		fprintf(stderr, "Sampler:: seek event\n");
		break;
	}

	case BTimedEventQueue::B_WARP: {
		fprintf(stderr, "Sampler:: warp even\n");
		break;
	}

	case BTimedEventQueue::B_DATA_STATUS: {
// we track our producer's data status for cases like offline, where we need to
// consult it in order to decide whether to request another buffer of data
		BBuffer* buffer = const_cast<BBuffer*>((BBuffer*) event->pointer);
		if (buffer) {
			media_header* 	hdr = buffer->Header();
			QuaAudioIn	*ip=nullptr;
			for (short i=0; i<nInput; i++) {
				if (hdr->destination == inPort[input[i]].input.destination.id) {
					ip = &inPort[input[i]];
				}
			}
			if (ip)
				ip->lineStatus = event->data;
		}
		fprintf(stderr, "Sampler:: data status event\n");
		break;
	}
	
	default:
		fprintf(stderr, "Sampler:: strange media event\n");
		break;
	}
}

void 
Sampler::CleanUpEvent(const media_timed_event *event)
{
	BMediaEventLooper::CleanUpEvent(event);
}

bigtime_t 
Sampler::OfflineTime()
{
	return BMediaEventLooper::OfflineTime();
}

void 
Sampler::ControlLoop()
{
	BMediaEventLooper::ControlLoop();
}
		
status_t 
Sampler::DeleteHook(BMediaNode* node)
{
	return BMediaEventLooper::DeleteHook(node);
}

////////////////////////////////
// BBufferConsumer methods//////
////////////////////////////////

// all of these next methods are pure virtual in BBufferConsumer

status_t 
Sampler::AcceptFormat(const media_destination& dest, media_format* format)
{
	fprintf(stderr, "Sampler::AcceptFormat(buffsize %d)\n", format->u.raw_audio.buffer_size);
// return an error if this isn't really our one input's destination
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		if (dest == inPort[i].input.destination) {
			// the destination given really is our input, and we accept any kind of media data,
			// so now we just confirm that we can handle whatever the producer asked for.
			inPort[i].input.format = *format;
			return B_OK;
		}
	}
	return B_MEDIA_BAD_DESTINATION;
}

status_t 
Sampler::GetNextInput(int32* cookie, media_input* out_input)
{
	fprintf(stderr, "Sampler::GetNextInput(%d)\n", *cookie);
	if (*cookie < MAX_QUA_AUDIO_INPUT && *cookie < nInput + 4) {
		inPort[*cookie].input.format.type = B_MEDIA_RAW_AUDIO;		// accept any format
		*out_input = inPort[*cookie].input;
		*cookie = *cookie+1;
		return B_OK;
	} else {
		return B_BAD_INDEX;
	}
}

void 
Sampler::DisposeInputCookie(int32 /*cookie*/ )
{
	// we don't use any kind of state or extra storage for iterating over our
	// inputs, so we don't have to do any special disposal of input cookies.
}

void 
Sampler::BufferReceived(BBuffer* buffer)
{
	status_t		err=B_OK;
	media_header	*header = buffer->Header();
	bigtime_t		bufferStartTime = header->start_time;
	bigtime_t		now = TimeSource()->Now();
	bigtime_t		how_early = bufferStartTime - now -
								EventLatency() - SchedulingLatency();
	switch (header->type) {

	case B_MEDIA_PARAMETERS: {
// There's a special case here with handling B_MEDIA_PARAMETERS buffers.
// These contain sets of parameter value changes, with their own performance
// times embedded in the buffers.  So, we want to dispatch those parameter
// changes as their own events rather than pushing this buffer on the queue to
// be handled later.
//		ApplyParameterData(buffer->Data(), buffer->SizeUsed());
		buffer->Recycle();
		break;
	}
	
	case B_MEDIA_NO_TYPE: {
		fprintf(stderr, "Sampler:: typeless media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_VBL: {					/* raw data from VBL area, 1600/line */
		fprintf(stderr, "Sampler:: VBL media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_TIMECODE: {			/* data format TBD */
		fprintf(stderr, "Sampler:: timecode media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_MIDI: {
		fprintf(stderr, "Sampler:: MIDI media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_TEXT: {					/* typically closed captioning */
		fprintf(stderr, "Sampler:: text media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_HTML: {
		fprintf(stderr, "Sampler:: typeless media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_MULTISTREAM: {			/* AVI, etc */
		fprintf(stderr, "Sampler:: typeless media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_ENCODED_AUDIO:	{		/* dts, AC3, ... */
		fprintf(stderr, "Sampler:: typeless media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}

	case B_MEDIA_ENCODED_VIDEO: {		/* Indeo, MPEG, ... */
		fprintf(stderr, "Sampler:: typeless media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}


	case B_MEDIA_RAW_VIDEO:	{			/* uncompressed raw_video -- linear relationship bytes <-> pixels */
		fprintf(stderr, "Sampler:: raw video media buffer received and ignored\n");
		buffer->Recycle();
		break;
	}
	
	case B_MEDIA_UNKNOWN_TYPE:
	case B_MEDIA_RAW_AUDIO: {			/* uncompressed raw_audio -- linear relationship bytes <-> samples */
		QuaAudioIn		*ip=nullptr;
		for (short i=0; i<nInput; i++) {
			if (header->destination == inPort[input[i]].input.destination.id) {
				ip = &inPort[input[i]];
				break;
			}
		}
		
		if (ip == nullptr) {
			fprintf(stderr, "Sampler:: wrong destination for audio buffer, %d: buffer ignored\n", header->destination);
			buffer->Recycle();
			return;
		}
		
		if (!ip->enabled) {
			buffer->Recycle();
			return;
		}
		
// the how_early calculate here doesn't include scheduling latency because
// we've already been scheduled to handle the buffer

// if the buffer is late, we won't ignore it and report the fact to the producer
// who sent it to us
		if (  RunMode() != B_OFFLINE &&			// lateness doesn't matter in offline mode...
			  RunMode() != B_RECORDING &&		// ...or in recording mode
			  how_early < 0) {
			ip->nLateBuffers++;
			NotifyLateProducer(ip->input.source, -how_early, bufferStartTime);
		}
// do some shit! stow this buffer away, so it can be processed
		long	nEventsHappened=0;
		if ((nEventsHappened=Consume(buffer, ip))>0) {
			for (short i=0; i<nEventsHappened; i++) {
				bigtime_t nextEvent =
					startTime +
						bigtime_t(
							  (double(framesSent+i*framesPerEvent) /
							   double(frameRate))
							* 1000000.0
						);
//				fprintf(stderr, "trigger from BuffRec() %Ld\n", nextEvent-startTime);
				media_timed_event 	nextBufferEvent(
									nextEvent,
									BTimedEventQueue::B_HANDLE_BUFFER);
				EventQueue()->AddEvent(nextBufferEvent);
			}
		}

// Offline mode requires some special handling.  When running offline the producer
// doesn't just spit buffers as fast as it can (and block waiting for downstream buffers
// to be recycled), it waits for the consumer to explicitly request them.  This means
// that it can react to other control messages while it's waiting to send new buffers.
		if (RunMode() == B_OFFLINE && ip->lineStatus == B_DATA_AVAILABLE) {
			err = RequestAdditionalBuffer(ip->input.source, buffer);
			if (err != B_OK) {
				fprintf(stderr, "request additional failed...\n");
				buffer->Recycle();
				return;
			}
		}
									
//		switch (ip->insert.format.u.raw_audio.format) {
//		case media_raw_audio_format::B_AUDIO_UCHAR: {
//			uchar		*data = (uchar *)buffer->Data();
//			for (int i=0; i<nFrames*nChannels; i++) {
//				tempBuf[i] = (data[i]-128.0) / 128.0;
//			}
//			break;
//		}
//		case media_raw_audio_format::B_AUDIO_SHORT: {
//			short		*data = (short *)buffer->Data();
//			for (int i=0; i<nFrames*nChannels; i++) {
//				tempBuf[i] = (data[i]/32767.0);
//			}
//			break;
//		}
//		
//		case media_raw_audio_format::B_AUDIO_FLOAT: {
//			memcpy(tempBuf, buffer->Data(), ip->insert.format.u.raw_audio.buffer_size);
//			break;
//		}			
//		case media_raw_audio_format::B_AUDIO_INT: {
//			int		*data = (int *)buffer->Data();
//			for (int i=0; i<nFrames*nChannels; i++) {
//				tempBuf[i] = 2*(((float)data[i])/0x7fffffff);
//			}
//			break;
//		}
//		
//		}

//		ip->Process(tempBuf, nFrames*nChannels);
		if (buffer->Flags() & BBuffer::B_SMALL_BUFFER) {
			buffer->Recycle();
		} else {
			buffer->Recycle();
		}

// HandleEvent() will recycle the buffer.  However, if we incurred an error trying to
// put the event into the queue, we have to recycle it ourselves, since HandleEvent()
// will never see the buffer in that case.
//		media_timed_event event(buffer->Header()->start_time,
//			BTimedEventQueue::B_HANDLE_BUFFER,
//			buffer, BTimedEventQueue::B_RECYCLE_BUFFER);
//		err = RequestAdditionalBuffer(inPort.input.source, now);
//		media_timed_event event(buffer->Header()->start_time,
//			BTimedEventQueue::B_HANDLE_BUFFER,
//			buffer, BTimedEventQueue::B_NO_CLEANUP);
//		err = EventQueue()->AddEvent(event);
//		if (err) {
//			fprintf(stderr, "Can't fit in event queue: %s\n", ErrorStr(err));
//			buffer->Recycle();
//		}

		break;
	}
	
	default: {
		fprintf(stderr, "Sampler:: media buffer of unknown type received and ignored\n");
		buffer->Recycle();
		break;
	}

	}
}

void 
Sampler::ProducerDataStatus(const media_destination& for_whom, int32 status, bigtime_t at_performance_time)
{
	QuaAudioIn	*ip=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		if (for_whom == inPort[i].input.destination) {
			ip = &inPort[i];
		}
	}
	if (ip)	{
		media_timed_event event(at_performance_time, BTimedEventQueue::B_DATA_STATUS,
			&ip->input, BTimedEventQueue::B_NO_CLEANUP, status, 0, nullptr);
		EventQueue()->AddEvent(event);
	}
}

status_t 
Sampler::GetLatencyFor(const media_destination& for_whom, bigtime_t* out_latency, media_node_id* out_timesource)
{
	QuaAudioIn	*ip=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		if (for_whom == inPort[i].input.destination) {
			ip = &inPort[i];
		}
	}
	if (ip == nullptr)	{
		return B_MEDIA_BAD_DESTINATION;
	}

// report internal latency + downstream latency here, NOT including scheduling latency.
// we're a final consumer (no outputs), so we have no downstream latency.
	*out_latency = internalLatency;
	*out_timesource = TimeSource()->ID();
	return B_OK;
}

/* here's a good place to request buffer group usage */
status_t 
Sampler::Connected(
	const media_source& producer,
	const media_destination& where,
	const media_format& format,
	media_input* out_input)
{
	QuaAudioIn	*ip=nullptr;
	short			ini=-1;
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		if (where == inPort[i].input.destination) {
			ip = &inPort[i];
			ini = i;
		}
	}
	if (ip == nullptr)	{
		return B_MEDIA_BAD_DESTINATION;
	}
	fprintf(stderr, "Sampler::Connected()\n");

	// record useful information about the connection, and return success
	ip->input.source = producer;
	ip->sampleSize = ip->input.format.u.raw_audio.format &
							media_raw_audio_format::B_AUDIO_SIZE_MASK;
	ip->frameSize =	format.u.raw_audio.channel_count * ip->sampleSize;
	ip->bufSize = format.u.raw_audio.buffer_size;
	ip->nChannel = format.u.raw_audio.channel_count;

// calculate my latency here, because it may depend on buffer sizes/durations, then
// tell the BMediaEventLooper how early we need to get the buffers
// Use a dry run to see how long it takes me to fill a buffer of data
	bigtime_t start, endt;
	size_t samplesPerBBuffer = ip->bufSize / ip->sampleSize;
	if (samplesPerBBuffer == 0) {
		fprintf(stderr, "Generic error: 0 samples per buffer\n");
		return B_ERROR;
	}
	float* data = new float[samplesPerBuffer];

	start = ::system_time();
// current load for rack is: sg 0 is paused with one kit part + synth...
//	Process(data, samplesPerBBuffer);
	endt = ::system_time();

	internalLatency = 300000; //(endt - start);
	delete [] data;
	SetEventLatency(internalLatency);

	ip->externConnection = true;
	for (short i=0; i<nOutput; i++) {
		if (input[i] == ini) {
			ip->externConnection = false;
			break;
		}
	}
	
	if (ip->externConnection) {
		input[nInput++] = ini;
//		BMessage	conMsg(CONNECT_OUTPUT);
//		conMsg.AddInt32("externally", outi);
//		conMsg.AddString("name", io_name);
//		theApp->PostMessage(&conMsg);
	}

	fprintf(stderr, "Sampler::Connected(). input (source (id %d port %d) destination (id %d port %d)producer (node %d port%d) where (%d %d)\n",
				 ip->input.source.id, ip->input.source.port,
				 ip->input.destination.id, ip->input.destination.port,
				 ip->producer.node, ip->producer.port,
				 where.id, where.port);
	*out_input = ip->input;
	return B_OK;
}

void 
Sampler::Disconnected(
	const media_source& producer,
	const media_destination& where)
{
	fprintf(stderr, "Sampler::Disconnected(). prod %d %d dest %d %d\n",
			producer.id, producer.port, where.id, where.port);
	QuaAudioIn	*ip=nullptr;
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		if (where == inPort[i].input.destination) {
			ip = &inPort[i];
		}
	}
	if (ip == nullptr)	{
		return;
	}

	ip->quaport = nullptr;
	ip->input.source = media_source::null;
}


/* The notification comes from the upstream producer, so he's already cool with */
/* the format; you should not ask him about it in here. */
status_t 
Sampler::FormatChanged(
	const media_source& producer,
	const media_destination& consumer,
	int32 change_tag,
	const media_format& format)
{
	return B_OK;
}

/* Given a performance time of some previous buffer, retrieve the remembered tag */
/* of the closest (previous or exact) performance time. Set *out_flags to 0; the */
/* idea being that flags can be added later, and the understood flags returned in */
/* *out_flags. */
status_t 
Sampler::SeekTagRequested(
	const media_destination& destination,
	bigtime_t in_target_time,
	uint32 in_flags,
	media_seek_tag* out_seek_tag,
	bigtime_t* out_tagged_time,
	uint32* out_flags)
{
	return B_OK;
}




status_t
Sampler::StartQuaNode()
{
	if (nOutput > 0 || nInput > 0) {
		fprintf(stderr, "Sampler::StartQuaNode()\n");
		status_t err=B_OK;
		BMediaRoster* r = BMediaRoster::Roster();

// recheck that the frame rate and buff quanta are ok...
		short	mf = 0x7fff;
		for (short i=0; i<nOutput; i++) {
			QuaAudioOut	*op = &outPort[output[i]];
			if (mf > op->bufSize / op->frameSize) {
				mf = op->bufSize / op->frameSize;
			}
			frameRate = op->output.format.u.raw_audio.frame_rate;
		}
//		for (short i=0; i<nInput; i++) {
//			QuaAudioIn	*op = &inPort[input[i]];
//			if (mf > op->bufSize / op->frameSize) {
//				mf = op->bufSize / op->frameSize;
//			}
//			frameRate = op->input.format.u.raw_audio.frame_rate;
//		}
		framesPerEvent = mf;
		fprintf(stderr, "Sampler::StartQuaNode(): %d input %d output, %d frames per event\n",
						nInput, nOutput, framesPerEvent);

// make sure we give the producer enough time to run buffers through
// the node chain, otherwise it'll start up already late
		bigtime_t latency = 0;
		r->GetLatencyFor(Node(), &latency);
		
		for (short i=0; i<nInput; i++) {
			short	wh = input[i];
			err = r->StartNode(inPort[wh].producer, masterTimeSource->Now());//*upstreamLatency);
			if (err != B_OK) {
				fprintf(stderr, "Sampler::StartQuaNode(): input node error: %s\n", ErrorStr(err));
			}
		}
		
		err = r->StartNode(Node(), masterTimeSource->Now());//*upstreamLatency);
		if (err != B_OK) {
			fprintf(stderr, "Sampler::StartQuaNode(): qua node error: %s\n", ErrorStr(err));
		}

		for (short i=0; i<nOutput; i++) {
			short	wh = output[i];
			err = r->StartNode(outPort[wh].consumer, masterTimeSource->Now());//*upstreamLatency);
			if (err != B_OK) {
				fprintf(stderr, "Sampler::StartQuaNode(): output node error: %s\n", ErrorStr(err));
			}
		}
		
		fprintf(stderr, "Qua central started!\n");
	}
	return B_OK;
}

status_t
Sampler::StopQuaNode()
{
	fprintf(stderr, "Sampler::StopQuaNode()\n");
	status_t err;
	BMediaRoster* r = BMediaRoster::Roster();
//		for (short i=0; i<nOutput; i++) 
//			outPort[output[i]].enabled = false;
//		for (short i=0; i<nInput; i++) 
//			inPort[input[i]].enabled = false;
// disbling seems important right now, as it prevents buffers being sent
// which seems necessary, else the stopping of the node blocks.

//	for (short i=0; i<nInput; i++) {
//		short	wh = input[i];
//		err = r->StopNode(inPort[wh].producer, masterTimeSource->Now());//*upstreamLatency);
//		if (err != B_OK) {
//			fprintf(stderr, "Sampler::StopQuaNode(): input node error: %s\n", ErrorStr(err));
//		}
//	}
	err = r->StopNode(Node(), 0, true);		// synchronous stop
	if (err != B_OK) {
		fprintf(stderr, "Sampler::StopQuaNode(): core node (%d/%d) stop error: %s...\n", Node().node, Node().port, ErrorStr(err));
		return err;
	}
	for (short i=0; i<nOutput; i++) {
		short	wh = output[i];
//		err = r->StopNode(outPort[wh].consumer, 0, true); //masterTimeSource->Now());//*upstreamLatency);
		if (err != B_OK) {
			fprintf(stderr, "Sampler::StopQuaNode(): output node error: %s\n", ErrorStr(err));
		}
	}
	fprintf(stderr, "Qua node stopped...\n");
	return B_OK;
}

#else

long
Sampler::OutCompWrapper( void* userData, long error)
{
	return ((Sampler*)userData)->OutComp();
}

bool
Sampler::OutStreamWrapper(void *userData, char *buffer, unsigned long count, void *header)
{
	return ((Sampler*)userData)->OutStream(buffer, count);
}

long
Sampler::OutComp()
{
	return TRUE;
}
long
Sampler::InCompWrapper( void* userData, long error)
{
	return ((Sampler*)userData)->InComp();
}

bool
Sampler::InStreamWrapper(void *userData, char *buffer, unsigned long count, void *header)
{
	return ((Sampler*)userData)->InStream(buffer, count);
}

long
Sampler::InComp()
{
	return TRUE;
}

bool
Sampler::InStream(char *buffer, unsigned long count)
{
	short *inSamples = (short *)buffer;
	ulong inSampleCount = count / sizeof(short);

	for (short i=0; i<uberQua->nChannel; i++) {
		Channel	*c = uberQua->channel[i];
		if (c->thruEnabled || c->recordState == RECORD_ING) {
			for (int i=0; i<inSampleCount; i++) {
				qmi_temp[i] = ((float)inSamples[i])/32767.0;
//				fprintf(stderr, "%g %d\n", tempBuf[i], inSamples[i]);
			}
	
			c->Receive(qmi_temp, inSampleCount/2, 2);
		}
	}
	return TRUE;
}


bool
Sampler::OutStream(char *buffer, unsigned long count)
{
	int			nOutSamples = count/(sizeof(short));
	short		*outSamples = (short *)buffer;
	float		tempSig[kAudioBufferSize];
	
	Generate(tempSig, nOutSamples/2, 2);
	
	for (int j=0; j<nOutSamples; j++) {
		outSamples[j] += 32000*tempSig[j];
	}

	return TRUE;
}

#endif
#endif

#ifdef _BEOS
// don't start the node till after we've connected, though. Do all the
// silly shit, and set things up.
void
Sampler::StartSampler()
{
	if (status != STATUS_RUNNING) {
		status = STATUS_RUNNING;
#ifdef NEW_MEDIA
		BMediaRoster* 	r = BMediaRoster::Roster();
		if (r == nullptr) {
			::reportError("Qua sampler start: no media roster");
			return;
		}

		status_t		err;
		err = r->RegisterNode(this);
		if (err != B_NO_ERROR) {
			::reportError("Qua StartSampler(): can't register node");
		}		

//// make sure the Media Roster knows that we're using the node
//		r->GetNodeFor(Node().node, &quacore);
//		fprintf(stderr, "Sampler:: audio core node %d port %d\n", quacore.node, quacore.port);

// set the producer's time source to be the "default" time source, which
// the Mixer uses too.
		if (   (err = r->GetTimeSource(&masterClock)) == B_OK &&
				(masterTimeSource = r->MakeTimeSourceFor(masterClock)) != nullptr) {
			fprintf(stderr, "System time source is node %d\n", masterClock.node);
			if ((err = r->SetTimeSourceFor(Node().node, masterClock.node)) != B_OK) {
				::reportError("Qua can't set time source: %s", ErrorStr(err));
			}
		} else {
			::reportError("Qua can't find system time source: %s", ErrorStr(err));
		}

#else
#endif
		resume_thread(buffyThread);
		fprintf(stderr, "Started sampler...\n");
	}
}

void
Sampler::StopSampler()
{
	if (status != STATUS_SLEEPING) {
		status = STATUS_SLEEPING;
		fprintf(stderr, "Qua stopping sampler...\n");
#ifdef NEW_MEDIA
		for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
			CloseInput(&inPort[i]);
		}
		for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
			CloseOutput(&outPort[i]);
		}
		BMediaEventLooper::Quit();
		
		BMediaRoster* 	r = BMediaRoster::Roster();
		if (r == nullptr) {
			::reportError("Qua sampler stop: no media roster");
			return;
		}
		masterTimeSource->Release();
		status_t	err = r->UnregisterNode(this);
		if (err != B_OK) {
			::reportError("Qua stop: can't deregister node:  %ld (%s)\n", err, strerror(err));
			return;
		}
		fprintf(stderr, "Sampler:: de-registered media node!\n");
		
//		err = r->ReleaseNode(quacore);
//		if (err != B_NO_ERROR) {
//			::reportError("Qua can't release node: %s", ErrorStr(err));
//		}
#else
		CloseInput(uberQua->quaAudio->dfltOutput.device, 0);
		CloseOutput(uberQua->quaAudio->dfltInput.device, 0);
#endif
		fprintf(stderr, "Sampler:: sampler stopped.\n");
	}
}

// my hooks to start the show and end it...

#ifdef NEW_MEDIA
QuaAudioIn *
Sampler::OpenInput(
	QuaAudioPort *iport,
	media_source &s,
	media_format &f
	)
{
	for (short i=0; i<nInput; i++) {
		QuaAudioIn	*ip = &inPort[input[i]];
		if (ip->source == s) {
			return ip;
		}
	}
	short	free = -1;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (inPort[i].source == media_source::null) {
			free = i;
			break;
		}
	}
	if (free < 0) {
		return nullptr;
	}

	media_output	src;
	src.source = s;
	src.format = f;
	src.node = iport->mediaNode;
	input[nInput] = free;
	inPort[free].quaport = iport;

	status_t	err = MakeInputConnection(&inPort[free], &src);
	if (err != B_OK) {
		::reportError("Sampler:: can't open input, %s", ErrorStr(err));
		return nullptr;
	}
	nInput++;

	return &inPort[free];
}

#else
status_t
Sampler::OpenInput(
	QuaAudioPort *, long
	)
{
	if (!inSubscriber->IsInStream()) {
		inSubscriber->EnterStream(nullptr,TRUE,this,
			&InStreamWrapper,&InCompWrapper,TRUE);
	}
	return B_OK;
}
#endif

#ifdef NEW_MEDIA
status_t
Sampler::MakeInputConnection(QuaAudioIn *ip, media_output *from)
{
	status_t	err=B_NO_ERROR;
	if (!ip->isConnected) {
		fprintf(stderr, "Making input connection...\n");

		BMediaRoster	*r = BMediaRoster::Roster();
		if (r == nullptr) {
			::reportError("Qua OpenInput(): no media roster");
			return B_ERROR;
		}
	
// make sure the Media Roster knows that we're using the node
//		err=r->GetNodeFor(Node().node, &ip->consumer);
//		if (err != B_NO_ERROR) {
//			::reportError("Qua OpenInput: can't find my self: %s", ErrorStr(err));
//			return err;
//		}		
	
//		// connect to the audio input
//		err = r->GetAudioInput(&inPort.producer);
//		if (err != B_NO_ERROR) {
//			::reportError("Qua OpenInput(): can't find audio input");
//			return err;
//		}
//
		// got the nodes; now we find the endpoints of the connection
		media_input		quaInput;
		media_output	inputNodeOutput;
		int32 			count;
		
//		err = StopQuaNode();
//		if (err != B_NO_ERROR) {
//			::reportError("Qua OpenInput(): could not stop qua node: %s", ErrorStr(err));
//			return err;
//		}
		
		err = r->GetFreeInputsFor(/*ip->consumer*/Node(), &quaInput, 1, &count);
		fprintf(stderr, "Sampler:: get free inputs of self %d %d, %s, count %d\n",
				quaInput.destination.id, quaInput.destination.port, ErrorStr(err), count);
		if (err != B_NO_ERROR) {
			::reportError("Qua OpenInput(): can't get free input to qua: %s", ErrorStr(err));
			return err;
		}
		
		ip->source = from->source;
		ip->destination = quaInput.destination;
		// got the endpoints; now we connect it!
		media_format format;

//		format.type = B_MEDIA_RAW_AUDIO;			// !!! hmmm.. how to fully wildcard this?
//		format.u.raw_audio = media_raw_audio_format::wildcard;
		format = from->format;

		err = r->Connect(
					ip->source,
					ip->destination,
					&format,
					&inputNodeOutput,
					&quaInput);
		if (err != B_NO_ERROR) {
			::reportError("Qua OpenInput(): unable to connect: %s", ErrorStr(err));
			status_t	err2;
			if ((err2=StartQuaNode()) != B_NO_ERROR) {
				::reportError("Qua OpenInput(): Couldn't start qua node: %s", ErrorStr(err2));
			}
			return err;
		
		} else {
			fprintf(stderr, "Connection made\n");
			ip->source = inputNodeOutput.source;
			ip->destination = quaInput.destination;
			ip->producer = from->node;
//			ip->consumer = Node();
			ip->data = new FloatQueue(16*ip->bufSize/ip->frameSize);
		
//			connection.format = format;
	
// set the input nodes' time sources
//			r->SetRunModeNode(ip->producer, BMediaNode::B_INCREASE_LATENCY);
			err=r->SetTimeSourceFor(ip->producer.node, masterClock.node);
	
// preroll first, to be a good citizen
//			r->PrerollNode(ip->consumer);
			r->PrerollNode(ip->producer);

			err=r->GetLatencyFor(ip->producer, &ip->upstreamLatency);
			err = r->StartNode(ip->producer, masterTimeSource->Now());
//fprintf(stderr, "now %Ld\n", ts->Now());
//			if (err != B_OK) {
//				fprintf(stderr, "Starting producer node error: %s\n", ErrorStr(err));
//			} else {
//				fprintf(stderr, "Producer started @ %Ld!\n", ts->Now());
//			}

//	short	mf = 0x7fff;
//	for (short i=0; i<nOutput; i++) {
//		RackOutput	*op = &outputInfo[output[i]];
//		if (mf > op->bufSize / op->frameSize) {
//			mf = op->bufSize / op->frameSize;
//		}
//	}
//	framesPerEvent = mf;
//	fprintf(stderr, "frames per event %d\n", framesPerEvent);
//	frameRate = op->outlet.format.u.raw_audio.frame_rate;
			sprintf(ip->insertName, "%s:%d", ip->quaport->sym->name, ip->destination.id); 
			ip->isConnected = true;
			ip->enabled = true;
			
			if ((err=StartQuaNode()) != B_NO_ERROR) {
				::reportError("Qua OpenInput(): Couldn't start qua node: %s", ErrorStr(err));
				return err;
			}
		}		

		fprintf(stderr, "Connection made!\n");
	} else {
		fprintf(stderr, "Already connected!\n");
	}
	return B_OK;
}
#endif

#ifdef NEW_MEDIA
status_t
Sampler::CloseInput(
	QuaAudioIn *ip
	)
{
	if (ip == nullptr) {
		return B_ERROR;
	}
	if (!ip->isConnected) {
		return B_OK;
	}
	
	short			indi=-1, indj=-1;
	for (short i=0; i<nInput; i++) {
		if (&inPort[input[i]] == ip) {
			indj = i;
			break;
		}
	}
	if (indj >= 0) {
		for (short i=indj; i<nInput-1; i++) {
			input[i] = input[i+1];
		}
		nInput--;
	}
	status_t	err  = BreakInputConnection(ip);	
	if (err != B_OK) {
		::reportError("Can't break input, %s", ErrorStr(err));
	}
	return B_OK;
}

#else
status_t
Sampler::CloseInput(
	QuaAudioPort *port,
	long insert
		)
{
	if (inSubscriber->IsInStream())
		inSubscriber->ExitStream(TRUE);
	return B_OK;
}
#endif

#ifdef NEW_MEDIA
status_t
Sampler::BreakInputConnection(QuaAudioIn *ip)
{
	if (ip->isConnected) {
		fprintf(stderr, "Breaking input connection...\n");
		status_t	err=B_OK;
		BMediaRoster* r = BMediaRoster::Roster();
		fprintf(stderr, "close: to stop input (source (id %d port %d) destination (id %d port %d)producer (node %d port%d)\n",
				 ip->source.id, ip->source.port,
				 ip->destination.id, ip->input.destination.port,
				 ip->producer.node, ip->producer.port);
		
		ip->enabled = false;
		
//		err=r->StopNode(inPort.producer, 0, true);		// synchronous stop
//		err=r->StopNode(inPort.producer, 0, false);		// a?synchronous stop
		if (err != B_OK) {
			fprintf(stderr, "Sampler:: upstream producer node stopped error: %s...\n", ErrorStr(err));
		}

		if ((err=StopQuaNode()) != B_OK) {
			fprintf(stderr, "qua node stop error: %s\n", ErrorStr(err));
		}

		err = r->Disconnect(
					ip->producer.node,
					ip->source,
					/*ip->consumer.node*/Node().node,
					ip->destination);
		if (err != B_NO_ERROR) {
			fprintf(stderr, "Qua input: line disconnect status: %s\n", ErrorStr(err));
		}
		err = r->ReleaseNode(ip->producer);
		if (err != B_NO_ERROR) {
			fprintf(stderr, "Qua CloseInput(): can't release producer: %s\n", ErrorStr(err));
		}

//		err = r->ReleaseNode(ip->consumer);
//		if (err != B_NO_ERROR) {
//			fprintf(stderr, "Qua CloseInput(): can't release consumer: %s\n", ErrorStr(err));
//		}

		ip->isConnected = false;
		ip->source = media_source::null;
		delete ip->data;
		if ((err=StartQuaNode()) != B_NO_ERROR) {
			::reportError("Couldn't start");
			return err;
		}
		fprintf(stderr, "Connection broken...\n");
	} else {
		fprintf(stderr, "No input connection to close...\n");
	}
	return B_OK;
}
#endif


#ifdef NEW_MEDIA
QuaAudioOut *
Sampler::OpenOutput(
	QuaAudioPort *oport,
	media_destination &d,
	media_format &f
	)
{
	fprintf(stderr, "Open Output n=%d\n", nOutput);
	for (short i=0; i<nOutput; i++) {
		QuaAudioOut	*op = &outPort[output[i]];
		if (op->destination == d) {
			return op;
		}
	}
	short	free = -1;
	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		if (outPort[i].destination == media_destination::null) {
			free = i;
			break;
		}
	}
	if (free < 0) {
		return nullptr;
	}

	media_input	dest;
	dest.destination = d;
	dest.format = f;
	dest.node = oport->mediaNode;
	outPort[free].quaport = oport;
	status_t	err = MakeOutputConnection(&outPort[free], &dest);
	if (err != B_OK) {
		::reportError("Sampler:: can't open output, %s", ErrorStr(err));
		return nullptr;
	}
	
//	output[nOutput++] = free;
	fprintf(stderr, "open output %d\n", nOutput);

	return &outPort[free];
}

#else
status_t
Sampler::OpenOutput(
	QuaAudioPort *port,
	long insert
	)
{
	if (!outSubscriber->IsInStream())
		outSubscriber->EnterStream(nullptr,TRUE,this,
			&OutStreamWrapper,&OutCompWrapper,TRUE);
	return B_NO_ERROR;
}
#endif

#ifdef NEW_MEDIA
status_t
Sampler::MakeOutputConnection(QuaAudioOut *op, media_input *dest)
{
	if (!op->isConnected) {
		status_t err;
		BMediaRoster* r = BMediaRoster::Roster();
		if (r == nullptr) {
			::reportError("Qua OpenOutput(): no media roster");
			return B_ERROR;
		}
	
// make sure the Media Roster knows that we're using the node
//		err = r->GetNodeFor(Node().node, &op->producer);
//		if (err != B_NO_ERROR) {
//			::reportError("Qua OpenOutput(): can't find my self: %s", ErrorStr(err));
//			return err;
//		}		
	
// got the nodes; now we find the endpoints of the connection
		media_output 	soundOutput;
		int32 			count = 1;
		err = r->GetFreeOutputsFor(/*op->producer*/Node(), &soundOutput, 1, &count);
		if (err != B_NO_ERROR) {
			::reportError("unable to get a free output from the producer node");
			return err;
		}
		
//		err = StopQuaNode();
//		if (err != B_OK) {
//			::reportError("Qua connect: can't stop node");
//			return err;
//		}
//		

		media_format 	format;
		format.type = B_MEDIA_RAW_AUDIO;	
//		format.u.raw_audio = media_raw_audio_format::wildcard;
		format = dest->format;
//		format.u.raw_audio.channel_count = 2;
		
		fprintf(stderr, "Qua connection format: %d channels, type %d, buff size %d\n",
			format.u.raw_audio.channel_count,
			format.u.raw_audio.format,
			format.u.raw_audio.buffer_size);
			
		op->source = soundOutput.source;		
		op->destination = dest->destination;
		
		media_input		soundInput;
		err = r->Connect(
					op->source,
					op->destination,
					&format,
					&soundOutput,
					&soundInput);
// correct stuff should be set in op->connection throughout the
// negotiation process. perhaps should verify against 'soundInput'
		if (err != B_NO_ERROR) {
			fprintf(stderr, "Sampler::MakeOutputConnection() failed to connect\n");
			fprintf(stderr, "\toutput src %d %d > dest %d %d\n",
							soundOutput.source.port,
							soundOutput.source.id,
							soundOutput.destination.port,
							soundOutput.destination.id);
			::reportError("Qua output: can't connect nodes: %s.", ErrorStr(err));
			return err;
		} else {
			fprintf(stderr, "Sampler::MakeOutputConnection() connected\n");
			fprintf(stderr, "\toutput src %d %d > dest %d %d\n",
							soundOutput.source.port,
							soundOutput.source.id,
							soundOutput.destination.port,
							soundOutput.destination.id);
			fprintf(stderr, "\tinput src %d %d > dest %d %d\n",
							soundInput.source.port,
							soundInput.source.id,
							soundInput.destination.port,
							soundInput.destination.id);
		}		

// the inputs and outputs might have been reassigned during the
// nodes' negotiation of the Connect().  That's why we wait until
// after Connect() finishes to save their contents.

		op->consumer = dest->node;
		sprintf(op->insertName, "%s:%d", op->quaport->sym->name, op->destination.id); 

		// Set an appropriate run mode for the producer
		r->SetRunModeNode(op->consumer, BMediaNode::B_INCREASE_LATENCY);
		err=StartQuaNode();
		if (err != B_NO_ERROR) {
			::reportError("Qua connect:: Couldn't start qua central");
			return err;
		}
	}

	return B_NO_ERROR;
}
#endif	

#ifdef NEW_MEDIA
status_t
Sampler::CloseOutput(
	QuaAudioOut *op
	)
{
	short			indj=-1;
	
	if (op == nullptr)
		return B_ERROR;
	
	for (short i=0; i<nOutput; i++) {
		if (op == &outPort[output[i]]) {
			indj = i;
			break;
		}
	}
	if (indj >= 0) {
		for (short i=indj; i<nOutput-1; i++) {
			output[i] = output[i+1];
		}
		nOutput--;
	}
	
	if (!op->isConnected) {
		return B_OK;
	}
	
	status_t	err  = BreakOutputConnection(op);	
	op->destination = media_destination::null;
	if (err != B_OK) {
		::reportError("Sampler: can't break output, %s", ErrorStr(err));
	}
	return B_OK;
}

#else
status_t
Sampler::CloseOutput(
	QuaAudioPort *port,
	long insert
	)
{
	if (outSubscriber->IsInStream())
		outSubscriber->ExitStream(TRUE);
	return B_OK;
}
#endif

#ifdef NEW_MEDIA
status_t
Sampler::BreakOutputConnection(QuaAudioOut *op)
{
	if (op->isConnected) {
		fprintf(stderr, "Closing output %s\n", op->insertName);

// Ordinarily we'd stop *all* of the nodes in the chain at this point.  However,
// one of the nodes is the System Mixer, and stopping the Mixer is a Bad Idea (tm).
// So, we just disconnect from it, and release our references to the nodes that
// we're using.  We *are* supposed to do that even for global nodes like the Mixer.
// I presume that nodes on other chains will be ok, though perhaps starved,
// and there may be a lateness backlash.
		status_t err = StopQuaNode();
		if (err != B_OK) {
			::reportError("Sampler::BreakOutputConnection() couldn't stop qua node: %s", ErrorStr(err));
			return err;
		}
		
		BMediaRoster* r = BMediaRoster::Roster();

		fprintf(stderr, "Sampler::BreakOutputConnection() disconnecting (node %d  %d, port %d/%d %d/%d)\n",
				/*op->producer.node*/Node().node,
				op->consumer.node,
				op->source.port,
				op->source.id,
				op->destination.port,
				op->destination.id
			);
		err = r->Disconnect(
					/*op->producer.node*/Node().node,
					op->source,
					op->consumer.node,
					op->destination);
		if (err != B_NO_ERROR) {
			fprintf(stderr, "Sampler::BreakOutputConnection() disconnection error (node %d  %d, port %d/%d %d/%d):\n\t%s\n",
				/*op->producer.node*/Node().node,
				op->consumer.node,
				op->source.port,
				op->source.id,
				op->destination.port,
				op->destination.id,
				ErrorStr(err));
		}
//		err = r->ReleaseNode(op->producer);
		err = r->ReleaseNode(op->consumer);
		if (err != B_NO_ERROR) {
			fprintf(stderr, "Cannot release node: %s", ErrorStr(err));
		}

// if there's still a reason to, restart the node.
		StartQuaNode();
	}
	return B_OK;
}
#endif


	
bool
Sampler::Generate(
#ifdef NEW_MEDIA
	bigtime_t event_time,  size_t nFrames
#else
	float *outSamples, size_t nFrames, short nChannels
#endif
)
{

#ifdef NEW_MEDIA	
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

#else
	for (int j=0; j<nFrames*samplesPerOutFrame; j++) {
		outSamples[j] = 0;
	}
#endif

//	fprintf(stderr, "filling it in...\n");

		/*
		 * Synthesize into the buffer that has just completed and set it running
		 */

	if (uberQua->status != STATUS_SLEEPING) {
		if (channels.Lock() != B_NO_ERROR) {
			::reportError("sampler: can't acquire sem\n");
			return false;
		}
		for (short i=0; i<channels.CountItems(); i++) {
	// ! must check for SR change ... one day
			Channel	*chan = (Channel *)channels.ItemAt(i);
			int nf = chan->Generate(
#ifdef NEW_MEDIA
								event_time,
								nFrames
#else
								qmo_temp,
								nFrames
#endif
						);
//			fprintf(stderr, "%d chan fill %d\n", i, chan->chanId);
#ifndef NEW_MEDIA
			for (int j=0; j<nf*samplesPerOutFrame; j++) {
				outSamples[j] += qmo_temp[j];
			}
#endif
		}
		
		channels.Unlock();
	} else {
	}
	
	editors.Lock();
	for (short i=0; i<editors.CountItems(); i++) {
		SampleEditor	*edit = (SampleEditor *)editors.ItemAt(i);
		int nf = edit->Generate(qmo_temp,
								nFrames,
								samplesPerOutFrame);
#ifndef NEW_MEDIA
		for (int j=0; j<nf*samplesPerOutFrame; j++) {
			outSamples[j] += qmo_temp[j];
		}
#endif
	}
	editors.Unlock();

#ifdef NEW_MEDIA	
	for (short i=0; i<nOutput; i++) {
		QuaAudioOut	*op = &outPort[output[i]];
		if (op->isConnected && op->buffer) {
			float	*s = op->quap_buf+op->offset;
			long	ns = nFrames * op->nChannel;
			switch (op->output.format.u.raw_audio.format) {
		
				case media_raw_audio_format::B_AUDIO_FLOAT: {
					float	*d = ((float *)op->buffer->Data()) + op->offset;
					for(unsigned long j=0; j<ns; j++){
						*d++ = *s++;
					}
					break;
				}
				case media_raw_audio_format::B_AUDIO_SHORT: {
					int16	*d = ((int16 *)op->buffer->Data()) + op->offset;
					for(unsigned long j=0; j<ns; j++){
						*d++ = AUDIO_16BIT_RANGE *  *s++;
					}
					break;
				}
				case media_raw_audio_format::B_AUDIO_INT: {
					int32	*d = ((int32 *)op->buffer->Data()) + op->offset;
					for(unsigned long j=0; j<ns; j++){
						*d++ = AUDIO_32BIT_RANGE * *s++;
					}
					break;
				}

				case media_raw_audio_format::B_AUDIO_CHAR: {
					int8	*d = ((int8 *)op->buffer->Data()) + op->offset;
					for(unsigned long j=0; j<ns; j++){
						*d++ = AUDIO_8BIT_RANGE * *s++;
					}
					break;
				}
			}

		}
	}
#endif	
	return true;
}

#ifdef NEW_MEDIA
long
Sampler::Consume(BBuffer *buffer, QuaAudioIn *ip)
{
	int			nFrames = ip->input.format.u.raw_audio.buffer_size/
							ip->frameSize;
	float		*indata = ip->data->InPtr();
	long		nin = nFrames*ip->nChannel;

	if (ip->data->Fits(nin)) {
		switch (ip->input.format.u.raw_audio.format) {
		case media_raw_audio_format::B_AUDIO_UCHAR: {
			uchar		*data = (uchar *)buffer->Data();
			for (int i=0; i<nin; i++) {
				indata[i] = (data[i]-128.0) / 128.0;
			}
			break;
		}
		case media_raw_audio_format::B_AUDIO_SHORT: {
			short		*data = (short *)buffer->Data();
			for (int i=0; i<nin; i++) {
				indata[i] = (data[i]/32767.0);
			}
			break;
		}
		
		case media_raw_audio_format::B_AUDIO_FLOAT:
			memcpy(indata, buffer->Data(), ip->bufSize);
			break;
			
		case media_raw_audio_format::B_AUDIO_INT: {
			int		*data = (int *)buffer->Data();
			for (int i=0; i<nin; i++) {
				indata[i] = (((float)data[i])/0x7fffffff);
			}
			break;
		}
		
		}
		
		ip->frameCount += nFrames;
		ip->data->Add(nFrames*ip->nChannel);
	} else {	// panic, the system has gacked up somewhere
		fprintf(stderr, "Qua: Sampler panic, input racing\n");
	}
	

//	
// check for triggered events
//
	long		nEventsTriggered=0, nFramesAvailable=0x0fffffff;
	for (short i=0; i<nInput; i++) {
		QuaAudioIn	*yp = &inPort[input[i]];
		if (yp->isConnected) {
			long nf = yp->data->nitems / yp->nChannel;
			if (nf < nFramesAvailable) {
				nFramesAvailable = nf;
			}
		}
	}
	nEventsTriggered = nFramesAvailable/framesPerEvent;
	
	if (nEventsTriggered > 0) {
// simultaneously flush equal amounts of data from all input
// ports to all receiving channels.
		for (short i=0; i<uberQua->nChannel; i++) {
			Channel	*c = uberQua->channel[i];
			bool	recording = (c->recordState = RECORD_ING);
	
			if (c->thruEnabled || recording) {
				c->Receive(nFramesAvailable);
			}
		}
		for (short i=0; i<nInput; i++) {
			QuaAudioIn	*yp = &inPort[input[i]];
			yp->data->Take(nFramesAvailable*yp->nChannel);
		}
	}	
	
	return nEventsTriggered;
}

#endif

#endif
#endif

