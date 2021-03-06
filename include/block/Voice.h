#ifndef _VOICE
#define _VOICE

class Qua;
class StabEnt;
class Block;
class Lambda;
class QuaControllerBridge;
class StreamTake;
class Channel;
struct IXMLDOMElement;

#include "Stream.h"
#include "Schedulable.h"
#include "Clip.h"

#include <ostream>

class Voice: public Schedulable
{
public:
						Voice(std::string nm, Qua *parent);
						~Voice();
						
	virtual bool		Init();
	
	virtual void		Cue(Time &t) override;
	virtual status_t	Wake(Instance *i) override;
	virtual	status_t	Sleep(Instance *i) override;
	virtual status_t	Recv(Stream &stream) override;
	virtual status_t	QuaStop() override;
	virtual status_t	QuaStart() override;
	virtual status_t	QuaRecord() override;

	virtual Instance *addInstance(const std::string &, const Time &startt, const Time &dur, Channel * const chan) override;
	virtual Instance *addInstance(const string &nm, const short chan_id, const Time &t, const Time &d, const bool disp) override;

	status_t			Save(ostream &out, short indent);
	status_t			SaveSnapshot(FILE *fp);

	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	StreamTake			*addStreamTake(std::string &nm, const Time &t, bool);
	StreamTake			*addStreamTake(std::string &nm, const std::string &fnm, bool);
	status_t			DeleteTake(StreamTake *, bool);
	status_t			SelectTake(StreamTake *, bool);
	StreamTake			*selectedTake;
	std::vector<StreamTake*> outTakes;

	Clip				*AddClip(std::string nm, StreamTake *, Time&, Time&, bool disp);
	void				RemoveClip(Clip *, bool disp);
	inline Clip			*streamClip(long i) {
		return ((size_t)i)<clips.size() && i>=0? clips[i]:nullptr;
	}
	long				nClip() {	return clips.size(); }

	std::vector<Clip*>	clips;

#ifdef VOICE_MAINSTREAM
	StabEnt				*streamSym;
    Stream				mainStream;
#endif
};

#include "Instance.h"

class VoiceInstance: public Instance
{
public:
	VoiceInstance(Voice &s, const std::string&, const Time &t, const Time &d, Channel *const c);
	virtual ~VoiceInstance();

	virtual size_t		Generate(float **bufs, long nf, short nc);	// add to out buf
	virtual status_t	Run();
};

#endif
