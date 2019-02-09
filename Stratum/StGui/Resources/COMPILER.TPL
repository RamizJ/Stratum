// <name> [imp] [arg] [ret] [out]

EPILOG
 name "_epilog_" out 0

OPERATORS
// special
 name ";"  imp 0,32767 out $name
 name "#"  imp 0,32767 out $name
 name "("  imp 0,32767 out $name
 name "~"  out $name
 name ","  imp 1 out $name
 name ")"  imp 1 out $name
 name "]"  imp 1 out $name

 name "=" imp 1 arg "FLOAT","FLOAT" out 19

 name ":=" imp 1 arg "FLOAT","FLOAT" out 11,$index
 name ":=_old" imp 1 arg "FLOAT","FLOAT" out 10,$index

 name ":=" imp 1 arg "STRING","STRING" out 123,$index
 name ":=_old" imp 1 arg "STRING","STRING" out 157,$index

 name ":=" imp 1 arg "COLORREF","COLORREF" out 13,$index
 name ":=_old" imp 1 arg "COLORREF","COLORREF" out 12,$index
 name ":=" imp 1 arg "HANDLE","HANDLE" out 13,$index
 name ":=_old" imp 1 arg "HANDLE","HANDLE" out 12,$index
 name ":=" imp 1 arg "INTEGER","INTEGER" out 13,$index
 name ":=_old" imp 1 arg "INTEGER","INTEGER" out 12,$index

//compare
 name "==" imp 7 arg "FLOAT","FLOAT" ret "FLOAT" out 54
 name "!=" imp 7 arg "FLOAT","FLOAT" ret "FLOAT" out 55
 name ">"  imp 8 arg "FLOAT","FLOAT" ret "FLOAT" out 56
 name ">=" imp 8 arg "FLOAT","FLOAT" ret "FLOAT" out 57
 name "<"  imp 8 arg "FLOAT","FLOAT" ret "FLOAT" out 58
 name "<=" imp 8 arg "FLOAT","FLOAT" ret "FLOAT" out 59

 name "==" imp 7 arg "STRING","STRING" ret "FLOAT" out 144
 name "!=" imp 7 arg "STRING","STRING" ret "FLOAT" out 145
 name ">"  imp 8 arg "STRING","STRING" ret "FLOAT" out 146
 name ">=" imp 8 arg "STRING","STRING" ret "FLOAT" out 147
 name "<"  imp 8 arg "STRING","STRING" ret "FLOAT" out 148
 name "<=" imp 8 arg "STRING","STRING" ret "FLOAT" out 149

 name "==" imp 7 arg "HANDLE","HANDLE" ret "FLOAT" out 82
 name "==" imp 7 arg "COLORREF","COLORREF" ret "FLOAT" out 82
 name "==" imp 7 arg "INTEGER","INTEGER" ret "FLOAT" out 82
 name "!=" imp 7 arg "HANDLE","HANDLE" ret "FLOAT" out 83
 name "!=" imp 7 arg "COLORREF","COLORREF" ret "FLOAT" out 83

// other

 name "&"  imp 6 arg "FLOAT","FLOAT" ret "FLOAT" out 105
 name "|"  imp 5 arg "FLOAT","FLOAT" ret "FLOAT" out 106
 name "&&" imp 4 arg "FLOAT","FLOAT" ret "FLOAT" out 43
 name "||" imp 3 arg "FLOAT","FLOAT" ret "FLOAT" out 44

 name "<<" imp 4 arg "FLOAT","FLOAT" ret "FLOAT" out 116
 name ">>" imp 3 arg "FLOAT","FLOAT" ret "FLOAT" out 117


 name "&&" imp 4 arg "HANDLE","HANDLE" ret "HANDLE" out 85
 name "||" imp 3 arg "HANDLE","HANDLE" ret "HANDLE" out 86

// unary
 name "un_minus" imp 30 arg "FLOAT" ret "FLOAT" out 113
 name "!" imp 30 arg "FLOAT" ret "FLOAT" out 45

 name "++" imp 30 arg "FLOAT" out 800,$index
// name "++" imp 30 arg &"FLOAT" out 800,$index


