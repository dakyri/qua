#include "qua_version.h"

#include "tinyxml2.h"

#include "QuaTime.h"
#include "Sample.h"
#include "Clip.h"

SampleTake *
TypedValue::SampleTakeValue()
{
	return (type == S_TAKE && val.take && val.take->type == Take::SAMPLE)?
		(SampleTake*)val.take: nullptr;
}

StreamTake *
TypedValue::StreamTakeValue()
{
	return (type == S_TAKE && val.take && val.take->type == Take::STREAM)?
		(StreamTake*)val.take: nullptr;
}


Take::Take(std::string nm, StabEnt *ctxt, short t)
{
	sym = DefineSymbol(nm, TypedValue::S_TAKE, 0,
					this, ctxt,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT);
	type = t;
	status = STATUS_UNLOADED;
}

Take::~Take()
{
}

StreamTake::StreamTake(std::string nm, StabEnt *ctxt, Time &dur):
	Take(nm, ctxt, Take::STREAM)
{
	duration = dur;
	status = STATUS_LOADED;
}

StreamTake::~StreamTake()
{
	stream.ClearStream();
}

status_t
StreamTake::SaveSnapshot(FILE *fp)
{
	if (sym && sym->name.size()) {
		fprintf(fp, "<take type=\"stream\" name=\"%s\" duration=\"%s\">\n", sym->name.c_str(), duration.StringValue());
		stream.SaveSnapshot(fp);
		fprintf(fp, "</take>\n");
	} else {
		return B_ERROR;
	}
	return B_OK;
}


status_t
StreamTake::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *positionVal = element->Attribute("duration");
	const char *nameAttrVal = element->Attribute("name");
	Time	duration;
	bool	hasNameAttr = false;
	bool	hasDurationAttr = false;


	if (positionVal != nullptr) {
			hasDurationAttr = true;
			duration.Set(positionVal);
	} else if (nameAttrVal != nullptr) {
		hasNameAttr = true;
	}


	std::string namestr = element->Value();

	if (namestr == "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "take") {
		LoadSnapshotChildren(element);
	} else if (namestr == "stream") {
		stream.ClearStream();
		stream.LoadSnapshotElement(element);
	}
	return B_OK;
}

status_t
StreamTake::LoadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (LoadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
		childElement = childElement->NextSiblingElement();
	}
	return B_OK;
}

SampleTake::SampleTake(Sample *s, std::string nm, std::string pathnm):
	Take(nm, s->sym, Take::SAMPLE)
{
	sample = s;
	status = STATUS_UNLOADED;
#ifdef QUA_V_SAMPLE_DRAW
#if QUA_V_SAMPLE_DRAW=='p'
	peakBuffer = nullptr;
	peakFile = nullptr;
	peakCacheCreationThread = nullptr;
#else
	peakBuffer = nullptr;
	rawBuffer = nullptr;
	peakBufferStartFrame = -1;
	peakBufferNFrames = 0;
#endif
#endif
	file = nullptr;
	SetSampleFile(pathnm, nullptr);
}

SampleTake::~SampleTake()
{
#if QUA_V_SAMPLE_DRAW=='p'
	ClosePeakCache();
#endif
	delete file;
}

status_t
SampleTake::SetSampleFile(const std::string pathnm, SampleFile *newFile)
{
	status_t	err=B_OK;
	status = STATUS_UNLOADED;
	if (pathnm.size() == 0) {
		return B_OK;
	}
	path = pathnm;
	if (file != nullptr) {
		delete file;
	}
	if (newFile != nullptr) {
		file = newFile;
	} else {
		file = new SampleFile();
		if ((err=file->SetTo(pathnm.c_str())) != B_OK) {
			internalError("AddSampleTake::can't open %s: %s", pathnm, err == B_ERROR? "format error":errorStr(err));
			status = STATUS_UNLOADED;
		}
	}
	return B_OK;
}


