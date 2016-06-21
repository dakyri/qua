#ifndef _CLIP
#define _CLIP

#include "QuaTime.h"
#include "Stream.h"

class StabEnt;
class Sample;
class SampleFile;
class Clip;

namespace tinyxml2 {
	class XMLElement;
}

class Take
{
public:
						Take(std::string nm, StabEnt *, short t);
						~Take();
						
	enum {
		NOT=0,
		STREAM=1,
		SAMPLE=2
	};

	virtual status_t	SaveSnapshot(FILE *fp)=0;

	StabEnt				*sym;
	short				type;
	status_t			status;
};

#if QUA_V_SAMPLE_DRAW=='p'
struct qpk_hdr_1 {
	uint32				magic;
	uint32				chunkLength;
	uint8				nChannels;
	uint8				unusedByte3_2;
	uint8				unusedByte3_3;
	uint8				unusedByte3_4;
	int32				unused4;
	int32				unused5;
	int32				unused6;
	int32				unused7;
	int32				unused8;
};
#endif
class SampleTake: public Take
{
public:
						SampleTake(Sample *, std::string nm, std::string path);
						~SampleTake();
	long				NextChunk(long, long, long);
	long				PrevChunk(long, long, long);

	status_t			SetSampleFile(const std::string path, SampleFile *);

	Sample *sample;
	SampleFile *file;
	std::string path;

	Time				&Duration();
	virtual status_t	SaveSnapshot(FILE *fp) override;
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

#if QUA_V_SAMPLE_DRAW=='p'
	enum {
		PEAK_BUFFER_CHUNKS = 512,
		PEAK_FILE_DFLT_CHUNK_LENGTH = 512
	};
	FILE				*peakFile;
	float				*peakBuffer;
	long				peakBufferLength;
	long				nPeakBufferChunks;
	long				peakBufferStartChunk;
	long				peakCacheChunkLength;
	thread_id			peakCacheCreationThread;

	int32				Peakerator();
	static int32		PeakeratorWrapper(void *data);
	bool				CreatePeakFile();
	bool				HasPeakData();
	bool				OpenPeakCache();
	bool				ClosePeakCache();
	bool				PeakData(long sfr, long efr, short channel, float &hi, float &lo);
#else
// baseline stuff ... read the data straight off file, and average it in the drawing thread.
// uggh!!!! but works...
	bool				PeakData(long sfr, long efr, short channel, float &hi, float &lo);

	enum {
		N_PEAK_BUFFER_FRAMES = 40*1024
	};
	float				*peakBuffer;
	char				*rawBuffer;
	long				peakBufferStartFrame;
	long				peakBufferNFrames;
#endif

};

class StreamTake: public Take
{
public:
						StreamTake(std::string, StabEnt *, Time &);
						~StreamTake();
						
	virtual status_t	SaveSnapshot(FILE *fp);
	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	Stream				stream;
	Time				duration;
	flag				saveFormat;
};

enum {
	STREAM_SAVE_BLOCK=0,	// as a block of code
	STREAM_SAVE_FUNCTION=1, // as a block referencing a local function
	STREAM_SAVE_BIN=1,		// as a piece of binary data
	STREAM_SAVE_MIDI=2		// as a midi file
};

class Clip
{
public:
						Clip(std::string nm, StabEnt *ctxt);
	bool				Set(Take *, Time&s, Time &t);
	status_t			SaveSnapshot(FILE *fp);

	Take				*media;
	Time				start;
	Time				duration;

	StabEnt				*sym;
};

#endif
