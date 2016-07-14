#include "qua_version.h"

#include "StdDefs.h"

#include "Markov.h"
#include "Other.h"
#include "MidiDefs.h"

extern flag debug_gen;

/*
 * IntToInd:
 *  convert an interval to an array index
 */
int
int_to_ind(int interv)
{
    return interv + INT_UNISON;
}

int
ind_to_int(int ind)
{
    return ind - INT_UNISON;
}


Note &
GenNextNote(struct MarkovInfo *minf)
{
    int		i=0,
			pitch,
			interv,
			last_interv;    
    Note	noteTo;
    float	dur;
    bool	silent=false;

    noteTo.dynamic = (vel_t)minf->baseNoteVelocity;
	noteTo.pitch = 255;
	noteTo.cmd = MIDI_CMD_NOT;
    
// rhythmic forms ....
//    if (minf->noteHistory[0].cmd == CMD_END_PHRASE) { /* first note */
//        minf->rFormNo = -1;
//    }
   	if (minf->rFormNo >= minf->rMarkov->nIsos) /* pathological case, ??????
   						there is a bug somewhere */
   	    minf->rFormNo = -1;
    if (minf->rFormNo < 0) {
    	minf->rFormNo = select_list(minf->rMarkov->nIsos,
				minf->rMarkov->startCell);
    	minf->rFormInd = 0;
    	if (debug_gen)
			fprintf(stderr, "start r-motif %d\n", minf->rFormNo);
	}
	
	dur = minf->rMarkov->isos[minf->rFormNo][minf->rFormInd].dur;
	silent = minf->rMarkov->isos[minf->rFormNo][minf->rFormInd].silent;
	minf->rFormInd++;
	if (dur == 0) {
		minf->rFormNo = select_list(minf->rMarkov->nIsos,
						minf->rMarkov->rCellMap[minf->rFormNo]);
		minf->rFormInd = 0;
		dur = minf->rMarkov->isos[minf->rFormNo][minf->rFormInd].dur;
		silent = minf->rMarkov->isos[minf->rFormNo][minf->rFormInd].silent;
		minf->rFormInd++;
		if (debug_gen)
			fprintf(stderr, "new r-motif %d\n", minf->rFormNo);
	}
	if (minf->rFormInd == 1) { /* first in a grouping */
    	noteTo.dynamic += minf->cellEmph;
	}
    noteTo.duration = dur;
	AttributeList currentAttribs;

	if (!silent) {
		short last = 0, next_last = 0;
		for (last=0;last < MAX_GEN_HISTORY-1 && minf->noteHistory[last].pitch >127; last++);
		for (next_last=last+1; next_last < MAX_GEN_HISTORY && minf->noteHistory[next_last].pitch >127; next_last++);

	    if (minf->attribHistory[last].has(Attribute::PHRASE_END)) { /* first note */
	    	pitch = select_list(MAX_ABS_NOTE, minf->markov->startNote);
	    } else if (minf->attribHistory[next_last].has(Attribute::PHRASE_END)) { /* first interval */
	    	interv = ind_to_int(select_list(MAX_INTERVALS, minf->markov->startInterval));
			pitch = minf->noteHistory[last].pitch + interv;
	    } else {
			int	chk, cnt;
		
			cnt = 0;
			last_interv = minf->noteHistory[last].pitch -
								minf->noteHistory[next_last].pitch;
			chk = select_list(MAX_INTERVALS, minf->markov->intervalMap[int_to_ind(last_interv)]);
			interv = ind_to_int(chk);
			pitch = minf->noteHistory[last].pitch + interv;
			if (debug_gen)
			    fprintf(stderr, "pitch %d %d %d => %d\n", last_interv, chk, interv, pitch);
	    }
	
	    if (pitch < minf->minNote || pitch > minf->maxNote) {
	    	if (debug_gen)
	    		fprintf(stderr, "out of bounds %d\n", pitch);
	    	pitch = select_list(MAX_ABS_NOTE, minf->markov->startNote);
			currentAttribs.add("", Attribute::PHRASE_END);
		}
		
		noteTo.pitch = pitch;
	}
	minf->noteHistory.insert(minf->noteHistory.begin(), noteTo);
	minf->attribHistory.insert(minf->attribHistory.begin(), currentAttribs);
	if (minf->noteHistory.size() > MAX_GEN_HISTORY) {
		minf->noteHistory.pop_back();
	}
	return noteTo;
}


