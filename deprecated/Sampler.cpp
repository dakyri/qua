#if defined _BEOS

#include "StdDefs.h"
#include "SampleFile.h"

#include "include/Sampler.h"
#include "include/Sample.h"
#include "include/Qua.h"
#include "include/SampleEditor.h"
#include "include/Quapp.h"
#include "include/Channel.h"
#include "include/QuaAudio.h"
#include "include/messid.h"
#ifndef NEW_MEDIA
#include "OldMediaKit.h"
#endif

#ifdef SEMWAIT
sem_id	fluffySem = create_sem(0, "fluff");
#endif

// short term hack!
#include "MediaKit.h"

Sampler::Sampler(Qua *q)
#ifdef NEW_MEDIA
	: BMediaNode("Qua Audio")
	, BBufferProducer(B_MEDIA_RAW_AUDIO)
	, BBufferConsumer(B_MEDIA_RAW_AUDIO)
	, BMediaEventLooper()
#endif
{
	status = STATUS_SLEEPING;
	uberQua = q;
#ifdef NEW_MEDIA
	
	preferredFormat.type = B_MEDIA_RAW_AUDIO;
	preferredFormat.u.raw_audio.format = media_raw_audio_format::B_AUDIO_FLOAT;
	preferredFormat.u.raw_audio.channel_count = 2;
	preferredFormat.u.raw_audio.frame_rate = 44100;		// measured in Hertz
	preferredFormat.u.raw_audio.byte_order = (B_HOST_IS_BENDIAN) ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN;

	// we'll use the consumer's preferred buffer size, if any
	preferredFormat.u.raw_audio.buffer_size = media_raw_audio_format::wildcard.buffer_size;

	for (short i=0; i<MAX_QUA_AUDIO_OUTPUT; i++) {
		outPort[i].output.format = preferredFormat;
		outPort[i].frameSize = 8;
		outPort[i].frameCount = 0;
		outPort[i].downstreamLatency = 0;
		outPort[i].bufferGroup = nullptr;
		outPort[i].quaport = nullptr;
	}
	for (short i=0; i<MAX_QUA_AUDIO_INPUT; i++) {
		inPort[i].frameSize = 8;
		inPort[i].input.format = preferredFormat;
		inPort[i].frameCount = 0;
		inPort[i].upstreamLatency = 0;
		inPort[i].quaport = nullptr;
	}
	
	internalLatency = 0;

	startTime = 0;
	framesPerEvent = 512;
	frameRate = 44100.0;
	nOutput = nInput = 0;
#else
	outStream = new BDACStream();
	outSubscriber = new BSubscriber("quadio out");
	outSubscriber->Subscribe( outStream);
	outStream->SetStreamBuffers(kAudioBufferSize,	kNumAudioBuffers);
	
	inStream = new BADCStream();
	inSubscriber = new BSubscriber("quadio in");
	inSubscriber->Subscribe(inStream);
	inStream->SetStreamBuffers(kAudioBufferSize,	kNumAudioBuffers);
#endif
    writerThread = spawn_thread(WriterWrapper, "fluffaerator",
								B_NORMAL_PRIORITY, this);
}

Sampler::~Sampler()
{
	fprintf(stderr, "deleting sampler...\n");

	StopSampler();
	status = STATUS_DEAD;

#ifdef SEMWAIT
	release_sem(fluffySem);
#endif
	resume_thread(buffyThread);
	resume_thread(writerThread);
	status_t	err;
	wait_for_thread(buffyThread, &err);
	wait_for_thread(writerThread, &err);
	fprintf(stderr, "deleted sampler\n");
}




void
Sampler::StartPreview(SampleEditor *i)
{
	editors.Lock();
	editors.AddItem(i);
    editors.Unlock();
}

void
Sampler::StopPreview(SampleEditor *i)
{
	editors.Lock();
	editors.RemoveItem(i);
    editors.Unlock();
}

#endif