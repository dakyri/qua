#ifndef _QUA_VERSIO
#define _QUA_VERSIO

#if defined(WIN32)

#define QUA_V_AUDIO
#define QUA_V_AUDIO_ASIO
#define QUA_V_DIRECT_MIDI
#define QUA_V_JOYSTICK
#define QUA_V_JOYSTICK_DX // windows direct input joystick ... preferred to mm device
#undef QUA_V_JOYSTICK_MMC
#define QUA_V_VST
#define QUA_V_VST_HOST
#undef QUA_V_VST_HOST_GUI
#undef QUA_V_RAM_LOCKED_BUFFERS
#define QUA_V_GDI_PLUS
#undef QUA_V_DEBUG_CONSOLE
#undef LOTSALOX // these are places where I think locking was unnecessary ... but keep in place for now
#undef VOICE_MAINSTREAM // whether voices define a stream internally ... these days i think they should, and not sure why this has been undef'd
#define INIT_BY_METHOD // defines how we are initializing the sequencer ... this sucks but it does work TODO XXXXX FIXME
#undef QUA_V_MULTIMEDIA_TIMER // affects what we are using as timing source. this is ok, but it's windows specific and would like to ditch os dependencies where possible
#undef QUA_V_SWITCH_TO_THREAD // another windows dependency we're trying to do without at this level
#undef QUA_V_ALARMCLOCK // 
#undef OLD_POOL_PLAY // older way that pools played data, an implicit player. these days function of a pool is a data repository. there are now
					// a few 'players' that work like this version worked, though .. i remember this as having a few neat aspects
#undef ASYNC_CHAN_OUT // one synchronization option on channel io ... not sure whether to keep or throw. experiment
#undef ASYNC_CHAN_IN
#undef DEFINE_COUNT // an early mode on the symbol table redefined symbols was allowed but led to indexed versions of symbols and a var#number syntax. unsure
#define GLOUB_LITTLE_ENDIAN
#undef Q_FRAME_MAP // a mapping of frames onto display objects ... debating which is better
#undef STATEMENT_ASSIGNEMTS // affecting at what point assignements are done in script::UpdateActive ... i think this is discardable but check
#undef OLD_WITH // earlier way the 'with' element works
#undef OLD_SAMPLE // earlier sample gen code ... probably to ditch, but also verify first
#undef  INIT_CONTROL_VAR_ONLY // more wierd variations ... probably in for testing ... another to either keep or throw
#undef QUA_V_SAMPLE_DRAW // controls how files drown =='p' drawing ... never had a good way of caching peak datat
#define QUA_V_BETTER_SCHEDULER
#undef QUA_V_HAS_POOLPLAYER
#define QUA_V_SAVE_INITASXML

#endif

// !! ?? probably not the most logical place for all these, but
// this is the first loaded file ... too many pointless changes else ...
#define MAX_QUA_AUDIO_OUTPUT	32
#define MAX_QUA_AUDIO_INPUT	16
#define MAX_QUA_CHANNEL_INPUT	16
#define MAX_QUA_CHANNEL_OUTPUT	16
#define MAX_QUA_MIDI_CONNECTION	16
#define MAX_FILTER_PARAMS	20
#define MAX_SAMPLE_PLAYER_PARAMS	20
#define MAX_CONTEXT_STACK	192
#define MAX_SYMBOLS	30*1024
#define MAX_STREAM	32767	// oops. way short .. what was I thinkin
#define MAX_FILTER	40
#define MAX_CHANNEL	42
#define MAX_MIDI_PORTS	32
#define MAX_DESTINATION_AUDIO_PORT	8	// a ceiling on the number of audio channels we process through a channe
#define QUA_MAX_AUDIO_BUFSIZE	4096	// expected max audio bufsize for a handful of sts
#define MAX_BUFFERS_PER_SAMPLE	64
#define MAX_REQUESTS_PER_SAMPLE	64
#define QUA_DFLT_SAMPLE_READ_BUF_BYTES	(16*4*1024)	


#define samplesPerBuffer	(16*1024)
#define bytesPerBuffer	(samplesPerBuffer*sizeof(float))
#define CHUNK_SIZE samplesPerBuffer

#endif