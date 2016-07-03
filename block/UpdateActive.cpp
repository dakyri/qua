#include "qua_version.h"


#include "StdDefs.h"

#include "QuaMidi.h"
#include "QuaOSC.h"
#include "Expression.h"
#include "Block.h"
#include "Qua.h"
#include "Voice.h"
#include "Pool.h"
#include "Lambda.h"
#include "QuaFX.h"
#include "Channel.h"
#include "QuasiStack.h"
#include "Instance.h"
#include "SamplePlayer.h"
#include "ControllerBridge.h"
#include "State.h"

#if defined(QUA_V_VST_HOST)
#include "VstPlugin.h"
#endif
#if defined(QUA_V_ARRANGER_INTERFACE)
#include "QuaDisplay.h"
#endif

#ifdef QUA_V_DEBUG_CONSOLE
flag		debug_update=0;
#endif
/*
 * perhaps a good idea to recognise parameter expressions that are constant, and don't need constant update
 */
/*
 * UpdateActiveBlock:
 *  generally tones are buffered in the voice stream structure. this
 * may be more than one at a time, if delays and multiple generators
 * are in the path.
 *  should this be looped through? yes.???????
 */
flag
UpdateActiveBlock(Qua *uberQua,
				Stream *mainStream,
				Block *B,
				Time *updateTime,
				Stacker *stacker,
				StabEnt *stackCtxt,
				QuasiStack *stack,
				short updateRate,
				bool	generate_on_the_fly)
{
    flag		ua_complete = BLOCK_COMPLETE;
    
    if (B == nullptr)
    	return BLOCK_COMPLETE;

#ifdef QUA_V_DEBUG_CONSOLE
	if (debug_update >= 2) {
		fprintf(stderr,
			"UpdateActive (%x type %d)\n\thead %x tail %x count %d stacker %x stack %x\n",
			B,
			B?B->type:-1,
			mainStream->head, mainStream->tail,
			mainStream->nItems,
			stacker, stack);
	}
#endif

    switch (B->type) {
    
////////////////////////////////////////////
// CONTROL STRUCTURES: FLUX ////////////////
////////////////////////////////////////////
    case Block::C_FLUX: {
//		long				pitch, rhythm;
		short				rate;
		long				dyn;

		Time				cycle_len;
		ResultValue			cv;
//		flag				blocked;
		ResultValue			val;
		Time		*nextTP =
				(Time*)B->crap.flux.timeVar.StackAddressValue(stack);
		
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "flux %d\n", nextTP->ticks);
#endif		    
	    if (uberQua->theTime >= *nextTP) {
			Stream		createdStream;
			
			rate = updateRate;
			dyn = 0;

#ifdef QUA_V_DEBUG_CONSOLE
			if (debug_update) {
				fprintf(stderr, "fluxing at %d for %d : %x %x\n",
					uberQua->theTime.ticks,
					nextTP->ticks,
					mainStream->head,
					mainStream->tail);
			}
#endif			
			if (B->crap.flux.rateExp) {
				val = EvaluateExpression(B->crap.flux.rateExp,
						mainStream->head, stacker, stackCtxt, stack);
				rate = (short)val.IntValue(stack);
			}

#ifdef QUA_V_DEBUG_CONSOLE
			if (debug_update >= 2)
				fprintf(stderr, "next %d %d from %d\n",
						cycle_len.ticks,
						cv.Blocked(),
						uberQua->theTime.ticks);
#endif					
	    	if (B->crap.flux.lengthExp) {
				cv = EvaluateExpression(
								B->crap.flux.lengthExp,
								mainStream->head, stacker, stackCtxt, stack);
				if (!cv.Blocked()) {
					cycle_len.ticks = cv.IntValue(stack);
//					cycle_len.Actual = cycle_len.ticks *
//								uberQua->timeQuanta;
					ua_complete = UpdateActiveBlock(uberQua,
										&createdStream,
										B->crap.flux.block,
										updateTime,
										stacker,
										stackCtxt,
										stack,
										rate,
										generate_on_the_fly);

					*nextTP = uberQua->theTime + cycle_len;
				}
			} else  {
				// work it out later
				ua_complete = UpdateActiveBlock(uberQua,
						&createdStream,
						B->crap.flux.block, updateTime,
						stacker, stackCtxt, stack,
						rate, generate_on_the_fly);
				*nextTP = uberQua->theTime + createdStream.Duration();
#ifdef QUA_V_DEBUG_CONSOLE
				if (debug_update >= 2)
				    fprintf(stderr, "flux: created stream duration %d\n",
				    			createdStream.Duration());
#endif
			}
			mainStream->AppendStream(&createdStream);
#ifdef QUA_V_DEBUG_CONSOLE
			if (debug_update >= 2)
			    fprintf(stderr, "flux next at %d\n", nextTP->ticks);
#endif		    
	
	
	 	} else {	/* not ready to generate! */
			;		
		}
		ua_complete = BLOCK_COMPLETE;
		break;
	
	}
	
////////////////////////////////////////////
// CONTROL STRUCTURES: LISTS ///////////////
////////////////////////////////////////////
	case Block::C_LIST: {
		switch(B->subType) {

		case Block::LIST_NORM: {
#ifdef QUA_V_DEBUG_CONSOLE
			if (debug_update)
				fprintf(stderr, "UpdateActive: normal list %x %x\n", B, B->crap.list.block);
#endif
			Block		*p;
			p = B->crap.list.block;
			short i=0;
	
		    uchar	*childCompleted =
		    		B->crap.list.stuffVar.StackAddressValue(stack);
			ua_complete = BLOCK_COMPLETE;
			while (p != nullptr) {
#ifdef QUA_V_DEBUG_CONSOLE
				if (debug_update >= 2)
					fprintf(stderr, "UpdateActive: normal list element %x bl %d comp %d\n", B, i, childCompleted[i]);
#endif
				if (!childCompleted[i] || childCompleted[i] == BLOCK_COOKED) {
					childCompleted[i] =
							UpdateActiveBlock(uberQua,
								mainStream,
								p,
								updateTime,
								stacker, stackCtxt,
								stack,
								updateRate,
								generate_on_the_fly);
				}
#ifdef QUA_V_DEBUG_CONSOLE
				if (debug_update >= 2)
					fprintf(stderr, "UpdateActive: completed list element %x bl %d comp %d\n", B, i, childCompleted[i]);
#endif	
	//
	// should this have a line to check for completeness??
	// this may cause other trouble
	//			if (ua_complete)
	// this should now work on the fly, but is it guaranteed to complete
	// when not???????????????????????????????????????
	//
	
				ua_complete = ua_complete && childCompleted[i];
				p = p->next;
				i++;
			}
			if (ua_complete)
				for (i=0; i<B->crap.list.nChildren; i++)
					childCompleted[i] = BLOCK_INCOMPLETE;
					
	//		ua_complete = BLOCK_COMPLETE;
			break;
		}
		
		case Block::LIST_FORK:
		case Block::LIST_PAR: {
			Block		*p = B->crap.list.block;
			while (p != nullptr) {
				ua_complete = UpdateActiveBlock(uberQua,
								mainStream, p,
								updateTime,
								stacker, 
								stackCtxt,
								stack,
								updateRate, 
								generate_on_the_fly);
#ifdef QUA_V_DEBUG_CONSOLE
				if (debug_update)
					fprintf(stderr, "parallel list element list %x bl comp\n", B);
#endif	
				p = p->next;
			}
			ua_complete = BLOCK_COMPLETE;
			break;
		}
	
		case Block::LIST_SEQ: {
			Block		*p;
	
			if (!generate_on_the_fly) {		
				p = B->crap.list.block;
		
				while (p != nullptr) {
					ua_complete = UpdateActiveBlock(uberQua,
							mainStream,
							p,
							mainStream->nItems==0?
								updateTime:
								&mainStream->EndTime(),
							stacker,
							stackCtxt,
							stack, updateRate,
							generate_on_the_fly);
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "sequential list element %d\n", mainStream->nItems);
#endif
					if (ua_complete)
						p = p->next;
				}
				ua_complete = BLOCK_COMPLETE;
			} else {
				Block	*active =
					(Block*)B->crap.list.stuffVar.IntValue(stack);		
				if (active == nullptr)
					active = B->crap.list.block;
		
				p = active;
						
				if (p != nullptr) {
					int			cnt	= mainStream->nItems;
					Block		*q = p;
			
					while (mainStream->nItems == cnt) {
						ua_complete = UpdateActiveBlock(
											uberQua,
											mainStream,
											p,
											updateTime,
											stacker,
											stackCtxt,
											stack,
											updateRate,
											generate_on_the_fly);
		
						if (ua_complete) {	// block p complete
							if ((active = p->next) == nullptr) {
								ua_complete = BLOCK_COMPLETE;
								active = B->crap.list.block;	// more from here !!
							} else {
								ua_complete = BLOCK_INCOMPLETE;
							}
							
							p = active;					
							if (p == q) {	// no infinite loop
								ua_complete = BLOCK_COMPLETE;
								break;
							}
						} else {
							break;
						}
					}
					
				} else {
					ua_complete = BLOCK_COMPLETE;
				}
				B->crap.list.stuffVar.SetValue((int32)active, stack);
			}
			break;
		}}
		break;
	}
	
