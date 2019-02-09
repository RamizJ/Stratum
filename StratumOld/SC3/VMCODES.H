/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			vmcodes.h
Description
			Virtual Machine Codes
Author:
			Alexander Shelemekhov
*/

#ifndef _VM_COMMANDS_CODES_
#define _VM_COMMANDS_CODES_

/*
	 Команды виртуальной машины
*/

//***************************************************************
// PUSH комманды
//***************************************************************

#define  V_END            0

#define  PUSH_FLOAT       1
#define _PUSH_FLOAT       2

#define  vmPUSH_LONG      3
#define vm_PUSH_LONG      4
#define vmPUSH_LONG_const 5

#define PUSH_FLOAT_const  6

#define PUSH_DATA         7 // ?
#define PUSH_DATA_        8 // ?
#define PUSH_DATA_const   9 // ?

//***************************************************************
// POP комманды
//***************************************************************

#define _POP_FLOAT_OLD    10
#define _POP_FLOAT        11

#define vm_POP_LONG_OLD   12
#define vm_POP_LONG       13

#define  POP_DATA         14
#define _POP_DATA         15

#define FLOAT_TO_LONG     16
#define LONG_TO_FLOAT     17

#define V_ADD_F           18
#define V_SUB_F           19
#define V_DIV_F           20
#define V_MUL_F           21
#define V_MOD             22

#define V_TAN             23
#define V_ATAN            24
#define V_SIN             25
#define V_ASIN            26
#define V_COS             27
#define V_ACOS            28

#define V_LN              29    // 1
#define V_LG              30    // 1
#define V_LOG             31    // 2
#define V_STEPEN          32    // 2

#define V_EXP             33    // 1
#define V_SQRT            34    // 1
#define V_SQR             35    // 1
#define V_ED              36    // 1
#define V_DELTA           37    // 1

#define V_MAX             38    // 2
#define V_MIN             39    // 2
#define V_AVERAGE         40    // 2

#define V_ROUND           41    // 2

#define V_RANDOM          42    // 1

#define V_AND             43    // 2
#define V_OR              44    // 2
#define V_NOT             45    // 1

#define V_ABS             46    //1

#define V_SGN             47    //1
#define V_RAD             48    //1
#define V_DEG             49    //1
#define V_STOP            50    //1

#define V_JMP             51    //0
#define V_JNZ             52    //1
#define V_JZ              53    //1

#define V_EQUAL           54
#define V_NOTEQUAL        55
#define V_MORE            56
#define V_MOREorEQUAL     57
#define V_LOW             58
#define V_LOWorEQUAL      59

#define V_MCREATE         60    //6
#define V_MDELETE         61    //2
#define V_MFILL           62    //3
#define V_MGET            63    //4
#define V_MPUT            64    //5
#define V_MEDITOR         65    //2
#define V_MDIAG           66    //3

#define V_MADDX           67    //3
#define V_MSUBX           68    //3
#define V_MDIVX           69    //3
#define V_MMULX           70    //3

#define V_MDET            71    //2
#define V_MDELTA          72    //2
#define V_MED             73    //2

#define V_TRANSP          74    //3
#define V_MADDC           75    //4
#define V_MNOT            76    //2
#define V_MSUM            77    //2

#define V_MSUBC  78 //4 (int Q1,int Q2,int Q3);
#define V_MMULC  79 //4 (int Q1,int Q2,int Q3);

#define V_TRUNC  80    // 2
//#define V_BIG_SUB 81
#define V_CLOSEALL 81
#define V_EQUALI    82
#define V_NOTEQUALI 83