void
NoteMarkov::Analyse(Stream &stream, Time &start, Time &end)
{
	short		accum[N_NOTES][N_NOTES];
	int			i, j,
    			tot_cnt;
    StreamItem	*si;
    
    interval_t	last_note;

    for (i=0; i<N_NOTES; i++) {
		for (j=0; j<N_NOTES; j++) {
	    	accum[i][j] = 0;
		}
    }

    si = stream.head;
    for (; si!=nullptr && si->type != TypedValue::S_NOTE; si = si->next);
    if (si) {
		StreamNote	*sn = (StreamNote *)si;
    	StreamNote	*last = (StreamNote *)si;
    	startNote[sn->note.pitch] = 1.0;
		for (; si!=nullptr && si->type != TypedValue::S_NOTE; si = si->next) {
		}

    	if (si) {
			sn = (StreamNote *)si;
		    last_note = last->note.pitch;
//		    startNote[int_to_ind(last_note)] = 1.0;
			last = sn;
			si = si->next;	    
		    for (; si->next != nullptr; si = si->next ) {
		   		for (; si!=nullptr && si->type != TypedValue::S_NOTE; si = si->next);
		   		if (si == nullptr) break;
				sn = (StreamNote *)si;
				accum[int_to_ind(last_note)][int_to_ind(sn->note.pitch)]++;
				last_note = sn->note.pitch;
				last = (StreamNote*)si;
		    }
		}
    }

    for (i=0; i<N_NOTES; i++) {
    	tot_cnt = 0;
		for (j=0; j<N_NOTES; j++) {
		    tot_cnt += accum[i][j];
		}
		if (tot_cnt > 0) {
		    for (j=0; j<MAX_INTERVALS; j++) {
		        noteMap[i][j] = (float) accum[i][j] / (float)tot_cnt;
		    }
		}
    }
}


void
IntervMarkov::Analyse(Stream &stream, Time &start, Time &end)
{
	short		accum[MAX_INTERVALS][MAX_INTERVALS];
	int			i, j,
    			tot_cnt;
    StreamItem	*si;
    
    interval_t	interval,
    			last_interval;

    for (i=0; i<MAX_INTERVALS; i++) {
		for (j=0; j<MAX_INTERVALS; j++) {
	    	accum[i][j] = 0;
		}
    }

    si = stream.head;
    for (; si!=nullptr && si->type != TypedValue::S_NOTE; si = si->next);
    if (si) {
		StreamNote	*sn = (StreamNote *)si;
    	StreamNote	*last = (StreamNote *)si;
    	startNote[sn->note.pitch] = 1.0;
		for (; si!=nullptr && si->type != TypedValue::S_NOTE; si = si->next) {
		}

    	if (si) {
			sn = (StreamNote *)si;
		    last_interval = last->note.pitch - sn->note.pitch;
		    startInterval[int_to_ind(last_interval)] = 1.0;
			last = sn;
			si = si->next;	    
		    for (; si->next != nullptr; si = si->next ) {
		   		for (; si!=nullptr && si->type != TypedValue::S_NOTE; si = si->next);
		   		if (si == nullptr) break;
				sn = (StreamNote *)si;
	    		interval = last->note.pitch - sn->note.pitch;
				accum[int_to_ind(last_interval)][int_to_ind(interval)]++;
				last_interval = interval;
				last = (StreamNote*)si;
		    }
		}
    }

    for (i=0; i<MAX_INTERVALS; i++) {
    	tot_cnt = 0;
		for (j=0; j<MAX_INTERVALS; j++) {
		    tot_cnt += accum[i][j];
		}
		if (tot_cnt > 0) {
		    for (j=0; j<MAX_INTERVALS; j++) {
		        intervalMap[i][j] = (float) accum[i][j] / (float)tot_cnt;
		    }
		}
    }
}

