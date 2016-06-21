#include "qua_version.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>
#endif

#include "SampleFile.h"
#include "StdDefs.h"

#define NORMALIZE_SHORT	32767.0
#define NORMALIZE_BYTE	127.0


void 	swab(char *buf, long len);
uint32	bendian32(uint32);
uint16	bendian16(uint16);
uint32	big_endian_to_native32(uint32);
uint16	big_endian_to_native16(uint16);
uint32	lendian32(uint32);
uint16	lendian16(uint16);

#include <math.h>

void double_to_ieee_extended(double num, unsigned char* bytes);
double ieee_extended_to_double(unsigned char* bytes);
#define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)
#define UnsignedToFloat(u)         (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)



static void swab(char *buf, long len)
{
	for (short i=0; i<len/2; i++) {
		char	t = buf[i];
		buf[i] = buf[len-1-i];
		buf[len-1-i] = t;
	}
}

inline uint32	big_endian_to_native32(uint32 x)
{
#ifdef GLOUB_LITTLE_ENDIAN
	swab((char *)&x, 4);
#endif
	return x;
}

inline uint16	big_endian_to_native16(uint16 x)
{
#ifdef GLOUB_LITTLE_ENDIAN
	swab((char *)&x, 2);
#endif
	return x;
}

inline uint32	bendian32(uint32 x)
{
#ifdef GLOUB_LITTLE_ENDIAN
	swab((char *)&x, 4);
#endif
	return x;
}

inline uint16	bendian16(uint16 x)
{
#ifdef GLOUB_LITTLE_ENDIAN
	swab((char *)&x, 2);
#endif
	return x;
}

inline uint32	lendian32(uint32 x)
{
#ifndef GLOUB_LITTLE_ENDIAN
	swab((char *)&x, 4);
#endif
	return x;
}

inline uint16	lendian16(uint16 x)
{
#ifndef GLOUB_LITTLE_ENDIAN
	swab((char *)&x, 2);
#endif
	return x;
}

bool	debug_sample=false;

SampleFile::SampleFile(short typ, short nc, short dsze, float sr, uint32 mode)
{
	fileType = typ;
	fd = -1;
	switch(typ) {
	case RAW_TYPE:
		break;
	default:
	case NO_TYPE:
		break;
	}
	SetFormat(nc, dsze, sr);

	openMode = mode;
	sampleType = 0;
	sampleDataStart = 0;
	nFrames = 0;
}


SampleFile::SampleFile(char *f, uint32 mode)
{
	SetFormat(2,4,44100.0);
	openMode = mode;
	sampleType = 0;
	sampleDataStart = 0;
	nFrames = 0;
	fd = -1;

	SetTo(f, mode);
}
	
void
SampleFile::SetFormat(short nchan, short dsize, long sr)
{
	nChannels = nchan;
	sampleSize = dsize;
	sampleRate = sr;
}

SampleFile::~SampleFile()
{
	if ((openMode & 3) == O_WRONLY) {
		Finalize();
	}
	if (fd >= 0) {
		close(fd);
	}
}

status_t
SampleFile::Finalize()
{
	status_t	err;
	char		*type;

//		if ((err=info.SetPreferredApp(my_id)) != B_NO_ERROR)
//			reportError("blah, blah, and i can't set the preferred app");
	nFrames = (Position()-sampleDataStart)/(sampleSize*nChannels);
	switch (fileType) {
	case SND_TYPE:
		type = "audio/snd";
		if ((err=WriteSndHeader()) != B_NO_ERROR) {
			lastError = ("No can write snd header: how unfortunate");
			return err;
		}
		break;
	case WAVE_TYPE: {
		type = "audio/x-wav";
		if ((err=WriteWaveHeader()) != B_NO_ERROR) {
			lastError = ("No can write wave header: how unfortunate");
			return err;
		}
		break;
	}
	case AIFF_TYPE:
		type = "audio/x-aiff";
		if ((err=WriteAiffHeader()) != B_NO_ERROR) {
			lastError = ("No can write aiff header: how unfortunate");
			return err;
		}
		break;
	case NO_TYPE:
	case RAW_TYPE:
	default:
		type = "audio/raw";
	}
	SetSize(sampleDataStart + (nFrames*sampleSize*nChannels));
	return B_NO_ERROR;
}


status_t
SampleFile::WriteSndHeader()
{
	status_t		err;
	if ((err=Seek(0,SEEK_SET)) >= 0) {
		return B_OK;
	}
	return GetLastError();
}

