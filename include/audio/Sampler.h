#ifndef _SAMPLER
#define _SAMPLER

// actually the main audio generator for qua.
// audio port and audio manager represent the actual hardware
// devices and a roster of such that this beast talks to.

#ifdef _BEOS

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <MediaKit.h>
#include <InterfaceKit.h>
#endif

class Sample;
class SampleInstance;
class SampleEditor;
class QuaAudioPort;

#define kAudioBufferSize 4096
#define kNumAudioBuffers 6
#define kSamplingRate	44100

#include "RWLock.h"
#include "QuaTypes.h"

class Sampler
#if defined(_BEOS) && defined(NEW_MEDIA)
	: public BBufferConsumer
	, public BBufferProducer
	, public BMediaEventLooper
#endif
{
public:
						Sampler(class Qua *);
						~Sampler();
#ifdef _BEOS

#ifdef NEW_MEDIA
// BMediaNode methods
	BMediaAddOn* 		AddOn(	int32* internal_id) const;
	void				Preroll();
	void				SetTimeSource(BTimeSource* time_source);
	status_t			RequestCompleted(const media_request_info & info);
// BBufferProducer methods
	status_t 			FormatSuggestionRequested(
								media_type type,
								int32 quality,
								media_format* format);
	status_t 			FormatProposal(
								const media_source& output,
								media_format* format);
	status_t 			FormatChangeRequested(
								const media_source& source,
								const media_destination& destination,
								media_format* io_format,
								int32* _deprecated_);
	status_t 			GetNextOutput(
								int32* cookie,
								media_output* out_output);
	status_t 			DisposeOutputCookie(int32 cookie);
	status_t			SetBufferGroup(
								const media_source& for_source,
								BBufferGroup* group);
	status_t 			GetLatency(bigtime_t* out_latency);
	status_t 			PrepareToConnect(
								const media_source& what,
								const media_destination& where,
								media_format* format,
								media_source* out_source,
								char* out_name);
	void 				Connect(status_t error, 
								const media_source& source,
								const media_destination& destination,
								const media_format& format,
								char* io_name);
	void 				Disconnect(
								const media_source& what,
								const media_destination& where);

	void 				LateNoticeReceived(
								const media_source& what,
								bigtime_t how_much,
								bigtime_t performance_time);
	void 				EnableOutput(
								const media_source & what,
								bool enabled,
								int32* _deprecated_);
	status_t 			SetPlayRate(int32 numer,int32 denom);
	status_t 			HandleMessage(
								int32 message,
								const void* data,
								size_t size);
	void 				AdditionalBufferRequested(
								const media_source& source,
								media_buffer_id prev_buffer,
								bigtime_t prev_time,
								const media_seek_tag* prev_tag);	//	may be nullptr
	void 				LatencyChanged(
								const media_source& source,
								const media_destination& destination,
								bigtime_t new_latency,
								uint32 flags);

// virtuals from class BBufferConsumer
	status_t 			AcceptFormat(
							const media_destination& dest,
							media_format* format);
	status_t 			GetNextInput(
							int32* cookie,
							media_input* out_input);
	void 				DisposeInputCookie( int32 cookie );
	void 				BufferReceived( BBuffer* buffer );

	void 				ProducerDataStatus(
							const media_destination& for_whom,
							int32 status,
							bigtime_t at_performance_time);
	status_t			GetLatencyFor(
							const media_destination& for_whom,
							bigtime_t* out_latency,
							media_node_id* out_timesource);
	status_t 			Connected(
							const media_source& producer,
							const media_destination& where,
							const media_format& with_format,
							media_input* out_input);
	void 				Disconnected(
							const media_source& producer,
							const media_destination& where);
	status_t			FormatChanged(
							const media_source& producer,
							const media_destination& consumer, 
							int32 change_tag,
							const media_format& format);
	status_t 			SeekTagRequested(
							const media_destination& destination,
							bigtime_t in_target_time,
							uint32 in_flags, 
							media_seek_tag* out_seek_tag,
							bigtime_t* out_tagged_time,
							uint32* out_flags);

// BMediaEventLooper methods
	void 				NodeRegistered();

	void 				Start(bigtime_t performance_time);
	void 				Stop(bigtime_t performance_time, bool immediate);
	void 				Seek(bigtime_t media_time, bigtime_t performance_time);
	void 				TimeWarp(bigtime_t at_real_time, bigtime_t to_performance_time);
	status_t 			AddTimer(bigtime_t at_performance_time, int32 cookie);

	void 				SetRunMode(run_mode mode);

	void 				HandleEvent(
							const media_timed_event* event,
							bigtime_t lateness,
							bool realTimeEvent = false);

protected:
	void 				CleanUpEvent(const media_timed_event *event);
	bigtime_t			OfflineTime();
	void 				ControlLoop();
	status_t 			DeleteHook(BMediaNode* node);

// my hooks
public:
	status_t			StartQuaNode();
	status_t			StopQuaNode();
	
	long				Consume(BBuffer *buffer, QuaAudioIn *ip);

#else
	static long			OutCompWrapper(void* userData, long error);
	static bool			OutStreamWrapper(void *userData, char *buffer, unsigned long count, void *h);
	long				OutComp();
	bool				OutStream(char *buffer, unsigned long count);
	static long			InCompWrapper(void* userData, long error);
	static bool			InStreamWrapper(void *userData, char *buffer, unsigned long count, void *h);
	long				InComp();
	bool				InStream(char *buffer, unsigned long count);
#endif


	bool				Generate(
#ifdef NEW_MEDIA
							bigtime_t event_time, size_t nof
#else
							float *, size_t, short
#endif
						);

#ifdef NEW_MEDIA
	QuaAudioIn *		OpenInput(
							QuaAudioPort *, media_source&, media_format&
							);
	status_t			CloseInput(
							QuaAudioIn *
							);
	QuaAudioOut *	OpenOutput(
							QuaAudioPort *, media_destination&, media_format&
							);
	status_t			CloseOutput(
							QuaAudioOut *
							);
							
	status_t			MakeOutputConnection(QuaAudioOut *, media_input *);
	status_t			MakeInputConnection(QuaAudioIn *, media_output *);
	status_t			BreakOutputConnection(QuaAudioOut *);
	status_t			BreakInputConnection(QuaAudioIn *);
#else
	status_t			OpenOutput(
							QuaAudioPort *, long which
							);
	status_t			CloseOutput(
							QuaAudioPort *, long which
							);
	status_t			CloseInput(
							QuaAudioPort *, long which
							);
	status_t			OpenInput(
							QuaAudioPort *, long which
							);
#endif

#endif	// BEOS specific bits


	void				StartSampler();
	void				StopSampler();
	
	void				StartPreview(SampleEditor *s);
	void				StopPreview(SampleEditor *s);

	SchLock				editorsLock;
	BList				editors;

#ifdef NEW_MEDIA
	short				nOutput, nInput;
	short				output[MAX_QUA_AUDIO_OUTPUT];
	short				input[MAX_QUA_AUDIO_INPUT];
	QuaAudioOut	 	outPort[MAX_QUA_AUDIO_OUTPUT];
	QuaAudioIn		inPort[MAX_QUA_AUDIO_INPUT];
	
	media_format 		preferredFormat;
	bigtime_t 			internalLatency;
	bigtime_t			startTime;
	long				framesSent;
	float				frameRate;
	short				framesPerEvent;
	media_node			masterClock;
	BTimeSource			*masterTimeSource;
#else	
	BSubscriber			*outSubscriber;
	BDACStream			*outStream;
	BSubscriber			*inSubscriber;
	BADCStream			*inStream;
#endif

	Qua					*uberQua;

	qua_status			status;
};

#undef SEMWAIT	//8

#endif

#endif