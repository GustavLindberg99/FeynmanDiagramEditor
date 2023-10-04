; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName         "FeynmanDiagramEditor"
#define MyAppExeName      MyAppName + ".exe"
#define MyAppExePath      "C:\Users\glind\OneDrive\Documents\FeynmanDiagramEditor\build-FeynmanDiagramEditor\release"
#define MyDepenciesPath   "C:\Users\glind\OneDrive\Documents\FeynmanDiagramEditor\windows-dependencies"
#define MyAppVersion      GetVersionNumbersString(MyAppExePath + "\" + MyAppExeName)
#define MyAppURL          "https://github.com/GustavLindberg99/FeynmanDiagramEditor"
#define MyAppOutputDir    "C:\Users\glind\OneDrive\Documents\FeynmanDiagramEditor"
#define MyAppOutputExe    "FeynmanDiagramEditor-setup-windows"
#define MyAppLicenseFile  "C:\Users\glind\OneDrive\Documents\FeynmanDiagramEditor\license.txt"
#define MyAppCompany      "Gustav Lindberg"
#define MyAppStartingYear "2021"
#define CurrentYear       GetDateTimeString('yyyy','','')

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8F9AD676-E4E5-4EAE-AEE4-34571E76729C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}

AppCopyright=(c) {#MyAppStartingYear}-{#CurrentYear} {#MyAppCompany}
AppPublisher={#MyAppCompany}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}

VersionInfoDescription={#MyAppName} installer
VersionInfoProductName={#MyAppName}
VersionInfoVersion={#MyAppVersion}

UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}

LicenseFile={#MyAppLicenseFile} 

ShowLanguageDialog=yes
UsePreviousLanguage=no
LanguageDetectionMethod=uilanguage

WizardStyle=modern

DefaultDirName={pf64}\{#MyAppName}
DisableProgramGroupPage=yes
OutputDir={#MyAppOutputDir}
OutputBaseFilename={#MyAppOutputExe}
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MyAppExePath}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyDepenciesPath}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Registry]
Root: HKCU; Subkey: Software\Classes\.fdg; ValueType: String; ValueData: FeynmanDiagramEditor.FDG; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Classes\NFeynmanDiagramEditor.FDG; ValueType: String; ValueData: "Feynman diagram"; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Classes\FeynmanDiagramEditor.FDG\DefaultIcon; ValueType: String; ValueData: "{app}\FeynmanDiagramEditor.exe,0"; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Classes\FeynmanDiagramEditor.FDG\Shell; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Classes\FeynmanDiagramEditor.FDG\Shell\open; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Classes\FeynmanDiagramEditor.FDG\Shell\open\command; ValueType: String; ValueData: """{app}\FeynmanDiagramEditor.exe"" ""%1"""; Flags: uninsdeletekey

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon  

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