status_t
SampleFile::WriteAiffHeader()
{
	FormAIFFHeader			head;
	FormChunkHeader			chunkHead;
	CommonChunk				commonChunk;
	SoundDataChunk			dataChunk;
	
	status_t		err;
	if ((err=Seek(0,SEEK_SET)) < 0) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	head.chunkType = bendian32('FORM');
	head.formType = bendian32('AIFF');
	head.chunkLen = bendian32(
						sampleDataStart + (nFrames*sampleSize*nChannels));
	if (Write(&head, sizeof(head)) != sizeof(head)) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	
	chunkHead.ckID = bendian32(CommonID);
	chunkHead.ckSize = bendian32(sizeof(CommonChunk));
	if (Write(&chunkHead, sizeof(chunkHead)) != sizeof(chunkHead)) {	// format chunk
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	commonChunk.numChannels = bendian16(nChannels);
	*((uint32*)&commonChunk.numSampleFrames1) = bendian32(nFrames); 
    commonChunk.sampleSize = bendian16(8*sampleSize); 
    double_to_ieee_extended(sampleRate, commonChunk.sampleRate); 
	if (Write(&commonChunk, sizeof(commonChunk)) != sizeof(commonChunk)) {
				// format chunk, should be 18 bytes!
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}

	chunkHead.ckID = bendian32(SoundDataID);
	chunkHead.ckSize = bendian32(
			nFrames * sampleSize * nChannels + sizeof(dataChunk));
	if (Write(&chunkHead, sizeof(chunkHead)) != sizeof(chunkHead)) {	// format chunk
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
    dataChunk.blockSize = bendian32(0); 
    dataChunk.offset = bendian32(0); 
	if (Write(&dataChunk, sizeof(dataChunk)) != sizeof(dataChunk)) {	// format chunk
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	sampleDataStart = Position();

	if (debug_sample)	
		fprintf(stderr, "aiff file header written: %d chans, %d size, %d sr, %d frames %d pos\n",
				nChannels, sampleSize,
				sampleRate, nFrames, sampleDataStart);

	return B_NO_ERROR;
}

status_t
SampleFile::WriteWaveHeader()
{
	RiffChunkHeader			head;
	RiffWaveCommonHeader	commonHead;
	RiffWavePCMHeader		unCommonHead;
	uint32					type;
	status_t				err;
	
	if ((err=Seek(0,SEEK_SET)) < 0) {
		return GetLastError();
	}
	head.chunkType = bendian32('RIFF');
	head.chunkLen = lendian32(
				sampleDataStart + (nFrames*sampleSize*nChannels));
	if (Write(&head, sizeof(head)) != sizeof(head)) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	
	type = bendian32('WAVE');
	if (Write(&type, sizeof(type)) != sizeof(type)) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	head.chunkType = bendian32('fmt ');
	head.chunkLen = lendian32(16);
	if (Write(&head, sizeof(head)) != sizeof(head)) {	// format chunk
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	
	commonHead.format = lendian16(1);
	commonHead.channels = lendian16(nChannels);
	commonHead.sampleRate = lendian32(sampleRate);
	commonHead.byteRate = lendian32(sampleRate / 2);
	commonHead.blockAlign = lendian16(0);

	if (Write(&commonHead, 14) != 14) { // sizeof(commonHead)) != sizeof(commonHead)) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	
	unCommonHead.bitsPerSample = lendian16(8 * sampleSize);
	if (Write(&unCommonHead, 2) != 2) { //sizeof(unCommonHead)) != sizeof(unCommonHead)) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}

	head.chunkType = bendian32('data');
	head.chunkLen = lendian32(nFrames*sampleSize*nChannels);
	if (Write(&head, sizeof(head)) != sizeof(head)) {
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	
	sampleDataStart = Position();

	if (debug_sample)
		fprintf(stderr, "wave file header written: %d chans, %d size, %d sr, %d frames %d pos\n",
				nChannels, sampleSize,
				sampleRate, nFrames, sampleDataStart);

	return B_NO_ERROR;
}

status_t
SampleFile::TryAiffFile()
{
	FormAIFFHeader			head;
	FormChunkHeader			chunkHead;
	CommonChunk				commonChunk;
	SoundDataChunk			dataChunk;
	
	if (Seek(0,SEEK_SET) < 0) {
		if (debug_sample) fprintf(stderr, "SampleFile::TryAiffFile: rewind seek error...\n");
		return FILE_ERROR;
	}
	if (Read(&head, sizeof(head)) != sizeof(head)) {
		return NOT_ERROR; // maybe it's a small raw file. don't crash, just try elsewhere
	}
	// all data in aif is big endian
	if (big_endian_to_native32(head.chunkType) != 'FORM' || big_endian_to_native32(head.formType) != 'AIFF') {
		return NOT_ERROR;
	}
	for (;;) {
		if (debug_sample) fprintf(stderr, "SampleFile::TryAiffFile: reading chunk header @%d...\n", Position());
		if (Read(&chunkHead, sizeof(chunkHead)) != sizeof(chunkHead)) {
			if (debug_sample) fprintf(stderr, "SampleFile::TryAiffFile: read chunk header error...\n");
			return FILE_ERROR;
		}
		off_t cs = Position();
		if (debug_sample)
			fprintf(stderr, "chunk at %d: %s %d\n", cs, uintstr(chunkHead.ckID).c_str(), big_endian_to_native32(chunkHead.ckSize));
		if (big_endian_to_native32(chunkHead.ckID) == CommonID) {
			if (debug_sample) fprintf(stderr, "SampleFile::TryAiffFile: reading common chunk...\n");
			if (Read(&commonChunk, 18) != 18) {
				return FILE_ERROR;
			}
		} else if (big_endian_to_native32(chunkHead.ckID) == SoundDataID) {
			if (debug_sample) fprintf(stderr, "SampleFile::TryAiffFile: reading data chunk...\n");
			if (Read(&dataChunk, sizeof(dataChunk)) != sizeof(dataChunk)) {
				return FILE_ERROR;
			}
			break;
		}
		Seek(cs+big_endian_to_native32(chunkHead.ckSize), SEEK_SET);
	}

	nChannels = big_endian_to_native16(commonChunk.numChannels);
	sampleRate = ieee_extended_to_double(commonChunk.sampleRate);
	fileType = AIFF_TYPE;
	sampleSize = big_endian_to_native16(commonChunk.sampleSize)/8;
	nFrames = *((uint32*)&commonChunk.numSampleFrames1);
	nFrames = big_endian_to_native32(nFrames);
	sampleDataStart = Position() + big_endian_to_native32(dataChunk.offset);
	
	if (debug_sample)
		fprintf(stderr, "aiff file %d chans, %d size (%d), %d sr (%d), %d frames %d pos\n",
				nChannels, sampleSize,(char*)&commonChunk.sampleSize-(char*)&commonChunk,
				sampleRate, (char*)&commonChunk.sampleRate-(char*)&commonChunk, nFrames, sampleDataStart);
	return HEADER_OK;
}

status_t
SampleFile::TrySndFile()
{
	if (Seek(0,SEEK_SET) < 0) {
		return FILE_ERROR;
	}
	return OBSCURE_ERROR;
}


status_t
SampleFile::TryRawFile()
{
	if (Seek(0,SEEK_SET) < 0) {
		return FILE_ERROR;
	}
	nChannels = 2;
	sampleSize = 2;
	GetSize(&nFrames); nFrames /= 4;
	fileType = RAW_TYPE;
	sampleType = 0;
	sampleRate = 44100;
	sampleDataStart = 0;
	
	if (debug_sample)
		fprintf(stderr, "raw file %d chans, %d size, %d sr, %d frames %d pos\n", nChannels, sampleSize,
				sampleRate, nFrames, sampleDataStart);

	return HEADER_OK;
}

status_t
SampleFile::TryWaveFile()
{
	RiffChunkHeader			head;
	RiffWaveCommonHeader	commonHead;
	RiffWavePCMHeader		unCommonHead;
	size_t					nRead;
	uint32					type;
	
	if (Seek(0,SEEK_SET) < 0) {
		return FILE_ERROR;
	}
	if (Read(&head, sizeof(head)) != sizeof(head)) {
		return NOT_ERROR;
	}
	if (head.chunkType != bendian32('RIFF')) {
		return NOT_ERROR;
	}
	if (Read(&type, sizeof(type)) != sizeof(type)) {
		return FILE_ERROR;
	}
	if (type != bendian32('WAVE')) {
		return OBSCURE_ERROR;
	}
	
	if (Read(&head, sizeof(head)) != sizeof(head)) {	// format chunk
		return FILE_ERROR;
	}
	head.chunkLen = lendian32(head.chunkLen);
	off_t		nextChunk = Position() + head.chunkLen;
	
	if (Read(&commonHead, 14) != 14) { // sizeof(commonHead)) != sizeof(commonHead)) {
		return FILE_ERROR;
	}
	
	commonHead.format = lendian16(commonHead.format);
	commonHead.channels = lendian16(commonHead.channels);
	commonHead.sampleRate = lendian32(commonHead.sampleRate);
	commonHead.byteRate = lendian32(commonHead.byteRate);
	commonHead.blockAlign = lendian16(commonHead.blockAlign);
	
	if (commonHead.format == RiffWaveFmtMSPCM) {	
		if (Read(&unCommonHead, sizeof(unCommonHead)) != sizeof(unCommonHead)) {
			return FILE_ERROR;
		}
		unCommonHead.bitsPerSample = lendian16(unCommonHead.bitsPerSample);
	} else {
		return OBSCURE_ERROR;
	}

	Seek(nextChunk, SEEK_SET);
	for (;;) {
		if (Read(&head, sizeof(head)) != sizeof(head)) {
			return FILE_ERROR;
		}
		head.chunkLen = lendian32(head.chunkLen);
		if (debug_sample)
			fprintf(stderr, "chunk %x %d\n", head.chunkType, head.chunkLen);
		if (head.chunkType == bendian32('data'))
			break;
		Seek(head.chunkLen, SEEK_CUR);
	}

// should now be at start of sample data ! ...
	nChannels = commonHead.channels;
	sampleRate = commonHead.sampleRate;
//	sampleRate = 44100;
	fileType = WAVE_TYPE;
	sampleSize = unCommonHead.bitsPerSample == 8? 1 : 2;
	nFrames = head.chunkLen / (nChannels * sampleSize);
	sampleDataStart = Position();
	
	if (debug_sample)
		fprintf(stderr, "wave file %d chans, %d size, %d sr, %d frames %d pos\n",
				nChannels, sampleSize,
				sampleRate, nFrames, sampleDataStart);

	return HEADER_OK;	
}


status_t
SampleFile::SetTo(const char *refp, uint32 mode)
{
// not sure why we have to open by file descriptor???
// 
	if (fd >= 0) {
		if ((openMode & 3) == O_WRONLY) {
			Finalize();
		}
		close(fd);
	}
	status_t err;
#ifdef WIN32
	mode |= (O_BINARY);		// and maybe O_SEQUENTIAL ?????????
#endif
	fd = open(refp, mode);
	if (fd < 0) {
		if (debug_sample) fprintf(stderr, "fopen error...\n");
		switch(errno) {
			case EACCES: return ERRNO_PERMISSION_DENIED;
			case EEXIST: return ERRNO_FILE_EXISTS;
			case EINVAL: return B_ERROR;
			case EMFILE: return ERRNO_NO_MORE_FDS;
			case ENOENT: return ERRNO_FILE_NOT_FOUND;
		}
		return B_ERROR;
	}
	openMode = mode;
	if ((err=ProcessHeaderInfo()) != B_NO_ERROR) {
		return err;
	}
	return B_OK;
}

status_t
SampleFile::ProcessHeaderInfo()
{
	status_t	err;
	if (debug_sample) fprintf(stderr, "processing header...\n");
	if ((openMode & 3) == O_RDONLY) {
		if ((err=ReadHeaderInfo()) != B_NO_ERROR) {
			return err;
		}
	} else if ((openMode & 3) == O_WRONLY || openMode & O_CREAT) {
		if ((err=WriteHeaderInfo()) != B_NO_ERROR) {
			return err;
		}
	} else if ((openMode & 3) == O_RDWR) {
		if ((err=ReadHeaderInfo()) != B_NO_ERROR) {
// not sure whether this is polite or convenient. it's just to check that i can ...
			if ((err=WriteHeaderInfo()) != B_NO_ERROR) {
				return err;
			}
		}
	} else {
		lastError = ("openning file in wierd mode");
		return B_ERROR;
	}
	
	return B_NO_ERROR;
}

status_t
SampleFile::WriteHeaderInfo()
{
	status_t	err;
	
	switch (fileType) {
	case SND_TYPE:
		if ((err=WriteSndHeader()) != B_NO_ERROR)
			return err;
		break;
	case WAVE_TYPE: {
		if ((err=WriteWaveHeader()) != B_NO_ERROR)
			return err;
		break;
	}
	case AIFF_TYPE:
		if ((err=WriteAiffHeader()) != B_NO_ERROR)
			return err;
		break;
	case NO_TYPE:
	case RAW_TYPE:
	default:
		;
	}
	
	return B_NO_ERROR;
}


status_t
SampleFile::ReadHeaderInfo()
{
	status_t	err;
	if ((err=TryWaveFile()) == FILE_ERROR) {
		if (debug_sample) fprintf(stderr, "SampleFile::ReadHeaderInfo: try wave error...\n");
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	if ((err == NOT_ERROR || err == OBSCURE_ERROR) && (err=TryAiffFile()) == FILE_ERROR) {
		if (debug_sample) fprintf(stderr, "SampleFile::ReadHeaderInfo: try aif error...\n");
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	if ((err == NOT_ERROR || err == OBSCURE_ERROR) && (err=TrySndFile()) == FILE_ERROR) {
		if (debug_sample) fprintf(stderr, "SampleFile::ReadHeaderInfo: try snd error...\n");
#ifdef WIN32
		return GetLastError();
#else
		return B_ERROR;
#endif
	}
	if (err == NOT_ERROR || err == OBSCURE_ERROR) {
		if (debug_sample) fprintf(stderr, "SampleFile::ReadHeaderInfo: try raw error...\n");
		err = TryRawFile();
	}
	
	return err==HEADER_OK?B_NO_ERROR:err;
}

void
SampleFile::NormalizeInput(char *buf, long nframes)
{
	if (fileType == WAVE_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) buf;

#ifndef GLOUB_LITTLE_ENDIAN	
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char *) &samples[i], 4);
			}
#endif
		} else if (sampleSize == 2) {
			short		*samples = (short *) buf;

#ifndef GLOUB_LITTLE_ENDIAN	
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char *) &samples[i], 2);
			}
#endif
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = buf;
			for (long i=nframes*nChannels; i>= 0; i--) {
				samples[2*i] = samples[i];
				samples[2*i+1] = 0;
			}
		}
	} else if (fileType == AIFF_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) buf;
#ifdef GLOUB_LITTLE_ENDIAN
	
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char*)&samples[i], 4);
			}
