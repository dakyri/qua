# Microsoft Developer Studio Project File - Name="QuaMFC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=QuaMFC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuaMFC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuaMFC.mak" CFG="QuaMFC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QuaMFC - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "QuaMFC - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "G:\source\Gloubals" /I "G:\source\Portals" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib G:\source\Portals\Portals\Debug\Portals.lib G:\source\Gloubals\Gloubal\Debug\Gloubal.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "QuaMFC - Win32 Release"
# Name "QuaMFC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "MFC Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\QuaMFC.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# ADD CPP /I "..\Source"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\QuaMFC.rc
# End Source File
# Begin Source File

SOURCE=.\QuaMFCDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\QuaMFCView.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "MFC Widget"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MFCArrangeView.cpp
# End Source File
# Begin Source File

SOURCE=.\MFCChannelView.cpp
# End Source File
# Begin Source File

SOURCE=.\MyPropertyPage1.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\PreviewWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\TransportControl.cpp
# End Source File
# End Group
# Begin Group "Qua"

# PROP Default_Filter ""
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Qua.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaPP.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaQua.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaQuaQua.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Block"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Block.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Esrap.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Expression.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Parse.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaFX.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Roll.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\UpdateActive.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Base Types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\BaseVal.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Clock.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Envelope.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Log.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Metric.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Note.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Properties.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaInsert.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Stacker.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\StreamItem.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Sym.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Time.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\ValList.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Devices"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Midi.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\PoolPlayer.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaAudio.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaMedia.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaParallel.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Executable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Channel.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Controllable.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Destination.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Executable.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Instance.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Method.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Nofifiable.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Pool.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaPort.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Sample.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Schedulable.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Template.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Voice.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Object View"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\ArrangerObject.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuaObject.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\SymObject.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\AQuarium.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\RosterView.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\SequencerWindow.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\TimeArrange.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# PROP Exclude_From_Build 1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Fundamental Structures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\ControlVar.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\QuasiStack.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Stream.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "QuaVST"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\VstPlugin.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Vst"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\aeffguieditor.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\AudioEffect.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\audioeffectx.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstcontrols.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstgui.cpp

!IF  "$(CFG)" == "QuaMFC - Win32 Release"

!ELSEIF  "$(CFG)" == "QuaMFC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "ASIO"

# PROP Default_Filter ""
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "MFC Main Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\QuaMFC.h
# End Source File
# Begin Source File

SOURCE=.\QuaMFCDoc.h
# End Source File
# Begin Source File

SOURCE=.\QuaMFCView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Portal Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Portals\DaApplication.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaBasicTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaErrorCodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaKernel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaMimeType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Portals\DaPath.h
# End Source File
# Begin Source File

SOURCE=..\..\Portals\DaPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\Portals\DaRect.h
# End Source File
# End Group
# Begin Group "Gloubal Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Gloubals\BaseTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\GLOUBALS\SampleFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\GLOUBALS\StdDefs.h
# End Source File
# End Group
# Begin Group "MFC Widget Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MFCArrangeView.h
# End Source File
# Begin Source File

SOURCE=.\MFCChannelView.h
# End Source File
# Begin Source File

SOURCE=.\MyPropertyPage1.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\PreviewWnd.h
# End Source File
# Begin Source File

SOURCE=.\PropertyFrame.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\TransportControl.h
# End Source File
# End Group
# Begin Group "Qua Headers"

# PROP Default_Filter ""
# Begin Group "Base Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\BaseVal.h
# End Source File
# Begin Source File

SOURCE=..\Source\Clock.h
# End Source File
# Begin Source File

SOURCE=..\Source\ControlVariable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Envelope.h
# End Source File
# Begin Source File

SOURCE=..\Source\Log.h
# End Source File
# Begin Source File

SOURCE=..\Source\Note.h
# End Source File
# Begin Source File

SOURCE=..\Source\Properties.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuasiStack.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuaTypes.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuickList.h
# End Source File
# Begin Source File

SOURCE=..\QuaWin32\SaveFmt.h
# End Source File
# Begin Source File

SOURCE=..\Source\Stacker.h
# End Source File
# Begin Source File

SOURCE=..\Source\Stream.h
# End Source File
# Begin Source File

SOURCE=..\Source\Sym.h
# End Source File
# Begin Source File

SOURCE=..\Source\Time.h
# End Source File
# Begin Source File

SOURCE=..\Source\TimeKeeper.h
# End Source File
# Begin Source File

SOURCE=..\Source\ValList.h
# End Source File
# Begin Source File

SOURCE=..\Source\version.h
# End Source File
# End Group
# Begin Group "Display Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Editor.h
# End Source File
# Begin Source File

SOURCE=..\Source\LalaLand.h
# End Source File
# Begin Source File

