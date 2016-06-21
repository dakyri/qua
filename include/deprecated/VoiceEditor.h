#ifndef _VOICEEDITOR
#define _VOICEEDITOR

class Voice;

class VoiceEditView: public BTextView
{
public:

					VoiceEditView(BRect rect);
};

class VoiceEditor: public BWindow
{
public:
					~VoiceEditor();
					VoiceEditor(BRect inRect, char *inTitle, Voice *V);
	virtual bool	QuitRequested();

	VoiceEditView	*edit;
	BScrollBar		*vert,
					*horz;
	Voice			*voice;
};

#endif