// binary
 name "+" imp 10,10 arg "FLOAT","FLOAT" ret "FLOAT" out 18
 name "+" imp 10,10 arg "STRING","STRING" ret "STRING" out 124
 name "+" imp 10,10 arg "STRING","FLOAT" ret "STRING" out 770
 name "+" imp 10,10 arg "FLOAT","STRING" ret "STRING" out 771
 name "-" imp 10,10 arg "FLOAT","FLOAT" ret "FLOAT" out 19
// name "_" imp 15,16 arg "FLOAT","FLOAT" ret "FLOAT" out 81
 name "*" imp 20 arg "FLOAT","FLOAT" ret "FLOAT" out 21
 name "/" imp 20 arg "FLOAT","FLOAT" ret "FLOAT" out 20
 name "%" imp 20 arg "FLOAT","FLOAT" ret "FLOAT" out 22
 name "^" imp 25 arg "FLOAT","FLOAT" ret "FLOAT" out 32



FUNCTIONS
include streams
include scbase
include string
include matrix
include windows
include graph2d
include stddlg
include math
include graph3d
include dynovars
include scvideo
include scaudio
include TextAnalyser
include OgreWrapper
include NUI

// special
 name "jmp" out 51,$index
 name "jnz" arg "FLOAT" out 52,$index
 name "jz"  arg "FLOAT" out 53,$index

 name "jnz" arg "HANDLE" out 110,$index
 name "jz"  arg "HANDLE" out 111,$index

 name "push" ret "FLOAT" out 1,$index
 name "push_new" ret "FLOAT" out 2,$index
 name "push_cst" ret "FLOAT" out 6

 name "NotBin" arg "FLOAT" ret "FLOAT" out 88
 name "Xor" arg "FLOAT","FLOAT" ret "FLOAT" out 97
 name "XorBin" arg "FLOAT","FLOAT" ret "FLOAT" out 98

 name "push" ret "STRING" out 120,$index
 name "push_new" ret "STRING" out 121,$index
 name "push_cst" ret "STRING" out 122

 name "push" ret "INTEGER" out 3,$index
 name "push_new" ret "INTEGER" out 4,$index
 name "push_cst" ret "INTEGER" out 5

 name "push" ret "HANDLE" out 3,$index
 name "push_new" ret "HANDLE" out 4,$index
 name "push_cst" ret "HANDLE" out 5

 name "push" ret "COLORREF" out 3,$index
 name "push_new" ret "COLORREF" out 4,$index

 name "integer" arg "FLOAT" ret "INTEGER" out 16
 name "float" arg "INTEGER" ret "FLOAT" out 17

 name "HANDLE" arg "FLOAT" ret "HANDLE" out 16
 name "FLOAT" arg "HANDLE" ret "FLOAT" out 17
 name "FLOAT" arg "COLORREF" ret "FLOAT" out 17

 name "RGB" arg "FLOAT","FLOAT","FLOAT" ret "COLORREF" out 150
 name "RGBEx" arg "FLOAT","FLOAT","FLOAT","FLOAT" ret "COLORREF" out 199
 name "RGBf" arg "FLOAT" ret "COLORREF" out 16

// advanced
 name "VFunction" out 478
 name "DLLFunction" out 479
 name "GetElement" arg "FLOAT",["FLOAT"] ret "FLOAT" out 480
 name "SetElement" arg "FLOAT",["FLOAT"] ret "FLOAT" out 481
 name "diff0" arg &"FLOAT",&"FLOAT",&"FLOAT","FLOAT"
 name "diff1" arg "FLOAT","FLOAT","FLOAT" ret "FLOAT" out 740
 name "diff2" arg "FLOAT","FLOAT",&"FLOAT",&"FLOAT",&"FLOAT",&"FLOAT","FLOAT" ret "FLOAT" out 742
 name "equation" arg "FLOAT" out 741,$index
 name "dequation" arg "FLOAT" out 743,$index