#endif
		} else if (sampleSize == 2) {
			short		*samples = (short *) buf;
#ifdef GLOUB_LITTLE_ENDIAN
	
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char*)&samples[i], 2);
			}
#endif
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = buf;
			for (long i=nframes*nChannels; i>= 0; i--) {
				samples[2*i] = samples[i];
				samples[2*i+1] = 0;
			}
		}
	} else if (fileType == RAW_TYPE) {
		short		*samples = (short *) buf;

		for (long i=0; i<nframes*nChannels; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
			swab((char*)&samples[i], sizeof(short));
#endif
		}
	}
}

// normalize output from 16bit data according to data size,
// correcting for byte-order. size of data is
// nfr * nch *dsz, and the buffer better be able to cut it.
void
SampleFile::NormalizeOutput(short *buf, long nframes)
{
	if (fileType == WAVE_TYPE) {
		if (sampleSize == 4) {
			float *samples = (float *)buf;
//			for (long i=nframes*nChannels; i>= 0; i--) {
//			}
#ifndef GLOUB_LITTLE_ENDIAN
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char *) &samples[i], 4);
			}
#endif
		} else if (sampleSize == 2) {
#ifndef GLOUB_LITTLE_ENDIAN
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char *) &buf[i], 2);
			}
#endif
		} else if (sampleSize == 1) {	// convert to short
			char		*samples = (char *)buf;
			for (long i=0; i<nframes*nChannels; i++) {
				samples[i] = samples[2*i];
			}
		}
	} else if (fileType == AIFF_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) buf;