status_t
SampleTake::LoadSnapshotElement(tinyxml2::XMLElement *element)
{
	const char *positionVal = element->Attribute("duration");
	const char *nameAttrVal = element->Attribute("name");
	const char *oathVal = element->Attribute("path");
	Time	duration;
	bool	hasNameAttr = false;
	bool	hasDurationAttr = false;


	if (positionVal != nullptr) {
		hasDurationAttr = true;
		duration.Set(positionVal);
	} else if (nameAttrVal != nullptr) {
		hasNameAttr = true;
	} else if (oathVal != nullptr) {
		hasNameAttr = true;
	}


	std::string namestr = element->Value();
	if (namestr, "snapshot") {
		LoadSnapshotChildren(element);
	} else if (namestr == "take") {
		LoadSnapshotChildren(element);
	}
	if (oathVal) {
		SetSampleFile(oathVal, nullptr);
	}
	return B_OK;
}

status_t
SampleTake::LoadSnapshotChildren(tinyxml2::XMLElement *element)
{
	tinyxml2::XMLElement *childElement = element->FirstChildElement();
	while (childElement != nullptr) {
		if (LoadSnapshotElement(childElement) == B_ERROR) {
			return B_ERROR;
		}
		childElement = childElement->NextSiblingElement();
	}
	return B_OK;
}


status_t
SampleTake::SaveSnapshot(FILE *fp)
{
	if (sym && sym->name.size()) {
		fprintf(fp, "<take type=\"sample\" ");
		const char	*pnm = path.c_str();
		if (pnm != nullptr && *pnm != '\0') {
			fprintf(fp, "path=\"%s\" ", pnm);
		}
		fprintf(fp, "name=\"%s\"/>\n", sym->name.c_str());
	} else {
		return B_ERROR;
	}
	return B_OK;
}


Time &
SampleTake::Duration()
{
	static	Time	dur_t;
	dur_t.Set(1,0,0,&Metric::std);
	if (file != nullptr) {
		dur_t.Set(file->nFrames, &Metric::sampleRate);
	}
	return dur_t;
}

long
SampleTake::NextChunk(long chunk, long sf, long ef)
{
	chunk++;
	if (chunk > Sample::ChunkForFrame(file, ef)) {
		chunk = Sample::ChunkForFrame(file, sf);
	}
	return chunk;
}

long
SampleTake::PrevChunk(long chunk, long sf, long ef)
{
	chunk--;
	if (chunk < Sample::ChunkForFrame(file, sf)) {
		chunk = Sample::ChunkForFrame(file, ef);
	}
	return chunk;
}


#if QUA_V_SAMPLE_DRAW=='p'
int32
SampleTake::PeakeratorWrapper(void *data)
{
	int ret = ((SampleTake *)data)->Peakerator();
	return ret;
}

int32
SampleTake::Peakerator()
{
	if (file == nullptr || status != STATUS_LOADED) {
		peakCacheCreationThread = nullptr;
		return B_ERROR;
	}

	BPath	peakCacheFile;
	FILE	*newPeakCacheFp;
	peakCacheFile = path;
	peakCacheFile.SetExtension("qpk");
	if ((newPeakCacheFp = fopen(peakCacheFile.Path(), "w")) == nullptr) {
		peakCacheCreationThread = nullptr;
		return B_ERROR;
	}

	qpk_hdr_1	hdr;
	hdr.magic = 'qpk1';
	hdr.nChannels = file->nChannels;
	hdr.chunkLength = PEAK_FILE_DFLT_CHUNK_LENGTH;
	hdr.unusedByte3_2 = 0;
	hdr.unusedByte3_3 = 0;
	hdr.unusedByte3_4 = 0;
	hdr.unused4 = 0;
	hdr.unused5 = 0;
	hdr.unused6 = 0;
	hdr.unused7 = 0;
	hdr.unused8 = 0;

	if (fwrite(&hdr, sizeof(hdr), 1, newPeakCacheFp) != 1) {
		peakCacheCreationThread = nullptr;
		return B_ERROR;
	}

	float	*hiVal = new float[hdr.nChannels];
	float	*loVal = new float[hdr.nChannels];
	off_t	soffset;
	long	frame = 0;
	while (frame < file->nFrames) {
		short	chan;
		char	rawbuf[PEAK_FILE_DFLT_CHUNK_LENGTH*4*4];
		float	sambuf[PEAK_FILE_DFLT_CHUNK_LENGTH*4];
		for (chan=0; chan<hdr.nChannels; chan++) {
			hiVal[chan] = 0;
			loVal[chan] = 0;
		}
//		file->SeekToFrame(soffset);
		long	nf = PEAK_FILE_DFLT_CHUNK_LENGTH;
		long	nb = file->Read(rawbuf, nf*file->nChannels*file->sampleSize);
		if (nb <= 0) {
			break;
		}
		nf = nb / (file->nChannels*file->sampleSize);
		long nr = file->NormalizeInputCpy(sambuf, rawbuf, nf);
		if (nr < 0) {

		}
		long	i=0;
		long	nsamp = hdr.nChannels*PEAK_FILE_DFLT_CHUNK_LENGTH;
		for (i=0; i<nsamp; i+=hdr.nChannels) {
			for (chan=0; chan<hdr.nChannels; chan++) {
				if (sambuf[i+chan] < loVal[chan]) {
					loVal[chan] = sambuf[i+chan];
				}
				if (sambuf[i+chan] > hiVal[chan]) {
					hiVal[chan] = sambuf[i+chan];
				}
			}
		}
		if (fwrite(hiVal, hdr.nChannels*sizeof(float), 1, newPeakCacheFp) != 1) {
			;
		}
		if (fwrite(loVal, hdr.nChannels*sizeof(float), 1, newPeakCacheFp) != 1) {
			;
		}
		frame += nf;
	}

	fclose(newPeakCacheFp);
	peakCacheCreationThread = nullptr;
	return B_NO_ERROR;
}

