#include "version.h"
#include "RosterView.h"
#include "Colors.h"

class CruiserWindow: public BWindow
{
public:
							CruiserWindow();
							~CruiserWindow();
	virtual bool			QuitRequested();
	RosterView				*rosterView;
};

class CruiserApp: public BApplication
{
public:
							CruiserApp();
							
	CruiserWindow			*cruiserWindow;
};

CruiserWindow::CruiserWindow():
	BWindow(BRect(10, 10, 50, 50),
			"Cruiser",
			B_TITLED_WINDOW, nullptr)
{
	rosterView = new RosterView(Bounds());
	rosterView->SetViewColor(blue);
	AddChild(rosterView);
	resume_thread(rosterView->cruiseThread);
}

CruiserWindow::~CruiserWindow()
{
}

bool
CruiserWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

CruiserApp::CruiserApp():
	BApplication("application/x-vnd.Dak-Cruiser")
{
	cruiserWindow = new CruiserWindow();
	cruiserWindow->Show();
}

main()
{
	CruiserApp		*theapp;
	
	theapp = new CruiserApp();
	theapp->Run();
}