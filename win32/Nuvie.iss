[Setup]
AppCopyright=2015
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
Name: "{group}\{cm:UninstallProgram, Nuvie}"; Filename: "{uninstallexe}"
Name: "{group}\Nuvie"; Filename: "{app}\nuvie.exe"; WorkingDir: "{app}"; Flags: createonlyiffileexists; Comment: "nuvie"
Name: "{group}\Reset video settings"; Filename: "{app}\nuvie.exe"; WorkingDir: "{app}"; Flags: createonlyiffileexists; Parameters: "--reset-video"; Comment: "Reset video settings"
Name: "{group}\AUTHORS"; Filename: "{app}\AUTHORS.txt"; WorkingDir: "{app}"; Flags: createonlyiffileexists; Comment: "AUTHORS"
Name: "{group}\COPYING"; Filename: "{app}\COPYING.txt"; WorkingDir: "{app}"; Flags: createonlyiffileexists; Comment: "COPYING"
Name: "{group}\ChangeLog"; Filename: "{app}\ChangeLog.txt"; WorkingDir: "{app}"; Flags: createonlyiffileexists; Comment: "ChangeLog"
Name: "{group}\README"; Filename: "{app}\README.txt"; WorkingDir: "{app}"; Flags: createonlyiffileexists; Comment: "README"
Name: "{group}\Configuration File"; Filename: "{app}\nuvie.cfg"; WorkingDir: "{app}"; Flags: createonlyiffileexists
Name: "{group}\Nuvie Support Forum"; Filename: "http://nuvie.sourceforge.net/phorum/"

[Run]
Filename: {app}\nuvie.exe; Description: Launch Nuvie; Flags: nowait skipifdoesntexist postinstall skipifsilent

[Code]
// code based on Exult's Exult.iss script
var
  FilePage: TInputFileWizardPage;
  DataDirPage: TWizardPage;
  GameSelectPage: TWizardPage;
  bSetPaths: Boolean;
  U6BrowseButton: TButton;
  MDBrowseButton: TButton;
  SEBrowseButton: TButton;
  U6Text: TNewStaticText;
  MDText: TNewStaticText;
  SEText: TNewStaticText;
  InfoText: TNewStaticText;
  U6Edit: TEdit;
  MDEdit: TEdit;
  SEEdit: TEdit;
  GameCombo: TNewComboBox;

function GetDirPath(const Value: string): string;
begin
  Result := '';
  if FileExists(FilePage.Values[0]) then
    Result := ExtractFilePath(FilePage.Values[0]);
end;

// Write Paths into Nuvie.cfg
procedure SetNuvieGamePaths(sNuvieDir, sU6Path, sMDPath, sSEPath: String);
external 'SetNuvieGamePaths@files:nvconfig.dll stdcall';

// Write default game selection
procedure SetNuvieLoadGame(sNuvieDir, sLoadGame: String);
external 'SetNuvieLoadGame@files:nvconfig.dll stdcall';

// Read Paths from Nuvie.cfg
function GetNuvieGamePaths(sNuvieDir, sU6Path, sMDPath, sSEPath, sConfigPath: String; MaxPath: Integer) : Integer;
external 'GetNuvieGamePaths@files:nvconfig.dll stdcall';

// Verify Game dir
function VerifyGameDirectory(sPath, gameName: String) : Integer;
external 'VerifyGameDirectory@files:nvconfig.dll stdcall';

procedure OnU6BrowseButton(Sender: TObject);
var
  sDir : string;
begin
    sDir := U6Edit.Text;
    if BrowseForFolder('Select the folder where Ultima VI is installed.', sDir, False ) then begin

      if VerifyGameDirectory ( sDir, 'ultima6' ) = 0 then begin
        if MsgBox ('Folder does not seem to contain a valid installation of Ultima VI. Do you wish to continue?', mbConfirmation, MB_YESNO) = IDYES then begin
          U6Edit.Text := sDir;
        end
      end else
        U6Edit.Text := sDir;
    end;
end;

procedure OnMDBrowseButton(Sender: TObject);
var
  sDir : string;
begin
    sDir := MDEdit.Text;
    if BrowseForFolder('Select the folder where Martian Dreams is installed.', sDir, False ) then begin

      if VerifyGameDirectory ( sDir, 'martian' ) = 0 then begin
        if MsgBox ('Folder does not seem to contain a valid installation of Martian Dreams. Do you wish to continue?', mbConfirmation, MB_YESNO) = IDYES then begin
          MDEdit.Text := sDir;
        end
      end else
        MDEdit.Text := sDir;
    end;