///////////////////////////////////////////////////
// GENERIC PLAYER CALL /////////////////////////////
///////////////////////////////////////////////////
	case Block::C_GENERIC_PLAYER: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2) {
		    fprintf(stderr, "sample player\n");
		}
#endif
		ua_complete = BLOCK_INCOMPLETE;
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(B->crap.call.frameIndex);
		SamplePlayInfo	*inf = B->crap.call.crap.splayer;
		Stackable *stackable = inf->playerSym->BuiltinValue()->stackable;
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			ResultValue		the_val;
			int				npar = 0;
			for (Block *bp=B->crap.call.parameters; bp!=nullptr; bp=bp->next) {
				the_val = EvaluateExpression(bp,mainStream->head,
							stacker, stackCtxt, stack);
				if (the_val.Blocked()) {
					return BLOCK_INCOMPLETE;
				}
//				if (!the_val.complete)
//					ua_complete = BLOCK_COMPLETE;
				StabEnt		*ctlsym = stackable->controller(npar);
				if (ctlsym && bp->type != Block::C_ASSIGN) {
					LValue		lval;
					ctlsym->SetLValue(lval,
									mainStream->head,
									stacker,
									stackCtxt,
									higherFrame);
					lval.StoreValue(&the_val);
				}
				npar++;
			}
		}
		stack->UnThunk();