// note that the following two calls shouldn't be made from parallel threads
bool
SampleTake::CreatePeakFile()
{
	if (peakCacheCreationThread != nullptr) { // already building cache
		return false;
	}
	if (file == nullptr) { // no sample data ... oops
		return false;
	}
	peakCacheCreationThread = spawn_thread(PeakeratorWrapper, "peakerator",
								B_DISPLAY_PRIORITY, this);
	return true;
}

bool
SampleTake::HasPeakData()
{
	if (peakFile != nullptr) {
		return true;
	}
	if (OpenPeakCache()) {
		return true;
	}
	CreatePeakFile();
	return false;
}

bool
SampleTake::OpenPeakCache()
{
	if (peakCacheCreationThread != nullptr) { // we're building it just at the moment
		return false;
	}
	if (peakFile != nullptr || peakBuffer != nullptr) { // cache already open 
		return false;
	}
	BPath	peakCacheFile;
	peakCacheFile = path;
	peakCacheFile.SetExtension("qpk");
	peakFile = fopen(peakCacheFile.Path(), "r");

	if (peakFile != nullptr) {
		struct qpk_hdr_1 hdr;
		if (file->nChannels <= 0 || fread(&hdr, sizeof(hdr), 1, peakFile) != 1) {
			fclose(peakFile);
			peakFile = nullptr;
			return false;
		}
		if (hdr.magic != 'qpk1') {
			fclose(peakFile);
			peakFile = nullptr;
			return false;
		}
		peakCacheChunkLength = hdr.chunkLength;
		if (hdr.nChannels != file->nChannels) {
			fclose(peakFile);
			peakFile = nullptr;
			return false;
		}
		peakBuffer = new float[file->nChannels*2*PEAK_BUFFER_CHUNKS];
		nPeakBufferChunks = 0;
		peakBufferStartChunk = -1;
		return true;
	}
	return false;
}

bool
SampleTake::ClosePeakCache()
{
	if (peakFile != nullptr) {
		fclose(peakFile);
		peakFile = nullptr;
	}
	if (peakBuffer != nullptr) {
		delete peakBuffer;
		peakBuffer = nullptr;
	}
	return true;
}
#endif
//FILE *sstderr = fopen("debug", "w");

