#ifndef _MIDIDEFS_H
#define _MIDIDEFS_H

#define MIDI_CMD_NOT	0x00
#define MIDI_CMD_BYTE	0x80
#define MIDI_NOTE_OFF	0x80
#define MIDI_NOTE_ON	0x90
#define MIDI_KEY_PRESS	0xa0
#define MIDI_CTRL		0xb0
#define MIDI_PROG		0xc0
#define MIDI_CHAN_PRESS	0xd0
#define MIDI_BEND		0xe0
#define MIDI_SYS		0xf0


#define MIDI_SYSX_START	0xf0
#define MIDI_TIME_CODE	0xf1
#define MIDI_SONG_POS	0xf2
#define MIDI_SONG_SEL	0xf3
#define MIDI_CABLE_MSG	0xf5
#define MIDI_TUNE_REQ	0xf6
#define MIDI_SYSX_END	0xf7
#define MIDI_CLOCK		0xf8
#define MIDI_START		0xfa
#define MIDI_CONT		0xfb
#define MIDI_STOP		0xfc
#define MIDI_SENSING	0xfe
#define MIDI_SYS_RESET	0xff

const uchar	MIDICTL_MODULATION			= 0x01;
const uchar	MIDICTL_BREATH_CONTROLLER	= 0x02;
const uchar	MIDICTL_FOOT_CONTROLLER		= 0x04;
const uchar	MIDICTL_PORTAMENTO_TIME		= 0x05;
const uchar	MIDICTL_DATA_ENTRY			= 0x06;
const uchar	MIDICTL_MAIN_VOLUME			= 0x07;
const uchar	MIDICTL_MIDI_BALANCE		= 0x08;
const uchar MIDICTL_PAN					= 0x0a;
const uchar	MIDICTL_EXPRESSION_CTRL		= 0x0b;
const uchar	MIDICTL_GENERAL_1			= 0x10;
const uchar	MIDICTL_GENERAL_2			= 0x11;
const uchar	MIDICTL_GENERAL_3			= 0x12;
const uchar	MIDICTL_GENERAL_4			= 0x13;
const uchar	MIDICTL_SUSTAIN_PEDAL		= 0x40;
const uchar	MIDICTL_PORTAMENTO			= 0x41;
const uchar	MIDICTL_SOSTENUTO			= 0x42;
const uchar	MIDICTL_SOFT_PEDAL			= 0x43;
const uchar	MIDICTL_HOLD_2				= 0x45;
const uchar	MIDICTL_GENERAL_CTRL_5		= 0x50;
const uchar	MIDICTL_GENERAL_CTRL_6		= 0x51;
const uchar	MIDICTL_GENERAL_CTRL_7		= 0x52;
const uchar	MIDICTL_GENERAL_CTRL_8		= 0x53;
const uchar	MIDICTL_EFFECTS_DEPTH		= 0x5b;
const uchar	MIDICTL_TREMOLO_DEPTH		= 0x5c;
const uchar	MIDICTL_CHORUS_DEPTH		= 0x5d;
const uchar	MIDICTL_CELESTE_DEPTH		= 0x5e;
const uchar	MIDICTL_PHASER_DEPTH		= 0x5f;
const uchar	MIDICTL_DATA_INCREMENT		= 0x60;
const uchar	MIDICTL_DATA_DECREMENT		= 0x61;
const uchar	MIDICTL_RESET_ALL			= 0x79;
const uchar	MIDICTL_LOCAL_CONTROL		= 0x7a;
const uchar	MIDICTL_ALL_NOTES_OFF		= 0x7b;
const uchar	MIDICTL_OMNI_MODE_OFF		= 0x7c;
const uchar	MIDICTL_OMNI_MODE_ON		= 0x7d;
const uchar	MIDICTL_MONO_MODE_ON		= 0x7e;
const uchar	MIDICTL_POLY_MODE_ON		= 0x7f;

const uchar	MIDICTL_TEMPO_CHANGE		= 0x51;


#define NOTE_C	0
#define NOTE_C_	1
#define NOTE_D	2
#define NOTE_D_	3
#define NOTE_E	4
#define NOTE_F	5
#define NOTE_F_	6
#define NOTE_G	7
#define NOTE_G_	8
#define NOTE_A	9
#define NOTE_A_	10
#define NOTE_B	11
#define N_NOTES 12




/*------------------------------------------------------------*/

#ifndef uchar
typedef unsigned char uchar;
#endif

/*--General MIDI instruments, 0-based----------------------------*/

