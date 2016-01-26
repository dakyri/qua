#include "qua_version.h"

/*
 * QuaPP:
 *  BApplication for qua!
 */

#ifdef _BEOS

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <Roster.h>
#endif

#include "StdDefs.h"
#include "Colors.h"
#include "TiledView.h"

#include "include/Block.h"
#include "include/Pool.h"
#include "include/QuaMidi.h"
#include "include/messid.h"
#include "include/Qua.h"
#include "include/Quapp.h"
#include "include/MixerWindow.h"
#include "include/Parse.h"
#include "include/SequencerWindow.h"
#include "include/Toolbox.h"
#include "include/Channel.h"
#include "include/GlblMessid.h"
#include "include/QuaJoystick.h"
#include "include/QuaAudio.h"
#include "include/QuaFX.h"
#include "include/Quaddon.h"
#include "include/QuaObject.h"
#include "include/Method.h"
#include "include/RosterView.h"
#include "include/Template.h"

//#include <3dUniverse.h>
#include "LalaLand.h"

uchar quaiconbits[] = {
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x00,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x00,0x5d,0x00,0x00,0x00,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x5d,0x5d,0x5d,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x00,
0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x00,0x00,0x3f,
0x3f,0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x00,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,
0x3f,0x3f,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x5d,0x5d,0x00,0x00,0x3f,0x3f,
0x3f,0x3f,0x00,0x00,0x00,0x5d,0x5d,0x00,0x5d,0x5d,0x5d,0x00,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x5d,0x5d,0x5d,0x00,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x00,0x5d,0x5d,0x5d,0x00,0x00,0x00,0x5d,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x5d,0x00,0x3f,0x3f,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x00,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x00,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
};

time_t		time();
flag		qua_debug;
	

FILE		*stderr=nullptr;

Quapp		*quapp;
ulong		quasig = 'quaq';

//float		libH;
//float		atX, atY, bibWid;

int
main(long argc, char **argv)
{
	quapp = new Quapp(argc, argv);

	if(quapp){
		quapp->Run( );
		delete quapp;
	} else {
	}
}



/*
 * Quapp:
 */
