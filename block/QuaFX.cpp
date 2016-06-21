#include "qua_version.h"

#if defined(WIN32)


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

#include <math.h>

#include "SampleFile.h"
#include "SampleBuffer.h"
#include "Block.h"
#include "QuaFX.h"
#include "Qua.h"
#include "Sample.h"
#include "Method.h"
#include "Clip.h"

#ifdef QUA_V_VST_HOST
#include "VstPlugin.h"
#endif

flag	debug_quafx=0;

// ????????????????????????????????????????????????????????????????
// need to work out something for vst fx that put out more than 2 channels. e.g. Battery 2
// such plugins probably want seperate fx treatments per channel, so need to create
// a construct to house, mix, and/or redirect some of the outs for them
// most outs I've seen so far is 8, 4 stereo pairs... TBL, which stacks multiple
// basslines under the same window. different outs activated by different midi
// data from different channels outputting to it. Can't see a lot of benefit to this
// in Qua, except minimizing screen space. Multiple outs from a drum machine
// is something I would like to have to deal with....
#define QUA_MAX_VST_OUT_PINS 8
float	**vstTmpSig = sample_buf_alloc(QUA_MAX_VST_OUT_PINS,QUA_MAX_AUDIO_BUFSIZE);
float	**vstTmpVex = new float*[QUA_MAX_VST_OUT_PINS];
float	**sampleSig = sample_buf_alloc(2, QUA_MAX_AUDIO_BUFSIZE);

