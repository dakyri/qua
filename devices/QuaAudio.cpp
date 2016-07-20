#include "qua_version.h"

#ifdef QUA_V_AUDIO

#include "StdDefs.h"

#include "QuaAudio.h"
#include "QuaPort.h"
#include "Instance.h"
#include "Qua.h"
#include "Channel.h"
#include "Block.h"
#include "Schedulable.h"
//#include "messid.h"
#include "Envelope.h"
#include "Sample.h"
#include "FloatQueue.h"

#include <algorithm>
#include "QuaDisplay.h"
#ifdef QUA_V_AUDIO_ASIO
QuaAsio		QuaAudioManager::asio;
#endif

QuaAudioManager		*audioManager = nullptr;

QuaAudioStreamIO::QuaAudioStreamIO(Qua *q, QuaAudioPort *p,
								   long chid, long gid, char *nm,
								   short sf, short nac):
	QuaStreamIO(q, p)
{
	nChannel = nac;
	sampleFormat = sf;
	insertName = nm;
	insertId = chid;
	groupId = gid;
}

QuaAudioOut::QuaAudioOut(Qua *q, QuaAudioPort *p,
								   long chid, long gid, char *nm,
								   short sf, short nac):
	QuaAudioStreamIO(q, p, chid, gid, nm, sf, nac)
{
	offset = 0;
	outbuf = nullptr;
}


QuaAudioIn::QuaAudioIn(Qua *q, QuaAudioPort *p,
								   long chid, long gid, char *nm,
								   short sf, short nac):
	QuaAudioStreamIO(q, p, chid, gid, nm, sf, nac)
{
	data = nullptr;
}

QuaAudioIn::~QuaAudioIn()
{
	if (data)
		delete data;
}

QuaAudioOut::~QuaAudioOut()
{
	if (outbuf)
		delete outbuf;
}

QuaAudioPort::QuaAudioPort(char *nm, QuaAudioManager *qa, short subt)
	: QuaPort(nm, QUA_DEV_AUDIO, subt, QUA_PORT_IO)
{
	quaAudio = qa;
}


const char *
QuaAudioPort::name(uchar dfmt)
{
#ifdef QUA_V_AUDIO_ASIO
	if (dfmt == NMFMT_NAME) {
		if (deviceSubType == QUA_AUDIO_ASIO) {
			if (quaAudio->asio.loaded) {
				return quaAudio->asio.CurrentDriver();
			} else if (quaAudio->asio.preferredDriver.c_str()) {
				return quaAudio->asio.preferredDriver.c_str();
			} else {
				return "asio";
			}
		}
	}
#endif
	return sym->name.c_str();
}


short
QuaAudioPort::NInputs()
{
#ifdef QUA_V_AUDIO_ASIO
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.nInputChannels;
		}
	}
#endif
	return 0;
}

short
QuaAudioPort::NOutputs()
{
#ifdef QUA_V_AUDIO_ASIO
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.nOutputChannels;
		}
	}
#endif
	return 0;
}

short
QuaAudioPort::NInputChannels(short port)
{
	if (deviceSubType == QUA_AUDIO_ASIO) {
		return 1;
	}
	return 0;
}

short
QuaAudioPort::NOutputChannels(short port)
{
	if (deviceSubType == QUA_AUDIO_ASIO) {
		return 1;
	}
	return 0;
}

const string
QuaAudioPort::InputName(port_chan_id ch_id)
{
	if (ch_id < 0 || ch_id >= NInputs()) {
		return "invalid";
	}

#ifdef QUA_V_AUDIO_ASIO
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.input[ch_id]->insertName;
		}
	}
#endif
	return string("In ") + std::to_string(ch_id);
}

const string
QuaAudioPort::OutputName(port_chan_id ch_id)
{
	if (ch_id < 0 || ch_id >= NOutputs()) {
		return "invalid";
	}
#ifdef QUA_V_AUDIO_ASIO
	if (deviceSubType == QUA_AUDIO_ASIO) {
		if (quaAudio->asio.loaded) {
			return quaAudio->asio.output[ch_id]->insertName;
		}
	}
#endif
	return string("Out ") + std::to_string(ch_id);
}