// this seems wrong ... but mebbe not
#ifdef GLOUB_LITTLE_ENDIAN
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char*)&samples[i], sizeof(float));
			}
#endif
		} if (sampleSize == 2) {
			short		*samples = (short *) buf;
// this seems wrong ... but mebbe not
#ifdef GLOUB_LITTLE_ENDIAN
			for (long i=0; i<nframes*nChannels; i++) {
				swab((char*)&samples[i], sizeof(short));
			}
#endif
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = (char *)buf;
			for (long i=0; i<nframes*nChannels; i++) {
				samples[i] = samples[2*i];
			}
		}
	} else if (fileType == RAW_TYPE) {
		short		*samples = (short *) buf;
// raw should be bendian
#ifdef GLOUB_LITTLE_ENDIAN
		for (long i=0; i<nframes*nChannels; i++) {
			swab((char*)&samples[i], sizeof(short));
		}
#endif
	}

}

void
SampleFile::NormalizeOutput(float *buf, long nframes)
{
	long i=0;
	if (fileType == WAVE_TYPE) {
		if (sampleSize == 4) {
			float *samples = (float *)buf;
//			for (long i=nframes*nChannels; i>= 0; i--) {
//			}
#ifndef GLOUB_LITTLE_ENDIAN
			for (i=0; i<nframes*nChannels; i++) {
				swab((char *) &samples[i], 4);
			}
#endif
		} else if (sampleSize == 2) {
			short		*samples = (short *)buf;
#ifndef GLOUB_LITTLE_ENDIAN
			for (i=0; i<nframes*nChannels; i++) {
				swab((char *) &samples[i], 2);
			}
#endif
			for (i=0; i<nframes*nChannels; i++) {
				samples[i] = NORMALIZE_SHORT*buf[i];
			}
		} else if (sampleSize == 1) {	// convert to short
			char		*samples = (char *)buf;
			for (i=0; i<nframes*nChannels; i++) {
				samples[i] = NORMALIZE_BYTE*buf[i];
			}
		}
	} else if (fileType == AIFF_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) buf;
// this seems wrong ... but mebbe not
#ifdef GLOUB_LITTLE_ENDIAN
			for (i=0; i<nframes*nChannels; i++) {
				swab((char*)&samples[i], sizeof(short));
			}
#endif
		} if (sampleSize == 2) {
			short		*samples = (short *) buf;
// this seems wrong ... but mebbe not
#ifdef GLOUB_LITTLE_ENDIAN
			for (i=0; i<nframes*nChannels; i++) {
				swab((char*)&samples[i], sizeof(short));
			}
#endif
			for (i=0; i<nframes*nChannels; i++) {
				samples[i] = NORMALIZE_SHORT*buf[i];
			}
		} else if (sampleSize == 1) {	// convert to short
			char		*samples = (char *)buf;
			for (i=0; i<nframes*nChannels; i++) {
				samples[i] = NORMALIZE_BYTE*buf[i];
			}
		}
	} else if (fileType == RAW_TYPE) {
		short		*samples = (short *) buf;
// raw should be bendian
#ifdef GLOUB_LITTLE_ENDIAN
		for (i=0; i<nframes*nChannels; i++) {
			swab((char*)&samples[i], sizeof(short));
		}
#endif
	}

}

