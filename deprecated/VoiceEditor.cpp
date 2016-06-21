#include "VoiceEditor.h"
#include "Voice.h"
#include "Parse.h"


VoiceEditor::VoiceEditor(BRect inRect, char *inTitle, Voice *V)
	: BWindow( inRect, inTitle, B_TITLED_WINDOW, nullptr )
{
	BRect	Box = Bounds();
	BMenuBar *mbar = new BMenuBar(Box, "mbar");
	BMenu	*mFileMenu;
	int		t;

	voice = V;

	mFileMenu = new BMenu("File");	
	mbar->AddItem(mFileMenu);
	AddChild(mbar);
	
	mFileMenu->AddItem(new BMenuItem("Save", new BMessage(B_CLOSE_REQUESTED), 'S'));
	mFileMenu->AddItem(new BMenuItem("Save AsÉ", new BMessage(B_CLOSE_REQUESTED)));
	mFileMenu->AddItem(new BMenuItem("Revert", new BMessage(B_CLOSE_REQUESTED)));
	mFileMenu->AddItem(new BMenuItem("Close", new BMessage(B_CLOSE_REQUESTED), 'W'));

	Lock();
	Box = mbar->Bounds();
	Unlock();
	t = Box.bottom;
	
	Box = Bounds();
	Box.right -= B_V_SCROLL_BAR_WIDTH + 1;
	Box.bottom -= B_H_SCROLL_BAR_HEIGHT + 1;
	Box.top += t+1;
	
	edit = new VoiceEditView(Box);
	AddChild(edit);
	
	Box = Bounds();
	Box.left = Box.right - B_V_SCROLL_BAR_WIDTH;
	Box.bottom -= B_H_SCROLL_BAR_HEIGHT;
	
    vert = new BScrollBar(Box, "", edit,
               0, 600, B_VERTICAL);
	AddChild(vert);
	
	Box = Bounds();
	Box.right -= B_V_SCROLL_BAR_WIDTH;
	Box.top = Box.bottom - B_H_SCROLL_BAR_HEIGHT;
	
	horz = new BScrollBar(Box, "", edit,
               0, 600, B_HORIZONTAL);
    AddChild(horz);
	Show();
}

VoiceEditor::~VoiceEditor()
{
	delete edit;
	delete vert;
	delete horz;
}

bool
VoiceEditor::QuitRequested()
{
	bool		succ;
	short		val;
	
	BAlert* about = new BAlert( "", "Save and re-compile?",
				"Cancel", "Don't Save", "Save" );
	if ((val=about->Go()) == 2) {
	
//		if (voice->MainBlock) delete voice->MainBlock;
//		if (voice->InitBlock) delete voice->InitBlock;
//		if (voice->SleepBlock) delete voice->SleepBlock;
//		if (voice->WakeBlock) delete voice->WakeBlock;
//		if (voice->DieBlock) delete voice->DieBlock;
//	
//		if (acquire_sem(voice->mySem) != B_NO_ERROR)
//			return FALSE;
//			
////		succ = CompileVoiceString(edit->Text(), voice);
//		
//		if (succ) {
//			succ = voice->Init();
//		}
//		
//		if (succ) {
//			free(voice->ProgText);
//			voice->ProgText = (char *)malloc(strlen(edit->Text())+1);
//			strcpy(voice->ProgText,edit->Text());
//		} else {
//			if (voice->MainBlock) {
//				 delete voice->MainBlock;
//				voice->MainBlock = nullptr;
//			}
//			if (voice->InitBlock) {
//				delete voice->InitBlock;
//				voice->InitBlock = nullptr;
//			}
//			if (voice->SleepBlock){
//				delete voice->SleepBlock;
//				voice->SleepBlock = nullptr;
//			}
//			if (voice->WakeBlock){
//				delete voice->WakeBlock;
//				voice->WakeBlock = nullptr;
//			}
//			if (voice->DieBlock){
//				delete voice->DieBlock;
//				voice->DieBlock = nullptr;
//			}
//		}
//		 
//		release_sem(voice->mySem);
//		
		return TRUE;
	} else if (val == 1) {
		return TRUE;
	} else
		return FALSE;
}

VoiceEditView::VoiceEditView(BRect rect)
	: BTextView(rect, "vd", rect, B_FOLLOW_ALL, B_WILL_DRAW)
{
}