Quapp::Quapp(long argc, char **argv):
	BApplication("application/x-vnd.Dak-Qua")
{
	extern BList	metrix;
	metrix.AddItem(&stdMetric);
	metrix.AddItem(&mSecMetric);
	metrix.AddItem(&sampleRateMetric);

	char		*ProgFileName;
	BRect		windowRect;
	BMenuItem	*item;
	
	audioFX = nullptr;
	quapp = this;		/* singular !!!! */	
    setbuf(stdout, nullptr);
#define RELEASE
#ifdef RELEASE
	dbfp = stderr;
#else
    dbfp = fopen("/boot/home/source/Qua/debug", "w");
    setbuf(dbfp, nullptr);
#endif
	fprintf(dbfp, "starting init...\n");
	nQua = 0;
	qua = nullptr;


////////////////////
// set up paths
////////////////////

	app_info ai; 
	BEntry ent; 
	
	be_app->GetAppInfo(&ai); 
	ent.SetTo(&ai.ref); 

	BFile			file(&ai.ref, B_READ_ONLY); 
	BAppFileInfo	afi(&file);
	quaBigIcon = new BBitmap(BRect(0,0,31,31), B_CMAP8);
	quaSmallIcon = new BBitmap(BRect(0,0,15,15), B_CMAP8);
	status_t err;
#define ICONFROMAPP
#ifdef ICONFROMAPP
	err=afi.GetIcon(quaBigIcon, B_LARGE_ICON);
	if (err != B_OK) {
		reportError("Can't get qua icon");
	}
	err=afi.GetIcon(quaSmallIcon, B_MINI_ICON);
	if (err != B_OK) {
		reportError("Can't get qua icon");
	}
	fprintf(stderr, "icon got from app...\n");
#else
	quaBigIcon->SetBits(quaiconbits, 1024, 0, B_CMAP8);
//	quaSmallIcon->SetBits(quasiconbits, 256, 0, B_CMAP8);
#endif

	if (ent.GetParent(&ent) != B_NO_ERROR) { 
		TragicError("Can't find Home...");
	}
	if (ent.GetPath(&homePath) != B_NO_ERROR) { 
		TragicError("Can't find Home...");
	}
	
	CheckFolderExists("Patches", patchPath);
	CheckFolderExists("Projects", projectPath);
	CheckFolderExists("Samples", samplePath);

///////////////////////////////
// collect up pretty pictures
///////////////////////////////
	BPath	imgPath;
	imgPath.SetTo(homePath.Path(), "images/background");
	backgroundPattern = GetBitmapForImage(((char *)imgPath.Path()));
	
	imgPath.SetTo(homePath.Path(), "images/midi_micon");
	midiSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (midiSmallIcon == nullptr)
		midiSmallIcon = quaSmallIcon;
	
	imgPath.SetTo(homePath.Path(), "images/joystick_micon");
	joySmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (joySmallIcon == nullptr)
		joySmallIcon = quaSmallIcon;
	
	imgPath.SetTo(homePath.Path(), "images/audio_micon");
	audioSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioSmallIcon == nullptr)
		audioSmallIcon = quaSmallIcon;

	imgPath.SetTo(homePath.Path(), "images/audioin_micon");
	audioInSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioInSmallIcon == nullptr)
		audioInSmallIcon = quaSmallIcon;

	imgPath.SetTo(homePath.Path(), "images/audioout_micon");
	audioOutSmallIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioOutSmallIcon == nullptr)
		audioOutSmallIcon = quaSmallIcon;

	imgPath.SetTo(homePath.Path(), "images/midi_icon");
	midiBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (midiBigIcon == nullptr)
		midiBigIcon = quaBigIcon;
	
	imgPath.SetTo(homePath.Path(), "images/joystick_icon");
	joyBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (joyBigIcon == nullptr)
		joyBigIcon = quaBigIcon;
	
	imgPath.SetTo(homePath.Path(), "images/audio_icon");
	audioBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioBigIcon == nullptr)
		audioBigIcon = quaBigIcon;

	imgPath.SetTo(homePath.Path(), "images/audioin_icon");
	audioInBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioInBigIcon == nullptr)
		audioInBigIcon = quaBigIcon;

	imgPath.SetTo(homePath.Path(), "images/audioout_icon");
	audioOutBigIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioOutBigIcon == nullptr)
		audioOutBigIcon = quaBigIcon;

	imgPath.SetTo(homePath.Path(), "images/audioctrl_icon");
	audioCtrlIcon = GetBitmapForImage(((char *)imgPath.Path()));
	if (audioCtrlIcon == nullptr)
		audioCtrlIcon = quaSmallIcon;

/////////////////////////////////
// build top level windows
/////////////////////////////////

	windowRect.Set( 10, 100, 790, 400 );
//	toolboxWindow = new ToolboxWindow(
//							windowRect,
//							"Qua 0.70: toolbox");
//	toolboxWindow->Lock();
//	if (backgroundPattern)
//		toolboxWindow->back->SetTilingPattern(*backgroundPattern);
//	libH = toolboxWindow->theLibrary->Bounds().bottom;
//	atX = atY = 5;
//	bibWid = 0;
//	toolboxWindow->Unlock();
//	toolboxWindow->Show();

////////////////////////////	
// load audio addons...
////////////////////////////
	BPath	addonFXPath(((char *)homePath.Path()), "AddOns/AudioFX");
	err;
    if ((err=LoadQuaFX(((char *)addonFXPath.Path()))) != B_NO_ERROR) {
		TragicError("Can't load addons: %s", ErrorStr(err));
    } else {
		fprintf(dbfp, "Found addons...\n");
	}