void
QuaAudioPort::ZotInsertCheck(bool check_input, bool check_output, bool check_connection, bool do_squish)
{

}

QuaAudioPort::~QuaAudioPort()
{

}	

#ifdef XXXX
/* the working B*OS version had the ports as schedulable and thus could have code blocks nofified on this event*/
status_t
QuaAudioPort::QuaStart()
{
	Schedulable::QuaStart();
	instanceLock.lock() /*read*/;
	bool	kikkit = false;
	for (short i=0; i<CountInstances(); i++) {
		Instance	*inst = InstanceNo(i);
		if (inst->status == STATUS_RUNNING) {
			kikkit = true;
		}
	}
	if (kikkit) {
//		KickStart();
	}
	instanceLock.unlock() /*read*/;
	return B_NO_ERROR;
}

status_t
QuaAudioPort::QuaStop()
{
	Schedulable::QuaStop();
	instanceLock.lock() /*read*/;
	bool	kikkit = false;
	for (short i=0; i<CountInstances(); i++) {
		Instance	*inst = InstanceNo(i);
		if (inst->status == STATUS_RUNNING) {
			kikkit = true;
		}
	}
	if (kikkit) {
	 	KickStart();
	}
	instanceLock.unlock() /*read*/;
//	for (short i=0; i<activeInstances.CountItems(); i++) {
//		Sleep((Instance *)activeInstances.ItemAt(i));
//	}
	return B_NO_ERROR;
}
#endif

void
QuasiStack::SetAudio(uchar v)
{
	if (v == AUDIO_NONE) {
		hasAudio = AUDIO_NONE;
	} else {
		hasAudio |= v;
		for (QuasiStack *p=lowerFrame; p != nullptr; p = p->lowerFrame) {
			p->hasAudio |= v;
		}
	}
}

///////////////////////////////////////////////////////////
// AudioManager
///////////////////////////////////////////////////////////
QuaAudioManager::QuaAudioManager()
{
	sampleRate = 44100.0;
	bufferSize = 512;
	audioManager = this;

	dfltInput = nullptr;
	dfltOutput = nullptr;

#ifdef QUA_V_AUDIO_ASIO
//	asio.audio = *this;

	QuaAudioPort *mp = new QuaAudioPort("asio", this, QUA_AUDIO_ASIO);
	if (dfltInput == nullptr) {
		dfltInput = mp;
	}
	if (dfltOutput == nullptr) {
		dfltOutput = mp;
	}
	ports.push_back(mp);
/*
	int32		na = 0;
	char		**devnm = QuaAudioManager::asio.DeviceNames(&na);

	int32			i, j;

	if (devnm) {
		for (i=0; i<na; i++) {
			QuaAudioPort *mp = new QuaAudioPort(devnm[i], this, QUA_AUDIO_ASIO);
			if (dfltInput == nullptr) {
				dfltInput = mp;
			}
			if (dfltOutput == nullptr) {
				dfltOutput = mp;
			}
			ports.AddItem(mp);
		}

		for (j=0; j<na; j++) {
			delete devnm[j];
		}
		delete devnm;
	}
*/
#endif

	status = STATUS_RUNNING;
	buffyThread = thread(&QuaAudioManager::buffaerator, this);
	writerThread = thread(&QuaAudioManager::writer, this);
}

QuaAudioManager::~QuaAudioManager()
{

	status = STATUS_DEAD;
	fprintf(stderr, "~QuaAudioManager: resuming buffer eater\n");
	resumeBuffaerator();
	fprintf(stderr, "~QuaAudioManager: resuming writer\n");
	resumeWriter();
	fprintf(stderr, "~QuaAudioManager: waiting on buffer eater\n");
	buffyThread.join();
	fprintf(stderr, "~QuaAudioManager: waiting on writer\n");
	writerThread.join();
	fprintf(stderr, "~QuaAudioManager: all done ... phew!\n");
}

QuaPort *
QuaAudioManager::findPortByName(const string name, int direction, int nports) {
	return ports.size() > 0 ? ports[0] : nullptr;
}