// hell does the garbage collection
   		break;
	}

///////////////////////////////////////////////////
// STREAM PLAYER CALL /////////////////////////////
///////////////////////////////////////////////////
	case Block::C_STREAM_PLAYER: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2) {
		    fprintf(stderr, "UpdateActive: stream player\n");
		}
#endif
		ua_complete = BLOCK_INCOMPLETE;
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(B->crap.call.frameIndex);
		StreamPlayInfo	*inf = B->crap.call.crap.mplayer;
		Stackable *stackable = inf->playerSym->BuiltinValue()->stackable;
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			ResultValue		the_val;
			int				npar = 0;
			for (Block *bp=B->crap.call.parameters; bp!=nullptr; bp=bp->next) {
				the_val = EvaluateExpression(bp,mainStream->head,
							stacker, stackCtxt, stack);
				if (the_val.Blocked()) {
					return BLOCK_INCOMPLETE;
				}
				npar++;
//				if (!the_val.complete)
//					ua_complete = BLOCK_COMPLETE;
				StabEnt		*ctlsym = stackable->controller(npar);
				if (ctlsym) {
					if (bp->type != Block::C_ASSIGN) {
						LValue		lval;
						ctlsym->SetLValue(lval,	mainStream->head,
												stacker, stackCtxt,
												higherFrame);
						lval.StoreValue(&the_val);
					}
				}
			}

			Clip *clip = (Clip *) inf->clipVar.PointerValue(stack);	// pointer to a clip
			int		state = 0;
			if (clip != nullptr) {
				Take *media = clip->media;
				if (media && media->type == Take::STREAM) {

// play the clip ///////////////////////////////////////
// grab the tones scheduled for now and play them
//				float gain = inf->gainVar.FloatValue(stack);	// play gain
//				float pan = inf->panVar.FloatValue(stack);	// play pan
//				flag dir = inf->directionVar.IntValue(stack);	// forwards or backwards

					flag			loop = inf->loopmodeVar.IntValue(stack);	// looping on or off
					float			rate = inf->rateVar.FloatValue(stack);	// play pitch
					StreamItem		*nextItem = (StreamItem *)inf->posVar.PointerValue(stack);
					Time			lastLoopStartTime = inf->loopstartVar.TimeValue(stack);

					StreamTake		*take = (StreamTake *)media;
					Stream			*stream = &take->stream;

					Time			loopTime = (*updateTime-lastLoopStartTime);

					if (loopTime.ticks <= 0 || nextItem == nullptr) {
						lastLoopStartTime = *updateTime;
						inf->loopstartVar.SetValue(updateTime, stack);
						loopTime.ticks = 0;
						nextItem = stream->IndexTo(clip->start);
					} else {
						if (loopTime >= (clip->duration)) {
					// what if clip and t are in different meters? think i'm ok here,
					// but should be careful to preserve the metrics
							if (loop) {	// set a loop end state, and keep chugging
								state = PLAYSTATE_LOOPEND;
								loopTime.ticks = 0;
								lastLoopStartTime = *updateTime;
								inf->loopstartVar.SetValue(updateTime, stack);
								nextItem = stream->IndexTo(clip->start);
							} else { // bug out and fill with zeros
								state = PLAYSTATE_LOOPEND;
								nextItem = nullptr;
							}
						}
					}

					if (nextItem) {
						loopTime += clip->start;
						while (nextItem != nullptr && nextItem->time <= loopTime) {
							mainStream->AddToStream(nextItem, &uberQua->theTime);
							nextItem = nextItem->next;
						}
					}
					inf->posVar.SetValue((int32)nextItem, stack);
					inf->looptimeVar.SetValue(&loopTime, stack);
				}
				inf->stateVar.SetValue((int32)state, stack);		// is playing or running or just stopped
			} else { // null clipe
				;
			}
		}
		stack->UnThunk();
// hell does the garbage collection
   		break;
	}

///////////////////////////////////////////////////
// SAMPLE PLAYER CALL /////////////////////////////
///////////////////////////////////////////////////
	case Block::C_SAMPLE_PLAYER: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2) {
		    fprintf(stderr, "UpdateActive: sample player\n");
		}
#endif
		ua_complete = BLOCK_INCOMPLETE;
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(B->crap.call.frameIndex);
		SamplePlayInfo	*inf = B->crap.call.crap.splayer;
		Stackable *stackable = inf->playerSym->BuiltinValue()->stackable;
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			ResultValue		the_val;
			int				npar = 0;
			for (Block *bp=B->crap.call.parameters; bp!=nullptr; bp=bp->next) {
				the_val = EvaluateExpression(bp, mainStream->head, stacker, stackCtxt, stack);
				if (the_val.Blocked()) {
					return BLOCK_INCOMPLETE;
				}
				npar++;
//				if (!the_val.complete)
//					ua_complete = BLOCK_COMPLETE;
				StabEnt		*ctlsym = stackable->controller(npar);
				if (ctlsym) {
					if (bp->type != Block::C_ASSIGN) {
						LValue		lval;
						ctlsym->SetLValue(lval, mainStream->head,
												stacker, stackCtxt,
												higherFrame);
						lval.StoreValue(&the_val);
					}
				}
			}
		}
		stack->UnThunk();
