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
Source: data\scripts\u6\actor.lua; DestDir: {app}\data\scripts\u6; Flags: ignoreversion
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
Source: tools\unpack_portraits.exe; DestDir: {app}\tools\; Flags: ignoreversion

[Dirs]
Name: {app}\data;
Name: {app}\martian_save;
Name: {app}\savage_save;
Name: {app}\tools;
Name: {app}\u6_sounds;
Name: {app}\u6_save;