status_t
QuaAudioManager::StartAudio()
{
	status_t	err = B_OK;
	status = STATUS_RUNNING;
	resumeBuffaerator();
#ifdef QUA_V_AUDIO_ASIO
	err = asio.Start();
#endif
	return err;
}

status_t
QuaAudioManager::StopAudio()
{
	status = STATUS_SLEEPING;
	pauseBuffaerator();
	status_t	err = B_OK;
#ifdef QUA_V_AUDIO_ASIO
	err = asio.Stop();
#endif
	return err;
}

char *
QuaAudioManager::sampleFormatName(const long sf)
{
	switch(sf) {
	case ASIOSTInt16MSB: return "Int 16 MSB";
	case ASIOSTInt24MSB: return "Int 24 MSB";		// used for 20 bits as well
	case ASIOSTInt32MSB: return "Int 32 MSB";
	case ASIOSTFloat32MSB: return "Float 32 MSB";		// IEEE 754 32 bit float
	case ASIOSTFloat64MSB: return "Float 64 MSB";		// IEEE 754 64 bit double float

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can be more easily used with these
	case ASIOSTInt32MSB16: return "Int 32 MSB16";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32MSB18: return "Int 32 MSB18";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32MSB20: return "Int 32 MSB20";		// 32 bit data with 20 bit alignment
	case ASIOSTInt32MSB24: return "Int 32 MSB24";		// 32 bit data with 24 bit alignment
	
	case ASIOSTInt16LSB: return "Int 16 LSB";
	case ASIOSTInt24LSB: return "Int 24 LSB";		// used for 20 bits as well
	case ASIOSTInt32LSB: return "Int 32 LSB";
	case ASIOSTFloat32LSB: return "Float 32 LSB";		// IEEE 754 32 bit float, as found on Intel x86 architecture
	case ASIOSTFloat64LSB: return "Float 64 LSB"; 		// IEEE 754 64 bit double float, as found on Intel x86 architecture

	// these are used for 32 bit data buffer, with different alignment of the data inside
	// 32 bit PCI bus systems can more easily used with these
	case ASIOSTInt32LSB16: return "Int 32 LSB16";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32LSB18: return "Int 32 LSB18";		// 32 bit data with 18 bit alignment
	case ASIOSTInt32LSB20: return "Int 32 LSB20";		// 32 bit data with 20 bit alignment
	case ASIOSTInt32LSB24: return "Int 32 LSB24";		// 32 bit data with 24 bit alignment
	}
	return "Unknown format";
}

void
QuaAudioManager::resumeBuffaerator() {
	{
		std::lock_guard<std::mutex> lk(buffyMux);
//		ready = true;
	}
	buffyCV.notify_all();
}


void
QuaAudioManager::pauseBuffaerator() {
	status = STATUS_SLEEPING;
}

void
QuaAudioManager::resumeWriter() {
	{
		std::lock_guard<std::mutex> lk(writerMux);
		//		ready = true;
	}
	writerCV.notify_all();
}