#define V_EDI       84
#define V_ANDI      85
#define V_ORI       86
#define V_NOTI      87
#define V_NOTbin    88
#define VM_SETCRDSYSTEM 89
#define V_MDIVC 90 //4 MDivC(int Q1,int Q2,int Q3);
#define V_MMUL  91 //4 MMul(int Q1,int Q2,int Q3);
#define V_MGLUE 92 //6 MGlue(int Q1,int Q2,int Q3,long y,long x);
#define V_MCUT  93 //7 MCut(int Q1,int Q2,long y,long x,long sy,long sx);
#define V_MMOVE 94 //4 MMove(int Q,long y,long x);
#define V_MOBR  95 //3 MObr(int Q1,int Q2);
#define V_MDIM   96

#define V_XOR   97
#define V_XORBIN 98


#define V_FOLDERDIALOG 99
#define V_MSAVEAS 100 //3
#define V_MLOAD   101 //3
#define V_SAVEDIALOG  102
#define V_LOADDIALOG  103
#define V_CREATEOBJECTFROMFILE 104

#define V_AND_BINARY       105
#define V_OR_BINARY        106
//#define V_NOT             45

#define V_QUITSC           107

#define POP_FLOAT_PTR      108
#define PUSH_FLOAT_PTR     109

#define V_JNZ_HANDLE       110
#define V_JZ_HANDLE        111
#define SETSTATUSTEXT      112
#define V_UN_MINUS         113
#define V_STRINGEX         114
#define GETTICKCOUNT       115

#define V_SHR              116
#define V_SHL              117
#define SCHANGE            119
#define PUSH_STRING        120
#define _PUSH_STRING       121
#define PUSH_STRING_CONST  122
#define _POP_STRING        123

#define PLUS_STRING        124
#define LEFT_STRING        125
#define RIGHT_STRING       126
#define SUBSTR_STRING      127
#define POS_STRING         128
#define REPLICATE_STRING   129
#define LOWER_STRING       130
#define UPPER_STRING       131
#define ANSI_TO_OEM_STRING 132
#define OEM_TO_ANSI_STRING 133
#define COMPARE_STRING     134
#define COMPAREI_STRING    135
#define LENGTH_STRING      136
#define LTRIM_STRING       137
#define RTRIM_STRING       138
#define ALLTRIM_STRING     139
#define ASCII_STRING       140
#define CHR_STRING         141
#define FLOAT_TO_STRING    142
#define STRING_TO_FLOAT    143

#define S_EQUAL            144
#define S_NOTEQUAL         145
#define S_MORE             146
#define S_MOREorEQUAL      147
#define S_LOW              148
#define S_LOWorEQUAL       149
#define RGB_COLOR          150


#define V_SYSTEM           151
#define V_SYSTEMSTR        152

#define V_REGISTEROBJECT   153
#define V_UNREGISTEROBJECT 154
#define V_SETCAPTURE       155
#define V_RELEASECAPTURE   156
#define _POP_STRING_OLD    157
#define VM_GETMOUSEPOS     158
#define vm_INPUT_BOX       159
#define MESSAGE_BOX        160

#define V_CREATESTREAM     161
#define V_CLOSESTREAM      162
#define V_SEEK             163
#define V_STREAMSTATUS     164
#define V_EOF              165
#define V_GETPOS           167
#define V_GETSIZE          168
#define V_SETWIDTH         169
#define V_FREAD            170
#define V_SREAD            171
#define V_FWRITE           172
#define V_SWRITE           173
#define V_GETLINE          174
#define V_COPYBLOCK        175
#define V_TRUNCATE         176

#define V_RANDOMIZE        177

#define V_CREATEFROMFILE3D 197
#define WINSHELL           198
#define RGB_COLORex        199
#define LOADSPACEWINDOW    200
#define OPENSCHEMEWINDOW   201
#define CLOSEWINDOW        202
#define V_GETNAMEBYHSP     203
#define V_GETHSPBYNAME     204
#define V_SETCLIENTSIZE    205

#define SETWINDOWTITLE     206
#define GETWINDOWTITLE     207
#define SHOWWINDOW         208
#define SETWINDOWPOS       209
#define SETWINDOWORG       210
#define SETWINDOWSIZE      211
#define ISWINDOWVISIBLE    212
#define ISICONIC           213
#define ISWINDOWEXIST      214
#define BRINGWINDOWTOTOP   215
#define CASCADEWINDOWS     216
#define TILE               217
#define ARRANGEICONS       218