// hell does the garbage collection
   		break;
	}

///////////////////////////////////////////////////
// TUNED SAMPLE PLAYER CALL ///////////////////////
///////////////////////////////////////////////////
	case Block::C_TUNEDSAMPLE_PLAYER: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2) {
		    fprintf(stderr, "UpdateActive: sample player\n");
		}
#endif
		ua_complete = BLOCK_INCOMPLETE;
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(B->crap.call.frameIndex);
		TunedSamplePlayInfo	*inf = B->crap.call.crap.tplayer;
		Stackable *stackable = inf->playerSym->BuiltinValue()->stackable;
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			ResultValue		the_val;
			int				npar = 0;
			for (Block *bp=B->crap.call.parameters; bp!=nullptr; bp=bp->next) {
				the_val = EvaluateExpression(bp, mainStream->head, stacker, stackCtxt, stack);
				if (the_val.Blocked()) {
					return BLOCK_INCOMPLETE;
				}
				npar++;
//				if (!the_val.complete)
//					ua_complete = BLOCK_COMPLETE;
				StabEnt		*ctlsym = stackable->controller(npar);
				if (ctlsym) {
					if (bp->type != Block::C_ASSIGN) {
						LValue		lval;
						ctlsym->SetLValue(lval, mainStream->head,
												stacker, stackCtxt,
												higherFrame);
						lval.StoreValue(&the_val);
					}
				}
			}
		}
		stack->UnThunk();
// hell does the garbage collection
   		break;
	}

///////////////////////////////////////////////////
// MIDI PLAYER CALL ///////////////////////////////
///////////////////////////////////////////////////
	case Block::C_MIDI_PLAYER: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2) {
		    fprintf(stderr, "UpdateActive: midi sample player\n");
		}
#endif
		ua_complete = BLOCK_INCOMPLETE;
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(B->crap.call.frameIndex);
		SamplePlayInfo	*inf = B->crap.call.crap.splayer;
		Stackable *stackable = inf->playerSym->BuiltinValue()->stackable;
		if (higherFrame->locusStatus == STATUS_RUNNING) {
			ResultValue		the_val;
			int				npar = 0;
			for (Block *bp=B->crap.call.parameters; bp!=nullptr; bp=bp->next) {
				the_val = EvaluateExpression(bp,mainStream->head,
							stacker, stackCtxt, stack);
				if (the_val.Blocked()) {
					return BLOCK_INCOMPLETE;
				}
				npar++;
//				if (!the_val.complete)
//					ua_complete = BLOCK_COMPLETE;
				StabEnt		*ctlsym = stackable->controller(npar);
				if (ctlsym) {
					if (bp->type != Block::C_ASSIGN) {
						LValue		lval;
						ctlsym->SetLValue(lval, mainStream->head,
												stacker, stackCtxt,
												higherFrame);
						lval.StoreValue(&the_val);
					}
				}
			}
// check for valid note on events, and restart/retune/etc
// perhaps should stop it b4 setting params, as they may be
// note dependent.
		}
		stack->UnThunk();
// hell does the garbage collection
   		break;
	}

///////////////////////////////////////////////////
// VST PLUGIN CALL/////////////////////////////////
///////////////////////////////////////////////////
#if defined (QUA_V_VST_HOST)
    case Block::C_VST: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2) {
		    fprintf(stderr, "UpdateActive: vst\n");
		}
#endif
		ua_complete = true;
		stack->Thunk();
		QuasiStack	*higherFrame = stack->FrameAt(B->crap.call.frameIndex);

		AEffect	*afx = higherFrame->stk.afx;
		Stackable *stackable = B->crap.call.crap.vstplugin;
		if (afx && higherFrame->locusStatus == STATUS_RUNNING) {
// fill in calculated parameters
			short positionalParam = 0;
			for (Block *bp=B->crap.call.parameters; bp!=nullptr; bp=bp->next) {
				
				ResultValue v = EvaluateExpression(bp,mainStream->head,
							stacker, stackCtxt, stack);
				if (bp->type != Block::C_ASSIGN && !v.Blocked()) {
					afx->setParameter(afx, positionalParam, v.FloatValue(higherFrame));
				}
				positionalParam++;
			}
			B->crap.call.crap.vstplugin->OutputStream(afx, mainStream);
// fill in interface parameters
// hopefully vst params will have this done already!
//			for (QuaControllerBridge *cvp=higherFrame->controlVariables;
//					cvp!=nullptr; cvp=cvp->next) {
//				LValue	l = cvp->stabEnt->SetLValue(nullptr, stacker, higherFrame);
//			}
		}
		stack->UnThunk();
// hell does the garbage collection
   		break;
    }
#endif

