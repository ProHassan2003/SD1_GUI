; Inno Setup Script for StudentGradesGui
; Change Name_Surname below before building.

#define MyAppName "StudentGradesGui"
#define MyAppVersion "1.0"
#define MyAppPublisher "VVK"
#define MyAppExeName "StudentGradesGui.exe"

[Setup]
AppId={{D3C38C6D-6FE2-4B2A-8E3B-0E2E6B2C9F61}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf}\VVK\Name_Surname
DefaultGroupName={#MyAppName}
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
DisableProgramGroupPage=yes

[Files]
; Update this path after building (Release\x64)
Source: "..\x64\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
; Optional: add VC++ redistributable if your teacher requires it

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
