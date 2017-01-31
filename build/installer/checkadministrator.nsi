!define TOKEN_READ   0x00020008
!define TokenGroups  2
 
!macro IsUserAdmin RESULT
 !define Index "Line${__LINE__}"
 StrCpy ${RESULT} 0
 
 # Construct the SID for the Admin group - (S-1-5-32-544 for administrators) put in $R4
  System::Call "*(&i1 0,&i4 0,&i1 5)i.r0"
  System::Call  "advapi32::AllocateAndInitializeSid(i r0,i 2,i 32,i 544,i 0,i 0,i 0,i 0,i 0,\
  i 0,*i .R4)i.r5"
  System::Free $0
 
 # Get a psuedo-handle of the current process and place it on R0
  System::Call 'kernel32::GetCurrentProcess()i.R0'
 
 # Open the Token from the psuedo process and place the handle on R1
  System::Call 'advapi32::OpenProcessToken(i R0,i ${TOKEN_READ},*i .R1)i.R9'
 
 # Get info from the token and place it in $R2 (the size goes to $R3)
  System::Call 'advapi32::GetTokenInformation(i R1,i ${TokenGroups},*i .R2,i 0,*i .R3)i.R9'
  System::Alloc $R3
   Pop $R2
  System::Call 'advapi32::GetTokenInformation(i R1,i ${TokenGroups},i R2,i $R3,*i .R3)i.R9'
 
 # Check how many TOKEN_GROUPS elements are in $R2 (place the number in $R5)
  System::Call '*$R2(i.R5,i.R6)'
 
 # Compare the SID structures
  StrCpy $1 0
	
 ${Index}_Start:
  StrCmp $1 $R5 ${Index}_Stop
  System::Call 'advapi32::EqualSid(i R4,i R6)i.R9'
  StrCmp $R9 "" ${Index}_Increment
  StrCmp $R9 0 +1 +3
  StrCpy ${RESULT} 0
   Goto ${Index}_Increment
  StrCpy ${RESULT} 1
  System::Call 'advapi32::FreeSid(i R6)i.R8'
   Goto ${Index}_Stop
 ${Index}_Increment:
  System::Call 'advapi32::GetLengthSid(i R6)i.R9'
  System::Call 'advapi32::FreeSid(i R6)i.R8'
  IntOp $R6 $R6 + $R9
  IntOp $1 $1 + 1
  Goto ${Index}_Start
 
 ${Index}_Stop:
 # Close the token handle
  System::Call 'kernel32::CloseHandle(i R1)i.R9'
 
 # cleanup
  System::Call 'advapi32::FreeSid(i R4)i.r5'
  System::Free $R2
  System::Free 0
 
 !undef Index
!macroend