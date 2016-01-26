#ifndef _MARKOV
#define _MARKOV

#include "Note.h"
#include "Block.h"

class StabEnt;

#define MAX_GEN_HISTORY	4

#define N_NOTES	12
#define MAX_INTERVALS	73
#define INT_UNISON	36
#define MAX_ABS_NOTE	128

#define N_BEATS	49
#define N_MK_BT	8

// analytic information, private to player
// ... store these structures on stack 
struct NoteMarkov
{
	void Analyse(Stream &stream, Time &start, Time &end);

    float noteMap[N_NOTES][N_NOTES];
	float startNote[N_NOTES]; /* set of possible starting tones */
	float beatMap[N_MK_BT][N_MK_BT]; /* same again for durations*/
	float startBeat[N_MK_BT];
};

struct IntervMarkov
{
	void Analyse(Stream &stream, Time &start, Time &end);

    float intervalMap[MAX_INTERVALS][MAX_INTERVALS];
    float startInterval[MAX_INTERVALS];
    float startNote[MAX_ABS_NOTE];
};

#define MAX_RFORM	30
#define MAX_ISO_LEN	25

struct RUnit {
	float	dur;
	bool	silent;
};

struct RhythmMarkov {
	void Analyse(Stream &stream, Time &start, Time &end);

    RUnit isos[MAX_RFORM][MAX_ISO_LEN];
    short nIsos;
    short rForm[MAX_RFORM];
    short nRForm;
    float rCellMap[MAX_RFORM][MAX_RFORM];
    float startCell[MAX_RFORM];
};

// info for the player
struct MarkovInfo  : public GenericPlayInfo
{
	short type;
    pitch_t maxNote;	// user param... max note generated
    pitch_t minNote;	// user param... min note generated
    vel_t baseNoteVelocity;	// user param... base velocity
    vel_t cellEmph;	// user param... added velocity at start of rhythmic cell

// markov analytical info ... set up when the player is reset,
// or when the clip analysed is changed.
    IntervMarkov *markov;
    RhythmMarkov *rMarkov;

// operational data used by
    short rFormNo;			// current rhythmic form number.
    short rFormInd;			// index into current rhythmic cell
    Note noteHistory[MAX_GEN_HISTORY]; // previous results
};

int int_to_ind(int interv);
int ind_to_int(int ind);

#endif