///////////////////////////////////////////////////
// OLD AUDIO ADDON FROM BE ORIG ///////////////////
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// ORDINARY METHOD CALL ///////////////////////////
///////////////////////////////////////////////////
	case Block::C_UNLINKED_CALL:
		break;
	case Block::C_CALL: {
		stack->Thunk();
		QuasiStack	*higherFrame = stack->frameAt(B->crap.call.frameIndex);
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update)
			fprintf(stderr, "UpdateActive: call %x of %s frame %x # %d stat %d\n",
					B,
			    	B->crap.call.crap.lambda->sym->name,
			    	higherFrame,
			    	B->crap.call.frameIndex,
			    	higherFrame->locusStatus);
#endif
		if (higherFrame->locusStatus == STATUS_RUNNING) {
// may also need to check lambda status
			Block		*p;
			Lambda		*lambda = B->crap.call.crap.lambda;
#ifdef LOTSALOX
			if (lambda->stackableLock.Lockable()) {
				lambda->stackableLock.Lock();
#endif
	
				ua_complete = BLOCK_COMPLETE;
				long	npar = 0;
				if (!higherFrame->isActive) {
					for (p=B->crap.call.parameters; p!=nullptr; p=p->next) {
						ResultValue the_val =
							EvaluateExpression(
								p,mainStream->head, stacker, stackCtxt, stack);
						if (the_val.Blocked()) {
#ifdef LOTSALOX
							S->stackableLock.Unlock();
#endif
							return BLOCK_INCOMPLETE;
						}
						if (!the_val.Complete())
							ua_complete = BLOCK_COMPLETE;
						StabEnt		*ctlsym = lambda->controller(npar);
						if (ctlsym) {
							LValue		lval;
							ctlsym->SetLValue(
									lval,
									mainStream->head,
									stacker, stackCtxt,
									higherFrame);
							lval.StoreValue(&the_val);
						}
						npar++;
					}
					higherFrame->isActive = true;
				}
							
				ua_complete = UpdateActiveBlock(uberQua,
								mainStream, 
								B->crap.call.crap.lambda->mainBlock,
								updateTime, 
								stacker, 
								stackCtxt,
								higherFrame,
								updateRate, generate_on_the_fly)
							&& ua_complete;
#ifdef LOTSALOX
				lambda->stackableLock.Unlock();
			}
#endif

			if (ua_complete) {
				higherFrame->isActive = false;
				if (B->crap.call.crap.lambda->isOncer) {
					higherFrame->locusStatus = STATUS_SLEEPING;
#if defined(QU_V_CONTROLLER_INTERFACE)
					if (higherFrame->controlPanel) {
						BMessage	dispMsg(DISPLAY_STATUS);
						higherFrame->controlPanel->Window()->
							PostMessage(&dispMsg, higherFrame->controlPanel);
					}
#endif
				}
				stack->UnThunk();
			// hell does the garbage collection
			}
		}
		break;
	}

	case Block::C_SCHEDULE: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "schedule %s\n", B->crap.call.crap.sym->name);
#endif
		// schedule this for whenever, or now...
		Schedulable		*s = B->crap.call.crap.sym?
				B->crap.call.crap.sym->SchedulableValue():nullptr;
		if (s) {
			s->Schedule(B->crap.call.parameters);
		}
		break;
    }


///////////////////////////////////////////////////
// SCHEDULING CALL ////////////////////////////////
///////////////////////////////////////////////////
	case Block::C_WAKE: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "wake %s\n", B->crap.call.crap.sym->name);
#endif
		// schedule this for whenever, or now...
		internalError("wake unimplimented");
		B->type = Block::C_UNKNOWN;
		ua_complete = true;
		break;
    }

	case Block::C_SUSPEND: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "suspend %s\n", B->crap.sym->name);
#endif
		internalError("suspend unimplimented");
		B->type = Block::C_UNKNOWN;
		ua_complete = true;
		break;
    }

	case Block::C_DIVERT: {
		ResultValue	tag_val;
	
		tag_val = EvaluateExpression(B->crap.divert.clockExp,
							mainStream->head, stacker, stackCtxt, stack);
		ua_complete = BLOCK_INCOMPLETE;
		if (!tag_val.Blocked()) {
			Time	tag(tag_val.IntValue(stack), uberQua->metric);
			Stream	diverted;
//			StreamItem	*p=nullptr;
			
			ua_complete = UpdateActiveBlock(uberQua,
					&diverted,
					 B->crap.divert.block,
					 &tag, 
					 stacker, 
					 stackCtxt,
					 stack,
					 updateRate, 
					 generate_on_the_fly);
			mainStream->AppendStream(&diverted);
		}
		break;
    }

#ifdef STATEMENT_ASSIGNEMTS
//////////////////////////////////////////////////////////////
// ASSIGNMENTS NOW HANDLED WITHIN EXPRESSION /////////////////
//////////////////////////////////////////////////////////////
	case Block::C_ASSIGN: {
//		int				i;
		StreamItem	*p;
//		StabEnt	*sym;
		ResultValue			val;
	
		if (debug_update >= 2)
		    fprintf(stderr, "assign %x %x str %x %x\n", B, B->next,
		    				mainStream->head, mainStream->tail);
	
		p = mainStream->head;
		
		LValue		lval = LValueAte(B->crap.assign.atom, p, stacker, stack);
		if (lval.sym->RefersToStream()) {
			while (p != nullptr) {
			    val = EvaluateExpression(
			    			B->crap.assign.exp,
			    			p, stacker, stack);
			    if (!val.blocked) {
			    	lval.StoreValue(&val);
			    }
				p = p->next;
				if (p) {
					lval = LValueAte(B->crap.assign.atom, p, stacker, stack);
				}
			}
		} else {
			val = EvaluateExpression(B->crap.assign.exp, mainStream->head, stacker, stack);
			if (!val.blocked) {
			    lval.StoreValue(&val);
				lval.sym->UpdateControllerBridge(updateTime, &val, stack);
				// and if this is a display var, display it
			}
		}
	
		ua_complete = BLOCK_COMPLETE;
		break;
	}