end;

procedure OnSEBrowseButton(Sender: TObject);
var
  sDir : string;
begin
    sDir := SEEdit.Text;
    if BrowseForFolder('Select the folder where The Savage Empire is installed.', sDir, False ) then begin

      if VerifyGameDirectory ( sDir, 'savage' ) = 0 then begin
        if MsgBox ('Folder does not seem to contain a valid installation of The Savage Empire. Do you wish to continue?', mbConfirmation, MB_YESNO) = IDYES then begin
          SEEdit.Text := sDir;
        end
      end else
        SEEdit.Text := sDir;
    end;
end;

//
// Create the Directory browsing page
//
procedure InitializeWizard();
begin

  DataDirPage := CreateCustomPage(wpSelectTasks,
    'Select Game Folders', 'Select the folders where Ultima VI and the Worlds of Ultima games are installed.');

  U6Text := TNewStaticText.Create(DataDirPage);
  U6Text.Caption := 'Please enter the path where Ultima VI is installed.';
  U6Text.AutoSize := True;
  U6Text.Parent := DataDirPage.Surface;

  U6BrowseButton := TButton.Create(DataDirPage);
  U6BrowseButton.Top := U6Text.Top + U6Text.Height + ScaleY(8);
  U6BrowseButton.Left := DataDirPage.SurfaceWidth - ScaleX(75);
  U6BrowseButton.Width := ScaleX(75);
  U6BrowseButton.Height := ScaleY(23);
  U6BrowseButton.Caption := '&Browse';
  U6BrowseButton.OnClick := @OnU6BrowseButton;
  U6BrowseButton.Parent := DataDirPage.Surface;

  U6Edit := TEdit.Create(DataDirPage);
  U6Edit.Top := U6Text.Top + U6Text.Height + ScaleY(8);
  U6Edit.Width := DataDirPage.SurfaceWidth - (U6BrowseButton.Width + ScaleX(8));
  U6Edit.Text := 'C:\ultima6';
  U6Edit.Parent := DataDirPage.Surface;


  MDText := TNewStaticText.Create(DataDirPage);
  MDText.Caption := 'Please enter the path where Martian Dreams is installed.';
  MDText.AutoSize := True;
  MDText.Top := U6Edit.Top + U6Edit.Height + ScaleY(23);
  MDText.Parent := DataDirPage.Surface;

  MDBrowseButton := TButton.Create(DataDirPage);
  MDBrowseButton.Top := MDText.Top + MDText.Height + ScaleY(8);
  MDBrowseButton.Left := DataDirPage.SurfaceWidth - ScaleX(75);
  MDBrowseButton.Width := ScaleX(75);
  MDBrowseButton.Height := ScaleY(23);
  MDBrowseButton.Caption := 'Brow&se';
  MDBrowseButton.OnClick := @OnMDBrowseButton;
  MDBrowseButton.Parent := DataDirPage.Surface;

  MDEdit := TEdit.Create(DataDirPage);
  MDEdit.Top := MDText.Top + MDText.Height + ScaleY(8);
  MDEdit.Width := DataDirPage.SurfaceWidth - (MDBrowseButton.Width + ScaleX(8));
  MDEdit.Text := 'C:\MARTIAN';
  MDEdit.Parent := DataDirPage.Surface;


  SEText := TNewStaticText.Create(DataDirPage);
  SEText.Caption := 'Please enter the path where Savage Empire is installed.';
  SEText.AutoSize := True;
  SEText.Top := MDEdit.Top + MDEdit.Height + ScaleY(23);
  SEText.Parent := DataDirPage.Surface;

  SEBrowseButton := TButton.Create(DataDirPage);
  SEBrowseButton.Top := SEText.Top + SEText.Height + ScaleY(8);
  SEBrowseButton.Left := DataDirPage.SurfaceWidth - ScaleX(75);
  SEBrowseButton.Width := ScaleX(75);
  SEBrowseButton.Height := ScaleY(23);
  SEBrowseButton.Caption := 'Brow&se';
  SEBrowseButton.OnClick := @OnSEBrowseButton;
  SEBrowseButton.Parent := DataDirPage.Surface;

  SEEdit := TEdit.Create(DataDirPage);
  SEEdit.Top := SEText.Top + SEText.Height + ScaleY(8);
  SEEdit.Width := DataDirPage.SurfaceWidth - (SEBrowseButton.Width + ScaleX(8));
  SEEdit.Text := 'C:\SAVAGE';
  SEEdit.Parent := DataDirPage.Surface;

  InfoText := TNewStaticText.Create(DataDirPage);
  InfoText.Caption := 'Previous settings were found at:';
  InfoText.AutoSize := True;
  InfoText.Top := SEEdit.Top + SEEdit.Height + ScaleY(23);
  InfoText.Parent := DataDirPage.Surface;

  GameSelectPage := CreateCustomPage(DataDirPage.ID, 'Select Game', 'Select the game that Nuvie will load on startup. (you can change this later)');
  GameCombo := TNewComboBox.Create(GameSelectPage);
  GameCombo.Parent := GameSelectPage.Surface;
  GameCombo.Style := csDropDownList;
  GameCombo.Items.Add('Ultima VI');
  GameCombo.Items.Add('Martian Dreams');
  GameCombo.Items.Add('The Savage Empire');
  GameCombo.ItemIndex := 0;

  bSetPaths := False;
