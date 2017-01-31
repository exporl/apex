Section -closelogfile
 FileClose $MyUninstLog
 SetFileAttributes "$INSTDIR\${MyUninstLog}" READONLY|SYSTEM|HIDDEN
SectionEnd
 
Section Uninstall
 
 ; Can't uninstall if uninstall log is missing!
 IfFileExists "$INSTDIR\${MyUninstLog}" +3
  MessageBox MB_OK|MB_ICONSTOP "$(MyUninstLogMissing)"
   Abort
 
 Push $R0
 Push $R1
 Push $R2
 SetFileAttributes "$INSTDIR\${MyUninstLog}" NORMAL
 FileOpen $MyUninstLog "$INSTDIR\${MyUninstLog}" r
 StrCpy $R1 0
 
 GetLineCount:
  ClearErrors
   FileRead $MyUninstLog $R0
   IntOp $R1 $R1 + 1
   IfErrors 0 GetLineCount
 
 LoopRead:
  FileSeek $MyUninstLog 0 SET
  StrCpy $R2 0
  FindLine:
   FileRead $MyUninstLog $R0
   IntOp $R2 $R2 + 1
   StrCmp $R1 $R2 0 FindLine
 
   StrCpy $R0 $R0 -2
   IfFileExists "$R0\*.*" 0 +3
    RMDir $R0  #is dir
   Goto +3
   IfFileExists $R0 0 +2
    Delete $R0 #is file
 
  IntOp $R1 $R1 - 1
  StrCmp $R1 0 LoopDone
  Goto LoopRead
 LoopDone:
 FileClose $MyUninstLog
 Delete "$INSTDIR\${MyUninstLog}"
 RMDir "$INSTDIR"
 Pop $R2
 Pop $R1
 Pop $R0
SectionEnd