void
SampleFile::NormalizeOutputCpy(char *dstBuf, float *srcBuf, long nframes)
{
	if (fileType == WAVE_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) dstBuf;
			for (long i=0; i<nframes*nChannels; i++) {
				samples[i] = srcBuf[i];
#ifndef GLOUB_LITTLE_ENDIAN
				swab((char *) &samples[i], 4);
#endif
			}
		} else if (sampleSize == 2) {
			short		*samples = (short *) dstBuf;
			for (long i=0; i<nframes*nChannels; i++) {
				samples[i] = NORMALIZE_SHORT*srcBuf[i];
#ifndef GLOUB_LITTLE_ENDIAN
				swab((char *) &samples[i], 2);
#endif
			}
		} else if (sampleSize == 1) {	// convert to short
							// highly unlikely to work or be necessary
			char	*samples = (char *)dstBuf;
			for (long i=nframes*nChannels; i>= 0; i--) {
				samples[i] = NORMALIZE_BYTE*srcBuf[i];
			}
		}
	} else if (fileType == AIFF_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) dstBuf;
			for (long i=0; i<nframes*nChannels; i++) {
				samples[i] = srcBuf[i];
#ifdef GLOUB_LITTLE_ENDIAN
// this seems wrong ... but mebbe not
				swab((char*)&samples[i], sizeof(short));
#endif
			}
		} else if (sampleSize == 2) {
			short		*samples = (short *) dstBuf;
			for (long i=0; i<nframes*nChannels; i++) {
				samples[i] = NORMALIZE_SHORT*srcBuf[i];
//				if (debug_sample) fprintf(stderr,"out %d sam %d nc %d\n", i, samples[i], nChannels);
#ifdef GLOUB_LITTLE_ENDIAN
// this seems wrong ... but mebbe not
				swab((char*)&samples[i], sizeof(short));
#endif
			}
		} else if (sampleSize == 1) {	// convert to short
							// see above... why the fuck would you want to?
			char	*samples = (char *)dstBuf;
			for (long i=nframes*nChannels; i>= 0; i--) {
				samples[i] = NORMALIZE_BYTE*srcBuf[i];
//				samples[i] = samples[2*i];
			}
		}
	} else if (fileType == RAW_TYPE) {
		short		*samples = (short *) dstBuf;
// raw should be bendian
		for (long i=0; i<nframes*nChannels; i++) {
			samples[i] = srcBuf[i];
#ifdef GLOUB_LITTLE_ENDIAN
			swab((char*)&samples[i], sizeof(short));
#endif
		}
	}

}