#endif

////////////////////////////////////////////
// CONTROL STRUCTURES: REPEAT //////////////
////////////////////////////////////////////
    case Block::C_REPEAT: {
		ResultValue	expV;		
		long	count = B->crap.repeat.countVar.IntValue(stack);

		if (count == 0) {
			expV = EvaluateExpression(B->crap.repeat.Exp,
				mainStream->head, stacker, stackCtxt, stack);
			if (!expV.Blocked()) {
				B->crap.repeat.boundVar.SetValue(
										expV.IntValue(stack),
										stack);
				count = 1;
			}
		}

		if (count <= B->crap.repeat.boundVar.IntValue(stack)) {
		    ua_complete = UpdateActiveBlock(uberQua, mainStream,
			    	 B->crap.repeat.block,
					 updateTime,
					 stacker,
					 stackCtxt,
					 stack,
			    	 updateRate,
					 generate_on_the_fly);
			if (ua_complete)
				count++;
			ua_complete = false;
		} else {
			count = 0;
			ua_complete = true;
		}

		B->crap.repeat.countVar.SetValue((int32)count, stack);

		break;
    }
    
////////////////////////////////////////////
// CONTROL STRUCTURES: IF //////////////////
////////////////////////////////////////////
	case Block::C_IF: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update)
			fprintf(stderr, "if: cond %d doeval %d flygen %d\n",
				B->crap.iff.condVar.IntValue(stack),
				B->crap.iff.doEvalVar.IntValue(stack),
				generate_on_the_fly);
#endif	
	/* maybe the simplest is the best !!! */
		bool	block = false;
		if (	!generate_on_the_fly ||
				B->crap.iff.doEvalVar.BoolValue(stack)) {
			ResultValue v	 = EvaluateExpression(B->crap.iff.condition,
					 mainStream->head, stacker, stackCtxt, stack);
			B->crap.iff.condVar.SetValue(v.IntValue(stack),stack);
			block = v.Blocked();
		}
		ua_complete = BLOCK_COMPLETE;
		
		if (block) {
			ua_complete = BLOCK_INCOMPLETE;
		    B->crap.iff.condVar.SetValue((int32)true,stack);
		} else {
			if (B->crap.iff.condVar.IntValue(stack)) {
			    ua_complete = UpdateActiveBlock(
					 uberQua, 
					 mainStream,
			    	 B->crap.iff.ifBlock,
					 updateTime,
					 stacker,
					 stackCtxt,
					 stack,
			    	 updateRate,
					 generate_on_the_fly);
			    if (B->crap.iff.elseBlock != nullptr)
			    	B->crap.iff.elseBlock->FlushOutput(false);
			    B->crap.iff.doEvalVar.SetValue((int32)ua_complete, stack);
			} else {
			    B->crap.iff.ifBlock->FlushOutput(false);
			    if (B->crap.iff.elseBlock != nullptr) {
		 	        ua_complete = UpdateActiveBlock(uberQua, mainStream,
		 	        		 B->crap.iff.elseBlock,
							 updateTime,
							 stacker,
							 stackCtxt,
							 stack,
		 	        		 updateRate,
							 generate_on_the_fly);
				    B->crap.iff.doEvalVar.SetValue((int32)ua_complete,stack);
			    } 
			}
		}
		break;
	}
	
////////////////////////////////////////////
// CONTROL STRUCTURES: FOREACH /////////////
////////////////////////////////////////////
	case Block::C_FOREACH: {
		Stream		temp;

		if (B->crap.foreach.condition) {
			for (StreamItem *p=mainStream->head; p!=nullptr; p=p->next) {
				 ResultValue val = EvaluateExpression(
				    			B->crap.foreach.condition,
				    			p, stacker, stackCtxt, 
				    			stack);
				 if (!val.Blocked()) {
					temp.nItems = 1;
					temp.head = p;
					StreamItem	*q = p->next;
					if (val.BoolValue(stack)) {
						ua_complete = UpdateActiveBlock(uberQua,
							&temp,
							B->crap.foreach.ifBlock,
							updateTime,
							stacker,
							stackCtxt,
							stack,
							updateRate,
							generate_on_the_fly);
					} else {
						ua_complete = UpdateActiveBlock(uberQua,
							&temp, 
							B->crap.foreach.elseBlock,
							updateTime,
							stacker, stackCtxt, 
							stack, 
							updateRate,
							generate_on_the_fly);
					}
					p->next = q;
				 }
			}
		} else {
			for (StreamItem *p=mainStream->head; p!=nullptr; p=p->next) {
				temp.nItems = 1;
				temp.head = p;
				StreamItem	*q = p->next;
				ua_complete = UpdateActiveBlock(uberQua,
					&temp,
					B->crap.foreach.ifBlock,
					updateTime,
					stacker,
					stackCtxt,
					stack,
					updateRate,
					generate_on_the_fly);
			}
		}
		ua_complete = BLOCK_COMPLETE;
		break;
	}
	