#define WIN_ORGX           219
#define WIN_ORGY           220
#define WIN_SIZEX          221
#define WIN_SIZEY          222

#define GETOBJECTHANDLE    223
#define GETOBJECTBYNAME    224

#define GETCLIENTWIDTH     225
#define GETCLIENTHEIGHT    226
#define CHOSECOLORDIALOG   227

#define JGETX              228
#define JGETY              229
#define JGETZ              230
#define JGETBTN            231
#define GETAKEYSTATE       232

#define GETCLASSDIR        233
#define GETPROJECTDIR      234
#define GETWINDOWSDIR      235
#define GETSYSTEMDIR       236
#define GETDIRFROMFILE     237
#define ADDSLASH           238
#define GETSTRATUMDIR      239
#define WINEXECUTE         240
#define SETSCROLLRANGE     241

#define VM_GETACTUALWIDTH  242
#define VM_GETACTUALHEIGHT 243
#define VM_SAVEIMAGE       244
#define V_LOCK2D           245
#define VM_LOADMENU        246
#define VM_DELETEMENU      247
#define REGISTEROBJECTS    248
#define UNREGISTEROBJECTS  249


// MCI
#define MCISENDSTRING      250
#define MCISENDSTRING_INT  251
#define MCISENDSTRINGEX    252
#define GETLASTMCIERROR    253
#define GETMCIERROR        254
#define VM_SETCALCORDER    255
#define VM_GETCALCORDER    256

#define VM_GETOBJECT3DFROMPOINT2D   296
#define VM_SWEEPEXTRUDE    297
#define V_ISINTERSECT2D    298
#define SETARROW2D         299
#define CREATELINE2D       300
#define ADDPOINT2D         301
#define DELPOINT2D         302
#define GETBRUSHOBJECT2D   303
#define GETPENOBJECT2D     304
#define GETRGNCREATEMODE   305
#define GETVECTORNUMPOINTS2D 306
#define GETVECTORPOINT2DX  307
#define GETVECTORPOINT2DY  308
#define SETBRUSHOBJECT2D   309
#define SETPENOBJECT2D     310
#define SETRGNCREATEMODE   311
#define SETVECTORPOINT2D   312
#define CREATEPEN2D        313
#define GETPENCOLOR2D      314
#define GETPENROP2D        315
#define GETPENSTYLE2D      316
#define GETPENWIDTH2D      317
#define SETPENCOLOR2D      318
#define SETPENROP2D        319
#define SETPENSTYLE2D      320
#define SETPENWIDTH2D      321
#define DELETETOOL2D       322
#define DELETEOBJECT2D     323
#define GETOBJECTORG2DX    324
#define GETOBJECTORG2DY    325
#define GETOBJECTPARENT2D  326
#define GETOBJECTANGLE2D   327
#define GETOBJECTSIZE2DX   328
#define GETOBJECTSIZE2DY   329
#define GETOBJECTTYPE      330
#define SETOBJECTORG2D     331
#define SETOBJECTSIZE2D    332
#define CREATEBRUSH2D      333
#define SETPOINTS2D        334

#define CREATEDID2D        335
#define CREATEDOUBLEDID2D  336
#define CREATERDID2D       337
#define CREATERDOUBLEDID2D 338
#define CREATEBITMAP2D     339
#define CREATEDOUBLEBITMAP2D 340
#define GETSPACEORGY       341
#define GETSPACEORGX       342
#define SETSPACEORG2D      343
#define SETSCALESPACE2D    344
#define GETSCALESPACE2D    345
#define SNDPLAYSOUND       346

