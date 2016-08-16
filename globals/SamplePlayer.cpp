#include "SamplePlayer.h"
#include "Sample.h"
#include "Block.h"
#include "Expression.h"

SamplePlayer::SamplePlayer(Sample *s)
{
	sample = s;
}

SamplePlayer::~SamplePlayer()
{
}

PlaySample::PlaySample(Sample *s):
	SamplePlayer(s)
{
}

PlaySample::~PlaySample()
{
}

void
PlaySample::Generate(float *outSig, long nFramesReqd, short nChannel)
{
	long	outFrame = 0;
	if (!QSample() || !QSample()->selectedFile())
		return;
	short nc = QSample()->selectedFile()->nChannels;
	SampleFile *f = QSample()->selectedFile();
	
	if (startFrame.ticks < endFrame.ticks) {
		while (outFrame < nFramesReqd) {
			if (currentFrame < startFrame.ticks) {
				currentFrame = startFrame.ticks;
			}
			if (currentFrame >= endFrame.ticks) {
//				loopCount++;
				currentFrame = startFrame.ticks;
				// other loop startish things ...
//				LoopEnvelopes();
			}
			if (  currentFrame < 0 ||
				  currentFrame >= f->nFrames) {	// a frame of nothing
				break;
			} else {
				if (!currentBuffer || !currentBuffer->HasFrame(f, currentFrame)) {
					currentBuffer = QSample()->BufferForFrame(f, currentFrame);
					if (currentBuffer == nullptr) {
						break;
					}				
				}
				
//			get crap from current frame
				int bufFrame = (currentFrame - currentBuffer->fromFrame);
				size_t	nFramesAvail = min32(nFramesReqd-outFrame,
											currentBuffer->nFrames-bufFrame);
				nFramesAvail = min32(nFramesAvail, endFrame.ticks-currentFrame);
	
				memcpy(	&outSig[outFrame*nc],
						&currentBuffer->data[bufFrame*nc],
						nFramesAvail*nc*sizeof(float));
				outFrame += nFramesAvail;
				currentFrame += nFramesAvail;
			}
		}
	}

	for (int i=outFrame; i<nFramesReqd; i++) {
		currentFrame++;
		outSig[2*i] = 0;
		outSig[2*i+1] = 0;
	}

// tempSig should now be float, regardless... but stereo or mono.
//	some FX could be in mono ...
//  some FX could change frame count, and could be dealt by the reader.
//  perhaps all could be but this will kill response ...

	float		gainl = gain * ((1-(pan))/2),
				gainr = gain * ((1+(pan))/2);		
	if (nChannel == 2) {
		for (int i=0; i<nFramesReqd; i++) {
			outSig[2*i] = gainl*outSig[2*i];
			outSig[2*i+1] = gainr*outSig[2*i+1];
		}
	} else if (nChannel == 1) {
		for (int i=nFramesReqd-1; i>=0; i--) {
			outSig[2*i] = gainl*outSig[i];
			outSig[2*i+1] = gainr*outSig[i];
		}
	}
}

void
PlaySample::SetParameters(ResultValue *, short) {

}
