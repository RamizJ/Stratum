// Data base functions
// based on Borland Database Engine
name "DbOpenBase"          arg "STRING","STRING","STRING","STRING" ret "HANDLE" out 500
name "DbOpenTable"         arg "HANDLE","STRING","STRING","STRING","STRING","FLOAT","STRING" ret "HANDLE" out 501
name "DbCloseBase"         arg "HANDLE" ret "FLOAT" out 502
name "DbCloseTable"        arg "HANDLE" ret "FLOAT" out 503
name "DbGetError"              ret "FLOAT" out 504
name "DbGetErrorStr"       arg "FLOAT" ret "STRING" out 505
name "DbSetDir"            arg "HANDLE","STRING" ret "FLOAT" out 506
name "DbCloseAll"              out 507
name "DbGoTop"             arg "HANDLE" ret "FLOAT" out 508
name "DbGoBottom"          arg "HANDLE" ret "FLOAT" out 509
name "DbSkip"              arg "HANDLE","FLOAT" ret "FLOAT" out 510
name "DbFieldId"           arg "HANDLE","STRING" ret "FLOAT" out 511
name "DbGetField"          arg "HANDLE","FLOAT" ret "STRING" out 512
name "DbSQL"               arg "HANDLE","STRING","FLOAT" ret "HANDLE" out 513
name "DbSQL"               arg "HANDLE","HANDLE","FLOAT" ret "HANDLE" out 1120
name "DbGetCount"          arg "HANDLE" ret "FLOAT" out 514
name "DbBrowse"            arg "STRING","HANDLE","STRING" ret "FLOAT" out 515
name "DBGetFieldN"         arg "HANDLE","FLOAT" ret "FLOAT" out 516
name "DBGetFieldName"      arg "HANDLE","FLOAT" ret "STRING" out 517
name "DBGetFieldType"      arg "HANDLE","FLOAT" ret "FLOAT" out 518
name "DbInsertRecord"      arg "HANDLE" ret "FLOAT" out 519
name "DbAppendRecord"      arg "HANDLE" ret "FLOAT" out 520
name "DbDeleteRecord"      arg "HANDLE" ret "FLOAT" out 521
name "DbSetField"          arg "HANDLE","FLOAT","STRING" ret "FLOAT" out 522
name "DbSetField"          arg "HANDLE","FLOAT","FLOAT" ret "FLOAT" out 523
name "DbSetField"          arg "HANDLE","STRING","STRING" ret "FLOAT" out 524
name "DbSetField"          arg "HANDLE","STRING","FLOAT" ret "FLOAT" out 525
name "DbGetFieldN"         arg "HANDLE","STRING" ret "FLOAT" out 526
name "DbGetField"          arg "HANDLE","STRING" ret "STRING" out 527
name "DbGetFieldCount"     arg "HANDLE" ret "FLOAT" out 550
name "DbCreateTable"       arg "HANDLE","STRING","STRING","STRING","FLOAT" ret "FLOAT" out 528
name "DbZap"               arg "HANDLE" ret "FLOAT" out 529
name "DbSortTable"         arg "HANDLE","FLOAT",["STRING"] ret "FLOAT" out 540
name "DbAddIndex"          arg "HANDLE", "STRING", "FLOAT", "STRING", "FLOAT", "FLOAT", "STRING", "STRING",["STRING"] ret "FLOAT" out 530
name "DbDeleteIndex"       arg "HANDLE", "STRING", "FLOAT", "STRING" ret "FLOAT" out 531
name "DbOpenIndex"         arg "HANDLE", "STRING", "FLOAT" ret "FLOAT" out 532
name "DbSwitchToIndex"     arg "HANDLE", "STRING", "FLOAT", "STRING", "FLOAT" ret "FLOAT" out 533
name "DbCloseIndex"        arg "HANDLE", "STRING" ret "FLOAT" out 534
name "DbRegenIndex"        arg "HANDLE", "STRING", "FLOAT", "STRING" ret "FLOAT" out 535
name "DbPackFile"          arg "HANDLE"  ret "FLOAT" out 539
name "DbSetTable"          arg "STRING","HANDLE","STRING" ret "FLOAT" out 580
name "DbSetControlTable"   arg "HANDLE","HANDLE","HANDLE","STRING" ret "FLOAT" out 581
name "DbGetBlob"           arg "HANDLE","FLOAT","HANDLE" ret "FLOAT" out 536
name "DbPutBlob"           arg "HANDLE","FLOAT","HANDLE" ret "FLOAT" out 537
name "DbFreeBlob"          arg "HANDLE","FLOAT" ret "FLOAT" out 538
name "DbGetDelMode"        arg "HANDLE"         ret "FLOAT" out  541
name "DbSetDelMode"        arg "HANDLE","FLOAT" ret "FLOAT" out  542
name "DbSetCodePage"       arg "HANDLE","FLOAT" ret "FLOAT" out  543
name "DbGetCodePage"       arg "HANDLE"         ret "FLOAT" out  544
name "DbLock"              arg "HANDLE","FLOAT" ret "FLOAT" out  545
name "DbUnlock"            arg "HANDLE","FLOAT" ret "FLOAT" out  546
name "DbUndeleteRecord"    arg "HANDLE"         ret "FLOAT" out  547
name "DbCopyTo"            arg "HANDLE","STRING","STRING" ret "FLOAT" out 548
name "DbSetToKey"          arg "HANDLE","STRING" ret "FLOAT" out 549
name "DbGetPos"            arg "HANDLE" ret "FLOAT" out   582