#define GETBOTTOMOBJECT2D      347
#define GETUPPEROBJECT2D       348
#define GETOBJECTFROMZORDER2D  349
#define GETLOWEROBJECT2D       350
#define GETTOPOBJECT2D         351
#define GETZORDER2D            352
#define OBJECTTOBOTTOM2D       353
#define OBJECTTOTOP2D          354
#define SETZORDER2D            355
#define SWAPOBJECT2D           356
#define SETBITMAPSRCRECT       357

#define SETOBJECTATTRIBUTE2D   358
#define GETOBJECTATTRIBUTE2D   359

#define CREATEFONT2D           360
#define CREATESTRING2D         361
#define CREATETEXT2D           362
#define CREATERASTERTEXT2D     363
#define SETSHOWOBJECT2D        364
#define SETLOGSTRING2D         365

#define ADDGROUPITEM2D         366
#define CREATEGROUP2D          367
#define DELETEGROUP2D          368
#define DELGROUPITEM2D         369
#define GETGROUPITEM2D         370
#define GETGROUPITEMS2D        371
#define GETGROUPITEMSNUM2D     372
#define ISGROUPCONTAINOBJECT2D 373
#define SETGROUPITEM2D         374
#define SETGROUPITEMS2D        375
#define COPYTOCLIPBOARD2D      376
#define PASTEFROMCLIPBOARD2D   377
#define GETOBJECTNAME2D        378
#define SETOBJECTNAME2D        379
#define GETOBJECTFROMPOINT2D   380
#define GETLASTPRIMARY         381
#define ROTATEOBJECT2D         382
#define SHOWOBJECT2D           383
#define HIDEOBJECT2D           384
#define STDHYPERJUMP           385
#define GETCHILDCOUNT          386
#define GETCHILDIDBYNUM        387
#define GETCHILDCLASS          389

#define VM_CREATECONTROL2D     390
#define VM_GETCONTROLTEXT      391
#define VM_SETCONTROLTEXT      392
#define VM_CHECKDLGBUTTON      393
#define VM_ISDLGBUTTONCHECKED  394
#define VM_ENABLECONTROL       395
#define SETBRUSHHATCH2D        396
#define GETLOGSTRING2D         397
#define V_GETCLASS             398

#define MESSAGE                400

#define GETBRUSHCOLOR2D        401
#define GETBRUSHROP2D          402
#define GETBRUSHDIB2D          403
#define GETBRUSHSTYLE2D        404
#define GETBRUSHHATCH2D        405
#define SETBRUSHCOLOR2D        406
#define SETBRUSHROP2D          407
#define SETBRUSHDIB2D          408
#define SETBRUSHSTYLE2D        409

#define VM_ADDCHILDREN         410
#define VM_REMOVECHILDREN      411
#define VM_CREATECLASS         412
#define VM_DELETECLASS         413
#define VM_OPENCLASSSCHEME     414
#define VM_CLOSECLASSSCHEME    415
#define VM_CREATELINK          416
#define VM_SETLINKVARS         417
#define VM_REMOVELINK          418
#define VM_GETUNIQUCLASSNAME   419

#define V_GETSCHEMEOBJECT      420
#define V_CREATEWINDOWEX       421
#define V_GETVARF              430
#define V_GETVARS              431
#define V_GETVARH              432

#define V_SETVARF              433
#define V_SETVARS              434
#define V_SETVARH              435

#define VM_CREATELINK2         436
#define VM_SETMODELTEXT        437
#define VM_GETMODELTEXT        438
#define VM_GETANGLEBYXY        439
#define GETOBJECTFROMPOINT2DEX 440