bool
SampleTake::PeakData(long startF, long endF, short channel, float &hiVal, float &loVal)
{
#if QUA_V_SAMPLE_DRAW=='p'
	hiVal = 0;
	loVal = 0;
	if (peakFile == nullptr || peakBuffer == nullptr || channel < 0 ||
				file == nullptr || file->nChannels <= 0 || channel >= file->nChannels) {
		return false;
	}
	long chunkNeeded;
	long frame = startF;
	while (frame < endF) {
		chunkNeeded = (frame/peakCacheChunkLength);
		if (peakBufferStartChunk < 0 || nPeakBufferChunks < 0 ||
					chunkNeeded < peakBufferStartChunk ||
					chunkNeeded >= peakBufferStartChunk+nPeakBufferChunks) {
			long chunkGroup = chunkNeeded / PEAK_BUFFER_CHUNKS;
			long chunk_offset = chunkGroup * (
									PEAK_BUFFER_CHUNKS * file->nChannels*2*sizeof(float))
									+ sizeof(qpk_hdr_1);

			fseek(peakFile, chunk_offset, SEEK_SET);
			nPeakBufferChunks = fread(peakBuffer,
										file->nChannels*2*sizeof(float),
										PEAK_BUFFER_CHUNKS,
										peakFile);
			if (nPeakBufferChunks <= 0) {
				return false;
			}
			peakBufferStartChunk = chunkGroup * PEAK_BUFFER_CHUNKS;
		}
		long cgo = (chunkNeeded - peakBufferStartChunk)*2*file->nChannels+2*channel;
		if (cgo < 0 || cgo >= nPeakBufferChunks*2*file->nChannels) {
			return false;
		}
		if (peakBuffer[cgo] > hiVal) {
			hiVal = peakBuffer[cgo];
		}
		if (peakBuffer[cgo] < loVal) {
			loVal = peakBuffer[cgo];
		}

		frame += peakCacheChunkLength;
	}

	return false;
#else
// baseline peak ... slow, ugly, and safe
// i'm sure we can do much better than this 
	hiVal = 0;
	loVal = 0;
	if (channel < 0 || file == nullptr || file->nChannels <= 0 || channel >= file->nChannels) {
		return false;
	}
	if (startF > file->nFrames) {
		return true;
	}
	if (peakBuffer == nullptr) {
		peakBuffer = new float[N_PEAK_BUFFER_FRAMES*file->nChannels];
	}
	if (rawBuffer == nullptr) {
		rawBuffer = new char[N_PEAK_BUFFER_FRAMES*file->nChannels*4];
	}
	long chunkNeeded;
	long frame = startF;
	while (frame < endF) {
		if (frame >= file->nFrames) {
			break;
		}
		if (peakBufferStartFrame < 0 ||
				frame < peakBufferStartFrame ||
				frame >= peakBufferStartFrame+peakBufferNFrames) {
			long	frame_chunk = frame/N_PEAK_BUFFER_FRAMES;
			long	chunk_start_frame = frame_chunk*N_PEAK_BUFFER_FRAMES;
			file->SeekToFrame(chunk_start_frame);
			long nr = file->Read(
							rawBuffer,
							N_PEAK_BUFFER_FRAMES*file->nChannels*file->sampleSize);
			if (nr <= 0) {
				return false;
			}
			peakBufferNFrames = nr/(file->nChannels*file->sampleSize);
			fprintf(stderr, "nread %d nframes %d at frame %d\n", nr, peakBufferNFrames, frame);
			nr = file->NormalizeInputCpy(peakBuffer, rawBuffer, nr);
			if (nr < 0) {
				// todo xxxx an error
			}
			peakBufferStartFrame = chunk_start_frame;
		}
		long cgo = (frame%N_PEAK_BUFFER_FRAMES)*file->nChannels + channel;
		if (peakBuffer[cgo] > hiVal) {
			hiVal = peakBuffer[cgo];
		}
		if (peakBuffer[cgo] < loVal) {
			loVal = peakBuffer[cgo];
		}
//		if (loVal < -1 || frame <0) {
//			fprintf(sstderr, "lov %g frame %d cgo %d\n", loVal, frame, cgo);
//		}

		frame ++;
	}
	return true;
#endif
}

Clip::Clip(std::string nm, StabEnt *ctxt)
{
	media = nullptr;
	sym = DefineSymbol(nm, TypedValue::S_CLIP, 0,
					this, ctxt,
					TypedValue::REF_VALUE, false, false, StabEnt::DISPLAY_NOT);
}

bool	
Clip::Set(Take *m, Time&s, Time &e)
{
	media = m;
	start = s;
	duration = e;
	return true;
}

status_t
Clip::SaveSnapshot(FILE *fp)
{
	if (sym && sym->name.size()) {
		fprintf(fp, "<clip name=\"%s\"", sym->name.c_str());
		if (media && media->sym && media->sym->name.size()) {
			fprintf(fp, " take=\"%s\"", media->sym->name.c_str());
		}
		fprintf(fp, " start=\"%s\"", start.StringValue());
		fprintf(fp, " duration=\"%s\"", duration.StringValue());
		fprintf(fp, "/>\n");
	}
	return B_OK;
}