////////////////////////////////////////////
// CONTROL STRUCTURES: WITH ////////////////
////////////////////////////////////////////
	case Block::C_WITH: {
#ifdef OLD_WITH
		Stream		temp;

		for (StreamItem *p=mainStream->head; p!=nullptr; p=p->next) {
			 ResultValue val = EvaluateExpression(
			    			B->crap.with.condition,
			    			p, stacker, 
			    			stack);
			 if (!val.blocked) {
				temp.nItems = 1;
				temp.head = p;
				StreamItem	*q = p->next;
				if (val.BoolValue(stack)) {
					ua_complete = UpdateActiveBlock(uberQua,
						&temp,
						B->crap.with.withBlock,
						updateTime,
						stacker, 
						stackCtxt,
						stack,
						updateRate, 
						generate_on_the_fly);
				} else {
					ua_complete = UpdateActiveBlock(uberQua,
						&temp, B->crap.with.withoutBlock, updateRate,
						updateTime, generate_on_the_fly,
						stacker, stack);
				}
				p->next = q;
			 }
		}
#else
		Stream		elseStream;

		if (mainStream->nItems > 0) {

			mainStream->Split(
						(cmd_t)B->crap.with.objectType->type,
						B->crap.with.condition,
						stacker, stackCtxt, stack,
						&elseStream);
			if (mainStream->nItems > 0 && B->crap.with.withBlock) {
				ua_complete = UpdateActiveBlock(uberQua,
					mainStream,
					B->crap.with.withBlock,
					updateTime,
					stacker,
					stackCtxt,
					stack,
					updateRate,
					generate_on_the_fly);
			}
			if (elseStream.nItems > 0 && B->crap.with.withoutBlock) {
				ua_complete = UpdateActiveBlock(uberQua,
					&elseStream, 
					B->crap.with.withoutBlock,
					updateTime,
					stacker,
					stackCtxt,
					stack, 
					updateRate, 
					generate_on_the_fly);
			}
			if (elseStream.nItems > 0) {
				mainStream->Merge(&elseStream);
			}
		}
			
#endif
		ua_complete = BLOCK_COMPLETE;
		break;
	}
	
////////////////////////////////////////////
// CONTROL STRUCTURES: WITH ////////////////
////////////////////////////////////////////
	case Block::C_WAIT: {
		if (generate_on_the_fly) {
			ResultValue v = EvaluateExpression(B->crap.block,
					 mainStream->head, stacker, stackCtxt, stack);
			ua_complete = (!v.Blocked() && v.BoolValue(stack));
		} else
			ua_complete = BLOCK_COMPLETE;
		break;
	}

////////////////////////////////////////////
// CONTROL STRUCTURES: GUARD ///////////////
////////////////////////////////////////////
	case Block::C_GUARD: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "guard %x %x\n", B, B->next);
#endif
		bool	block=false;
		if (!(generate_on_the_fly && !B->crap.guard.doEvalVar.IntValue(stack))) {
			ResultValue v = EvaluateExpression(B->crap.guard.condition,
					 mainStream->head,
					 stacker, stackCtxt, stack);
			block = v.Blocked();
			B->crap.guard.condVar.SetValue(v.IntValue(stack),stack);
		}

		do {
			ua_complete = BLOCK_COMPLETE;
			
			if (block) {
				ua_complete = BLOCK_INCOMPLETE;
			    B->crap.guard.doEvalVar.SetValue((int32)true, stack);
			} else if (B->crap.guard.doEvalVar.IntValue(stack)) {
				if (B->crap.guard.condVar.IntValue(stack)) {
				    ua_complete = UpdateActiveBlock(uberQua, mainStream,
				    	 B->crap.guard.block,
						 updateTime,
						 stacker,
						 stackCtxt,
						 stack,
				    	 updateRate, 
						 generate_on_the_fly);
				    B->crap.guard.doEvalVar.
				    		SetValue((int32)ua_complete, stack);
				    ua_complete = BLOCK_INCOMPLETE;
				} else {
					B->crap.guard.doEvalVar.
							SetValue((int32)true,stack);
					ua_complete = BLOCK_COMPLETE;
				}
				
			} else {
			    ua_complete = UpdateActiveBlock(uberQua, mainStream,
			    	 B->crap.guard.block,
					 updateTime, 
					 stacker,
					 stackCtxt,
					 stack,
			    	 updateRate,
					 generate_on_the_fly);
			    B->crap.guard.doEvalVar.SetValue((int32)ua_complete, stack);
			    ua_complete = BLOCK_INCOMPLETE;
			}
		} while (!generate_on_the_fly && !ua_complete);
		break;
    }

////////////////////////////////////////////
// CONTROL STRUCTURES: BREAK ///////////////
////////////////////////////////////////////
	case Block::C_BREAK: {
	
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "break\n");
#endif
		ua_complete = BLOCK_COMPLETE;
		break;
    }