//////////////////////////////////
// load global stuff
//////////////////////////////////
	BPath	quaRCPath(homePath.Path(), "QuaRC");
    if ((err=LoadQuaRC(((char *)quaRCPath.Path()))) != B_NO_ERROR) {
		TragicError("Can't load templates: %s", ErrorStr(err));
    } else {
		fprintf(dbfp, "Found Templates...\n");
	}

////////////////////////////////////
// start up devices
////////////////////////////////////
	
//	toolboxWindow->Lock();
//	
//	BPoint	p(bibWid+5, 5);
//	
//	toolboxWindow->theLibrary->atX = BPoint(p.x+5,5);
//	toolboxWindow->theLibrary->atX = BPoint(
//					p.x+5,
//					toolboxWindow->theLibrary->lastRB.y+5);
//	toolboxWindow->theLibrary->atX = BPoint(
//					p.x+5,
//					toolboxWindow->theLibrary->lastRB.y+5);
//	toolboxWindow->Unlock();

	filePanel = nullptr;

//	SetPulseRate(100000.0);

//	lalaLand = new B3dUniverse("lalaland");
//	lalaLand->SetFrameRate(25.0);

	while (argc > 1) {
		if (!LoadQua(argv[argc-1])) {
			fprintf(stderr, "load of %s fails\n", argv[argc-1]);
////			AddQua(new Qua(argv[argc-1]));
		}
		if (nQua > 0 && quas[nQua-1]) {
			qua = quas[nQua-1];
			qua->sequencerWindow->Show();
			qua->mixerWindow->Show();
		}
		argc--;
	}
}

void
Quapp::CheckFolderExists(char *nm, BPath &p)
{
	p.SetTo(homePath.Path(), nm);
	BDirectory		pDir(p.Path());
	status_t		err;
	if ((err=pDir.InitCheck()) != B_NO_ERROR) {
		BDirectory		homeDir(homePath.Path());
		if ((err=homeDir.InitCheck()) != B_NO_ERROR) {
			TragicError("Can't find home: %s ...", ErrorStr(err));
		}
		err = homeDir.CreateDirectory(p.Path(), &pDir);
		if (err != B_NO_ERROR) {
			TragicError("Can't create folder: %s ...", ErrorStr(err));
		}
	}
}

Quapp::~Quapp()
{
	fprintf(stderr, "~Quapp: halting all the Qua...\n");
	if (qua) {
		qua->Stop();
	}

	fprintf(stderr, "~Quapp: deleting all the Qua...\n");
	for (short i=0; i<nQua; i++) {
		delete quas[i];
	}
	
	fprintf(stderr, "~Quapp: popping context stack...\n");
//	glob.PopContext();
//	glob.PopContext();
	glob.PopContext();
	
	fprintf(stderr, "~Quapp: context stack popped...\n");
}

void Quapp::AboutRequested( )
{
	BAlert* about = new BAlert( "", "Qua 0.93\n" \
		"Another fine mess that you've got us into!\n" \
		"Comments and bug reports to:\n\n" \
		"    Dak (dak@zog.net.au)", "Yes, and?" );
	about->Go( );
}

bool Quapp::QuitRequested()
{
	long	crap;
	
   	fprintf(dbfp, "Quapp:: asked to leave\n");
//	TxMidiClose();
//	toolboxWindow->PostMessage(B_QUIT_REQUESTED);
//	status_t	err;
//	wait_for_thread(toolboxWindow->Thread(), &err);
	SetPulseRate(0);
	fprintf(dbfp, "done the deed\n");
	return TRUE;
}

void Quapp::Pulse()
{
}

void
Quapp::ReadyToRun()
{
	fprintf(stderr, "Quapp::ReadyToRun()\n");
#ifdef NEW_MEDIA
	BMediaRoster	*roster = BMediaRoster::Roster();
#endif
	if (nQua == 0) {
		PostMessage(QUA_NEW);
	} else if (qua) {
		qua->sequencerWindow->Show();
		qua->mixerWindow->Show();
	}
}