// other
 name "SetStringBufferMode" arg "FLOAT" out 1115
 name "gettickcount" ret "FLOAT" out 115
 name "Stop" arg "FLOAT" out 50
 name "Quit" arg "FLOAT" out 107
 name "LogMessage" arg "STRING" out 491

 name "CloseAll" out 81
 name "SetStatusText" arg "FLOAT","STRING" out 112
 name "WinExecute" arg  "STRING","FLOAT"  ret "FLOAT" out   240
 name "Shell" arg "STRING","STRING","STRING","FLOAT"  ret "FLOAT" out 198
 name "ShellWait" arg "STRING","STRING","STRING","FLOAT"  ret "FLOAT" out 882
 name "SendSMS" arg "STRING","STRING" out 841
 name "SendSMS" arg "STRING","STRING","FLOAT"  ret "FLOAT" out 1117
 name "SendMail" arg "STRING" out 843
 name "SendMail" arg "STRING" ,"FLOAT"  ret "FLOAT" out 1118

 name "system" arg "FLOAT",["FLOAT"] ret "FLOAT" out 151
 name "systemstr" arg "FLOAT" ret "STRING" out 152

 name "SendMessage" arg "STRING","STRING",["STRING","STRING"] out 600
 name "Exit"    out 0

 name "GetROMDriveNames" ret "STRING" out 881

 name "EncryptStream" arg "HANDLE","HANDLE" ret "FLOAT" out 1126
 name "DecryptStream" arg "HANDLE","HANDLE" ret "FLOAT" out 1128
 name "SendData" arg "STRING" ,"FLOAT"  ret "FLOAT" out 1127

//userkey

 name "GetUserKeyValue" arg "STRING" ret "STRING" out 876
 name "GetUserKeyFullValue" arg "STRING" ret "STRING" out 877
 name "SendUserResult" arg "HANDLE" ret "FLOAT" out 879
 name "CopyUserResult" ret "FLOAT" out 880
 name "UserKeyIsAutorized" ret "FLOAT" out 889
 name "UserKeyIsAutorized" arg "HANDLE" ret "FLOAT" out 890

 name "ReadUserKey" arg "STRING" ret "HANDLE" out 883
 name "ReadProjectKey" arg "HANDLE","STRING" ret "FLOAT" out 888
 name "GetUserKeyValue" arg "HANDLE","STRING" ret "STRING" out 884
 name "GetUserKeyFullValue" arg "HANDLE","STRING" ret "STRING" out 885
 name "SendUserResult" arg "HANDLE","HANDLE" ret "FLOAT" out 886
 name "CopyUserResult" arg "HANDLE" ret "FLOAT" out 887


//***************************************************//
//                Directorys                         //
//***************************************************//

 name "GetClassDirectory"    arg  "STRING"  ret "STRING" out 233
 name "GetProjectDirectory"       ret "STRING" out      234
 name "GetWindowsDirectory"       ret "STRING" out      235
 name "GetSystemDirectory"        ret "STRING" out      236
 name "GetPathFromFile"      arg  "STRING"  ret "STRING" out  237
 name "AddSlash"             arg  "STRING"  ret "STRING" out    238
 name "GetStratumDirectory"       ret "STRING" out   239
 name "GetTempDirectory"       ret "STRING" out   878

//***************************************************//
//                MCI control functions              //
//***************************************************//

 name "MCISENDSTRINGstr"   arg  "STRING" ret "STRING"  out 250
 name "MCISENDSTRING"      arg  "STRING" ret "FLOAT"  out 251
 name "MCISENDSTRINGEX"    arg  "STRING","FLOAT" ret "STRING"  out 252
 name "GETLASTMCIERROR"    ret "FLOAT" out 253
 name "GETMCIERRORSTR"     arg  "FLOAT" ret "STRING" out 254
 name "SndPlaySound"       arg "STRING","FLOAT"  ret "FLOAT" out 346

 name "GetAsyncKeyState"   arg  "FLOAT"  ret "FLOAT" out 232
 name "joygetx"            arg  "FLOAT"  ret "FLOAT" out 228
 name "joygety"            arg  "FLOAT"  ret "FLOAT" out 229
 name "joygetz"            arg  "FLOAT"  ret "FLOAT" out 230
 name "joygetbuttons"      arg  "FLOAT"  ret "FLOAT" out 231

