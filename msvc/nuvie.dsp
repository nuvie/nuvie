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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I ".." /I "../actors" /I "../conf" /I "../files" /I "../gui" /I "../misc" /I "../pathfinder" /I "../screen" /I "../views" /I "../usecode" /I "../sound" /I "../sound/adplug" /I "../save" /I "../fonts" /I "../visualc7" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 sdl.lib sdlmain.lib sdl_mixer.lib /nologo /subsystem:console /machine:I386

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".." /I "../actors" /I "../conf" /I "../files" /I "../gui" /I "../misc" /I "../pathfinder" /I "../screen" /I "../views" /I "../usecode" /I "../sound" /I "../sound/adplug" /I "../save" /I "../fonts" /I "../visualc7" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_STLP_DEBUG" /D "_STLP_USE_NEWALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sdl.lib sdlmain.lib sdl_mixer.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /pdbtype:sept

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

SOURCE=..\files\NuvieFileList.cpp
# End Source File
# Begin Source File

SOURCE=..\files\NuvieFileList.h
# End Source File
# Begin Source File

SOURCE=..\files\NuvieIO.cpp
# End Source File
# Begin Source File

SOURCE=..\files\NuvieIO.h
# End Source File
# Begin Source File

SOURCE=..\files\NuvieIOFile.cpp
# End Source File
# Begin Source File

SOURCE=..\files\NuvieIOFile.h
# End Source File
# Begin Source File

SOURCE=..\files\U6Bmp.cpp
# End Source File
# Begin Source File

SOURCE=..\files\U6Bmp.h
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
# Begin Source File

SOURCE=..\files\U6Shape.cpp
# End Source File
# Begin Source File

SOURCE=..\files\U6Shape.h
# End Source File
# End Group
# Begin Group "misc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\misc\iAVLTree.cpp
# End Source File
# Begin Source File

SOURCE=..\misc\iAVLTree.h
# End Source File
# Begin Source File

SOURCE=..\misc\U6def.h
# End Source File
# Begin Source File

SOURCE=..\misc\U6LList.cpp
# End Source File
# Begin Source File

SOURCE=..\misc\U6LList.h
# End Source File
# Begin Source File

SOURCE=..\misc\U6misc.cpp
# End Source File
# Begin Source File

SOURCE=..\misc\U6misc.h
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

SOURCE=..\AnimManager.cpp
# End Source File
# Begin Source File

SOURCE=..\AnimManager.h
# End Source File
# Begin Source File

SOURCE=..\Background.cpp
# End Source File
# Begin Source File

SOURCE=..\Background.h
# End Source File
# Begin Source File

SOURCE=..\Book.cpp
# End Source File
# Begin Source File

SOURCE=..\Book.h
# End Source File
# Begin Source File

SOURCE=..\CommandBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CommandBar.h
# End Source File
# Begin Source File

SOURCE=..\Converse.cpp
# End Source File
# Begin Source File

SOURCE=..\Converse.h
# End Source File
# Begin Source File

SOURCE=..\ConverseInterpret.cpp
# End Source File
# Begin Source File

SOURCE=..\ConverseInterpret.h
# End Source File
# Begin Source File

SOURCE=..\ConverseSpeech.cpp
# End Source File
# Begin Source File

SOURCE=..\ConverseSpeech.h
# End Source File
# Begin Source File

SOURCE=..\Cursor.cpp
# End Source File
# Begin Source File

SOURCE=..\Cursor.h
# End Source File
# Begin Source File

SOURCE=..\Effect.cpp
# End Source File
# Begin Source File

SOURCE=..\Effect.h
# End Source File
# Begin Source File

SOURCE=..\EffectManager.cpp
# End Source File
# Begin Source File

SOURCE=..\EffectManager.h
# End Source File
# Begin Source File

SOURCE=..\EggManager.cpp
# End Source File
# Begin Source File

SOURCE=..\EggManager.h
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

SOURCE=..\GameClock.cpp
# End Source File
# Begin Source File

SOURCE=..\GameClock.h
# End Source File
# Begin Source File

SOURCE=..\GameSelect.cpp
# End Source File
# Begin Source File

SOURCE=..\GameSelect.h
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

SOURCE=..\nuvieDefs.h
# End Source File
# Begin Source File

SOURCE=..\ObjManager.cpp
# End Source File
# Begin Source File

SOURCE=..\ObjManager.h
# End Source File
# Begin Source File

SOURCE=..\Party.cpp
# End Source File
# Begin Source File

SOURCE=..\Party.h
# End Source File
# Begin Source File

SOURCE=..\Player.cpp
# End Source File
# Begin Source File

SOURCE=..\Player.h
# End Source File
# Begin Source File

SOURCE=..\Portrait.cpp
# End Source File
# Begin Source File

SOURCE=..\Portrait.h
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
# Begin Source File

SOURCE=..\TimedEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\TimedEvent.h
# End Source File
# Begin Source File

SOURCE=..\U6objects.h
# End Source File
# End Group
# Begin Group "msvc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msvc_kludges.h
# End Source File
# End Group
# Begin Group "views files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\views\ActorView.cpp
# End Source File
# Begin Source File

SOURCE=..\views\ActorView.h
# End Source File
# Begin Source File

SOURCE=..\views\DollWidget.cpp
# End Source File
# Begin Source File

SOURCE=..\views\DollWidget.h
# End Source File
# Begin Source File

SOURCE=..\views\InventoryView.cpp
# End Source File
# Begin Source File