void Quapp::RefsReceived(BMessage* inMsg)
{
	
	entry_ref	fileRef;
	status_t err = inMsg->FindRef("refs", &fileRef);
	BEntry	ent(&fileRef);
	BPath	p;
	ent.GetPath(&p);
	
	if (!LoadQua(p.Path())) {
		AddQua(new Qua(&p));
	}
}

bool
Quapp::LoadQua(const char *path)
{
	Qua			*q=Qua::LoadQuaFile(path);
	if (q != nullptr) {
		AddQua(q);
		return true;
	} else {
		reportError("crappy file ref");
		if (nQua == 0) {
			PostMessage(B_QUIT_REQUESTED);
		}
			
	}
	return false;
}


void
Quapp::AddQua(Qua *q)
{
	quas[nQua] = q;
	if (qua == nullptr)
		qua = q;
	Lock();
	AddHandler(q);
	q->SetTargets();
	Unlock();
	q->rightHere = BMessenger(q);
	nQua++;
}

void
Quapp::RemoveQua(Qua *q)
{
	short i;
	for (i=0; i<nQua; i++) {
		if (quas[i] == q)
			break;
	}
	if (i=nQua)
		return;
	for (;i<nQua-1; i++) {
		quas[i] = quas[i+1];
	}
	if (qua == q)
		qua = nullptr;
	Lock();
	RemoveHandler(q);
	Unlock();
	nQua--;
}

void
Quapp::MessageReceived(BMessage* inMsg)
{
//	fprintf(stderr, "quapp:mr(%s)\n", uintstr(inMsg->what));
	switch ( inMsg->what ) {
	case B_ABOUT_REQUESTED:
		AboutRequested();
		break;
		
	case B_QUIT_REQUESTED:
		fprintf(stderr, "why here and now?\n");
		break;
		
	case SET_WINDOW:
//		if (inMsg->HasInt32("toolbox")) {
//			if (inMsg->FindInt32("toolbox")) {
//				BMenuItem	*s;
//				inMsg->FindPointer("source", (void**)&s);
//				s->SetMarked(true);
//				toolboxWindow->Show();
//			} else {
//				toolboxWindow->Hide();
//			}
//		}
		break;

		
	case QUA_RESET:
		break;
		
	case QUA_SELECT:
		break;
		
	case QUA_LOAD: {
		if (inMsg->HasRef("refs")) {
			entry_ref	fileRef;
			status_t err = inMsg->FindRef("refs", &fileRef);
			BEntry	ent(&fileRef);
			BPath	p;
			ent.GetPath(&p);
			
			if (qua) {
				qua->Stop();
				BMessage	killWin(B_QUIT_REQUESTED);
				killWin.AddInt32("le bombe", 1);
				BWindow *w = qua->sequencerWindow;
				RemoveQua(qua);
				w->PostMessage(&killWin);
				delete qua;
				qua = nullptr;
			}
			if (!LoadQua(p.Path())) {
				fprintf(stderr, "failed to load qua file...\n");
				AddQua(new Qua(&p));
			}
			if (qua) {
				qua->sequencerWindow->Show();
				qua->mixerWindow->Show();
			}
		} else {
			if (filePanel) {
				delete filePanel;
			}
			BMessenger	rightHere(this);
			BEntry		ldDir(projectPath.Path(), true);
			BMessage	ldMsg(QUA_LOAD);
			filePanel = new BFilePanel(
							B_OPEN_PANEL,
							&rightHere,
							nullptr,
							0,
							false,
							&ldMsg,	// mess
							nullptr,	// filter
							false,	// modal
							true);	// hide
			if (ldDir.InitCheck() == B_OK) {
				filePanel->SetPanelDirectory(&ldDir);
			}
			filePanel->Show();
		}
		break;
	}

	case QUA_NEW: {
		if (inMsg->HasInt32("load")) {
			if (filePanel) {
				delete filePanel;
			}
			BMessenger	rightHere(this);
			BEntry		ldDir(projectPath.Path(), true);
			filePanel = new BFilePanel(
							B_OPEN_PANEL,
							&rightHere,
							nullptr,
							0,
							false,
							nullptr,	// mess
							nullptr,	// filter
							false,	// modal
							true);	// hide
			if (ldDir.InitCheck() == B_OK) {
				filePanel->SetPanelDirectory(&ldDir);
			}
			filePanel->Show();
		} else {
			AddQua(new Qua("New"));
		}
		break;
	}
		
	case QUA_CLOSE:
		/* free blocks */
		fprintf(stderr, "closing qua %\n", qua);

		if (qua) {
			qua->Stop();
			delete qua;
			short	i=0;
			for (i=0; i < nQua-1; i++) {
				if (quas[i] == qua)
					break;
			}
			for (; i < nQua-1; i++) {
				quas[i] = quas[i+1];
			}
			nQua--;
			if (nQua == 0) {
				qua = nullptr;
				fprintf(stderr, "exitting ungracefully\n");
				PostMessage(B_QUIT_REQUESTED);
			} else {
				qua = quas[0];
				quas[0]->sequencerWindow->Activate();
			}
		}
		break;
			
//	case DISPLAY_TIMER:
//		qua[0]->Time.Actual = inMsg->FindFloat("time");
//		qua[0]->Time.ticks = inMsg->FindFloat("time");
////		qua[0]->MSecs = inMsg->FindInt32("ticks");
////		qua[0]->IterCounter = inMsg->FindInt32("quanta");
//		qua[0]->sequencerWindow->Lock();
////		qua[0]->sequencerWindow->SetTime(qua[0]->IterCounter,
////				 qua[0]->Time.Actual, qua[0]->Time.ticks);
//		qua[0]->sequencerWindow->Unlock();
//		break;
		
	default:
		BApplication::MessageReceived( inMsg );
	}
}

