#ifndef _SAMPLEFILE
#define _SAMPLEFILE

#include "StdDefs.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
enum {
	HEADER_OK = 0,
	FILE_ERROR = -1,
	NOT_ERROR = 1,
	OBSCURE_ERROR = 2,
	MALFORM_ERROR = 3
};


// form chunk header


#define FormID		'FORM'
#define AIFFckID	'AIFF'

struct FormChunkHeader
{
	uint32				ckID;
	uint32				ckSize;
};

struct FormAIFFHeader
{
	uint32				chunkType;
	uint32				chunkLen;
	uint32				formType;
};

#define CommonID    'COMM'  /* ckID for Common Chunk */ 

struct  CommonChunk { 
    uint16          numChannels; 
    uint16			numSampleFrames1; 
    uint16			numSampleFrames2; 
    uint16          sampleSize; 
    unsigned char	sampleRate[10];	
};  

#define SoundDataID 'SSND'  /* ckID for Sound Data Chunk */ 

struct  SoundDataChunk { 
    uint32			offset; 
    uint32			blockSize; 
//    uint32			soundData[]; 

};   

#define MarkerCkID    'MARK'  /* ckID for Marker Chunk */ 

typedef short   MarkerId; 

struct  Marker{ 
    MarkerId            id; 
    unsigned long       position; 
//	char             	markerName[];	// pascal ?? string 
}; 

struct  MarkerChunk{ 
    unsigned short      numMarkers; 
//    Marker              Markers[]; 
}; 


#define InstrumentID    'INST'  /* ckID for Instrument Chunk */ 

#define NoLooping               0 
#define ForwardLooping          1 
#define ForwardBackwardLooping  2 

struct  Loop{ 
    short           playMode; 
    MarkerId        beginLoop; 
    MarkerId        endLoop; 
}; 

struct  InstrumentChunk{ 
    char            baseNote; 
    char            detune; 
    char            lowNote; 
    char            highNote; 
    char            lowVelocity; 
    char            highVelocity; 
    short           gain; 
    Loop            sustainLoop; 
    Loop            releaseLoop; 
}; 


#define MIDIDataID  'MIDI'  /* ckID for MIDI Data Chunk */ 

struct  MIDIDataChunk { 
//    unsigned char       MIDIdata[]; 
}; 

#define AudioRecordingID  'AESD'        /* ckID for Audio Recording */ 
                                        /*   Chunk.                 */ 

struct  AudioRecordingChunk{ 
    unsigned char       AESChannelStatusData[24]; 
}; 

#define ApplicationSpecificID  'APPL'   /* ckID for Application */ 
                                        /*  Specific Chunk.     */ 

struct  ApplicationSpecificChunk { 
    uint32		applicationSignature; 
//    char        data[]; 
}; 


#define CommentID       'COMT'  /* ckID for Comments Chunk.  */ 

struct  Comment{ 
    unsigned long       timeStamp; 
    short				markID; 
    unsigned short      count; 
//    char                text[]; 
}; 

struct  CommentsChunk{ 
    unsigned short      numComments; 
//   Comment             comments[]; 
}; 

#define NameID          'NAME'  /* ckID for Name Chunk.  */ 
#define AuthorID        'AUTH'  /* ckID for Author Chunk.  */ 
#define CopyrightID     '(c) '  /* ckID for Copyright Chunk.  */ 
#define AnnotationID    'ANNO'  /* ckID for Annotation Chunk.  */ 

struct TextChunk { 
//   char                text[]; 
}; 

// riff wave header

struct RiffChunkHeader
{
	uint32				chunkType;
	uint32				chunkLen;
};

struct RiffWaveCommonHeader
{
	uint16				format;
	uint16				channels;
	uint32				sampleRate;
	uint32				byteRate;
	uint16				blockAlign;
};

struct RiffWavePCMHeader
{
	uint16				bitsPerSample;
};

enum {
	RiffWaveFmtMSPCM = 1,
	RiffWaveFmtIBMMuLaw = 0x101,
	RiffWaveFmtIBMALaw = 0x102,
	RiffWaveFmtIBMAPCM = 0x103
};

// base sample file class

class SampleFile
{
public:
						SampleFile(short typ=NO_TYPE, short nchan=2, short dize=2, float sr=44100, uint32 mode=O_RDONLY);
						SampleFile(char *, uint32 mode=O_RDONLY);
						~SampleFile();
						
	void				SetFormat(short nchan, short dsize, long sr);

	status_t			SetTo(const char *p, uint32 mode=O_RDONLY);

	status_t			ReadHeaderInfo();
	status_t			WriteHeaderInfo();
	status_t			ProcessHeaderInfo();
	status_t			TryAiffFile();
	status_t			TrySndFile();
	status_t			TryWaveFile();
	status_t			TryRawFile();
	status_t			WriteWaveHeader();
	status_t			WriteAiffHeader();
	status_t			WriteSndHeader();

	void				NormalizeInput(char * buf, long nf);
	void				NormalizeOutput(float * buf, long nf);
	void				NormalizeOutput(short * buf, long nf);
	void				NormalizeOutputCpy(char *buf, float *b2, long nf);
	size_t				NormalizeInputCpy(float *buf, char *b2, long nf);
	size_t				NormalizeInputCpy(short *buf, char *b2, long nf);

	long				Frame();
	long				FrameToOffset(long fr);
	long				OffsetToFrame(long fr);
	long				FrameToByte(long fr);
	long				ByteToFrame(long nb);

	void				SetType(short typ);
	inline short		Type() { return fileType; }
	status_t			Plonk(short *buffer);
	status_t			Finalize();

	inline off_t		ByteSize() { return nFrames*nChannels*sampleSize; }
	inline off_t		Position() { return fd>=0? tell(fd):0; }	
	inline off_t		Write(void *buf, off_t nb) { return fd>=0? write(fd, buf, nb): -1; }
	inline off_t		Read(void *buf, off_t nb) { return fd>=0? read(fd, buf, nb): -1; }
	inline off_t		Seek(long a, int b) { return fd>=0? lseek(fd, a, b): -1; }
	status_t			GetSize(off_t*);
	status_t			SetSize(off_t);

	inline status_t		SeekToFrame(off_t fr) {	Seek(FrameToOffset(fr), SEEK_SET); return B_NO_ERROR; }
	void				operator=(const SampleFile&f);

	int					fd;
	
	uint32				openMode;
	short				nChannels;
	short				sampleSize;
	off_t				nFrames;
	short				fileType;
	short				sampleType;
	off_t				sampleDataStart;
	long				sampleRate;

	enum {
		NO_TYPE = 0,
		RAW_TYPE = 1,
		WAVE_TYPE = 2,
		AIFF_TYPE = 3,
		SND_TYPE = 4,
		AIFC_TYPE = 5,	// just a compressed mode for aif
		MP3_TYPE = 6,	// wish list
		MP4_TYPE = 7,	// even wishier
		OGG_TYPE = 8,	// not too optimistic
		AU_TYPE = 9,	// probably should already have done this.
		AAC_TYPE = 10,	// mucky but not impossible
		WMA_TYPE = 11	// exceptionally optimistic
	};
};

inline long SampleFile::Frame()
{
	return (Position() - sampleDataStart) / (sampleSize * nChannels);
}

inline long SampleFile::FrameToOffset(long fr)
{
	return fr*(sampleSize * nChannels) + sampleDataStart;
}

inline long SampleFile::FrameToByte(long fr)
{
	return fr*(sampleSize * nChannels);
}

inline long SampleFile::ByteToFrame(long bt)
{
	return bt/(sampleSize * nChannels);
}

#endif