end;

//
// Read in the config file if needed
//
procedure CurPageChanged(CurPageID: Integer);
var
  sU6Path: String;
  sMDPath: String;
  sSEPath: String;
  sConfigPath: String;
  iConfigExists: Integer;
begin
  if CurPageID = DataDirPage.ID then begin
    if bSetPaths = False then begin
      setlength(sU6Path, 1024);
      setlength(sMDPath, 1024);
      setlength(sSEPath, 1024);
      setlength(sConfigPath, 1024);
      iConfigExists := GetNuvieGamePaths(ExpandConstant('{app}'), sU6Path, sMDPath, sSEPath, sConfigPath, 1023 );

      if (iConfigExists = 1) and not(CompareText(sU6Path, '') = 0) then begin
        U6Edit.Text := sU6Path;
      end;
      if (iConfigExists = 1) and not(CompareText(sMDPath, '') = 0) then begin
        MDEdit.Text := sMDPath;
      end;
      if (iConfigExists = 1) and not(CompareText(sSEPath, '') = 0) then begin
        SEEdit.Text := sSEPath;
      end;
      if iConfigExists = 0 then begin
        InfoText.Caption := '';
      end else
        InfoText.Caption := 'Previous settings were found at ' + sConfigPath;

    end;
  end;

end;

//
// Make sure the Paths are correct when we hit next button
//
function NextButtonClick(CurPageID: Integer): Boolean;
var
  iU6Verified: Integer;
  iMDVerified: Integer;
  iSEVerified: Integer;
  sDir : string;
begin
  //DataDirPage := CreateCustomPage(wpSelectTasks,
  //  'Select Game Folders', 'Select the folders where Ultima VI and the Worlds of Ultima games are installed.');
  if CurPageID = DataDirPage.ID then begin
      iU6Verified := 0;
      iMDVerified := 0;
      iSEVerified := 0;
 
      sDir := U6Edit.Text;
      if Length(sDir) > 0 then begin iU6Verified := VerifyGameDirectory ( sDir, 'ultima6' );
      end else iU6Verified := 0;

      sDir := MDEdit.Text;
      if Length(sDir) > 0 then begin iMDVerified := VerifyGameDirectory ( sDir, 'martian' );
      end else iMDVerified := 0;

      sDir := SEEdit.Text;
      if Length(sDir) > 0 then begin iSEVerified := VerifyGameDirectory ( sDir, 'savage' );
      end else iSEVerified := 0;

      if (iU6Verified = 0) AND (iMDVerified = 0) AND (iSEVerified = 0) then begin
        if MsgBox ('Warning: No valid game installations found. Do you wish to continue?', mbError, MB_YESNO or MB_DEFBUTTON2) = IDYES then begin
          Result := True;
        end else
          Result := False;
      end else
        Result := True;
  end else
    Result := True;
end;

//
// Skip page because of components?
//
function ShouldSkipPage(PageID: Integer): Boolean;
var
  sU6Path: String;
  sMDPath: String;
  sSEPath: String;
