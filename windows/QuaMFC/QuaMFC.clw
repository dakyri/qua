; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CChildFrame
LastTemplate=CView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "QuaMFC.h"
LastPage=0

ClassCount=17
Class1=CQuaMFCApp
Class2=CQuaMFCDoc
Class3=CQuaMFCView
Class4=CMainFrame
Class7=CSplashWnd

ResourceCount=11
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDR_QUAMFCTYPE
Class5=CChildFrame
Class6=CAboutDlg
Resource4=IDR_MAINFRAME (English (U.S.))
Resource5=IDD_PROP_CONFIG
Resource6=IDD_ABOUTBOX (English (U.S.))
Class8=TransportControl
Resource7=IDD_PROP_MIDI
Resource8=IDD_PROP_VST
Resource9=IDR_QUAMFCTYPE (English (U.S.))
Resource10=IDD_TRANSPORT_CTL
Class9=PreferencesPropertySheet
Class10=CPreviewWnd
Class11=CPropertyFrame
Class12=CMyPropertyPage1
Class13=CMyPropertyPage2
Class14=CMyPropertyPage3
Class15=CMyPropertyPage4
Class16=MFCArrangeView
Class17=MFCChannelView
Resource11=IDD_PROP_AUDIO

[CLS:CQuaMFCApp]
Type=0
HeaderFile=QuaMFC.h
ImplementationFile=QuaMFC.cpp
Filter=N

[CLS:CQuaMFCDoc]
Type=0
HeaderFile=QuaMFCDoc.h
ImplementationFile=QuaMFCDoc.cpp
Filter=N
LastObject=CQuaMFCDoc

[CLS:CQuaMFCView]
Type=0
HeaderFile=QuaMFCView.h
ImplementationFile=QuaMFCView.cpp
Filter=C
LastObject=CQuaMFCView
BaseClass=CView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CMDIFrameWnd
VirtualFilter=fWC


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
LastObject=CChildFrame
BaseClass=CMDIChildWnd
VirtualFilter=mfWC


[CLS:CAboutDlg]
Type=0
HeaderFile=QuaMFC.cpp
ImplementationFile=QuaMFC.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
CommandCount=8
Command8=ID_APP_ABOUT

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
CommandCount=8
Command8=ID_APP_ABOUT

[MNU:IDR_QUAMFCTYPE]
Type=1
Class=CQuaMFCView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
CommandCount=21
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
CommandCount=14
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE


[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_CONTEXT_HELP
CommandCount=9

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_FILE_PREFERENCES
Command6=ID_APP_EXIT
Command7=ID_VIEW_TOOLBAR
Command8=ID_VIEW_STATUS_BAR
Command9=ID_APP_ABOUT
Command10=ID_HELP_DONTPANIC
CommandCount=10

[MNU:IDR_QUAMFCTYPE (English (U.S.))]
Type=1
Class=CQuaMFCView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_FILE_PREFERENCES
Command11=ID_APP_EXIT
Command12=ID_EDIT_UNDO
Command13=ID_EDIT_CUT
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_VIEW_TOOLBAR
Command17=ID_VIEW_STATUS_BAR
Command18=ID_WINDOW_NEW
Command19=ID_WINDOW_CASCADE
Command20=ID_WINDOW_TILE_HORZ
Command21=ID_WINDOW_ARRANGE
Command22=ID_APP_ABOUT
Command23=ID_HELP_DONTPANIC
CommandCount=23

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342312577
Control2=IDC_STATIC,static,1342312961
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342312449

[CLS:CSplashWnd]
Type=0
HeaderFile=Splash.h
ImplementationFile=Splash.cpp
BaseClass=CWnd
LastObject=CSplashWnd

[DLG:IDD_TRANSPORT_CTL]
Type=1
Class=TransportControl
ControlCount=5
Control1=IDC_PLAYBUT,button,1342242944
Control2=IDC_STATIC,static,1342177294
Control3=IDC_RICHEDIT1,RICHEDIT,1350631552
Control4=IDC_CUSTOM1,,1342242816
Control5=IDC_CUSTOM2,,1342242816

[CLS:TransportControl]
Type=0
HeaderFile=TransportControl.h
ImplementationFile=TransportControl.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_APP_ABOUT
VirtualFilter=dWC

[CLS:PreferencesPropertySheet]
Type=0
HeaderFile=PreferencesPropertySheet.h
ImplementationFile=PreferencesPropertySheet.cpp
BaseClass=CPropertySheet

[CLS:CPreviewWnd]
Type=0
HeaderFile=PreviewWnd.h
ImplementationFile=PreviewWnd.cpp
BaseClass=CWnd

[CLS:CPropertyFrame]
Type=0
HeaderFile=PropertyFrame.h
ImplementationFile=PropertyFrame.cpp
BaseClass=CMiniFrameWnd

[CLS:CMyPropertyPage1]
Type=0
HeaderFile=MyPropertyPage1.h
ImplementationFile=MyPropertyPage1.cpp
BaseClass=CPropertyPage

[CLS:CMyPropertyPage2]
Type=0
HeaderFile=MyPropertyPage1.h
ImplementationFile=MyPropertyPage1.cpp
BaseClass=CPropertyPage

[CLS:CMyPropertyPage3]
Type=0
HeaderFile=MyPropertyPage1.h
ImplementationFile=MyPropertyPage1.cpp
BaseClass=CPropertyPage

[CLS:CMyPropertyPage4]
Type=0
HeaderFile=MyPropertyPage1.h
ImplementationFile=MyPropertyPage1.cpp
BaseClass=CPropertyPage

[DLG:IDD_PROP_CONFIG]
Type=1
Class=CMyPropertyPage1
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_PROP_VST]
Type=1
Class=CMyPropertyPage2
ControlCount=5
Control1=IDC_STATIC,static,1342308353
Control2=IDC_LIST1,listbox,1352728835
Control3=IDC_LIST2,listbox,1352728835
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_PROP_MIDI]
Type=1
Class=CMyPropertyPage3
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_PROP_AUDIO]
Type=1
Class=CMyPropertyPage4
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[CLS:MFCArrangeView]
Type=0
HeaderFile=MFCArrangeView.h
ImplementationFile=MFCArrangeView.cpp
BaseClass=CView
Filter=C
LastObject=ID_APP_ABOUT

[CLS:MFCChannelView]
Type=0
HeaderFile=MFCChannelView.h
ImplementationFile=MFCChannelView.cpp
BaseClass=CView
Filter=C
LastObject=ID_APP_ABOUT

