# Microsoft Developer Studio Project File - Name="nuvie" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=nuvie - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nuvie.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nuvie.mak" CFG="nuvie - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nuvie - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "nuvie - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nuvie - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../conf" /I "../files" /I "../misc" /I "../screen" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 sdl.lib sdlmain.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "nuvie - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../conf" /I "../files" /I "../misc" /I "../screen" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_STLP_DEBUG" /D "_STLP_USE_NEWALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib sdlmain.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "nuvie - Win32 Release"
# Name "nuvie - Win32 Debug"
# Begin Group "conf files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\conf\ConfigNode.h
# End Source File
# Begin Source File

SOURCE=..\conf\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=..\conf\Configuration.h
# End Source File
# Begin Source File

SOURCE=..\conf\misc.h
# End Source File
# Begin Source File

SOURCE=..\conf\XMLNode.cpp
# End Source File
# Begin Source File

SOURCE=..\conf\XMLNode.h
# End Source File
# Begin Source File

SOURCE=..\conf\XMLTree.cpp
# End Source File
# Begin Source File

SOURCE=..\conf\XMLTree.h
# End Source File
# End Group
# Begin Group "files files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\files\U6Bmp.cpp
# End Source File
# Begin Source File

SOURCE=..\files\U6Bmp.h
# End Source File
# Begin Source File

SOURCE=..\files\U6File.cpp
# End Source File
# Begin Source File

SOURCE=..\files\U6File.h
# End Source File
# Begin Source File

SOURCE=..\files\U6Lib_n.cpp
# End Source File
# Begin Source File

SOURCE=..\files\U6Lib_n.h
# End Source File
# Begin Source File

SOURCE=..\files\U6Lzw.cpp
# End Source File
# Begin Source File

SOURCE=..\files\U6Lzw.h
# End Source File
# End Group
# Begin Group "misc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\misc\U6def.h
# End Source File
# Begin Source File

SOURCE=..\misc\U6LList.cpp
# End Source File
# Begin Source File

SOURCE=..\misc\U6LList.h
# End Source File
# End Group
# Begin Group "screen files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\screen\GamePalette.cpp
# End Source File
# Begin Source File

SOURCE=..\screen\GamePalette.h
# End Source File
# Begin Source File

SOURCE=..\screen\Scale.cpp
# End Source File
# Begin Source File

SOURCE=..\screen\Scale.h
# End Source File
# Begin Source File

SOURCE=..\screen\Scale.inl
# End Source File
# Begin Source File

SOURCE=..\screen\Screen.cpp
# End Source File
# Begin Source File

SOURCE=..\screen\Screen.h
# End Source File
# Begin Source File

SOURCE=..\screen\Surface.cpp
# End Source File
# Begin Source File

SOURCE=..\screen\Surface.h
# End Source File
# End Group
# Begin Group "nuvie files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Actor.cpp
# End Source File
# Begin Source File

SOURCE=..\Actor.h
# End Source File
# Begin Source File

SOURCE=..\ActorManager.cpp
# End Source File
# Begin Source File

SOURCE=..\ActorManager.h
# End Source File
# Begin Source File

SOURCE=..\Book.cpp
# End Source File
# Begin Source File

SOURCE=..\Book.h
# End Source File
# Begin Source File

SOURCE=..\Event.cpp
# End Source File
# Begin Source File

SOURCE=..\Event.h
# End Source File
# Begin Source File

SOURCE=..\Game.cpp
# End Source File
# Begin Source File

SOURCE=..\Game.h
# End Source File
# Begin Source File

SOURCE=..\Look.cpp
# End Source File
# Begin Source File

SOURCE=..\Look.h
# End Source File
# Begin Source File

SOURCE=..\main.cpp
# End Source File
# Begin Source File

SOURCE=..\main.h
# End Source File
# Begin Source File

SOURCE=..\Map.cpp
# End Source File
# Begin Source File

SOURCE=..\Map.h
# End Source File
# Begin Source File

SOURCE=..\MapWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\MapWindow.h
# End Source File
# Begin Source File

SOURCE=..\MsgScroll.cpp
# End Source File
# Begin Source File

SOURCE=..\MsgScroll.h
# End Source File
# Begin Source File

SOURCE=..\nuvie.cpp
# End Source File
# Begin Source File

SOURCE=..\nuvie.h
# End Source File
# Begin Source File

SOURCE=..\ObjManager.cpp
# End Source File
# Begin Source File

SOURCE=..\ObjManager.h
# End Source File
# Begin Source File

SOURCE=..\Player.cpp
# End Source File
# Begin Source File

SOURCE=..\Player.h
# End Source File
# Begin Source File

SOURCE=..\Text.cpp
# End Source File
# Begin Source File

SOURCE=..\Text.h
# End Source File
# Begin Source File

SOURCE=..\TileManager.cpp
# End Source File
# Begin Source File

SOURCE=..\TileManager.h
# End Source File
# End Group
# Begin Group "msvc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msvc_kludges.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ChangeLog
# End Source File
# Begin Source File

SOURCE=..\nuvie.cfg
# End Source File
# End Target
# End Project