#define VM_OPENVIDEO          441
#define VM_CLOSEVIDEO         442
#define VM_CREATEVIDEOFRAME2D 446
#define VM_VIDEOSETPOS        447
#define VM_FRAMESETPOS2D      448
#define VM_VIDEOPLAY2D        449
#define VM_VIDEOPAUSE2D       450
#define VM_VIDEORESUME2D      451
#define VM_VIDEOSTOP2D        452
#define VM_SETVIDEOSRC2D      453
#define VM_VIDEOGETPOS        454
#define VM_FRAMEGETPOS        455
#define VM_FRAMEGETHVIDEO     456
#define VM_BEGINWRITEVIDEO         457
#define VM_VIDEOCOMPRESSDIALOG 458
#define VM_WRITEFRAME          459
#define SETCONTROLSTYLE2D     460
#define GETCONTROLSTYLE2D     461
#define LBADDSTRING           462
#define LBINSERTSTRING        463
#define LBGETSTRING           464
#define LBCLEARLIST           465
#define LBDELETESTRING        466
#define LBGETCOUNT            467
#define LBGETSELINDEX         468
#define LBSETSELINDEX         469
#define LBGETCARETINDEX       470
#define LBSETCARETINDEX       471
#define LBFINDSTRING          472
#define LBFINDSTRINGEXACT     473
#define GETPIXEL2D            474

#define VM_GETTIME            475
#define PUSHPTR               476 // <- не трогать номер, а то сглючит компилер !!!!
#define PUSHPTRNEW            477 // <- не трогать номер, а то сглючит компилер !!!!

#define VFUNCTION             478
#define DLLFUNCTION           479
#define GETELEMENT            480
#define SETELEMENT            481
#define EMPTYSPACE2D          482
#define CREATEPOLYLINE2D      485
#define VM_GETNAMEBYHANDLE    486

#define V_SETSPACELAYERS      487
#define V_GETSPACELAYERS      488
#define V_GETOBJECTLAYER2D    489
#define V_SETOBJECTLAYER2D    490
#define VM_SCMESSAGE          491
#define VM_SETVARTODEF        492
#define GETTEXTOBJECT2D       493
#define GETTEXTSTRING2D       494
#define GETTEXTFONT2D         495
#define VM_LOADOBJECTSTATE    496
#define VM_SAVEOBJECTSTATE    497
#define VM_GETWINDOWPROP      498
#define VM_CAMERAPROC         499

#ifdef DBE
#define DB_OPENBASE            500
#define DB_OPENTABLE           501
#define DB_CLOSEBASE           502
#define DB_CLOSETABLE          503
#define DB_GETERROR            504
#define DB_GETERRORSTR         505
#define DB_SETDIR              506
#define DB_CLOSEALLBASES       507
#define DB_GOTOP               508
#define DB_GOBOTTOM            509
#define DB_SKIP                510
#define DB_GETFID              511
#define DB_GETFSTR             512
#define DB_SELECT              513
#define DB_GETCOUNT            514
#define DB_BROWSE              515
#define DB_GETDOUBLE           516
#define DB_GETFNAME            517
#define DB_GETFTYPE            518
#define DB_INSERTRECORD        519
#define DB_APPENDRECORD        520
#define DB_DELETERECORD        521
#define DB_SETFIELD            522
#define DB_SETFIELDNUM         523

#define DB_SSETFIELD           524
#define DB_SSETFIELDNUM        525
#define DB_SGETDOUBLE          526
#define DB_SGETFSTR            527

#define DB_CREATETABLE         528
#define DB_ZAP                 529


#define DB_ADDINDEX            530 // by AK
#define DB_DELETEINDEX         531 // by AK
#define DB_OPENINDEX           532 // by AK
#define DB_SWITCHTOINDEX       533 // by AK
#define DB_CLOSEINDEX          534 // by AK
#define DB_REGENINDEX          535 // by AK

#define DB_READBLOB            536
#define DB_WRITEBLOB           537
#define DB_FREEBLOB            538
#define DB_PACKTABLE           539
#define DB_SORTTABLE           540

#define DB_GETDELMODE          541
#define DB_SETDELMODE          542
#define DB_SETCODEPAGE         543
#define DB_GETCODEPAGE         544
#define DB_LOCK                545
#define DB_UNLOCK              546
#define DB_UNDELETE            547
#define DB_COPYTO              548
#define DB_SETTOKEY            549
#define DB_GETFCOUNT           550
#define DB_SETWINTABLE         580
#define DB_SETCONTROLTABLE     581
#define DB_RECNO               582


