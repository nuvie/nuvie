[Setup]
AppCopyright=2011
AppName=Nuvie
AppVerName=Nuvie SVN
AppPublisher=The Nuvie Team
AppPublisherURL=http://nuvie.sourceforge.net/
AppSupportURL=http://nuvie.sourceforge.net/
AppUpdatesURL=http://nuvie.sourceforge.net/
DefaultDirName={pf}\Nuvie
DefaultGroupName=Nuvie
AllowNoIcons=true
AlwaysUsePersonalGroup=false
EnableDirDoesntExistWarning=false
OutputDir=C:\Ultima6
OutputBaseFilename=Nuviewin32
DisableStartupPrompt=true
AppendDefaultDirName=false    
SolidCompression=true
DirExistsWarning=no 

[Icons]
Name: {group}\{cm:UninstallProgram, Nuvie}; Filename: {uninstallexe}
Name: {group}\Nuvie; Filename: {app}\nuvie.exe; WorkingDir: {app}; Comment: nuvie; Flags: createonlyiffileexists
Name: {group}\AUTHORS; Filename: {app}\AUTHORS.txt; WorkingDir: {app}; Comment: AUTHORS; Flags: createonlyiffileexists
Name: {group}\COPYING; Filename: {app}\COPYING.txt; WorkingDir: {app}; Comment: COPYING; Flags: createonlyiffileexists
Name: {group}\ChangeLog; Filename: {app}\ChangeLog.txt; WorkingDir: {app}; Comment: ChangeLog; Flags: createonlyiffileexists
Name: {group}\README; Filename: {app}\README.txt; WorkingDir: {app}; Comment: README; Flags: createonlyiffileexists

[Run]
Filename: {app}\nuvie.exe; Description: Launch Nuvie; Flags: nowait skipifdoesntexist postinstall skipifsilent