//***************************************************//
//         AUTOSINTEZ and common                     //
//***************************************************//

 name "CreateObject"     arg "STRING","STRING","STRING","FLOAT","FLOAT" ret "HANDLE" out 410
 name "GetUniqueClassName" arg "STRING" ret "STRING" out 419

 name "DeleteObject"     arg "STRING","HANDLE","FLOAT" ret "FLOAT" out 411
 name "CreateClass"      arg "STRING","STRING","FLOAT" ret "FLOAT" out 412
 name "DeleteClass"      arg "STRING" out 413

 name "OpenClassScheme"  arg "STRING","FLOAT" ret "HANDLE" out 414
 name "CloseClassScheme" arg "STRING" ret "FLOAT" out 415

 name "CreateLink"       arg "STRING","HANDLE","HANDLE" ret "HANDLE" out 416
 name "CreateLink"       arg "STRING","STRING","STRING" ret "HANDLE" out 436
 name "SetLinkVars"      arg "STRING","HANDLE","STRING" ret "FLOAT" out 417
 name "RemoveLink"       arg "STRING","HANDLE" ret "FLOAT" out 418
 name "GetLink"          arg "STRING","HANDLE","HANDLE" ret "HANDLE" out 607

 name "GetObjectCount"    arg "STRING" ret "FLOAT" out 386
 name "GetHObjectByNum"  arg "STRING","FLOAT" ret "HANDLE" out 387
 name "GetObjectClass"    arg "STRING","HANDLE" ret "STRING" out 389

 name "SetObjectName"  arg "STRING","HANDLE","STRING" ret "FLOAT" out 601

 name "GethObjectByName"  arg "STRING" ret "HANDLE" out 420
 name "GethObject"  ret "HANDLE" out 223
 name "GetNameByHandle" arg "STRING","HANDLE" ret "STRING" out 486
 // name "gethobjectbyname" arg "STRING" ret "HANDLE" out 224

 name "SetModelText" arg "STRING","HANDLE" ret "FLOAT" out 437
 name "SetModelText" arg "STRING","HANDLE","FLOAT" ret "FLOAT" out 1116
 name "GetModelText" arg "STRING","HANDLE" ret "FLOAT" out 438
 name "SetVarsToDefault" arg "STRING" ret "FLOAT" out 492

 name "LoadObjectState" arg "STRING","STRING" ret "FLOAT" out 496
 name "SaveObjectState" arg "STRING","STRING" ret "FLOAT" out 497

 name "GetClassName" arg "STRING" ret "STRING" out 398

 name "GetCalcOrder" arg "STRING","HANDLE" ret "FLOAT" out 256
 name "SetCalcOrder" arg "STRING","HANDLE","FLOAT" ret "FLOAT" out 255

 name "GetVarCount" arg "STRING" ret "FLOAT" out 751
 name "GetVarInfo"  arg "STRING","FLOAT",&"STRING",&"STRING",&"STRING",&"STRING" ret "FLOAT" out 750
//                                       name,type,defdata,remark
name "GetVarInfo"  arg "STRING","FLOAT",&"STRING",&"STRING",&"STRING",&"STRING",&"FLOAT" ret "FLOAT" out 799
//                                       name,type,defdata,remark,flags


 name "GetProjectClasses" arg "FLOAT" ret "HANDLE" out 1109
 name "GetClassFile" arg "STRING" ret "STRING" out 1110


//***************************************************//
//               Project Function
//***************************************************//

 name "LoadProject" arg "STRING" ret "FLOAT" out 720
 name "UnloadProject" arg "STRING" ret "FLOAT" out 721
 name "SetActiveProject" arg "STRING" ret "FLOAT" out 722
 name "IsProjectExist" arg "STRING" ret "FLOAT" out 723
 name "SetProjectProp" arg "STRING","STRING","FLOAT" ret "FLOAT" out 725
 name "GetProjectProp" arg "STRING","STRING" ret "FLOAT" out 726
 name "SetProjectProp" arg "STRING","STRING","STRING" ret "FLOAT" out 727

//***************************************************//
//         Set & Get vars                            //
//***************************************************//

 name "GetVarF"  arg "STRING","STRING" ret "FLOAT" out 430
 name "GetVarS"  arg "STRING","STRING" ret "STRING" out 431
 name "GetVarH"  arg "STRING","STRING" ret "HANDLE" out 432
 name "GetVarC"  arg "STRING","STRING" ret "COLORREF" out 432

 name "SetVar"   arg "STRING","STRING","FLOAT" out 433
 name "SetVar"   arg "STRING","STRING","STRING" out 434
 name "SetVar"   arg "STRING","STRING","HANDLE" out 435
 name "SetVar"   arg "STRING","STRING","COLORREF" out 435

 name "GetTime" arg &"FLOAT",&"FLOAT",&"FLOAT",&"FLOAT" out 475
 name "GetDate" arg &"FLOAT",&"FLOAT",&"FLOAT" out 748