#define VM_BDEMIN        500
#define VM_BDEMAX        599
#endif

#define VM_SENDMESSAGE 600
#define VM_SETIMAGENAME 601
#define VM_SETLOGTEXT2D 602
#define GETTEXTFG2D     603
#define GETTEXTBK2D     604

#define VM_DIALOG       605
#define VM_DIALOGEX     606
#define VM_GETLINK      607
#define VM_DIALOGEX2    608

#define VM_GETRVALUE    609
#define VM_GETGVALUE    610
#define VM_GETBVALUE    611

#define VM_GETTOOLREF2D 612
#define VM_GETNEXTTOOL2D 613
#define VM_GETNEXTOBJECT 614

#define VM_GETOBJECTPOINT3D  635
#define VM_SETOBJECTPOINT3D  636
#define VM_SETPRIMITIVE3D    637
#define VM_CREATEOBJECT3D      638
#define VM_DELPRIMITIVE3D      639
#define VM_GETNUMPRIMITIVES3D  640
#define VM_ADDPRIMITIVE3D      641
#define VM_DELPOINT3D          642
#define VM_GETNUMPOINTS3D      643
#define VM_ADDOBJECTPOINT3D    644

#define VM_SETMATRIX3D       645
#define VM_GETMATRIX3D       646
#define VM_TRANSFORMPOINTS3D 647
#define VM_ROTATEPOINTS3D    648
#define VM_GETOBJECTBASE3D   649
#define VM_SETOBJECTBASE3D   650
#define VM_GETBASE3D_2       651
#define VM_GETSPACE3D        652
#define VM_GETDIM3D          653
#define VM_TRANSFORM3D       654
#define VM_ROTATE3D          655
#define VM_SETCOLOR3D         656
#define VM_CREATEDEFCAMERA3D  657
#define VM_SWITCHTOCAMERA3D   658
#define VM_GETACTIVECAMERA3D  659
#define VM_CREATESPACE3D      660
#define VM_DELETESPACE3D      661
#define VM_CREATEPROJECTION   662
#define VM_GETPOINTS3D        663
#define VM_SETPOINTS3D        664
#define VM_GETCOLOR3D	      665

#define VM_SETCURRENTOBJ2D    666
#define VM_GETCURRENTOBJ2D    667
#define VM_MAKEFACE3D         668
#define VM_FITTOCAMERA3D      669
#define VM_CREATEMATERIAL     670

#define PUSHCRDSYSTEM3D       671
#define POPCRDSYSTEM3D        672
#define SELECTLOCALCRD3D      673
#define SELECTWORLDCRD3D      674
#define SELECTVIEWCRD3D       675
#define VM_TRCAMERA3D         676
#define VM_SETCOLORS3D        677
#define VM_GETCOLORS3D        678
#define VM_GETMATERIALs       679 // get by name
#define VM_new                700
#define VM_delete             701
#define VM_VCLEARALL          702
#define VM_VINSERT            703
#define VM_VDELETE            704
#define VM_VGETCOUNT          705
#define VM_VGETTYPE           706
#define VM_VGETf              707
#define VM_VGETs              708
#define VM_VGETh              709
#define VM_VSETf              710
#define VM_VSETs              711
#define VM_VSETh              712
#define VM_GETCONTROLTEXTH    713
#define VM_SETCONTROLTEXTH    714