size_t
SampleFile::NormalizeInputCpy(float *dstBuf, char *srcBuf, long nb)
{
	if (fileType == WAVE_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) srcBuf;
	
			for (long i=0; i<nb/4; i++) {
#ifndef GLOUB_LITTLE_ENDIAN
				swab((char *) &samples[i], 4);
#endif
				dstBuf[i] = samples[i];
			}
			return nb/(4*nChannels);
		} else if (sampleSize == 2) {	// assume signed!
			short		*samples = (short *) srcBuf;
	
			for (long i=0; i<nb/2; i++) {
#ifndef GLOUB_LITTLE_ENDIAN
				swab((char *) &samples[i], 2);
#endif
				dstBuf[i] = samples[i]/NORMALIZE_SHORT;
			}
			return nb/(2*nChannels);
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = srcBuf;
			for (long i=0; i<nb; i++) {
				dstBuf[i] = (((short)samples[i])<<8)/NORMALIZE_SHORT;
			}
			return nb/nChannels;
		}
	} else if (fileType == AIFF_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) srcBuf;
	
			for (long i=0; i<nb/4; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
				swab((char*)&samples[i], 4);
#endif
				dstBuf[i] = samples[i];
			}
			return nb/(4*nChannels);
		} else if (sampleSize == 2) {
			short		*samples = (short *) srcBuf;
	
			for (long i=0; i<nb/2; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
				swab((char*)&samples[i], 2);
#endif
				dstBuf[i] = samples[i]/NORMALIZE_SHORT;
//				if (debug_sample) fprintf(stderr,"inp %d sam %d\n", i, samples[i]);
			}
			return nb/(2*nChannels);
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = srcBuf;
			for (long i=0; i<nb; i++) {
				dstBuf[i] = (((short)samples[i])<<8)/NORMALIZE_SHORT;
			}
			return nb/nChannels;
		}
	} else if (fileType == RAW_TYPE) {
		short		*samples = (short *) srcBuf;

		for (long i=0; i<nb/2; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
			swab((char*)&samples[i], sizeof(short));
#endif
			dstBuf[i] = samples[i];
		}
		return nb/(2*nChannels);
	} else {
		lastError = ("sample file type not known");
		return B_ERROR;
	}
	return 0;
}

