#ifndef _CHANNEL
#define _CHANNEL

#include "qua_version.h"


#include <stdio.h>
#include <vector>

class Qua;

#include "Stream.h"
#include "Note.h"
#include "QuaTime.h"
#include "Edittable.h"
#include "Stacker.h"
#include "Executable.h"
#include "Destination.h"

class InputList: public std::vector<Input *>
{
public:
	inline bool Has(Input *i) { return qut::find(*this, i) != end(); }
	inline Input *Item(short i) { return i >= 0 && ((size_t)i) < size() ? at(i) : nullptr; }
	inline void Add(Input *p) { if (!Has(p)) push_back(p); }
	inline void Del(Input *i) { auto ci = qut::find(*this, i); if (ci != end()) erase(ci); }
};

class OutputList: public std::vector<Output *>
{
public:
	inline bool Has(Output *i) { return qut::find(*this, i) != end();	}
	inline Output *Item(short i) { return i >= 0 && ((size_t)i) < size() ? at(i) : nullptr; }
	inline void Add(Output *p) { if (!Has(p)) push_back(p); }
	inline void Del(Output *i) { auto ci = qut::find(*this, i); if (ci != end()) erase(ci); }
};



#define MAX_NOTE_HISTORY	30

enum {
	RECORD_NOT = 0,
	RECORD_ENABLE = 1,
	RECORD_ING = 2
};


class ChannelView;
class Instance;
class SampleInstance;
class PoolInstance;
class Instance;
class QuaControllerBridge;
class QuaChannelBridge;
class Block;
class QuaPort;
class FloatQueue;
struct IXMLDOMElement;

enum {
	CBUT_INENABLE=0,
	CBUT_OUTENABLE=1,
	CBUT_THRUENABLE=2,
	CBUT_RECORDENABLE=3
};

	
class Channel:	public Stackable,
				public Edittable,
				public Stacker
{
public:
						Channel(std::string nm,
								short id,
								bool authru,
								bool midithru,
								short nic,
								short noc,
								Qua *q);
						~Channel();


	void				OutputStream(Stream *S);
	void				InputStream(Stream *S);
	void				CheckOutBuffers();
	void				ClearOutBuffers(bool Clr);
	void				CheckInBuffers();
	bool				ItemsAvailable();
	void				ClearInBuffers(bool Clr);
	void				Stop();
	void				Start();

	Block				*initBlock;
	bool				Init();

	status_t			SaveSnapshot(FILE *fp);
	status_t			Save(FILE *fp, short in);

	status_t			LoadSnapshotElement(tinyxml2::XMLElement *);
	status_t			LoadSnapshotChildren(tinyxml2::XMLElement *element);

	size_t				Generate(size_t nf);
	size_t				Receive(size_t nf);
	
	Event				rx;
	Event				tx;
	
//	RWLock				fltLock;

	QuasiStack			*rxStack;
	QuasiStack			*txStack;
	short				chanId;
	
	float				**inSig;
	float				**instSig;
	float				**outSig;
	
  	QuaControllerBridge		*parameters;
	void				EnableIn(bool);
	void				EnableOut(bool);
	void				EnableAudioThru(bool);
	void				EnableMidiThru(bool);
	bool				audioThru;
	bool				midiThru;
	bool				inEnabled;
	bool				outEnabled;

	uchar				hasAudio;
	
	Input				*AddInput(char *, QuaPort *, short, bool);
	Output				*AddOutput(char *, QuaPort *, short, bool);
	void				SetInput(Input *, QuaPort *, short);
	void				SetOutput(Output *, QuaPort *, short);
	void				RemoveInput(Input *);
	void				RemoveOutput(Output *);
	bool				Enable(Input *, bool);
	bool				Enable(Output *, bool);
	
	void				RemoveReceivingInstance(Instance *i);
	void				AddReceivingInstance(Instance *i);
	void				SetAudioChannelCount(short);

  	short				nAudioIns;
 	short				nAudioOuts;

	Stream				outStream;
	Stream				inStream;

	std::mutex receivingLock;
	std::vector<Instance*> receivingInstances;
	std::vector<Instance*> audioInstances;
	std::mutex activePortsLock;

	InputList			inputs;
	OutputList			outputs;
	InputList			activeStreamInputs;
	OutputList			activeStreamOutputs;
	InputList			activeAudioInputs;
	OutputList			activeAudioOutputs;
	
#ifdef ASYNC_CHAN_OUT
	SchLock				outMutex;
#endif
#ifdef ASYNC_CHAN_IN
	SchLock				inMutex;
#endif
	float				gain;
	float				pan;
	
// record stuff...
	status_t			StartRecording();
	status_t			StopRecording();
//	void				UpdateRecordDisplay();
	void				CheckRecord(Stream *);
	void				UpdateRecordDisplay();
	void				EnableAudioRecord(SampleInstance *i);
	void				EnableStreamRecord(Instance *i);
	void				DisableRecord();
		
	SampleInstance		*audioRecordInstance;
	Instance			*streamRecordInstance;
	Time				recordStarted;
	short				recordState;
	bool				reRecord;

};

Channel	*LoadChannel(FILE *fp, Qua *u);

Channel	*FindChannelForMidi(uchar port, uchar chan);

#endif