#define VM_LOADPROJECT        720
#define VM_UNLOADPROJECT      721
#define VM_SETACTIVEPROJECT   722
#define VM_ISPROJECTEXIST     723
#define VM_EXECUTEPROJECT     724
#define VM_SETPROJECTPROP     725
#define VM_GETPROJECTPROP     726
#define VM_GETPROJECTPROPS    727
#define VM_APPLYTEXTUTRE      728
#define VM_REMOVETEXTURE      729
#define VM_CREATEDIR          730
#define VM_DELETEDIR          731
#define VM_FILERENAME         732
#define VM_FILECOPY           733
#define VM_FILEEXIST          734
#define VM_FILEDELETE         735
#define VM_GETFILELIST        736
#define VM_GETACTUALSIZE      737
#define VM_SETBKBRUSH         738
#define VM_GETBKBRUSH         739

#define VM_DIFF1			  		 740
#define VM_EQUATION	  		 	 741
#define VM_DIFF2	  		  	 742
#define VM_DEQUATION   	  	 743
#define VM_GETDIBOBJECT2D    744
#define VM_SETDIBOBJECT2D    745
#define VM_GETDDIBOBJECT2D   746
#define VM_SETDDIBOBJECT2D   747
#define VM_GETDATE           748
#define VM_VIDEOGETMARKER    749
#define VM_GETVARINFO        750
#define VM_GETVARCOUNT       751
#define VM_VSTORE            752
#define VM_VLOAD             753
#define GETBITMAPSRCRECT     754
#define VM_SETHYPERJUMP      755

#define SETRDIB2D            756
#define SSETRDDIB2D          757
#define GETRDIB2D            758
#define GETRDDIB2D           759

#define V_MSORT              760    //new
#define VM_DUPLICATEOBJECT3D 761
#define VM_CHECKMENU         762
#define VM_ENABLEMENU        763

#endif

//Добавил Марченко С.В.
#define PLUS_STRING_FLOAT    770
#define PLUS_FLOAT_STRING    771
#define VM_GETSCREENWIDTH    772
#define VM_GETSCREENHEIGHT   773

#define VM_GETWORKAREAX			 774
#define VM_GETWORKAREAY 		 775
#define VM_GETWORKAREAWIDTH  776
#define VM_GETWORKAREAHEIGHT 777

#define VM_GETKEYBOARDLAYOUT 778
#define VM_SUBSTR1_STRING    779

#define VM_SETBRUSHOBJECT2D_HANDLE  780  //нужны для поддержки старых версий проектов
#define VM_SETPENOBJECT2D_HANDLE    781

#define VM_SETWINDOWTRANSPARENT 			782
#define VM_SETWINDOWTRANSPARENT_H 			793
#define VM_SETWINDOWTRANSPARENTCOLOR 	783
#define VM_SETWINDOWTRANSPARENTCOLOR_H 	794

#define VM_SETWINDOWREGION   			784
#define VM_SETWINDOWREGION_H   		795

#define VM_GETTITLEHEIGHT   			785
#define VM_GETSMALLTITLEHEIGHT  	786

#define VM_GETFIXEDFRAMEHEIGHT   	787
#define VM_GETFIXEDFRAMEWIDTH   	788
#define VM_GETSIZEFRAMEHEIGHT   	789
#define VM_GETSIZEFRAMEWIDTH   		790

#define VM_WINDOWINTASKBAR_H   		791
#define VM_WINDOWINTASKBAR_S   		792

#define VM_SHOWCURSOR				   		796

#define VM_SCREENSHOT				   		797
#define VM_SCREENSHOT_FULL				842
#define VM_SCREENSHOT_DESKTOP		  844
#define VM_SCREENSHOT_DESKTOP_FULL 845

#define VM_LOCKOBJECT2D			   		798
#define VM_GETVARINFOFULL		   		799

#define VM_INCREMENT_AFTER   			800

#define VM_SENDSMS   							841
#define VM_SENDMAIL   						843

#define VM_GETFONTNAME2D					846
#define VM_GETFONTSIZE2D					847
#define VM_GETTEXTCOUNT2D					848
#define VM_GETFONTSTYLE2D					849
#define VM_SETFONTSIZE2D					850
#define VM_SETFONTSTYLE2D					851
#define VM_SETFONTNAME2D					852

