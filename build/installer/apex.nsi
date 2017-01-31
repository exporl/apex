!include "MUI.nsh"
;!include "onlyinstalled_header.nsi" 
!include "checkadministrator.nsi"

; The name of the installer
Name "Apex 3 installer"

; The file to write
OutFile "..\..\bin\apex3_setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\apex3

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\ExpORL\apex3" "Install_Dir"

;--------------------------------

; Pages

; classic ui
;Page components;
;Page directory
;Page instfiles

;UninstPage uninstConfirm
;UninstPage instfiles

  !insertmacro MUI_PAGE_LICENSE "..\..\doc\license\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
  !define MUI_FINISHPAGE_TEXT "Remove any remaining files from $INSTDIR"
  !insertmacro MUI_UNPAGE_FINISH 

;--------------------------------


;Languages
 
  !insertmacro MUI_LANGUAGE "English"
  
 ;-------------------------------- 
 
 


; The stuff to install
Section "Binaries (required)" secBinaries

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  
  ; Everything except examples and matlab
  
  File /r /x examples /x amt /x manual ..\..\bin\release_pa5_flash_installed\*
  

  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\ExpORL\apex3 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\apex3" "DisplayName" "Apex 3"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\apex3" "UninstallString" '"$INSTDIR\uninstall.exe"'
;  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\apex3" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\apex3" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Documentation" secDoc
	SetOutPath $INSTDIR\manual
	file ..\..\bin\release_pa5_flash_installed\manual\*.pdf
SectionEnd

Section "Matlab Toolbox" secMatlab
	SetOutPath $INSTDIR\amt
	file ..\..\bin\release_pa5_flash_installed\amt\amt.zip
SectionEnd

Section "Examples" secExamples
	SetOutPath $INSTDIR\examples
	file /r /x .svn /x *.swf /x *.qic /x l34 /x clarion /x movies ..\..\bin\release_pa5_flash_installed\examples\*
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\APEX 3"
  CreateShortCut "$SMPROGRAMS\APEX 3\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\APEX 3\APEX 3.lnk" "$INSTDIR\bin\apex.exe" "" "$INSTDIR\bin\apex.exe" 0
  
SectionEnd

Section "Associate file extensions" secExtension

!insertmacro IsUserAdmin $0
strcmp $0 1 "IsAdminLabel"
messageBox MB_OK "You are not administrator, can not add file extension for Apex."
"IsAdminLabel:"


; back up old value of .opt
!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".apx" ""
  StrCmp $1 "" "${Index}-NoBackup"
    StrCmp $1 "ApexExperiment" "${Index}-NoBackup"
    WriteRegStr HKCR ".apx" "backup_val" $1
"${Index}-NoBackup:"
  WriteRegStr HKCR ".apx" "" "ApexExperiment"
  ReadRegStr $0 HKCR "ApexExperiment" ""
  StrCmp $0 "" 0 "${Index}-Skip"
	WriteRegStr HKCR "ApexExperiment" "" "Apex 3 experiment file"
	WriteRegStr HKCR "ApexExperiment\shell" "" "open"
	WriteRegStr HKCR "ApexExperiment\DefaultIcon" "" "$INSTDIR\bin\apex.exe,0"
"${Index}-Skip:"
  WriteRegStr HKCR "ApexExperiment\shell\open\command" "" \
    '$INSTDIR\bin\apex.exe "%1"'
 
  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
!undef Index

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Apex3"
  DeleteRegKey HKLM SOFTWARE\NSIS_Apex3

  ; Remove files and uninstaller
  Delete $INSTDIR\Apex3.nsi
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\APEX 3\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\APEX 3"
  RMDir "$INSTDIR"
  
  
  ; remove all
  RMDir /r "$INSTDIR"
  
 
  ; reset extension
  
   ;start of restore script
  !define Index "Line${__LINE__}"
    ReadRegStr $1 HKCR ".apx" ""
    StrCmp $1 "OptionsFile" 0 "${Index}-NoOwn" ; only do this if we own it
      ReadRegStr $1 HKCR ".apx" "backup_val"
      StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
        DeleteRegKey HKCR ".apx"
      Goto "${Index}-NoOwn"
  "${Index}-Restore:"
        WriteRegStr HKCR ".apx" "" $1
        DeleteRegValue HKCR ".apx" "backup_val"
     
      DeleteRegKey HKCR "ApexExperiment" ;Delete key with association settings
   
      System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
  "${Index}-NoOwn:"
  !undef Index
  ;rest of script

SectionEnd



;!include "onlyinstalled_footer.nsi"

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_secBinaries ${LANG_ENGLISH} "The main Apex 3 program files"
  LangString DESC_secDoc ${LANG_ENGLISH} "The manual"
  LangString DESC_secExamples ${LANG_ENGLISH} "Example experiment files"
  LangString DESC_secMatlab ${LANG_ENGLISH} "The Apex Matlab Toolbox"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${secBinaries} $(DESC_secBinaries)
    !insertmacro MUI_DESCRIPTION_TEXT ${secDoc} $(DESC_secDoc)
    !insertmacro MUI_DESCRIPTION_TEXT ${secExamples} $(DESC_secExamples)
    !insertmacro MUI_DESCRIPTION_TEXT ${secMatlab} $(DESC_secMatlab)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
  
  
  