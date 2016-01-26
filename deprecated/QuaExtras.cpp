
#if defined(_BEOS)
void
Qua::MessageReceived(BMessage* inMsg)
{
	fprintf(stderr, "qua:mr(%s)\n", uintstr(inMsg->what));
	switch(inMsg->what) {
	case QUA_TIME: {
		BMessage		*Forward;
		int				val;
							
		if   (inMsg->HasFloat("tempo")) {
			SetTempo(inMsg->FindFloat("tempo"));
		}
		if (inMsg->HasInt32("ticks")) {
			theTime.ticks = inMsg->FindInt32("ticks");
			theTime.GetBBQValue(currentBar, currentBarBeat, currentBeatTick);
		} else if (inMsg->HasInt32("msticks")) {
			theTime.SetToSecs(inMsg->FindInt32("ticks")/1000.0);
			theTime.GetBBQValue(currentBar, currentBarBeat, currentBeatTick);
		}
		if (inMsg->HasFloat("actual")) {
			actualTime = inMsg->FindFloat("actual");
			theClock->Set(actualTime);
		}
		nextTickTime = 0;
		theClock->Zero();
		break;
	}
	
	case QUA_SAVE: {
		DoSave(inMsg->HasInt32("terminally"));
		break;
	}
	
	case QUA_SAVE_AS: {
		DoSaveAs(inMsg->HasInt32("terminally"));
		break;
	}
	
	case B_CANCEL: {
		if (inMsg->HasInt32("terminally")) {
#ifdef _BEOS
			quapp->PostMessage(QUA_CLOSE, this);
#endif
		}
		break;
	}
	
	case B_SAVE_REQUESTED: {
		if (inMsg->HasString("name")) {	// do it!
			entry_ref	file_ref;
			BDirectory	*the_dir = new BDirectory();
			int			i;				
			char		saveName[MAX_QUA_NAME_LENGTH];
			
			if (savePanel) {
				delete savePanel;
				savePanel = nullptr;
			}
			strcpy(saveName, inMsg->FindString("name"));
			fprintf(stderr, "save \"%s\" \n", saveName);
			SetName(saveName, !inMsg->HasInt32("terminally"));
			
			entry_ref	dir_ref;
 			inMsg->FindRef("directory", &dir_ref);
 			BEntry		ent(&dir_ref);
 			
 			ent.GetPath(&quaSaveDirPath);
 			
			if (the_dir->SetTo(&dir_ref) != B_NO_ERROR) {
				reportError("Save: Can't set directory\n");
				break;
			}
			quaSavePath.SetTo(quaSaveDirPath.Path(), saveName); 
			strcat(saveName, ".crp");
			quapSavePath.SetTo(quaSaveDirPath.Path(), saveName); 

//			if (the_dir->CreateFile(saveName, quaSaveFile)
//						!= B_NO_ERROR) {
//				reportError("cannot %s add to dir\n", saveName);
//				break;
//			}
//
//			strcat(saveName, ".crp");
//			if (the_dir->CreateFile(saveName, quapSaveFile)
//						!= B_NO_ERROR) {
//				reportError("cannot %s add to dir\n", saveName);
//				break;
//			}
			
			DoSave(inMsg->HasInt32("terminally"));

		} else {
			DoSaveAs(inMsg->HasInt32("terminally"));
		}
		break;
	} 
	
	case QUA_GO:
		Start();
		break;
	
	case QUA_PAUSE:
		Stop();
		break;
	
	case QUA_RECORD:
		StartRecording();
		break;
		
	case SET_SOURCE:
		if (inMsg->HasPointer("input")) {
			Input	*s=nullptr;
			inMsg->FindPointer("input", (void **)&s);
			if (s) {
				s->SetSource(inMsg);
			}
		}
		break;
		
	case SET_DESTINATION:
		if (inMsg->HasPointer("output")) {
			Output	*s=nullptr;
			inMsg->FindPointer("output", (void **)&s);
			if (s) {
				s->SetDestination(inMsg);
			}
		}
		break;
		
	case ENABLE_PLACE:
		if (inMsg->HasPointer("input")) {
			Input	*s=nullptr;
			inMsg->FindPointer("input", (void **)&s);
			if (s) {
				int	en = inMsg->FindInt32("enable");
				s->NoodleEnable(en);
				BMessage	enableMsg(ENABLE_PLACE);
				enableMsg.AddInt32("enable", s->enabled);
				s->Window()->PostMessage(&enableMsg, s);
			}
		} else if (inMsg->HasPointer("output")) {
			Output	*s=nullptr;
			inMsg->FindPointer("output", (void **)&s);
			if (s) {
				int	en = inMsg->FindInt32("enable");
				s->NoodleEnable(en);
				BMessage	enableMsg(ENABLE_PLACE);
				enableMsg.AddInt32("enable", s->enabled);
				s->Window()->PostMessage(&enableMsg, s);
			}
		}
		break;
		
	default:
		BHandler::MessageReceived(inMsg);
	}
}

#endif

#if defined(_BEOS)
status_t
Qua::DoSaveAs(bool andQuit)
{
	if (savePanel) {
		delete savePanel;
		savePanel = nullptr;
	}

	BEntry	save_dir(quaSaveDirPath.Path(), true);

	savePanel = new BFilePanel(
						B_SAVE_PANEL,
						&rightHere,
						nullptr,
						0,
						FALSE);
				
	BMessage		m(B_SAVE_REQUESTED);
	if (andQuit) {
		m.AddInt32("terminally", 0);
	}
	savePanel->SetMessage(&m);
	savePanel->SetButtonLabel(B_CANCEL_BUTTON, "no way");
	savePanel->SetSaveText(sym->name);
	if (save_dir.InitCheck() == B_NO_ERROR) {
	fprintf(stderr, "save to dir %s\n", quaSaveDirPath.Path());
		savePanel->SetPanelDirectory(&save_dir); //quaSaveDirPath.Path());
	}
	
	savePanel->Window()->SetTitle("qua: save shgite information");
	savePanel->Refresh();
	savePanel->Show();
 	return B_NO_ERROR;
}
#endif

#ifdef _BEOS

char	*RefNm(entry_ref ref);
char *
RefNm(entry_ref ref)
{
	static char	nm[MAX_QUA_NAME_LENGTH];
	
	BEntry	theFile(&ref);
	theFile.GetName(nm);
	return nm;
}

status_t
Qua::DoSave(bool andQuit)
{
	status_t		err=B_NO_ERROR;
	
	if (projectScriptPath.InitCheck() != B_OK) {
		DoSaveAs(andQuit);
	} else {
		FILE	*quaSaveFile = fopen(projectScriptPath.Path(), "w");
		status_t err = sym->Save(quaSaveFile, 0);
//		quaSaveFile->SetSize(quaSaveFile->Position());
		if (err != B_NO_ERROR)
			reportError("can't save file");
// ????? work out something less arbitrary for binary data
// regular midi files might not cut it
//		err = WriteChunk(quapSaveFile, 'QUAP', 0, 0);
//		if (err < B_NO_ERROR)
//			reportError("can't save .crp file");
//		if (andQuit) {
//#if defined(_BEOS)
//			quapp->PostMessage(QUA_CLOSE, this);
//#endif
//		}
	}
	return err;
}


#endif