#define VM_GETTEXTSTRING2D_I 			853
#define VM_GETTEXTFONT2D_I 		  	854
#define VM_GETTEXTFG2D_I 		  		855
#define VM_GETTEXTBG2D_I 		  		856

#define VM_SETTEXTSTRING2D_I 	 		857
#define VM_SETTEXTFONT2D_I 	 			858
#define VM_SETTEXTFG2D_I 	 				859
#define VM_SETTEXTBG2D_I 	 				860
#define VM_SETTEXT2D_I 	 					861

#define VM_CREATEFONT2DPT					862

#define VM_SETSTANDARTCURSOR			863
#define VM_SETSTANDARTCURSOR_S		864
#define VM_LOADCURSOR							865
#define VM_LOADCURSOR_S						866

#define VM_INC						     		867
#define VM_INC_STEP				     		868
#define VM_DEC						     		869
#define VM_DEC_STEP				     		870
#define VM_LIMIT					     		871

#define VM_GETFONTLIST		     		872

#define VM_VSORT					     		873
#define VM_VSORT_ASC			     		874
#define VM_VSORT_DIFFERENT                              875

#define VM_GETUSERKEYVALUE                              876
#define VM_GETUSERKEYFULLVALUE                          877
#define VM_SENDUSERRESULT                           	879
#define VM_COPYUSERRESULT                               880
#define VM_USERKEYISAUTORIZED                           889
#define VM_USERKEYISAUTORIZED_UK                        890

#define VM_READUSERKEY                              	883
#define VM_READPROJECTKEY                               888
#define VM_GETUSERKEYVALUE_UK                           884
#define VM_GETUSERKEYFULLVALUE_UK                       885
#define VM_SENDUSERRESULT_UK                            886
#define VM_COPYUSERRESULT_UK                            887

#define VM_GETTEMPDIRECTORY				878

#define VM_GETROMDRIVENAMES				881

#define VM_SHELLWAIT                        		882

#define VM_SETCONTROLFONT                           	1101
#define VM_SETCONTROLTEXTCOLOR                          1102
#define VM_GETCONTROLTEXTLENGTH                         1103
#define VM_GETCONTROLTEXTPART                           1104
#define VM_ADDCONTROLTEXT                           	1105
#define VM_INSERTCONTROLTEXT                            1106

#define VM_SETWINDOWOWNER                           	1107
#define VM_SETWINDOWPARENT				1108

#define VM_GETPROJECTCLASSES                            1109
#define VM_GETCLASSFILE                                 1110

#define VM_INSERTTEXT2D                             	1111
#define VM_REMOVETEXT2D                                 1112
#define VM_ADDTEXT2D                                    1113

#define VM_LBGETSELINDEXS                               1114

#define VM_SETSTRINGBUFFERMODE                          1115

#define VM_SETMODELTEXTWITHOUTERROR                     1116

#define VM_SENDSMSWAIT                              	1117
#define VM_SENDMAILWAIT                             	1118

#define VM_SETCONTROLFOCUS 				1119

#define VM_DBSQLH                                       1120

#define VM_SETOBJECTALPHA2D				1121
#define VM_GETOBJECTALPHA2D				1122

#define VM_SETBRUSHPOINTS2D				1123
#define VM_SETBRUSHCOLORS2D				1124

#define VM_SETPIXEL2D					1125

#define VM_ENCRYPTSTREAM    				1126
#define VM_SENDDATA                                     1127
#define VM_DECRYPTSTREAM                            	1128

#define VM_ROUNDT           				1129

#define VM_SETSPACERENDERERENGINE2D 1130

//Последний занятый номер - 1130
//801..830 определены в файле audio\scaudio.h
//831..840 - 891..899 определены в файле TextAnalyser\TextAnalyser.h
//900..1100 и 1200..1500 определены в 3d движке Панькова Д.
//1501..1600 NUI.h


#define VM_MAXIMUM_code  1600    // макcимальное количество функций виртуальной машины


