!define NAME "NesTrisLauncher"
!define REGPATH_UNINSTSUBKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
Name "${NAME}"
OutFile "Install ${NAME}.exe"
Unicode True
RequestExecutionLevel Admin ; Request admin rights on WinVista+ (when UAC is turned on)
InstallDir "$ProgramFiles\$(^Name)"
InstallDirRegKey HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString"

!include LogicLib.nsh
!include Integration.nsh


Page Directory
Page Components
Page InstFiles

Uninstpage UninstConfirm
Uninstpage InstFiles


!macro EnsureAdminRights
  UserInfo::GetAccountType
  Pop $0
  ${If} $0 != "admin" ; Require admin rights on WinNT4+
    MessageBox MB_IconStop "Administrator rights required!"
    SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
    Quit
  ${EndIf}
!macroend

Function .onInit
  SetShellVarContext All
  !insertmacro EnsureAdminRights
FunctionEnd

Function un.onInit
  SetShellVarContext All
  !insertmacro EnsureAdminRights
FunctionEnd


Section "Program files (Required)"
  SectionIn Ro

  SetOutPath $InstDir
  WriteUninstaller "$InstDir\Uninst.exe"
  WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayName" "${NAME}"
  WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" "$InstDir\NesTrisLauncher.exe,0"
  WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$InstDir\Uninst.exe"'
  WriteRegStr HKLM "${REGPATH_UNINSTSUBKEY}" "QuietUninstallString" '"$InstDir\Uninst.exe" /S'
  WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
  WriteRegDWORD HKLM "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

  File "/oname=$InstDir\NesTrisLauncher.exe" "bin\Release\NesTrisLauncher.exe"
SectionEnd

Section "Associate NesTrisLauncher with browser"
  WriteRegStr HKCR "nestrischamps" "" "URL:nestrischamps"
  WriteRegStr HKCR "nestrischamps" "URL Protocol" ""
  WriteRegStr HKCR "nestrischamps\DefaultIcon" "" "$InstDir\NesTrisLauncher.exe"
  WriteRegStr HKCR "nestrischamps\shell" "" ""
  WriteRegStr HKCR "nestrischamps\shell\Open" "" ""
  WriteRegStr HKCR "nestrischamps\shell\Open\command" "" "$InstDir\NesTrisLauncher.exe %1"
SectionEnd

Section "Add Start Menu Shortcut"
  CreateShortcut /NoWorkingDir "$SMPrograms\${NAME}.lnk" "$InstDir\NesTrisLauncher.exe"
SectionEnd

Section "Launch NesTrisLauncher"
	Exec '"$WINDIR\explorer.exe" "$InstDir\NesTrisLauncher.exe"'
SectionEnd

!macro DeleteFileOrAskAbort path
  ClearErrors
  Delete "${path}"
  IfErrors 0 +3
    MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP 'Unable to delete "${path}"!' IDRETRY -3 IDIGNORE +2
    Abort "Aborted"
!macroend

Section -Uninstall
  !insertmacro DeleteFileOrAskAbort "$InstDir\NesTrisLauncher.exe"
  Delete "$InstDir\Uninst.exe"
  RMDir "$InstDir"
  DeleteRegKey HKLM "${REGPATH_UNINSTSUBKEY}"
  DeleteRegKey HKCR "nestrischamps"

  ${UnpinShortcut} "$SMPrograms\${NAME}.lnk"
  Delete "$SMPrograms\${NAME}.lnk"
SectionEnd