SOURCE=..\views\InventoryView.h
# End Source File
# Begin Source File

SOURCE=..\views\InventoryWidget.cpp
# End Source File
# Begin Source File

SOURCE=..\views\InventoryWidget.h
# End Source File
# Begin Source File

SOURCE=..\views\PartyView.cpp
# End Source File
# Begin Source File

SOURCE=..\views\PartyView.h
# End Source File
# Begin Source File

SOURCE=..\views\PortraitView.cpp
# End Source File
# Begin Source File

SOURCE=..\views\PortraitView.h
# End Source File
# Begin Source File

SOURCE=..\views\View.cpp
# End Source File
# Begin Source File

SOURCE=..\views\View.h
# End Source File
# Begin Source File

SOURCE=..\views\ViewManager.cpp
# End Source File
# Begin Source File

SOURCE=..\views\ViewManager.h
# End Source File
# End Group
# Begin Group "usecode files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\usecode\MDUseCode.cpp
# End Source File
# Begin Source File

SOURCE=..\usecode\MDUseCode.h
# End Source File
# Begin Source File

SOURCE=..\usecode\SEUseCode.cpp
# End Source File
# Begin Source File

SOURCE=..\usecode\SEUseCode.h
# End Source File
# Begin Source File

SOURCE=..\usecode\U6UseCode.cpp
# End Source File
# Begin Source File

SOURCE=..\usecode\U6UseCode.h
# End Source File
# Begin Source File

SOURCE=..\usecode\UseCode.cpp
# End Source File
# Begin Source File

SOURCE=..\usecode\UseCode.h
# End Source File
# End Group
# Begin Group "actors files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\actors\Actor.cpp
# End Source File
# Begin Source File

SOURCE=..\actors\Actor.h
# End Source File
# Begin Source File

SOURCE=..\actors\ActorManager.cpp
# End Source File
# Begin Source File

SOURCE=..\actors\ActorManager.h
# End Source File
# Begin Source File

SOURCE=..\actors\U6Actor.cpp
# End Source File
# Begin Source File

SOURCE=..\actors\U6Actor.h
# End Source File
# End Group
# Begin Group "pathfinder files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pathfinder\AStar.cpp
# End Source File
# Begin Source File

SOURCE=..\pathfinder\AStar.h
# End Source File
# Begin Source File

SOURCE=..\pathfinder\LPath.cpp
# End Source File
# Begin Source File

SOURCE=..\pathfinder\LPath.h
# End Source File
# Begin Source File

SOURCE=..\pathfinder\PathFinder.cpp
# End Source File
# Begin Source File

SOURCE=..\pathfinder\PathFinder.h
# End Source File
# Begin Source File

SOURCE=..\pathfinder\ZPath.cpp
# End Source File
# Begin Source File

SOURCE=..\pathfinder\ZPath.h
# End Source File
# End Group
# Begin Group "gui files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Gui\GUI.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_area.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_area.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_button.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_button.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_CallBack.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_Dialog.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_DragArea.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_DragManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_DragManager.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_font.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_font.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_loadimage.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_loadimage.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_ScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_ScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_Scroller.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_Scroller.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_status.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_text.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_text.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_TextInput.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_TextInput.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_types.h
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_widget.cpp
# End Source File
# Begin Source File

SOURCE=..\Gui\GUI_widget.h
# End Source File
# Begin Source File

SOURCE=..\GUI\GUI_YesNoDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\GUI\GUI_YesNoDialog.h
# End Source File
# Begin Source File

SOURCE=..\Gui\the_font.h
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Group "adplug files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sound\adplug\adplug_player.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\adplug_player.h
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\emuopl.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\emuopl.h
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\fmopl.c
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\fmopl.h
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\opl.h
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\silentopl.h
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\u6m.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\adplug\u6m.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\sound\Sample.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\Sample.h
# End Source File
# Begin Source File

SOURCE=..\sound\Song.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\Song.h
# End Source File
# Begin Source File

SOURCE=..\sound\SongAdPlug.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\SongAdPlug.h
# End Source File
# Begin Source File

SOURCE=..\sound\Sound.h
# End Source File
# Begin Source File

SOURCE=..\sound\SoundManager.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\SoundManager.h
# End Source File
# End Group
# Begin Group "fonts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\fonts\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\fonts\Font.h
# End Source File
# Begin Source File

SOURCE=..\fonts\FontManager.cpp
# End Source File
# Begin Source File

SOURCE=..\fonts\FontManager.h
# End Source File
# End Group
# Begin Group "save files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\save\SaveDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\save\SaveDialog.h
# End Source File
# Begin Source File

SOURCE=..\save\SaveGame.cpp
# End Source File
# Begin Source File

SOURCE=..\save\SaveGame.h
# End Source File
# Begin Source File

SOURCE=..\save\SaveManager.cpp
# End Source File
# Begin Source File

SOURCE=..\save\SaveManager.h
# End Source File
# Begin Source File

SOURCE=..\save\SaveSlot.cpp
# End Source File
# Begin Source File

SOURCE=..\save\SaveSlot.h
# End Source File
# End Group
# Begin Group "visualc7 files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\visualc7\dirent.cpp
# End Source File
# Begin Source File

SOURCE=..\visualc7\dirent.h
# End Source File
# Begin Source File

SOURCE=..\visualc7\vc7inc.h
# End Source File
# End Group
# Begin Group "gui folder"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=..\ChangeLog
# End Source File
# Begin Source File

SOURCE=..\nuvie.cfg
# End Source File
# End Target
# End Project