status_t
Quapp::LoadQuaFX(char *path)
{
	BDirectory	addonDir;
	status_t	err;
	
	audioFX = nullptr;
	
	if ((err=addonDir.SetTo(path)) != B_NO_ERROR)
		return err;

	fprintf(stderr, "checking addons in %s\n", path);

	BEntry		addonEnt;
	uint32		addon_code = 0;
	
	while (addonDir.GetNextEntry(&addonEnt, true) != B_ENTRY_NOT_FOUND) {

//		toolboxWindow->Lock();
		
		BPath	addonPath;
		addonEnt.GetPath(&addonPath);
		image_id	id = load_add_on(addonPath.Path());

		if (id < 0) {
			return id;
		}

		uint32		*n_addon_hook_p;
		uint32		n_addon_hook;
		Quaddon		*p;
	
		if (	(err=get_image_symbol(
						id,
						"n_addon_hooks",
						B_SYMBOL_TYPE_DATA,
						(void**)&n_addon_hook_p)) < B_NO_ERROR ||
				(n_addon_hook = *n_addon_hook_p) <= 1) {
			p = new Quaddon(((char *)addonPath.Path()), id, 0);
			p->next = audioFX;
			audioFX = p;
//			QuaSymbolBridge *s = new QuaSymbolBridge(
//									p->sym,
//									BRect(0,0,50,20),
//									quaSmallIcon,
//									quaBigIcon,
//									nullptr,
//									pink);
//			BRect	r = s->Frame();
//			if (r.bottom > libH) {
//				atX = bibWid+5;
//				atY = 5;
//				s->MoveTo(atX, atY);
//				r = s->Frame();
//				bibWid = r.right;
//			} else if (r.right > bibWid) {
// 				bibWid = r.right;
// 			}
//			atY = r.bottom + 5;
		} else {
			for (short i=0; i<n_addon_hook; i++) {
				p = new Quaddon(((char *)addonPath.Path()), id, i);
				p->next = audioFX;
				audioFX = p;
//				QuaSymbolBridge *s = new QuaSymbolBridge(
//										p->sym,
//										BRect(0,0,50,20),
//										quaSmallIcon,
//										quaBigIcon,
//										nullptr/*toolboxWindow->theLibrary*/,
//										pink);
//				BRect	r = s->Frame();
//				if (r.bottom > libH) {
//					atX = bibWid+5;
//					atY = 5;
//					s->MoveTo(atX, atY);
//					r = s->Frame();
//					bibWid = r.right;
//				} else if (r.right > bibWid) {
//					bibWid = r.right;
//				}
//				atY = r.bottom + 5;
			}
		}

//		toolboxWindow->Unlock();
		
	}
	
	return B_NO_ERROR;
}

