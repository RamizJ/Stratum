// streams

 name "CreateStream" arg "STRING","STRING","STRING" ret "HANDLE" out 161
 name "CloseStream"  arg "HANDLE" ret "FLOAT" out     162
 name "Seek" arg "HANDLE","FLOAT" ret "FLOAT" out     163
 name "StreamStatus" arg "HANDLE" ret "FLOAT" out     164
 name "Eof" arg "HANDLE" ret "FLOAT" out              165
 name "Getpos"  arg "HANDLE" ret "FLOAT" out          167
 name "GetSize"  arg "HANDLE" ret "FLOAT" out         168

 name "SetWidth" arg "HANDLE","FLOAT" ret "FLOAT" out 169
 name "Read"  arg "HANDLE" ret "FLOAT" out 170
 name "ReadLn"  arg "HANDLE" ret "STRING" out 171
 name "Write" arg "HANDLE","FLOAT" ret "FLOAT" out 172
 name "WriteLn" arg "HANDLE","STRING" ret "FLOAT" out 173
 name "GetLine" arg "HANDLE","FLOAT","STRING" ret "STRING" out 174
 name "CopyBlock" arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "FLOAT" out 175
 name "Truncate" arg "HANDLE" ret "FLOAT" out 176

 name "CreateDir" arg "STRING" ret "FLOAT" out 730
 name "DeleteDir" arg "STRING" ret "FLOAT" out 731
 name "FileRename" arg "STRING","STRING" ret "FLOAT" out 732
 name "FileCopy" arg "STRING","STRING" ret "FLOAT" out 733
 name "FileExist" arg "STRING" ret "FLOAT" out 734
 name "FileDelete" arg "STRING" ret "FLOAT" out 735
 name "GetFileList" arg "STRING","FLOAT" ret "HANDLE" out 736