size_t
ApplyQuaFX(QuasiStack *stack, Block *fxBlock, float **outSig, long nFrames, short nChan)
{
	if (debug_quafx)
		fprintf(stderr, "Apply Quafx %x %d stack %x\n", (unsigned)fxBlock, fxBlock?fxBlock->type:-1, (unsigned)stack);

	if (!fxBlock)
		return nFrames;
		
	switch (fxBlock->type) {
    
    case Block::C_FLUX: {
		ApplyQuaFX(stack, fxBlock->crap.flux.block, outSig, nFrames, nChan);
		break;
	
	}

	case Block::C_LIST: {
		switch (fxBlock->subType) { 
		case Block::LIST_NORM: {	// apply effects in a chain
			Block		*p;
			p = fxBlock->crap.list.block;
	
			while (p != nullptr) {
				ApplyQuaFX(stack, p, outSig, nFrames, nChan);
				p = p->next;
			}
					
			break;
		}
		
		case Block::LIST_PAR: {
// apply effects in a chain
			Block		*p;
			p = fxBlock->crap.list.block;
	
			while (p != nullptr) {
				ApplyQuaFX(stack, p, outSig, nFrames, nChan);
				p = p->next;
			}
					
			break;
		}
		
		case Block::LIST_FORK: {
// apply effects in seperate chains
			Block		*block;
			short		i=0, j=0;
			float		**tmpSig = fxBlock->crap.list.tmpbuf1;
			float		**inSig = fxBlock->crap.list.tmpbuf2;

			block = fxBlock->crap.list.block;
			sample_buf_copy(inSig, outSig, nChan, nFrames);
			sample_buf_zero(outSig, nChan, nFrames);
			while (block != nullptr) {
				sample_buf_copy(tmpSig, inSig, nChan, nFrames);
				ApplyQuaFX(stack, block, tmpSig, nFrames, nChan);
				sample_buf_add(outSig, tmpSig, nChan, nFrames);
				block = block->next;
			}
					
			break;
		}
		
		case Block::LIST_SEQ: {
// have a go at the current block, let the main loop run the sequencing of the list
// may not be exactly what's wanted.
// need to find a neat way for audio units like vst's to have an effect on
// the sequence. ??????
			Block	*active =
				(Block*)fxBlock->crap.list.stuffVar.IntValue(stack);		
			if (active == nullptr)
				active = fxBlock->crap.list.block;
	
			if (active != nullptr) {
				ApplyQuaFX(stack, active, outSig, nFrames, nChan);
			}
			
			break;
		}
		}
		break;
	}

	case Block::C_SAMPLE_PLAYER: {
// need data per instance:
//		clip (
//			sample data,
//			start point,
//			end point),
//		loop mode
//		play direction
//		play pitch .. enveloped
//		  
//		current position, 
//		play state (playing, stopped, first start)
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(fxBlock->crap.call.frameIndex);
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			SamplePlayInfo	*inf = fxBlock->crap.call.crap.splayer;

			long pos = inf->posVar.IntValue(higherFrame);		// sample position in frames
			flag loop = inf->loopmodeVar.IntValue(higherFrame);	// looping on or off
			flag dir = inf->directionVar.IntValue(higherFrame);	// forwards or backwards
//			float pitch = inf->pitchVar.FloatValue(higherFrame);	// play pitch
			float gain = inf->gainVar.FloatValue(higherFrame);	// play gain
			float pan = inf->panVar.FloatValue(higherFrame);	// play pan

			Clip *clip = (Clip *) inf->clipVar.PointerValue(higherFrame);	// pointer to a clip
			Take *media;
			int	state = 0;
			
			if (clip != nullptr) {
//				fprintf(stderr, "playing sample pos %d clip %x loop %d dir %d pitch %g gain %g pan %g\n", pos, clip, loop, dir, pitch, gain, pan);
				media = clip->media;
				if (media && media->type == Take::SAMPLE) {
					// ???????????????
					// assume clip is
					// measuring time in frames ...
					long	sf = clip->start.TickValue(&Metric::sampleRate);
					long	df = clip->duration.TickValue(&Metric::sampleRate);
					pos = ((SampleTake*)media)->sample->Play(
								sampleSig, nChan, nFrames,
								(SampleTake*)media,	sf, sf+df, pos,
								gain, pan, state, loop, dir);
					sample_buf_add(outSig, sampleSig, nChan, nFrames);
				}
			} else {
				media = (Take *)inf->mediaVar.PointerValue(higherFrame);
//				fprintf(stderr, "playing sample pos %d media %x loop %d dir %d pitch %g gain %g pan %g\n", pos, media, loop, dir, pitch, gain, pan);
				if (media && media->type == Take::SAMPLE) {
					pos = ((SampleTake*)media)->sample->Play(
								sampleSig, nChan, nFrames,
								(SampleTake *)media, 0, ((SampleTake *)media)->file->nFrames, pos,
								gain, pan, state, loop, dir);
					sample_buf_add(outSig, sampleSig, nChan, nFrames);
				}
			}
			inf->posVar.SetValue((int32)pos, higherFrame);
			inf->stateVar.SetValue((int32)state, higherFrame);		// is playing or running or just stopped
		}
		break;
	}

	case Block::C_TUNEDSAMPLE_PLAYER: {
// need data per instance:
//		clip (
//			sample data,
//			start point,
//			end point),
//		loop mode
//		play direction
//		play pitch .. enveloped
//		  
//		current position, 
//		play state (playing, stopped, first start)
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(fxBlock->crap.call.frameIndex);
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			TunedSamplePlayInfo	*inf = fxBlock->crap.call.crap.tplayer;

			double pos = inf->posVar.DoubleValue(higherFrame);		// sample position in frames
			flag loop = inf->loopmodeVar.IntValue(higherFrame);	// looping on or off
			flag dir = inf->directionVar.IntValue(higherFrame);	// forwards or backwards
			float pitch = inf->pitchVar.FloatValue(higherFrame);	// play pitch
			float gain = inf->gainVar.FloatValue(higherFrame);	// play gain
			float pan = inf->panVar.FloatValue(higherFrame);	// play pan
			double idx = inf->idxVar.DoubleValue(higherFrame);
			float lastpitch = inf->lastpitchVar.FloatValue(higherFrame);
			float *last= inf->lastSampleArray.FloatPValue(higherFrame);

			Clip *clip = (Clip *) inf->clipVar.PointerValue(higherFrame);	// pointer to a clip
			Take *media;
			int	state = 0;

			if (pitch != lastpitch) {
				lastpitch = pitch;
				idx = pow(2, pitch);
				inf->lastpitchVar.SetValue(lastpitch, higherFrame);
				inf->idxVar.SetDoubleValue(idx, higherFrame);
			}
			if (clip != nullptr) {
//				fprintf(stderr, "playing sample pos %d clip %x loop %d dir %d pitch %g gain %g pan %g\n", pos, clip, loop, dir, pitch, gain, pan);
				media = clip->media;
				if (media && media->type == Take::SAMPLE) {
					// ???????????????
					// assume clip is
					// measuring time in frames ...
					long	sf = clip->start.TickValue(&Metric::sampleRate);
					long	df = clip->duration.TickValue(&Metric::sampleRate);
					pos = ((SampleTake*)media)->sample->PlayPitched(
								sampleSig, nChan, nFrames,
								(SampleTake*)media,	sf, sf+df, pos,
								gain, pan, idx, last, state, loop, dir);
					sample_buf_add(outSig, sampleSig, nChan, nFrames);
				}
			} else {
				media = (Take *)inf->mediaVar.PointerValue(higherFrame);
//				fprintf(stderr, "playing sample pos %d media %x loop %d dir %d pitch %g gain %g pan %g\n", pos, media, loop, dir, pitch, gain, pan);
				if (media && media->type == Take::SAMPLE) {
					pos = ((SampleTake*)media)->sample->PlayPitched(
								sampleSig, nChan, nFrames,
								(SampleTake *)media, 0, ((SampleTake *)media)->file->nFrames, pos,
								gain, pan, idx, last, state, loop, dir);
					sample_buf_add(outSig, sampleSig, nChan, nFrames);
				}
			}
			inf->posVar.SetDoubleValue(pos, higherFrame);
			inf->stateVar.SetValue((int32)state, higherFrame);		// is playing or running or just stopped
		}
		break;
	}

	case Block::C_MIDI_PLAYER: {
// need data per instance:
//		clip (
//			sample data,
//			start point,
//			end point),
//		loop mode
//		play direction
//		play pitch .. enveloped
//		  
//		current position, 
//		play state (playing, stopped, first start)
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(fxBlock->crap.call.frameIndex);
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			TunedSamplePlayInfo	*inf = fxBlock->crap.call.crap.tplayer;

			double pos = inf->posVar.DoubleValue(higherFrame);		// sample position in frames
			flag loop = inf->loopmodeVar.IntValue(higherFrame);	// looping on or off
			flag dir = inf->directionVar.IntValue(higherFrame);	// forwards or backwards
			float pitch = inf->pitchVar.FloatValue(higherFrame);	// play pitch
			float gain = inf->gainVar.FloatValue(higherFrame);	// play gain
			float pan = inf->panVar.FloatValue(higherFrame);	// play pan
			double idx = inf->idxVar.DoubleValue(higherFrame);
			float lastpitch = inf->lastpitchVar.FloatValue(higherFrame);
			float *last= inf->lastSampleArray.FloatPValue(higherFrame);

			Clip *clip = (Clip *) inf->clipVar.PointerValue(higherFrame);	// pointer to a clip
			Take *media;
			int	state = 0;

			if (pitch != lastpitch) {
				lastpitch = pitch;
				idx = pow(2, pitch);
				inf->lastpitchVar.SetValue(lastpitch, higherFrame);
				inf->idxVar.SetDoubleValue(idx, higherFrame);
			}
			if (clip != nullptr) {
//				fprintf(stderr, "playing sample pos %d clip %x loop %d dir %d pitch %g gain %g pan %g\n", pos, clip, loop, dir, pitch, gain, pan);
				media = clip->media;
				if (media && media->type == Take::SAMPLE) {
					// ???????????????
					// assume clip is
					// measuring time in frames ...
					long	sf = clip->start.TickValue(&Metric::sampleRate);
					long	df = clip->duration.TickValue(&Metric::sampleRate);
					pos = ((SampleTake*)media)->sample->PlayPitched(
								sampleSig, nChan, nFrames,
								(SampleTake*)media,	sf, sf+df, pos,
								gain, pan, idx, last, state, loop, dir);
					sample_buf_add(outSig, sampleSig, nChan, nFrames);
				}
			} else {
				media = (Take *)inf->mediaVar.PointerValue(higherFrame);
//				fprintf(stderr, "playing sample pos %d media %x loop %d dir %d pitch %g gain %g pan %g\n", pos, media, loop, dir, pitch, gain, pan);
				if (media && media->type == Take::SAMPLE) {
					pos = ((SampleTake*)media)->sample->PlayPitched(
								sampleSig, nChan, nFrames,
								(SampleTake *)media, 0, ((SampleTake *)media)->file->nFrames, pos,
								gain, pan, idx, last, state, loop, dir);
					sample_buf_add(outSig, sampleSig, nChan, nFrames);
				}
			}
			inf->posVar.SetDoubleValue(pos, higherFrame);
			inf->stateVar.SetValue((int32)state, higherFrame);		// is playing or running or just stopped
		}
		break;
	}

	case Block::C_CALL: {
// this could be delicate ground for audio plugins that
// have a memory.. delays, filters, etc. actually ok but confusing
// the seperateness of the stacks should give seperate
// instances of plugins for each syntactic call.
// what about looped calls?
// vst plugins have methods that need pseudo methods/variable to access
// enabling
// how would we find the particular plugin instance in the wierd gui?
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(fxBlock->crap.call.frameIndex);
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			Method	*S = fxBlock->crap.call.crap.method;
#ifdef LOTSALOX
			if (S->stackableLock.Lockable()) {
				S->stackableLock.Lock();
#endif
				ApplyQuaFX(higherFrame, S->mainBlock, outSig, nFrames, nChan);
#ifdef LOTSALOX 
				S->stackableLock.Unlock();
			}
#endif
		}
		stack->UnThunk();