typedef enum midi_axe {

  /* Pianos */
  B_ACOUSTIC_GRAND=0,
  B_BRIGHT_GRAND,
  B_ELECTRIC_GRAND,
  B_HONKY_TONK,
  B_ELECTRIC_PIANO_1,
  B_ELECTRIC_PIANO_2,
  B_HARPSICHORD,
  B_CLAVICHORD,

  /* Tuned Idiophones */
  B_CELESTA,
  B_GLOCKENSPIEL,
  B_MUSIC_BOX,
  B_VIBRAPHONE,
  B_MARIMBA,
  B_XYLOPHONE,
  B_TUBULAR_BELLS,
  B_DULCIMER,

  /* Organs */
  B_DRAWBAR_ORGAN,
  B_PERCUSSIVE_ORGAN,
  B_ROCK_ORGAN,
  B_CHURCH_ORGAN,
  B_REED_ORGAN,
  B_ACCORDION,
  B_HARMONICA,
  B_TANGO_ACCORDION,

  /* Guitars */
  B_ACOUSTIC_GUITAR_NYLON,
  B_ACOUSTIC_GUITAR_STEEL,
  B_ELECTRIC_GUITAR_JAZZ,
  B_ELECTRIC_GUITAR_CLEAN,
  B_ELECTRIC_GUITAR_MUTED,
  B_OVERDRIVEN_GUITAR,
  B_DISTORTION_GUITAR,
  B_GUITAR_HARMONICS,
  
  /* Basses */
  B_ACOUSTIC_BASS,
  B_ELECTRIC_BASS_FINGER,
  B_ELECTRIC_BASS_PICK,
  B_FRETLESS_BASS,
  B_SLAP_BASS_1,
  B_SLAP_BASS_2,
  B_SYNTH_BASS_1,
  B_SYNTH_BASS_2,

  /* Strings */
  B_VIOLIN,
  B_VIOLA,
  B_CELLO,
  B_CONTRABASS,
  B_TREMOLO_STRINGS,
  B_PIZZICATO_STRINGS,
  B_ORCHESTRAL_STRINGS,
  B_TIMPANI,

  /* Ensemble strings and voices */
  B_STRING_ENSEMBLE_1,
  B_STRING_ENSEMBLE_2,
  B_SYNTH_STRINGS_1,
  B_SYNTH_STRINGS_2,
  B_VOICE_AAH,
  B_VOICE_OOH,
  B_SYNTH_VOICE,
  B_ORCHESTRA_HIT,

  /* Brass */
  B_TRUMPET,
  B_TROMBONE,
  B_TUBA,
  B_MUTED_TRUMPET,
  B_FRENCH_HORN,
  B_BRASS_SECTION,
  B_SYNTH_BRASS_1,
  B_SYNTH_BRASS_2,

  /* Reeds */
  B_SOPRANO_SAX,
  B_ALTO_SAX,
  B_TENOR_SAX,
  B_BARITONE_SAX,
  B_OBOE,
  B_ENGLISH_HORN,
  B_BASSOON,
  B_CLARINET,

  /* Pipes */
  B_PICCOLO,
  B_FLUTE,
  B_RECORDER,
  B_PAN_FLUTE,
  B_BLOWN_BOTTLE,
  B_SHAKUHACHI,
  B_WHISTLE,
  B_OCARINA,

  /* Synth Leads*/
  B_LEAD_1,
  B_SQUARE_WAVE = B_LEAD_1,
  B_LEAD_2,
  B_SAWTOOTH_WAVE = B_LEAD_2,
  B_LEAD_3,
  B_CALLIOPE = B_LEAD_3,
  B_LEAD_4,
  B_CHIFF = B_LEAD_4,
  B_LEAD_5,
  B_CHARANG = B_LEAD_5,
  B_LEAD_6,
  B_VOICE = B_LEAD_6,
  B_LEAD_7,
  B_FIFTHS = B_LEAD_7,
  B_LEAD_8,
  B_BASS_LEAD = B_LEAD_8,
  
  /* Synth Pads */
  B_PAD_1,
  B_NEW_AGE = B_PAD_1,
  B_PAD_2,
  B_WARM = B_PAD_2,
  B_PAD_3,
  B_POLYSYNTH = B_PAD_3,
  B_PAD_4,
  B_CHOIR = B_PAD_4,
  B_PAD_5,
  B_BOWED = B_PAD_5,
  B_PAD_6,
  B_METALLIC = B_PAD_6,
  B_PAD_7,
  B_HALO = B_PAD_7,
  B_PAD_8,  
  B_SWEEP = B_PAD_8,

  /* Effects */
  B_FX_1,
  B_FX_2,
  B_FX_3,
  B_FX_4,
  B_FX_5,
  B_FX_6,
  B_FX_7,
  B_FX_8,

  /* Ethnic */
  B_SITAR,
  B_BANJO,
  B_SHAMISEN,
  B_KOTO,
  B_KALIMBA,
  B_BAGPIPE,
  B_FIDDLE,
  B_SHANAI,

  /* Percussion */
  B_TINKLE_BELL,
  B_AGOGO,
  B_STEEL_DRUMS,
  B_WOODBLOCK,
  B_TAIKO_DRUMS,
  B_MELODIC_TOM,
  B_SYNTH_DRUM,
  B_REVERSE_CYMBAL,

  /* Sound Effects */
  B_FRET_NOISE,
  B_BREATH_NOISE,
  B_SEASHORE,
  B_BIRD_TWEET,
  B_TELEPHONE,
  B_HELICOPTER,
  B_APPLAUSE,
  B_GUNSHOT
} midi_axe;

#endif
	