begin
{
 if PageID = DataDirPage.ID then begin
    Result := (IsComponentSelected('Paths') = False);

    if Result = True then begin
      if bSetPaths = False then begin
        setlength(sU6Path, 1024);
        setlength(sMDPath, 1024);
        setlength(sSEPath, 1024);
        //GetExultGamePaths(ExpandConstant('appdir'), sU6Path, sMDPath, 1023 );

        U6Edit.Text := sU6Path;
        MDEdit.Text := sMDPath;
        SEEdit.Text := sSEPath;
      end;

      if ( CompareStr(U6Edit.Text,'') = 0) and (CompareStr(MDEdit.Text,'') = 0) and (CompareStr(SEEdit.Text,'') = 0) then
        Result := False;
    end

  end else if PageID = wpSelectProgramGroup then begin
    Result := (IsComponentSelected('Icons') = False);
  end else
 }    Result := False;
end;

//
// Write out the Config file and Registry Entries
//
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    SetNuvieGamePaths(ExpandConstant('{app}'), U6Edit.Text, MDEdit.Text, SEEdit.Text );
    SetNuvieLoadGame(ExpandConstant('{app}'), GameCombo.Items[GameCombo.ItemIndex] );
    // TODO: allow changing savegame path (i.e. to Documents)
    //SetNuvieSavePaths(ExpandConstant('{userdocs}\Nuvie'));
    //RegWriteStringValue(HKEY_LOCAL_MACHINE, 'Software\Exult', 'Path', ExpandConstant('{app}'));
    //if IsComponentSelected('Icons') then
    //  RegWriteStringValue(HKEY_LOCAL_MACHINE, 'Software\Exult', 'ShellObjectFolder', ExpandConstant('{groupname}'));
  end
end;