size_t
SampleFile::NormalizeInputCpy(short *dstBuf, char *srcBuf, long nb)
{
	if (fileType == WAVE_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) srcBuf;
	
			for (long i=0; i<nb/4; i++) {
#ifndef GLOUB_LITTLE_ENDIAN
				swab((char *) &samples[i], 4);
#endif
				dstBuf[i] = samples[i];
			}
			return nb/(4*nChannels);
		} else if (sampleSize == 2) {
			short		*samples = (short *) srcBuf;
	
			for (long i=0; i<nb/2; i++) {
#ifndef GLOUB_LITTLE_ENDIAN
				swab((char *) &samples[i], 2);
#endif
				dstBuf[i] = samples[i];
			}
			return nb/(2*nChannels);
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = srcBuf;
			for (long i=0; i<nb; i++) {
				dstBuf[i] = ((short)samples[i])<<8;
			}
			return nb/nChannels;
		}
	} else if (fileType == AIFF_TYPE) {
		if (sampleSize == 4) {
			float		*samples = (float *) srcBuf;
	
			for (long i=0; i<nb/4; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
				swab((char*)&samples[i], 4);
#endif
				dstBuf[i] = samples[i];
			}
			return nb/(4*nChannels);
		} else if (sampleSize == 2) {
			short		*samples = (short *) srcBuf;
	
			for (long i=0; i<nb/2; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
				swab((char*)&samples[i], 2);
#endif
				dstBuf[i] = samples[i];
			}
			return nb/(2*nChannels);
		} else if (sampleSize == 1) {	// convert to short
			char	*samples = srcBuf;
			for (long i=0; i<nb; i++) {
				dstBuf[i] = ((short)samples[i])<<8;
			}
			return nb/nChannels;
		}
	} else if (fileType == RAW_TYPE) {
		short		*samples = (short *) srcBuf;

		for (long i=0; i<nb/2; i++) {
#ifdef GLOUB_LITTLE_ENDIAN
			swab((char*)&samples[i], sizeof(short));
#endif
			dstBuf[i] = samples[i];
		}

		return nb/(2*nChannels);
	} else {
		lastError = ("sample file type not known");
		return B_ERROR;
	}
	return 0;
}

