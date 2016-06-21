#ifndef _QUA_VERSIO
#define _QUA_VERSIO

#if defined(WIN32)

#undef QUA_V_APP_HANDLER
#undef QUA_V_QUADDON
#undef QUA_V_PORT_PARAM
#undef QUA_V_STREAM_MESG
#undef QUA_V_CONTROLLER_INTERFACE
#undef QUA_V_ARRANGER_INTERFACE
#undef QUA_V_EDITOR_INTERFACE
#undef QUA_V_MIXER_INTERFACE
#define QUA_V_AUDIO
#undef QUA_V_AUDIO_ASIO
#define QUA_V_DIRECT_MIDI
#undef QUA_V_JOYSTICK
#undef QUA_V_JOYSTICK_DX
#undef QUA_V_JOYSTICK_MMC
#undef QUA_V_VST
#undef QUA_V_VST_HOST
#undef QUA_V_VST_HOST_GUI
#undef QUA_V_RAM_LOCKED_BUFFERS

// different structural versions ...
#define QUA_V_BETTER_SCHEDULER
#undef QUA_V_HAS_POOLPLAYER

#endif

#undef OLD_LIST

// !! ?? probably not the most logical place for all these, but
// this is the first loaded file ... too many pointless changes else ...
#define MAX_QUA_NAME_LENGTH	255
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