[Files]
Source: AUTHORS.txt; DestDir: {app}; Flags: ignoreversion
Source: ChangeLog.txt; DestDir: {app}; Flags: ignoreversion
Source: COPYING.txt; DestDir: {app}; Flags: ignoreversion
Source: nuvie.cfg.sample; DestDir: {app}; Flags: ignoreversion
Source: nuvie.exe; DestDir: {app}; Flags: ignoreversion
Source: README.txt; DestDir: {app}; Flags: ignoreversion isreadme
Source: README-SDL.txt; DestDir: {app}; Flags: ignoreversion
Source: SDL.dll; DestDir: {app}; Flags: ignoreversion
Source: data\BorderU6_1.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_2.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_3.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_4.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_5.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_6.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_7.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\BorderU6_8.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\ScrollBarDown_1.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\ScrollBarDown_2.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\ScrollBarUp_1.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\ScrollBarUp_2.bmp; DestDir: {app}\data\; Flags: ignoreversion
Source: data\defaultkeys.txt; DestDir: {app}\data\; Flags: ignoreversion
Source: data\u6keys.txt; DestDir: {app}\data\; Flags: ignoreversion
Source: data\sekeys.txt; DestDir: {app}\data\; Flags: ignoreversion
Source: data\mdkeys.txt; DestDir: {app}\data\; Flags: ignoreversion
Source: data\images\gumps\gump_btn_down.bmp; DestDir: {app}\data\images\gumps; Flags: ignoreversion
Source: data\images\gumps\gump_btn_up.bmp; DestDir: {app}\data\images\gumps; Flags: ignoreversion
Source: data\images\gumps\left_arrow.bmp; DestDir: {app}\data\images\gumps; Flags: ignoreversion
Source: data\images\gumps\right_arrow.bmp; DestDir: {app}\data\images\gumps; Flags: ignoreversion
Source: data\images\gumps\portrait_bg.bmp; DestDir: {app}\data\images\gumps; Flags: ignoreversion
Source: data\images\gumps\sign\sign_bg.bmp; DestDir: {app}\data\images\gumps\sign; Flags: ignoreversion
Source: data\images\gumps\sign\sign_font.bmp; DestDir: {app}\data\images\gumps\sign; Flags: ignoreversion
Source: data\images\gumps\sign\sign_font.dat; DestDir: {app}\data\images\gumps\sign; Flags: ignoreversion
Source: data\images\gumps\container\bag_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\barrel_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\chest_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\corpse_animal_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\corpse_body_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\corpse_cyclops_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\corpse_gargoyle_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\crate_bg.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_doll_btn_down.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_doll_btn_up.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_down_btn_down.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_down_btn_up.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_left_btn_down.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_left_btn_up.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_right_btn_down.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_right_btn_up.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_up_btn_down.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\container\cont_up_btn_up.bmp; DestDir: {app}\data\images\gumps\container; Flags: ignoreversion
Source: data\images\gumps\doll\combat_btn_down.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\combat_btn_up.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\doll_bg.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\heart_btn_down.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\heart_btn_up.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\inventory_btn_down.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\inventory_btn_up.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\party_btn_down.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\party_btn_up.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\avatar_U6_06.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\avatar_U6_07.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\avatar_U6_08.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\avatar_U6_09.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\avatar_U6_10.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\doll\avatar_U6_11.bmp; DestDir: {app}\data\images\gumps\doll; Flags: ignoreversion
Source: data\images\gumps\mapeditor\up_btn_up.bmp; DestDir: {app}\data\images\gumps\mapeditor; Flags: ignoreversion
Source: data\images\gumps\mapeditor\up_btn_down.bmp; DestDir: {app}\data\images\gumps\mapeditor; Flags: ignoreversion
Source: data\images\gumps\mapeditor\down_btn_up.bmp; DestDir: {app}\data\images\gumps\mapeditor; Flags: ignoreversion
Source: data\images\gumps\mapeditor\down_btn_down.bmp; DestDir: {app}\data\images\gumps\mapeditor; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_bg.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_left_arrow.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_right_arrow.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_000.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_001.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_002.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_003.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_004.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_005.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_006.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_007.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_008.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_009.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_016.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_017.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_018.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_019.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_020.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_021.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_022.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_023.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_024.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_025.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_032.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_033.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_034.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_035.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_036.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_037.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_038.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_039.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_040.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_041.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_048.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_049.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_050.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_051.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_052.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_053.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_054.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_055.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_056.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_057.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_064.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_065.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_066.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_067.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_068.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_069.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_070.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_071.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_072.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_073.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_080.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_081.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_082.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_083.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_084.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_085.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_086.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_087.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_088.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_089.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_096.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_097.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_098.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_099.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_100.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_101.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_102.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_103.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_104.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_105.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_112.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_113.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_114.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_115.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_116.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_117.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_118.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_119.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_120.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\spellbook_spell_121.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\1.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\2.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\3.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\4.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\5.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\6.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\7.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\8.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\st.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\nd.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\gumps\spellbook\rd.bmp; DestDir: {app}\data\images\gumps\spellbook; Flags: ignoreversion
Source: data\images\roof_tiles.bmp; DestDir: {app}\data\images; Flags: ignoreversion
Source: data\maps\u6\roof_map_00.dat; DestDir: {app}\data\maps\u6; Flags: ignoreversion
Source: data\scripts\md\actor.lua; DestDir: {app}\data\scripts\md; Flags: ignoreversion
Source: data\scripts\md\init.lua; DestDir: {app}\data\scripts\md; Flags: ignoreversion
Source: data\scripts\md\intro.lua; DestDir: {app}\data\scripts\md; Flags: ignoreversion
Source: data\scripts\se\actor.lua; DestDir: {app}\data\scripts\se; Flags: ignoreversion
Source: data\scripts\se\init.lua; DestDir: {app}\data\scripts\se; Flags: ignoreversion
Source: data\scripts\se\intro.lua; DestDir: {app}\data\scripts\se; Flags: ignoreversion
Source: data\scripts\u6\actor.lua; DestDir: {app}\data\scripts\u6; Flags: ignoreversion
Source: data\scripts\u6\ending.lua; DestDir: {app}\data\scripts\u6; Flags: ignoreversion
Source: data\scripts\u6\init.lua; DestDir: {app}\data\scripts\u6; Flags: ignoreversion
Source: data\scripts\u6\intro.lua; DestDir: {app}\data\scripts\u6; Flags: ignoreversion
Source: data\scripts\u6\magic.lua; DestDir: {app}\data\scripts\u6; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\create_food.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\detect_magic.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\detect_trap.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\dispel_magic.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\douse.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\harm.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\heal.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\help.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\ignite.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_01\light.lua; DestDir: {app}\data\scripts\u6\magic\circle_01; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\infravision.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\magic_arrow.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\poison.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\reappear.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\sleep.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\telekinesis.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\trap.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\unlock_magic.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\untrap.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_02\vanish.lua; DestDir: {app}\data\scripts\u6\magic\circle_02; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\curse.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\dispel_field.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\fireball.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\great_light.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\magic_lock.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\mass_awaken.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\mass_sleep.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\peer.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\protection.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_03\repel_undead.lua; DestDir: {app}\data\scripts\u6\magic\circle_03; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\animate.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\conjure.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\disable.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\fire_field.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\great_heal.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\locate.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\mass_dispel.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\poison_field.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\sleep_field.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_04\wind_change.lua; DestDir: {app}\data\scripts\u6\magic\circle_04; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\energy_field.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\explosion.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\insect_swarm.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\invisibility.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\lightning.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\paralyze.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\pickpocket.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\reveal.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\seance.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_05\xray.lua; DestDir: {app}\data\scripts\u6\magic\circle_05; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\charm.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\clone.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\confuse.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\flame_wind.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\hail_storm.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\mass_protect.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\negate_magic.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\poison_wind.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\replicate.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_06\web.lua; DestDir: {app}\data\scripts\u6\magic\circle_06; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\chain_bolt.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\enchant.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\energy_wind.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\fear.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\gate_travel.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\kill.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\mass_curse.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\mass_invisibility.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\wing_strike.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_07\wizard_eye.lua; DestDir: {app}\data\scripts\u6\magic\circle_07; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\armageddon.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\death_wind.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\eclipse.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\mass_charm.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\mass_kill.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\resurrect.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\slime.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\summon.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\time_stop.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: data\scripts\u6\magic\circle_08\tremor.lua; DestDir: {app}\data\scripts\u6\magic\circle_08; Flags: ignoreversion
Source: tools\SDL.dll; DestDir: {app}\tools\
Source: tools\u6decomp.exe; DestDir: {app}\tools\; Flags: ignoreversion
Source: tools\unpack.exe; DestDir: {app}\tools\; Flags: ignoreversion
Source: tools\unpack_conv.exe; DestDir: {app}\tools\; Flags: ignoreversion
Source: tools\unpack_font.exe; DestDir: {app}\tools\; Flags: ignoreversion
Source: tools\unpack_maptiles.exe; DestDir: {app}\tools\; Flags: ignoreversion
Source: tools\unpack_portraits.exe; DestDir: {app}\tools\; Flags: ignoreversion
Source: tools\unpack_talk.exe; DestDir: {app}\tools\; Flags: ignoreversion

[Dirs]
Name: {app}\data;
Name: {app}\martian_save;
Name: {app}\savage_save;
Name: {app}\tools;
Name: {app}\u6_sounds;
Name: {app}\u6_save;
