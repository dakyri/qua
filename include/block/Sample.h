#ifndef _SAMPLE
#define _SAMPLE

#include <string>
#include <vector>
#include <mutex>
#include <ostream>

#include "SampleFile.h"

#include "Schedulable.h"
#include "QuaTime.h"
#include "Instance.h"

class SampleInstance;
class SampleFile;
class Sample;
struct IXMLDOMElement;


class SampleBuffer {
public:
						SampleBuffer();
						~SampleBuffer();
	inline bool			HasFrame(SampleFile *f, long fr) {
		return (file == f && fr >= fromFrame && fr < fromFrame+nFrames);
	}
	void				Set(SampleFile *, long, long, long);
	long				nRequest;
	float				*data;
	long				nFrames;
	long				fromFrame;
	long				chunk;
	SampleFile			*file;
	SampleBuffer		*next;
};

#include "Clip.h"

#define SAMPLE_DATA_REQUEST_NOTHING	-1

class Sample: public Schedulable {
public:
						Sample(std::string nm, std::string path, Qua *uq, short, short);
						~Sample();
						
	virtual bool		Init();
	virtual Instance *addInstance(const std::string &, const Time &startt, const Time &dur, Channel * const chan) override;
	virtual Instance *addInstance(const string &nm, const short chan_id, const Time &t, const Time &d, const bool disp) override;
	virtual void		RemoveInstance(Instance *i, bool display);

	virtual void		Cue(Time &t) override;
	virtual status_t	Wake(Instance *i) override;
	virtual	status_t	Sleep(Instance *i) override;
	virtual status_t	Recv(Stream &stream) override;

	status_t			Save(ostream &out, short indent);
	status_t			SaveSnapshot(FILE *fp);

	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	long				SynchronizeBuffers();
	long				Stash(float **, short, long);
	status_t			FlushRecordBuffers(bool);
	bool				RequestChunk(SampleTake *take, long chunk);
	long				Play(float **outSig, short nc, long nf,
							SampleTake *media, long sf, long ef, long currentFrame,
							float gain, float pan,
							int32 &state, flag loop, flag dir);
	double				PlayPitched(float **outSig, short nAudioChannels, long nFramesReqd,
							SampleTake *take, long startFrame, long endFrame, double position,
							float gain, float pan, double index, float *last, int32 &state, flag loop, flag dir);

	long				ResetClip(Clip *clip);
	long				ResetTake(SampleTake *take);

	
	long sampleRate;

	SampleFile *selectedFile();
	std::string selectedPath();

	Clip *AddClip(std::string nm, SampleTake *, Time&, Time&, bool disp);
	void RemoveClip(Clip *, bool disp);
	inline Clip *sampleClip(long i) {
		return i >= 0 && ((size_t)i) < clips.size() ? clips[i]: nullptr; 
	}
	long nClip() {	return clips.size(); }

	std::vector<Clip*> clips;

	std::vector<SampleTake*> takes;

	inline SampleTake * Sample::take(short i) { 
		return i >= 0 && ((size_t)i)<takes.size() ? takes[i] : nullptr; 
	}
	inline int Sample::countTakes() { return takes.size(); }

	SampleTake *addSampleTake(const std::string &nm, const std::string &path, bool disp);
	status_t DeleteTake(SampleTake *, bool disp);
	status_t SelectTake(SampleTake *, bool disp);
	SampleTake *AddRecordTake(long fileType, short nChan, short sampleSize, float sampleRate);
	
	status_t			SetTakeList(Block *b);
	
	StabEnt *ampSym;
	StabEnt *panSym;
	StabEnt *sfrSym;
	StabEnt *efrSym;
	
	static long ChunkForFrame(SampleFile *, long);
	static long StartFrameOfChunk(SampleFile *, long);
	static long ChunkForFrame(short, long);
	static long StartFrameOfChunk(short, long);
	SampleBuffer *BufferForFrame(SampleFile *, long f);
	SampleBuffer *BufferForChunk(SampleFile *, long f);

	mutex bufferLock;

	char				*fileBuffer;
	int					fileBufferLength;

	short				nBuffers;
	short				maxBuffers;
	short				maxRequests;
	SampleBuffer		**buffer;
	long				*request;
	SampleTake			**requestedTake;

	SampleBuffer		*currentRecordBuffer;
	SampleBuffer		*pendingRecordBuffers;
	SampleBuffer		*freeRecordBuffers;
	SampleTake			*recordTake;
	long				recordFrame;
	std::mutex			recordbufferLock;
};

inline long
Sample::ChunkForFrame(SampleFile *f, long fr)
{
	return 	(fr*f->nChannels)/samplesPerBuffer;
}

inline long
Sample::ChunkForFrame(short nc, long fr)
{
	return 	(fr*nc)/samplesPerBuffer;
}

inline long
Sample::StartFrameOfChunk(SampleFile *f, long ch)
{
	return 	(ch*samplesPerBuffer)/f->nChannels;
}

inline long
Sample::StartFrameOfChunk(short nc, long ch)
{
	return 	(ch*samplesPerBuffer)/nc;
}

class SampleInstance: public Instance {
public:
	SampleInstance(Sample &s, const std::string &nm, const Time &t, const Time &d, Channel * const c);
	~SampleInstance();

	virtual size_t		Generate(float **bufs, long nf, short nc);	// add to out buf
	virtual void		Reset();
	virtual void		Cue(Time &t);
	virtual bool		Init();
	virtual status_t	Run();

	bool				LoopConditionMet(StreamItem *, Instance *, QuasiStack *);
	
	int WantedChunks(
			short  pcc[],
			short  pcs[],
			short  pce[],
			short  pccl[],
			int np,
			int max);

	Sample &sample;
	
	bool mute;
	float amp;
	float pan;
	
// !!!!!!!! should be off_t
	Time startFrame;
	Time endFrame;
	long loopCount;
	
//processing blocks
	Block *sFrameExp;
	Block *eFrameExp;
	Block *loopCondition;
};


#endif