status_t
SampleFile::Plonk(short *theBuf)
{
	char	buf[CHUNK_SIZE];
	
	SeekToFrame(0);
	off_t	toRead = nFrames;
	
	while (toRead > 0) {
		long	nbr = Min(sizeof(buf), toRead*nChannels*sampleSize);
		if (Read(buf, nbr) != nbr) {
#ifdef WIN32
			return GetLastError();
#else
			return B_ERROR;
#endif
		}
		long nf = NormalizeInputCpy(theBuf, buf, nbr);
		theBuf += nf*nChannels;
		toRead -= nf;
	}
	return B_NO_ERROR;
}

void
SampleFile::operator=(const SampleFile&f)
{
	fd = f.fd;

	nChannels = f.nChannels;
	sampleSize = f.sampleSize;
	nFrames = f.nFrames;
	fileType = f.fileType;
	sampleType = f.sampleType;
	sampleDataStart = f.sampleDataStart;
	sampleRate = f.sampleRate;
}

status_t
SampleFile::GetSize(off_t *offp)
{
	return B_NO_ERROR;
}

status_t
SampleFile::SetSize(off_t off)
{
	return B_NO_ERROR;
}


/*
 * C O N V E R T   T O   I E E E   E X T E N D E D
 */

/* Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

void double_to_ieee_extended(double num, unsigned char* bytes)
{
    int    sign;
    int expon;
    double fMant, fsMant;
    unsigned long hiMant, loMant;

    if (num < 0) {
        sign = 0x8000;
        num *= -1;
    } else {
        sign = 0;
    }

    if (num == 0) {
        expon = 0; hiMant = 0; loMant = 0;
    }
    else {
        fMant = frexp(num, &expon);
        if ((expon > 16384) || !(fMant < 1)) {    /* Infinity or NaN */
            expon = sign|0x7FFF; hiMant = 0; loMant = 0; /* infinity */
        }
        else {    /* Finite */
            expon += 16382;
            if (expon < 0) {    /* denormalized */
                fMant = ldexp(fMant, expon);
                expon = 0;
            }
            expon |= sign;
            fMant = ldexp(fMant, 32);          
            fsMant = floor(fMant); 
            hiMant = FloatToUnsigned(fsMant);
            fMant = ldexp(fMant - fsMant, 32); 
            fsMant = floor(fMant); 
            loMant = FloatToUnsigned(fsMant);
        }
    }
    
    bytes[0] = expon >> 8;
    bytes[1] = expon;
    bytes[2] = hiMant >> 24;
    bytes[3] = hiMant >> 16;
    bytes[4] = hiMant >> 8;
    bytes[5] = hiMant;
    bytes[6] = loMant >> 24;
    bytes[7] = loMant >> 16;
    bytes[8] = loMant >> 8;
    bytes[9] = loMant;
}


/*
 * C O N V E R T   F R O M   I E E E   E X T E N D E D  
 */

/* 
 * Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */
/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/

double ieee_extended_to_double(unsigned char* bytes)
{
    double    f;
    int    expon;
    unsigned long hiMant, loMant;
    
    expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
    hiMant    =    ((unsigned long)(bytes[2] & 0xFF) << 24)
            |    ((unsigned long)(bytes[3] & 0xFF) << 16)
            |    ((unsigned long)(bytes[4] & 0xFF) << 8)
            |    ((unsigned long)(bytes[5] & 0xFF));
    loMant    =    ((unsigned long)(bytes[6] & 0xFF) << 24)
            |    ((unsigned long)(bytes[7] & 0xFF) << 16)
            |    ((unsigned long)(bytes[8] & 0xFF) << 8)
            |    ((unsigned long)(bytes[9] & 0xFF));

    if (expon == 0 && hiMant == 0 && loMant == 0) {
        f = 0;
    }
    else {
        if (expon == 0x7FFF) {    /* Infinity or NaN */
            f = HUGE_VAL;
        }
        else {
            expon -= 16383;
            f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
            f += ldexp(UnsignedToFloat(loMant), expon-=32);
        }
    }

    if (bytes[0] & 0x80)
        return -f;
    else
        return f;
}