// hell does the garbage collection,
// but I do the disinit
		break;
	}

	case Block::C_DIVERT: {
		break;
    }

	case Block::C_IF: {
		if (fxBlock->crap.iff.condVar.IntValue(stack)) {
		    if (fxBlock->crap.iff.ifBlock != nullptr) {
				ApplyQuaFX(stack, fxBlock->crap.iff.ifBlock,
					outSig, nFrames, nChan);
		    } 
		} else {
		    if (fxBlock->crap.iff.elseBlock != nullptr) {
				ApplyQuaFX(stack, fxBlock->crap.iff.elseBlock,
					outSig, nFrames, nChan);
		    } 
		}
		break;
	}
	
	case Block::C_GUARD: {
		if (fxBlock->crap.guard.condVar.IntValue(stack)) {
		    if (fxBlock->crap.guard.block != nullptr) {
				ApplyQuaFX(stack, fxBlock->crap.guard.block,
					outSig, nFrames, nChan);
		    } 
		}
		break;
    }


    case Block::C_VST: {
#ifdef QUA_V_VST_HOST
		stack->Thunk();
		QuasiStack	*higherFrame = stack->FrameAt(fxBlock->crap.call.frameIndex);
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			VstPlugin	*vst = fxBlock->crap.call.crap.vstplugin;
			if (vst->status == VST_PLUG_LOADED) {
				AEffect	*afx = higherFrame->stk.afx;
				if (debug_quafx) {
					fprintf(stderr, "Applying vst plugin %s cookie %x %d frames %d chan\n",
							fxBlock->crap.call.crap.vstplugin->sym->name,
							afx, nFrames, nChan);
				}
// currently I think a synth that takes inputs should filter rather than
// accumulate to its inputs.
// ?????????????? i think a lot of the buffer copying can be optimised for the
// ?????????????? 0,1,2 in and 2 out on a stereo channel case
				if (afx) {
					if (nChan == 1) {
						if (vst->canReplacing) {
							if (vst->numInputs == 0) { // most regular synths
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								sample_buf_add_to_mono(outSig[0], vstTmpSig, vst->numOutputs, nFrames);
							} else {
								if (vst->numInputs != 1) { // expand the mono in
									for (short k=0; k<vst->numInputs; k++) {
										vstTmpVex[k] = outSig[0];
									}
									afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								} else {
									afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								}
								sample_buf_copy_to_mono(outSig[0], vstTmpSig, vst->numOutputs, nFrames);
							}
						} else {
							sample_buf_zero(vstTmpSig, vst->numOutputs, nFrames);
							if (vst->numInputs == 0) { // most regular synths
								afx->process(afx, outSig, vstTmpSig, nFrames);
								sample_buf_add_to_mono(outSig[0], vstTmpSig, vst->numOutputs, nFrames);
							} else {
								if (vst->numInputs != 1) { // expand mono in to multi
									for (short k=0; k<vst->numInputs; k++) {
										vstTmpVex[k] = outSig[0];
									}
									afx->process(afx, vstTmpVex, vstTmpSig, nFrames);
								} else {
									afx->process(afx, outSig, vstTmpSig, nFrames);
								}
								sample_buf_copy_to_mono(outSig[0], vstTmpSig, vst->numOutputs, nFrames);
							}
						}
					} else if (nChan == 2) { // optimize the crap out of this one!!!! ??????
						if (vst->canReplacing) {
							if (vst->numInputs == 0) {
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								if (vst->numOutputs == 2) { // the most normal thing
									sample_buf_add(outSig, vstTmpSig, 2, nFrames);
//									for (short ii=0; ii<nFrames; ii++) {
//										fprintf(stderr, "%g ", outSig[0][ii]);
//									} fprintf(stderr, "\n");
								} else if (vst->numOutputs == 1) {
									sample_buf_add_mono(outSig, vstTmpSig[0], 2, nFrames);
								} else {
									sample_buf_add_multi(outSig, 2, vstTmpSig, vst->numOutputs, nFrames);
								}
							} else if (vst->numInputs == 1) {	// convert stereo in to mono
								sample_buf_to_mono(outSig, nChan, nFrames);
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								if (vst->numOutputs == 2) { // a not uncommon thing
									sample_buf_copy(outSig, vstTmpSig, 2, nFrames);
								} else if (vst->numOutputs == 1) {
									sample_buf_copy_mono(outSig, vstTmpSig[0], 2, nFrames);
								} else {
									sample_buf_copy_multi(outSig, 2, vstTmpSig, vst->numOutputs, nFrames);
								}
							} else if (vst->numInputs == 2) {
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								if (vst->numOutputs == 2) {// the most normal thing
									sample_buf_copy(outSig, vstTmpSig, 2, nFrames);
								} else if (vst->numOutputs == 1) {
									sample_buf_copy_mono(outSig, vstTmpSig[0], 2, nFrames);
								} else {
									sample_buf_copy_multi(outSig, 2, vstTmpSig, vst->numOutputs, nFrames);
								}
							} else { // unheard of but not impossible
								fprintf(stderr, "unexpected vst configuration: nchan = %d nin = %d nout = %d\n", nChan, vst->numInputs, vst->numOutputs);
							}
						} else {
							sample_buf_zero(vstTmpSig, vst->numOutputs, nFrames);
							if (vst->numInputs == 0) {
								afx->process(afx, outSig, vstTmpSig, nFrames);
								if (vst->numOutputs == 2) {
									sample_buf_add(outSig, vstTmpSig, 2, nFrames);
								} else if (vst->numOutputs == 1) {
									sample_buf_add_mono(outSig, vstTmpSig[0], 2, nFrames);
								} else {
									sample_buf_add_multi(outSig, 2, vstTmpSig, vst->numOutputs, nFrames);
								}
							} else if (vst->numInputs == 1) {	// convert stereo in to mono
								sample_buf_to_mono(outSig, nChan, nFrames);
								afx->process(afx, outSig, vstTmpSig, nFrames);
								if (vst->numOutputs == 2) {
									sample_buf_copy(outSig, vstTmpSig, 2, nFrames);
								} else if (vst->numOutputs == 1) {
									sample_buf_copy_mono(outSig, vstTmpSig[0], 2, nFrames);
								} else {
									sample_buf_copy_multi(outSig, 2, vstTmpSig, vst->numOutputs, nFrames);
								}
							} else if (vst->numInputs == 2) {
								afx->process(afx, outSig, vstTmpSig, nFrames);
								if (vst->numOutputs == 2) {
									sample_buf_copy(outSig, vstTmpSig, 2, nFrames);
								} else if (vst->numOutputs == 1) {
									sample_buf_copy_mono(outSig, vstTmpSig[0], 2, nFrames);
								} else {
									sample_buf_copy_multi(outSig, 2, vstTmpSig, vst->numOutputs, nFrames);
								}
							} else { // unheard of but not impossible
								fprintf(stderr, "unexpected vst configuration: nchan = %d nin = %d nout = %d\n", nChan, vst->numInputs, vst->numOutputs);
							}
						}
					} else if (nChan == vst->numOutputs) {
						// there's room for however many vst channels are out coming
						if (vst->canReplacing) {
							if (vst->numInputs == 0) {
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								sample_buf_add(outSig, vstTmpSig, vst->numOutputs, nFrames);
							} else if (vst->numInputs == 1) {	// convert stereo in to mono
								sample_buf_to_mono(outSig, nChan, nFrames);
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								sample_buf_copy(outSig, vstTmpSig, vst->numOutputs, nFrames);
							} else if (vst->numInputs == 2) {
								afx->processReplacing(afx, outSig, vstTmpSig, nFrames);
								sample_buf_copy(outSig, vstTmpSig, vst->numOutputs, nFrames);
							} else { // unheard of but not impossible
								fprintf(stderr, "unexpected vst configuration: nchan = %d nin = %d nout = %d\n", nChan, vst->numInputs, vst->numOutputs);
							}
						} else {
							sample_buf_zero(vstTmpSig, vst->numOutputs, nFrames);
							if (vst->numInputs == 0) {
								afx->process(afx, outSig, vstTmpSig, nFrames);
								sample_buf_add(outSig, vstTmpSig, vst->numOutputs, nFrames);
							} else if (vst->numInputs == 1) {	// convert stereo in to mono
								sample_buf_to_mono(outSig, nChan, nFrames);
								afx->process(afx, outSig, vstTmpSig, nFrames);
								sample_buf_copy(outSig, vstTmpSig, vst->numOutputs, nFrames);
							} else if (vst->numInputs == 2) {
								afx->process(afx, outSig, vstTmpSig, nFrames);
								sample_buf_copy(outSig, vstTmpSig, vst->numOutputs, nFrames);
							} else { // unheard of but not impossible
								fprintf(stderr, "unexpected vst configuration: nchan = %d nin = %d nout = %d\n", nChan, vst->numInputs, vst->numOutputs);
							}
						}
					} else {
						// bigger than two and bad... and will probably always be
						// doubt this will ever happen
						// 8 out plug to a quad send... qua would handle this differently
						fprintf(stderr, "unexpected vst configuration: nchan = %d nin = %d nout = %d\n", nChan, vst->numInputs, vst->numOutputs);
					}
				}
			}
		}
		stack->UnThunk();
// hell does the garbage collection
#endif
    	break;
    }

	case Block::C_BUILTIN: {
		switch (fxBlock->subType) {
			default: {
				break;
			}
		}
		break;
	}

	case Block::C_OUTPUT:
	case Block::C_ASSIGN:
	case Block::C_FOREACH:
	case Block::C_WITH:
	case Block::C_WAIT:
	case Block::C_SUSPEND:
	case Block::C_WAKE:
	case Block::C_BREAK:
	case Block::C_INPUT:
	default: {	// nothing to do
	
    }}
	return nFrames;
}