SOURCE=..\Source\RateCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Source\RenderStatusView.h
# End Source File
# Begin Source File

SOURCE=..\Source\RosterView.h
# End Source File
# Begin Source File

SOURCE=..\Source\SampleEditor.h
# End Source File
# Begin Source File

SOURCE=..\Source\SchedulablePanel.h
# End Source File
# Begin Source File

SOURCE=..\Source\SelectorPanel.h
# End Source File
# Begin Source File

SOURCE=..\Source\SequencerWindow.h
# End Source File
# Begin Source File

SOURCE=..\Source\SymEditBlockView.h
# End Source File
# Begin Source File

SOURCE=..\Source\SymEditPanel.h
# End Source File
# Begin Source File

SOURCE=..\Source\TakeView.h
# End Source File
# Begin Source File

SOURCE=..\Source\TimeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Source\Toolbox.h
# End Source File
# Begin Source File

SOURCE=..\Source\VoiceController.h
# End Source File
# Begin Source File

SOURCE=..\Source\VoiceEditor.h
# End Source File
# Begin Source File

SOURCE=..\Source\XYView.h
# End Source File
# End Group
# Begin Group "Executable Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Channel.h
# End Source File
# Begin Source File

SOURCE=..\Source\Controllable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Destination.h
# End Source File
# Begin Source File

SOURCE=..\Source\Executable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Insertable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Instance.h
# End Source File
# Begin Source File

SOURCE=..\Source\Notifiable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Pool.h
# End Source File
# Begin Source File

SOURCE=..\Source\Quaddon.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuaFX.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuaInsert.h
# End Source File
# Begin Source File

SOURCE=..\Source\Sample.h
# End Source File
# Begin Source File

SOURCE=..\Source\SampleSplice.h
# End Source File
# Begin Source File

SOURCE=..\Source\Schedulable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Stackable.h
# End Source File
# Begin Source File

SOURCE=..\Source\Template.h
# End Source File
# Begin Source File

SOURCE=..\Source\Voice.h
# End Source File
# End Group
# Begin Group "Block Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Block.h
# End Source File
# Begin Source File

SOURCE=..\Source\Expression.h
# End Source File
# Begin Source File

SOURCE=..\Source\Parse.h
# End Source File
# End Group
# Begin Group "Device Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\QuaAudio.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuaMidi.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuaParallel.h
# End Source File
# Begin Source File

SOURCE=..\Source\QuaPort.h
# End Source File
# Begin Source File

SOURCE=..\Source\SamplePlayer.h
# End Source File
# Begin Source File

SOURCE=..\Source\Sampler.h
# End Source File
# End Group
# Begin Group "Main Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Qua.h
# End Source File
# Begin Source File

SOURCE=..\Source\Quapp.h
# End Source File
# End Group
# Begin Group "Display Object Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\ChannelObjectView.h
# End Source File
# Begin Source File

SOURCE=..\Source\Controller.h
# End Source File
# Begin Source File

SOURCE=..\Source\Locus.h
# End Source File
# Begin Source File

SOURCE=..\Source\ObjectView.h
# End Source File
# Begin Source File

SOURCE=..\Source\Other.h
# End Source File
# Begin Source File

SOURCE=..\Source\Panel.h
# End Source File
# Begin Source File

SOURCE=..\Source\PortObject.h
# End Source File
# Begin Source File

SOURCE=..\Source\SampleController.h
# End Source File
# Begin Source File

SOURCE=..\Source\SymObject.h
# End Source File
# End Group
# Begin Group "QuaVST headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\VSTPlugin.h
# End Source File
# End Group
# End Group
# Begin Group "Vst Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\AEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\aeffectx.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\aeffguieditor.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\audioeffectx.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstcontrols.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstfxstore.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstgui.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstplugscarbon.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstplugsmac.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\vstsdk2.3\source\common\vstplugsmacho.h
# End Source File
# End Group
# Begin Group "ASIO headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\VST\asiosdk2\common\asio.h
# End Source File
# Begin Source File

SOURCE=..\..\VST\asiosdk2\common\asiosys.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\background.BMP
# End Source File
# Begin Source File

SOURCE=.\background256.bmp
# End Source File
# Begin Source File

SOURCE=.\res\QuaMFC.ico
# End Source File
# Begin Source File

SOURCE=.\res\QuaMFC.rc2
# End Source File
# Begin Source File

SOURCE=.\res\QuaMFCDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\transporter256.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section QuaMFC : {72ADFD78-2C39-11D0-9903-00A0C91BC942}
# 	1:10:IDB_SPLASH:102
# 	2:21:SplashScreenInsertKey:4.0
# End Section
# Section QuaMFC : {72ADFD66-2C39-11D0-9903-00A0C91BC942}
# 	2:10:CMainFrame:Palette support added
# End Section