void
QuaAudioManager::buffaerator()
{
	while (status != STATUS_DEAD) {
		if (samples.size() == 0 || status == STATUS_SLEEPING) {
			std::unique_lock<std::mutex> lk(buffyMux);
			buffyCV.wait(lk /*, []{return ready;}*/);
		} else {
			sampleLock.lock(); /*read*/
			for (short i=0; ((unsigned)i)<samples.size(); i++) {
				Sample	*sam = samples[i];
				sam->bufferLock.lock() /*read*/;
				status_t	err = sam->SynchronizeBuffers();
				if (err != B_NO_ERROR) {
					sam->uberQua->bridge.reportError("sample reader: file error on %s: %s", sam->sym->name, errorStr(err));
				}
				sam->bufferLock.unlock() /*read*/;
			}
			sampleLock.unlock() /*read*/;
		}

//		snooze(100000.0);
// 1 sample buffer = 2024 frames =~ 50ms
// ??? maybe we can be safely 100ms ahead
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void
QuaAudioManager::writer()
{
	while(status != STATUS_DEAD) {
// this semaphore should not be locked by the main thread
		recordInstanceLock.lock();
		if (recordInstances.size() == 0) {
			recordInstanceLock.unlock();
			std::unique_lock<std::mutex> lk(writerMux);
			writerCV.wait(lk /*, []{return ready;}*/);
		} else {
			for (short i=0; ((unsigned)i)<recordInstances.size(); i++) {
				SampleInstance	*si = recordInstances[i];
				if (si->sample.FlushRecordBuffers(false) != B_NO_ERROR) {
					recordInstances.erase(recordInstances.begin()+i);
					break;
				}
			}
			recordInstanceLock.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool
QuaAudioManager::Generate(size_t nFrames)
{
// for the moment, we will just generate regardless of whether stopped or not
	for (short i=0; ((unsigned)i)<channels.size(); i++) {
	// ! must check for SR change ... one day
		Channel	*chan = channels[i];
		int nf = chan->Generate(nFrames);
	}

	// editors originally generated their own stuff on a free channel .. that's necessary in the big picture
/*
	editors.Lock();
	for (short i=0; i<editors.CountItems(); i++) {
		SampleEditor	*edit = (SampleEditor *)editors.ItemAt(i);
		int nf = edit->Generate(qmo_temp,
								nFrames,
								samplesPerOutFrame);
	}
	editors.Unlock();
	*/
	return true;
}

QuaAudioIn *
QuaAudioManager::OpenInput(QuaAudioPort *p, short ch, short nch)
{
	QuaAudioIn	*con;
	con = InputConnectionForPort(p, ch);
	if (con == nullptr) {
//		con = new QuaAudioIn(uberQua, nch, p, ch);
//		if (con->Open(p) != B_OK) {
//			delete con;
//			return nullptr;
//		}
//		inputs.AddItem(con);
	}
	return con;
}

QuaAudioOut *
QuaAudioManager::OpenOutput(QuaAudioPort *p, short ch, short nch)
{
	QuaAudioOut	*con;
	con = OutputConnectionForPort(p, ch);
	if (con == nullptr) {
//		con = new QuaAudioOut(uberQua, nch, p, ch);
//		if (con->Open(p) != B_OK) {
//			delete con;
//			return nullptr;
//		}
//		outputs.AddItem(con);
	}
	return con;
}

status_t
QuaAudioManager::CloseInput(QuaAudioIn *c)
{
//	c->Close();
	return B_OK;
}

status_t
QuaAudioManager::CloseOutput(QuaAudioOut *c)
{
//	c->Close();
	return B_OK;
}

QuaAudioIn *
QuaAudioManager::InputConnectionForPort(QuaAudioPort *, short)
{
	return nullptr;
}

QuaAudioOut *
QuaAudioManager::OutputConnectionForPort(QuaAudioPort *, short)
{
	return nullptr;
}

status_t
QuaAudioManager::connect(Input *s)
{
	long err = B_OK;
	if (s->device == nullptr) {
		return B_OK;
	}
	s->src.audio.port = nullptr;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		if ((err=asio.Load()) != B_OK) {
			return err;
		}
		s->src.audio.port = asio.EnableInput(s->channel->uberQua, s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			if ((err=asio.Load()) != B_OK) {
				return err;
			}
			s->src.audio.xport = asio.EnableInput(s->channel->uberQua, s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	if (s->src.audio.port == nullptr) {
		s->enabled = false;
		return err;
	} else {
		s->enabled = true;
	}
	return err;
}


char *
QuaAudioManager::ErrorString(status_t err)
{
#ifdef QUA_V_AUDIO_ASIO
	return asio.ErrorString(err);
#else
	return nullptr;
#endif
}

status_t
QuaAudioManager::connect(Output *s)
{
	s->dst.audio.port = nullptr;
	long err = B_OK;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		if ((err=asio.Load()) != B_OK) {
			return err;
		}
		s->dst.audio.port = asio.EnableOutput(s->channel->uberQua, s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			if ((err=asio.Load()) != B_OK) {
				return err;
			}
			s->dst.audio.xport = asio.EnableOutput(s->channel->uberQua, s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	if (s->dst.audio.port == nullptr) {
		s->enabled = false;
		return err;
	}
	s->enabled = true;
	return err;
}

status_t
QuaAudioManager::disconnect(Input *s)
{
	long err = B_OK;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		asio.DisableInput(s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			asio.DisableInput(s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	s->src.audio.xport = s->src.audio.port = nullptr;
	s->enabled = false;
	return err;
}

status_t
QuaAudioManager::disconnect(Output *s)
{
	long err = B_OK;
	if (s->device->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
		asio.DisableOutput(s->deviceChannel);
#endif
	}
	if(s->xDevice) {
		if (s->xDevice->deviceSubType == QUA_AUDIO_ASIO) {
#ifdef QUA_V_AUDIO_ASIO
			asio.DisableOutput(s->xChannel);
#endif
		}
	}
#ifdef QUA_V_AUDIO_ASIO
	err = asio.AllocateBuffers(); // which will start and stop me if I'm still going
#endif
	s->dst.audio.xport = s->dst.audio.port = nullptr;
	s->enabled = false;
	return err;
}

void
QuaAudioManager::addSample(Sample *s)
{
	sampleLock.lock();
	samples.push_back(s);
	sampleLock.unlock();
	resumeBuffaerator();
}

/*
 * remove a sample from the buffer scrounging list
 * 
 */
void
QuaAudioManager::removeSample(Sample *s)
{
	sampleLock.lock();
	auto it = std::find(samples.begin(), samples.end(), s);
	if (it != samples.end()) {
		samples.erase(it);
	}
	sampleLock.unlock();
}

void
QuaAudioManager::startInstance(Instance *i)
{
	fprintf(stderr, "start audio instance %s\n", i->sym->name.c_str());
	if (i->channel == nullptr)
		return;
	Channel	*c = i->channel;
	channelLock.lock();
	auto it = find(c->audioInstances.begin(), c->audioInstances.end(), i);
	if (it == c->audioInstances.end())
		c->audioInstances.push_back(i);
	auto ct = find(channels.begin(), channels.end(), c);
	if (ct == channels.end()) {
		channels.push_back(c);
	}
	channelLock.unlock();
}

void
QuaAudioManager::stopInstance(Instance *i)
{
	if (i->channel == nullptr)
		return;
	Channel	*c = i->channel;
	channelLock.lock();
	auto it = find(c->audioInstances.begin(), c->audioInstances.end(), i);
	if (it != c->audioInstances.end()) {
		c->audioInstances.erase(it);
	}
	if (c->audioInstances.size() == 0 && !(c->hasAudio&AUDIO_HAS_PLAYER)) {
		auto ct = find(channels.begin(), channels.end(), c);
		if (ct != channels.end()) {
			channels.erase(ct);
		}
	}
    channelLock.unlock();
}

void
QuaAudioManager::startChannel(Channel *c)
{
	channelLock.lock();
	auto ct = find(channels.begin(), channels.end(), c);
	if (ct == channels.end()) {
		channels.push_back(c);
	}
    channelLock.unlock();
}

status_t
QuaAudioManager::startRecording(SampleInstance *ri)
{
	status_t	err = B_NO_ERROR;
	if (ri == nullptr || ri->channel == nullptr) {
		return B_ERROR;
	}

	recordInstanceLock.lock();
	for (short i=0; ((unsigned)i)<recordInstances.size(); i++) {
		if (&ri->sample == &((SampleInstance*)recordInstances[i])->sample) {
			recordInstanceLock.unlock();
			return B_ERROR;
		}
	}
	

	SampleTake *recdTake = nullptr;
	recdTake = ri->sample.AddRecordTake(
		SampleFile::WAVE_TYPE,
		ri->channel->nAudioIns,
		4, 44100.0);
	if (recdTake == nullptr) {
		ri->sample.uberQua->bridge.reportError("Output file not initialised: %s", errorStr(err));
	}
	else {
		ri->sample.recordTake->file->SeekToFrame(0);
		ri->sample.status = STATUS_RECORDING;

		recordInstances.push_back(ri);

		if (recordInstances.size() >= 1) {
			resumeWriter();
		}
	}

	recordInstanceLock.unlock();

	return B_NO_ERROR;
}

void
QuaAudioManager::stopRecording(SampleInstance *ri)
{
	recordInstanceLock.lock();
	auto it = find(recordInstances.begin(), recordInstances.end(), ri);	
	if (it != recordInstances.end()) {
		recordInstances.erase(it);
	}
	if (recordInstances.size() == 0) {
		;
	}

	// no more flushing from main writer
	ri->sample.FlushRecordBuffers(true);
	ri->endFrame = ri->sample.recordTake->file->nFrames;
	ri->sample.status = STATUS_RUNNING;
	SampleTake		*newest = ri->sample.recordTake;
	ri->sample.recordTake = nullptr;
	if (ri->sample.sampleClip(0)->media == nullptr) {
		ri->sample.SelectTake(newest, true);
	}
	fprintf(stderr, "Sampler: stopped recording\n");
	recordInstanceLock.unlock();
}


/*
bool
destinationIndex(KeyIndex *, short nc)
{
	return true;
}

bool
sourceIndex(KeyIndex *, short nc)
{
	return true;
}


bool
DestinationChannelIndex(KeyIndex *, QuaAudioPort *, short nc)
{
	return true;
}


bool
SourceChannelIndex(KeyIndex *, QuaAudioPort *, short nc)
{
	return true;
}

*/
//		media_node	liveOne;
//		err=r->GetNodeFor(liveNodes[i].node.node, &liveOne);
//		for (short j=0; j<nIns; j++) {
//			fprintf(stdout, "\tnode %d <= (%d %d)\n",
//				liveOne.node,
//				inCon[j].source.id,
//				inCon[j].source.port);
//		}
//		r->ReleaseNode(liveOne);
//		media_node	liveOne;
//		err=r->GetNodeFor(liveNodes[i].node.node, &liveOne);
//		if (err != B_NO_ERROR)
//			fprintf(stderr, "get node for error, %s\n", ErrorStr(err));
//		for (short j=0; j<nOuts; j++) {
//			fprintf(stdout, "\tnode %d => (%d %d)\n",
//				liveOne.node,
//				outCon[j].destination.id,
//				outCon[j].destination.port
//				);
//		}
//		r->ReleaseNode(liveOne);

//						if (( (err=r->GetConnectedInputsFor(
//									liveOne, inputs,
//									15, &nConIns))< B_NO_ERROR)
//							||(err=r->GetFreeInputsFor(
//									liveOne, inputs+nConIns,
//									15-nConIns, &nFreeIns)< B_NO_ERROR)
//							||(err=r->GetConnectedOutputsFor(
//									liveOne, outputs,
//									15, &nConOuts)< B_NO_ERROR)
//							||(err=r->GetFreeOutputsFor(
//									liveOne, outputs+nConOuts,
//									15-nConIns, &nFreeOuts)<B_NO_ERROR)) {
//							fprintf(stderr, "%s: get input/output connection error, %s\n", liveInfo.name, ErrorStr(err));
//						} else {
//							for (short i=0; i<nConIns+nFreeIns; i++) {
//								if (inputs[i].format.IsAudio()) {
//									isAudio=true;
//								}
//							}					
//							for (short i=0; i<nConOuts+nFreeOuts; i++) {
//								if (outputs[i].format.IsAudio()) {
//									isAudio=true;
//								}
//							}
//						}					
//	
//	//					nAttribs=r->GetNodeAttributesFor(
//	//								liveOne, attribs, 40);
//	//					if (nAttribs < B_NO_ERROR) {
//	//						fprintf(stderr, "%s: get attribute error, %s\n", liveInfo.name, ErrorStr(nAttribs));
//	//					} else if (nAttribs == 0) {
//	//						fprintf(stderr, "%s: no attributes to speak of\n", liveInfo.name);
//	//					} else {
//	//						for (short i=0; i<nAttribs; i++) {
//	//							fprintf(stderr, "%s: at %d %x %Lx\n",
//	//								liveInfo.name, attribs[i].what, attribs[i].flags, attribs[i].data);
//	//						}
//	//					}
//						if (isAudio) {
//						} else {
//							fprintf(stderr, "%s: probably not an audio node\n", liveInfo.name);
//							r->ReleaseNode(liveOne); // not released by port
//						}
#endif