void
RhythmMarkov::Analyse(Stream &stream, Time &start, Time &end)
{
	short	accum[MAX_INTERVALS][MAX_INTERVALS];
    int		n, i, j, k, tot_cnt;
    RUnit	IR[MAX_ISO_LEN];
    StreamItem		*si;
	StreamNote		*sn;

    n = 0;
    nRForm = nIsos = 0;
    i = 0;

	si = stream.head;
	    
    while (si != nullptr) {
		for (j=0; j<MAX_ISO_LEN; j++) {
		    IR[j].dur = 0;
		    IR[j].silent = false;
		}
		
		int		rg = 0;
		for (; si != nullptr; si=si->next) {
			if (si->type == TypedValue::S_NOTE) {
				sn = (StreamNote *)si;
				if (sn->note.pitch > 127) {
		    		IR[rg].silent = true;
				}
				IR[rg++].dur = sn->note.duration;
				if (rg == MAX_ISO_LEN-1 ||
		    		(si->next && si->type == TypedValue::S_NOTE &&
		    			((StreamNote *)si->next)->attributes.has(Attribute::CELL_START))){
		    		si = si->next;
		    		break;
				}
			}
		}
		    
		for (j=0; j<nIsos; j++) {
		    for (k=0; k<MAX_ISO_LEN; k++) {
				if (IR[k].dur != isos[j][k].dur ||
					IR[k].silent != isos[j][k].silent) 
				    break;
			}
		    if (k == MAX_ISO_LEN)
				break;
		}
		if (j == nIsos) {
		    nIsos++;
		    for (k=0; k<MAX_ISO_LEN; k++) {
				isos[j][k].dur = IR[k].dur;
				isos[j][k].silent = IR[k].silent;
		    }
		}
		rForm[nRForm++] = j;
    }

    for (i=0; i<MAX_INTERVALS; i++) {
		for (j=0; j<MAX_INTERVALS; j++) {
	    	accum[i][j] = 0;
		}
    }

    for (i=0; i<nRForm; i++)
    	startCell[i] = 0.0;
	    startCell[0] = 1.0;
	    
	    for (i=1; i<nRForm; i++) {
			accum[rForm[i-1]][rForm[i]]++;
    }
    bool safeend = false;
    for (i=0; i<nRForm; i++) {
    	if (accum[rForm[nRForm-1]][i] != 0) {
    		safeend = true;
    	}
    }
    if (!safeend)
    	accum[rForm[nRForm-1]][rForm[nRForm-1]] = 1;
    for (i=0; i<MAX_RFORM; i++) {
    	tot_cnt = 0;
		for (j=0; j<MAX_RFORM; j++) {
		    tot_cnt += accum[i][j];
		}
	
		if (tot_cnt > 0) {
		    for (j=0; j<MAX_RFORM; j++) {
		        rCellMap[i][j] = (float) accum[i][j] / (float)tot_cnt;
		    }
		}
    }
}


#ifdef XXXXX
/*

// Evaluate expression .........
    case BUILTIN_MARKOV: {
		if (debug_exp >= 1)
		    fprintf(stderr, "markov generate\n");
	
		ret_val.Set(TypedValue::S_NOTE, REF_POINTER);
		ret_val.SetValue(GenNextNote(block->crap.call.crap.markov));
		break;
    }
    
// old initialisation routines ......
			    case BUILTIN_MARKOV: {
//					int		i, j;
				
					p->crap.call.crap.markov->type = TypedValue::S_NOTE;
						
					p->crap.call.crap.markov->poolName = ((char *)(
						p->crap.call.parameters->type == C_NAME?
							p->crap.call.parameters->crap.name :	
							(p->crap.call.parameters->type == C_SYM?
								p->crap.call.parameters->crap.sym->name :	
								"")));
					p->crap.call.crap.markov->maxNote = 100;
					p->crap.call.crap.markov->minNote = 30;
			
					if (par->next) {		
						p->crap.call.crap.markov->baseNoteVelocity =
							(short)par->next->crap.constant.value.IntValue(nullptr);	
						if (par->next->next) {
							p->crap.call.crap.markov->cellEmph =
								(short)par->next->next->crap.constant.value.IntValue(nullptr);
							if (par->next->next->next) {
								p->crap.call.crap.markov->minNote =
									(uchar)par->next->next->next->crap.constant.value.IntValue(nullptr); 
								if (par->next->next->next->next) {
									p->crap.call.crap.markov->maxNote =
										(uchar)par->next->next->next->next->crap.constant.value.IntValue(nullptr);
								}
							}
						} else
							p->crap.call.crap.markov->cellEmph = 0;
					} else {
						p->crap.call.crap.markov->baseNoteVelocity = 80;
						p->crap.call.crap.markov->cellEmph = 0;
					}
			        p->crap.call.crap.markov->rFormNo = -1;
			        p->crap.call.crap.markov->rFormInd = -1;
					
					p->crap.call.crap.markov->noteHistory[0].AddProperty(PROP_PHRASE_END);
					p->crap.call.crap.markov->noteHistory[0].dynamic = 0;
					p->crap.call.crap.markov->noteHistory[0].duration = 0;
					p->crap.call.crap.markov->noteHistory[0].pitch = -1;
				
					varp = p;
					
					break;
			    }
			    
*/
			    
#endif