status_t
Quapp::LoadQuaRC(char *path)
{
	BDirectory	rcDir;
	status_t	err;
	
	if ((err=rcDir.SetTo(path)) != B_NO_ERROR)
		return err;

	fprintf(stderr, "checking templates in %s\n", path);

	BEntry		rcEnt;
	
	while (rcDir.GetNextEntry(&rcEnt, true) != B_ENTRY_NOT_FOUND) {
		BPath	rcPath;
		rcEnt.GetPath(&rcPath);
		
		FILE		*theFile;

		theFile = fopen(rcPath.Path(), "r");
		if (theFile != nullptr) {
			fprintf(stderr, "loading template %s...", rcPath.Path());
	
			Parser		*p = new Parser(theFile, rcPath.Leaf(), nullptr);
			if (p->ParseProgFile()) {
			
				fprintf(stderr, "parsed\n");
				
	//			toolboxWindow->Lock();
				
				Method		*SON, *S;
			    for(S=p->methods; S!=nullptr; S=SON) {
			    	SON = S->next;
					if (S->sym->context == nullptr) {
			 			if (S->Init()) {
	//						QuaSymbolBridge *s = new QuaSymbolBridge(
	//										S->sym,
	//										BRect(0,0,50,20),
	//										quaSmallIcon,
	//										quaBigIcon,
	//										nullptr/*toolboxWindow->theLibrary*/,
	//										aqua);
	//						BRect	r = s->Frame();
	//						if (r.bottom > libH) {
	//							atX = bibWid+5;
	//							atY = 5;
	//							s->MoveTo(atX, atY);
	//							r = s->Frame();
	//							bibWid = r.right;
	//						} else if (r.right > bibWid) {
	//							bibWid = r.right;
	//						}
	//						atY = r.bottom + 5;
					    	S->next = methods;
					    	methods = S;
					    }
					}
			    }
			    
			    Template	*F, *FON;
			    for(F=p->templates; F!=nullptr; F=FON) {
			    	FON = F->next;
					if (F->sym->context == nullptr) {
			 			if (F->Init()) {
	//						QuaSymbolBridge *s = new QuaSymbolBridge(
	//										F->sym,
	//										BRect(0,0,50,20),
	//										F->sicon?F->sicon:quaSmallIcon,
	//										F->bicon?F->bicon:quaBigIcon,
	//										nullptr /*toolboxWindow->theLibrary*/,
	//										green);
	//						BRect	r = s->Frame();
	//						if (r.bottom > libH) {
	//							atX = bibWid+5;
	//							atY = 5;
	//							s->MoveTo(atX, atY);
	//							r = s->Frame();
	//							bibWid = r.right;
	//						} else if (r.right > bibWid) {
	//							bibWid = r.right;
	//						}
	//						atY = r.bottom + 5;
					    	F->next = templates;
					    	templates = F;
					    }
					}
			    }
			    
			    if (p->schedulees) {
			    	reportError("schedulable objects cannot be defined in null context");
			    }
	//   			toolboxWindow->Unlock();
	
			}
			fclose(theFile);
		}
	}
	
	return B_NO_ERROR;
}

#endif