[Files]
Source: "AUTHORS.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "ChangeLog.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "COPYING.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "nuvie.cfg.sample"; DestDir: "{app}"; Flags: ignoreversion
Source: "nuvie.cfg.sample"; DestDir: "{app}"; DestName: "nuvie.cfg"; Flags: ignoreversion onlyifdoesntexist
Source: "nuvie.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "README-SDL.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "SDL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "nvconfig.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\BorderU6_1.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_2.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_3.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_4.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_5.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_6.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_7.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\BorderU6_8.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\ScrollBarDown_1.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\ScrollBarDown_2.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\ScrollBarUp_1.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\ScrollBarUp_2.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\U6_WOU_Scroll_bg.bmp"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\defaultkeys.txt"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\u6keys.txt"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\sekeys.txt"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\mdkeys.txt"; DestDir: "{app}\data\"; Flags: ignoreversion
Source: "data\images\tiles\U6\avatar_001_0410.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\avatar_001_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\avatar_006_0410.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\avatar_006_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\avatar_010_0410.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\avatar_010_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_002_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_002_0376.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_003_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_003_0377.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_004_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_004_0386.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_004_0392.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_009_0354.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_062_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_062_0387.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_066_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_066_0386.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_066_0392.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_067_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_067_0387.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_097_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_097_0385.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_113_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_113_0386.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_113_0392.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_114_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_114_0387.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_131_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_131_0376.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_137_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_137_0383.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_186_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_186_0376.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_162_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_162_0376.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_164_0339.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\tiles\U6\actor_164_0363.bmp"; DestDir: "{app}\data\images\tiles\U6"; Flags: ignoreversion
Source: "data\images\gumps\gump_btn_down.bmp"; DestDir: "{app}\data\images\gumps"; Flags: ignoreversion
Source: "data\images\gumps\gump_btn_up.bmp"; DestDir: "{app}\data\images\gumps"; Flags: ignoreversion
Source: "data\images\gumps\left_arrow.bmp"; DestDir: "{app}\data\images\gumps"; Flags: ignoreversion
Source: "data\images\gumps\right_arrow.bmp"; DestDir: "{app}\data\images\gumps"; Flags: ignoreversion
Source: "data\images\gumps\portrait_bg.bmp"; DestDir: "{app}\data\images\gumps"; Flags: ignoreversion
Source: "data\images\gumps\celestial\0.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\1.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\2.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\3.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\4.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\5.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\6.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\7.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\celestial\8.bmp"; DestDir: "{app}\data\images\gumps\celestial"; Flags: ignoreversion
Source: "data\images\gumps\container\backpack_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\barrel_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\chest_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\corpse_animal_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\corpse_body_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\corpse_cyclops_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\corpse_gargoyle_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\crate_bg.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_doll_btn_down.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_doll_btn_up.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_down_btn_down.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_down_btn_up.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_left_btn_down.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_left_btn_up.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_right_btn_down.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_right_btn_up.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_up_btn_down.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\container\cont_up_btn_up.bmp"; DestDir: "{app}\data\images\gumps\container"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_002.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_003.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_004.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_009.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_062.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_066.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_067.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_097.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_113.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_114.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_131.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_137.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_162.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_164.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\actor_U6_186.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_00.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_01.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_02.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_03.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_04.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_05.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_06.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_07.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_08.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_09.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_10.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\avatar_U6_11.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_002.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_003.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_004.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_009.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_062.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_066.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_067.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_097.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_113.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_114.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_131.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_137.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_162.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_164.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\actor_U6_186.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_00.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_01.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_02.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_03.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_04.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_05.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_06.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_07.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_08.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_09.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_10.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\avatar_U6_11.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style"; Flags: ignoreversion
Source: "data\images\gumps\doll\orig_style\doll_bg.bmp"; DestDir: "{app}\data\images\gumps\doll\orig_style\"; Flags: ignoreversion
Source: "data\images\gumps\doll\combat_btn_down.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\combat_btn_up.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\doll_bg.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\heart_btn_down.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\heart_btn_up.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\inventory_btn_down.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\inventory_btn_up.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\party_btn_down.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\doll\party_btn_up.bmp"; DestDir: "{app}\data\images\gumps\doll"; Flags: ignoreversion
Source: "data\images\gumps\fonts\MD.bmp"; DestDir: "{app}\data\images\gumps\fonts"; Flags: ignoreversion
Source: "data\images\gumps\fonts\MD.dat"; DestDir: "{app}\data\images\gumps\fonts"; Flags: ignoreversion
Source: "data\images\gumps\fonts\SE.bmp"; DestDir: "{app}\data\images\gumps\fonts"; Flags: ignoreversion
Source: "data\images\gumps\fonts\SE.dat"; DestDir: "{app}\data\images\gumps\fonts"; Flags: ignoreversion
Source: "data\images\gumps\fonts\U6.bmp"; DestDir: "{app}\data\images\gumps\fonts"; Flags: ignoreversion
Source: "data\images\gumps\fonts\U6.dat"; DestDir: "{app}\data\images\gumps\fonts"; Flags: ignoreversion
Source: "data\images\gumps\mapeditor\down_btn_up.bmp"; DestDir: "{app}\data\images\gumps\mapeditor"; Flags: ignoreversion
Source: "data\images\gumps\mapeditor\down_btn_down.bmp"; DestDir: "{app}\data\images\gumps\mapeditor"; Flags: ignoreversion
Source: "data\images\gumps\mapeditor\up_btn_up.bmp"; DestDir: "{app}\data\images\gumps\mapeditor"; Flags: ignoreversion
Source: "data\images\gumps\mapeditor\up_btn_down.bmp"; DestDir: "{app}\data\images\gumps\mapeditor"; Flags: ignoreversion
Source: "data\images\gumps\sign\sign_bg.bmp"; DestDir: "{app}\data\images\gumps\sign"; Flags: ignoreversion
Source: "data\images\gumps\sign\sign_font.bmp"; DestDir: "{app}\data\images\gumps\sign"; Flags: ignoreversion
Source: "data\images\gumps\sign\sign_font.dat"; DestDir: "{app}\data\images\gumps\sign"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\1.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\2.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\3.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\4.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\5.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\6.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\7.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\8.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\nd.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\rd.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_bg.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_left_arrow.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_right_arrow.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_000.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_001.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_002.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_003.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_004.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_005.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_006.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_007.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_008.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_009.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_016.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_017.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_018.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_019.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_020.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_021.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_022.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_023.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_024.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_025.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_032.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_033.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_034.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_035.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_036.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_037.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_038.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_039.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_040.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_041.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_048.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_049.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_050.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_051.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_052.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_053.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_054.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_055.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_056.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_057.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_064.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_065.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_066.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_067.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_068.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_069.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_070.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_071.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_072.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_073.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_080.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_081.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_082.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_083.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_084.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_085.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_086.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_087.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_088.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_089.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_096.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_097.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_098.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_099.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_100.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_101.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_102.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_103.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_104.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_105.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_112.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_113.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_114.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_115.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_116.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_117.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_118.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_119.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_120.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\spellbook_spell_121.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\gumps\spellbook\st.bmp"; DestDir: "{app}\data\images\gumps\spellbook"; Flags: ignoreversion
Source: "data\images\roof_tiles.bmp"; DestDir: "{app}\data\images"; Flags: ignoreversion
Source: "data\maps\u6\roof_map_00.dat"; DestDir: "{app}\data\maps\u6"; Flags: ignoreversion
Source: "data\scripts\common\actor.lua"; DestDir: "{app}\data\scripts\common"; Flags: ignoreversion
Source: "data\scripts\common\common.lua"; DestDir: "{app}\data\scripts\common"; Flags: ignoreversion
Source: "data\scripts\common\intro_common.lua"; DestDir: "{app}\data\scripts\common"; Flags: ignoreversion
Source: "data\scripts\common\lang.lua"; DestDir: "{app}\data\scripts\common"; Flags: ignoreversion
Source: "data\scripts\md\actor.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\combat.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\dreamworld.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\ending.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\init.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\intro.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\look.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\player.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\talk.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\usecode.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\worktype.lua"; DestDir: "{app}\data\scripts\md"; Flags: ignoreversion
Source: "data\scripts\md\lang\en\game.lua"; DestDir: "{app}\data\scripts\md\lang\en"; Flags: ignoreversion
Source: "data\scripts\md\lang\it\game.lua"; DestDir: "{app}\data\scripts\md\lang\it"; Flags: ignoreversion
Source: "data\scripts\se\actor.lua"; DestDir: "{app}\data\scripts\se"; Flags: ignoreversion
Source: "data\scripts\se\init.lua"; DestDir: "{app}\data\scripts\se"; Flags: ignoreversion
Source: "data\scripts\se\intro.lua"; DestDir: "{app}\data\scripts\se"; Flags: ignoreversion
Source: "data\scripts\se\look.lua"; DestDir: "{app}\data\scripts\se"; Flags: ignoreversion
Source: "data\scripts\se\player.lua"; DestDir: "{app}\data\scripts\se"; Flags: ignoreversion
Source: "data\scripts\se\usecode.lua"; DestDir: "{app}\data\scripts\se"; Flags: ignoreversion
Source: "data\scripts\se\lang\en\game.lua"; DestDir: "{app}\data\scripts\se\lang\en"; Flags: ignoreversion
Source: "data\scripts\u6\actor.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\ending.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\init.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\intro.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\player.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\usecode.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\lang\en\game.lua"; DestDir: "{app}\data\scripts\u6\lang\en"; Flags: ignoreversion
Source: "data\scripts\u6\magic.lua"; DestDir: "{app}\data\scripts\u6"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\create_food.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\detect_magic.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\detect_trap.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\dispel_magic.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\douse.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\harm.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\heal.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\help.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\ignite.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_01\light.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_01"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\infravision.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\magic_arrow.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\poison.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\reappear.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\sleep.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\telekinesis.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\trap.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\unlock_magic.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\untrap.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_02\vanish.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_02"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\curse.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\dispel_field.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\fireball.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\great_light.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\magic_lock.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\mass_awaken.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\mass_sleep.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\peer.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\protection.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_03\repel_undead.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_03"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\animate.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\conjure.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\disable.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\fire_field.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\great_heal.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\locate.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\mass_dispel.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\poison_field.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\sleep_field.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_04\wind_change.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_04"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\energy_field.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\explosion.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\insect_swarm.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\invisibility.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\lightning.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\paralyze.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\pickpocket.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\reveal.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\seance.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_05\xray.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_05"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\charm.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\clone.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\confuse.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\flame_wind.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\hail_storm.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\mass_protect.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\negate_magic.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\poison_wind.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\replicate.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_06\web.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_06"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\chain_bolt.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\enchant.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\energy_wind.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\fear.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\gate_travel.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\kill.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\mass_curse.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\mass_invisibility.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\wing_strike.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_07\wizard_eye.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_07"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\armageddon.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\death_wind.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\eclipse.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\mass_charm.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\mass_kill.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\resurrect.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\slime.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\summon.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\time_stop.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "data\scripts\u6\magic\circle_08\tremor.lua"; DestDir: "{app}\data\scripts\u6\magic\circle_08"; Flags: ignoreversion
Source: "tools\SDL.dll"; DestDir: "{app}\tools\"
Source: "tools\u6decomp.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack_conv.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack_font.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\pack_font.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack_lzc.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\pack_lzc.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack_maptiles.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack_portraits.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion
Source: "tools\unpack_talk.exe"; DestDir: "{app}\tools\"; Flags: ignoreversion

[Dirs]
Name: "{app}\data"
Name: "{app}\martian_save"
Name: "{app}\savage_save"
Name: {app}\tools;
Name: "{app}\u6_sounds"
Name: "{app}\u6_save"
