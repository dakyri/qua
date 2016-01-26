#ifndef _QUAPP
#define _QUAPP

#if defined(WIN32)

class BBitmap;
#endif

#define MAX_QUA	5

class MixerWindow;
class Qua;
class ToolboxWindow;
class Quaddon;
class Template;
class Method;
class QuaPort;
class StabEnt;

class Quapp : public BApplication
{
public:
						Quapp(long argc, char **argv);
						~Quapp();
#ifdef _BEOS
	virtual void		MessageReceived(BMessage* inMsg);
	virtual void		Pulse();
	virtual bool		QuitRequested();
	virtual void		AboutRequested();
	virtual void		RefsReceived(BMessage *inMess);
#endif

	virtual void		ReadyToRun();
	
	bool				LoadQua(const char *path);
	void				AddQua(Qua *q);
	void				RemoveQua(Qua *q);

	status_t			LoadQuaFX(char *path);
	status_t			LoadQuaRC(char *path);
	
	void				CheckFolderExists(char *, BPath &);

#ifdef _BEOS
	BFilePanel			*filePanel;
#endif

	BPath				homePath;
	BPath				patchPath;
	BPath				projectPath;
	BPath				samplePath;
	
	BBitmap				*backgroundPattern;
	BBitmap				*quaBigIcon;
	BBitmap				*quaSmallIcon;
	BBitmap				*joyBigIcon;
	BBitmap				*joySmallIcon;
	BBitmap				*audioBigIcon;
	BBitmap				*audioSmallIcon;
	BBitmap				*audioOutBigIcon;
	BBitmap				*audioInBigIcon;
	BBitmap				*audioOutSmallIcon;
	BBitmap				*audioInSmallIcon;
	BBitmap				*midiBigIcon;
	BBitmap				*midiSmallIcon;
	BBitmap				*audioCtrlIcon;
	
	Method				*methods;
	Template			*templates;
	Quaddon 			*audioFX;

	Qua					*quas[MAX_QUA];
	int					nQua;
	Qua					*qua;
	
};

extern Quapp	*quapp;

#endif