//////////////////////////////////////////////////////////////
// DIRECT OUTPUT FROM STREAM /////////////////////////////////
//////////////////////////////////////////////////////////////
	case Block::C_OUTPUT: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 1)
		    fprintf(stderr, "output %x str %x %d\n", B, 
						 mainStream->head, mainStream->nItems);
#endif	
		B->crap.channel->OutputStream(mainStream);
// ??????????????????????????????????? this will let more
// interesting sequences be done, but will block output in
// sequenced block lists 
//		ua_complete = BLOCK_COMPLETE;
		ua_complete = BLOCK_COOKED;	// maybe ???????????
		break;
    }

//////////////////////////////////////////////////////////////
// DIRECT INPUT INTO STREAM //////////////////////////////////
//////////////////////////////////////////////////////////////
	case Block::C_INPUT: {
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update >= 2)
		    fprintf(stderr, "input %d %d\n", B->crap.constant.value, mainStream->nItems);
#endif	
		B->crap.channel->InputStream(mainStream);
#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update) {
		    fprintf(stderr, "\tgot %d\n", mainStream->nItems);
		}
#endif	
		ua_complete = BLOCK_COOKED;
		break;
    }


//////////////////////////////////////////////////////////////
// BUILTINS THAT OPERATE DIRECTLY ON THE STREAM //////////////
//////////////////////////////////////////////////////////////
	case Block::C_BUILTIN: {
	
		switch (B->subType) {
			case Block::BUILTIN_FLUSH: {
#ifdef QUA_V_DEBUG_CONSOLE
				if (debug_update >= 2) {
					fprintf(stderr, "sink\n");
				}
#endif		
				mainStream->ClearStream();
				return BLOCK_COMPLETE;
			}
		}
		// other builtin ... as an expression
		// fall through to the default !!!!!
    }
    
//////////////////////////////////////////////////////////////
// AN EXPRESSION OR ASSIGNMENT ///////////////////////////////
//////////////////////////////////////////////////////////////
	default: {	
    	ResultValue		val;

#ifdef QUA_V_DEBUG_CONSOLE
		if (debug_update) {
			fprintf(stderr, "UpdateActive: expression %d tag_t %d tq %g\n",
		     		uberQua->theTime.ticks, updateTime->ticks, uberQua->secsPerTick);
		}
#endif
		do {		
			val = EvaluateExpression(B,	mainStream->head, stacker, stackCtxt, stack);

			if (val.type == TypedValue::S_LIST) {
#ifdef OLD_LIST
				for (short i=0; i<val.ListValue()->Count; i++) {
					mainStream->AddToStream(
							&val.ListValue()->Items[i],
							updateTime);
				}
				delete val.ListValue();
#else
//				for (short i=0; i<val.ListValue()->Count; i++) {
//					mainStream->AddToStream(
//							&val.ListValue()->Items[i],
//							updateTime);
//				}
				val.ListValue()->Clear();
#endif // OLD_LIST
			} else {
				switch (val.type) {
				case TypedValue::S_NOTE: {
					Note		*tp = val.NoteValue();
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", tp);
#endif						
					if (tp) {
						tp->duration /= updateRate;
						mainStream->AddToStream(tp, updateTime);
					}
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
				    	mainStream->PrintStream(stderr);
#endif						
				    break;
				}
#ifdef QUA_V_APP_HANDLER				
				case TypedValue::S_MESSAGE: {
					BMessage		*mp = val.MessageValue();
					
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", mp);
#endif	
				    mainStream->AddToStream(mp, updateTime);
				    break;
				}
#endif
				case TypedValue::S_CTRL: {
					Ctrl		*mp = val.CtrlValue();
					
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", mp);
#endif	
						
				    mainStream->AddToStream(mp, updateTime);
				    break;
				}
				
				case TypedValue::S_BEND: {
					Bend		*mp = val.BendValue();
					
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", mp);
#endif	
						
				    mainStream->AddToStream(mp, updateTime);
				    break;
				}
				
				case TypedValue::S_PROG: {
					Prog		*mp = val.ProgValue();
					
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", mp);
#endif	
						
				    mainStream->AddToStream(mp, updateTime);
				    break;
				}
				
				case TypedValue::S_SYSX: {
					SysX		*mp = val.SysXValue();
					
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", mp);
#endif	
						
				    mainStream->AddToStream(mp, updateTime);
				    break;
				}
				
				case TypedValue::S_SYSC: {
					SysC		*mp = val.SysCValue();
					
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created %x\n", mp);
#endif
						
				    mainStream->AddToStream(mp, updateTime);
				    break;
				}
				
				case TypedValue::S_STREAM_ITEM: {
#ifdef QUA_V_DEBUG_CONSOLE
					if (debug_update)
						fprintf(stderr, "created (not) str val %d\n", val.type);
#endif
						
				    mainStream->AddToStream(&val, updateTime);
				    break;
				}
				
				default:
					break;
				}
			}
		} while (!generate_on_the_fly && !val.Complete());
		ua_complete = val.Complete();
	
    }}

#ifdef QUA_V_DEBUG_CONSOLE
	if (debug_update >= 2)
        fprintf(stderr, "\texit update.1 %x %s %d\n", ua_complete, "", mainStream->nItems);
#endif
    return ua_complete;
}

