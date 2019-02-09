
 name "GetRValue" arg "COLORREF" ret "FLOAT" out 609
 name "GetGValue" arg "COLORREF" ret "FLOAT" out 610
 name "GetBValue" arg "COLORREF" ret "FLOAT" out 611


// name "SetImageSpace2d"    arg  "STRING","HANDLE" out 256
// name "GetImageSpace2d"    arg  "STRING"  ret "HANDLE" out 255

 name "EmptySpace2d"        arg "HANDLE" ret "FLOAT" out 482
 name "LockSpace2d" arg "HANDLE","FLOAT" ret "FLOAT" out 245
 name "SetCrdSystem2d" arg "HANDLE","FLOAT","FLOAT" ret "FLOAT" out 89
//***************************************************//
//***************************************************//
//         T O O L S                                 //
//***************************************************//
//***************************************************//

 name "DeleteTool2d"       arg "HANDLE","FLOAT","HANDLE"  ret "FLOAT"  out 322
 name "GetToolRef2d"       arg "HANDLE","FLOAT","HANDLE"  ret "FLOAT"  out 612
 name "GetNextTool2d"       arg "HANDLE","FLOAT","HANDLE"  ret "HANDLE" out 613

// P E N 2 D

 name "CreatePen2d"         arg "HANDLE","FLOAT","FLOAT","COLORREF","FLOAT" ret "HANDLE" out 313
 name "GetPenColor2d"       arg "HANDLE","HANDLE" ret "COLORREF" out 314
 name "GetPenRop2d"         arg "HANDLE","HANDLE" ret "FLOAT" out 315
 name "GetPenStyle2d"       arg "HANDLE","HANDLE" ret "FLOAT" out 316
 name "GetPenWidth2d"       arg "HANDLE","HANDLE" ret "FLOAT" out 317
 name "SetPenColor2d"       arg "HANDLE","HANDLE","COLORREF" ret "FLOAT" out 318
 name "SetPenROP2d"         arg "HANDLE","HANDLE","FLOAT"    ret "FLOAT" out 319
 name "SetPenStyle2d"       arg "HANDLE","HANDLE","FLOAT"    ret "FLOAT" out 320
 name "SetPenWidth2d"       arg "HANDLE","HANDLE","FLOAT"    ret "FLOAT" out 321

// B R U S H 2 D

 name "CreateBrush2d"       arg "HANDLE","FLOAT","FLOAT","COLORREF","HANDLE","FLOAT" ret "HANDLE" out 333

 name "GetBrushColor2d"     arg "HANDLE","HANDLE" ret "COLORREF" out 401
 name "GetBrushRop2d"       arg "HANDLE","HANDLE" ret "FLOAT" out 402
 name "GetBrushStyle2d"     arg "HANDLE","HANDLE" ret "FLOAT" out 404
 name "GetBrushHatch2d"     arg "HANDLE","HANDLE" ret "FLOAT" out 405
 name "GetBrushDib2d"       arg "HANDLE","HANDLE" ret "HANDLE" out 403

 name "SetBrushColor2d"     arg "HANDLE","HANDLE","COLORREF" ret "FLOAT" out 406
 name "SetBrushRop2d"       arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 407
 name "SetBrushStyle2d"     arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 409
 name "SetBrushHatch2d"     arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 396
 name "SetBrushDib2d"       arg "HANDLE","HANDLE","HANDLE" ret "FLOAT" out 408

 name "SetSpaceRenderEngine2d" arg "HANDLE","FLOAT" ret "FLOAT" out 1130
 
 name "SetBkBrush2d"        arg "HANDLE","HANDLE" ret "FLOAT" out 738
 name "GetBkBrush2d"        arg "HANDLE"         ret "HANDLE" out 739

 name "SetBrushPoints2d"    arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 1123
 name "SetBrushColors2d"    arg "HANDLE","HANDLE","HANDLE" ret "FLOAT" out 1124
  

//font
name "CreateFont2d"       arg "HANDLE","STRING","FLOAT","FLOAT" ret "HANDLE" out 360
name "CreateFont2dpt"    arg "HANDLE","STRING","FLOAT","FLOAT" ret "HANDLE" out 862
name "GetFontName2d"   arg "HANDLE","HANDLE" ret "STRING" out 846
name "GetFontSize2d"     arg "HANDLE","HANDLE" ret "FLOAT" out 847
name "GetFontStyle2d"    arg "HANDLE","HANDLE" ret "FLOAT" out 849
name "GetFontList"          ret "HANDLE" out 872

name "SetFontSize2d"        arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 850
name "SetFontStyle2d"       arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 851
name "SetFontName2d"      arg "HANDLE","HANDLE","STRING" ret "FLOAT" out 852

//string
name "CreateString2d"       arg "HANDLE","STRING" ret "HANDLE" out 361
name "GetString2d"       arg "HANDLE","HANDLE" ret "STRING" out 397
name "SetString2d"       arg "HANDLE","HANDLE","STRING" ret "FLOAT" out 365

//text
name "CreateRasterText2d"    arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 363
name "CreateText2d"         arg "HANDLE","HANDLE","HANDLE","COLORREF","COLORREF" ret "HANDLE" out 362

name "GetTextObject2d"       arg "HANDLE","HANDLE" ret "HANDLE" out 493
name "GetTextCount2d"       arg "HANDLE","HANDLE" ret "FLOAT" out 848
name "GetTextString2d"      arg "HANDLE","HANDLE" ret "HANDLE" out 494
name "GetTextString2d"      arg "HANDLE","HANDLE","FLOAT" ret "HANDLE" out 853
name "GetTextFont2d"        arg "HANDLE","HANDLE" ret "HANDLE" out 495
name "GetTextFont2d"        arg "HANDLE","HANDLE","FLOAT" ret "HANDLE" out 854
name "GetTextFgColor2d"     arg "HANDLE","HANDLE" ret "COLORREF" out 603
name "GetTextFgColor2d"     arg "HANDLE","HANDLE","FLOAT" ret "COLORREF" out 855
name "GetTextBkColor2d"     arg "HANDLE","HANDLE" ret "COLORREF" out 604
name "GetTextBkColor2d"     arg "HANDLE","HANDLE","FLOAT" ret "COLORREF" out 856

name "SetText2d"            arg "HANDLE","HANDLE","HANDLE","HANDLE","COLORREF","COLORREF" ret "FLOAT" out 602
name "SetText2d"            arg "HANDLE","HANDLE","FLOAT","HANDLE","HANDLE","COLORREF","COLORREF" ret "FLOAT" out 861
name "SetTextString2d"      arg "HANDLE","HANDLE","FLOAT","HANDLE" ret "FLOAT" out 857
name "SetTextFont2d"        arg "HANDLE","HANDLE","FLOAT","HANDLE" ret "FLOAT" out 858
name "SetTextFgColor2d"     arg "HANDLE","HANDLE","FLOAT","COLORREF" ret "FLOAT" out 859
name "SetTextBkColor2d"     arg "HANDLE","HANDLE","FLOAT","COLORREF" ret "FLOAT" out 860

name "AddText2d"         arg "HANDLE","HANDLE","FLOAT","HANDLE","HANDLE","COLORREF","COLORREF" ret "FLOAT" out 1111
name "AddText2d"            arg "HANDLE","HANDLE", "HANDLE","HANDLE","COLORREF","COLORREF" ret "FLOAT" out 1113
name "RemoveText2d"         arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 1112

//dib
 name "CreateDIB2d"         arg "HANDLE","STRING" ret "HANDLE" out 335
 name "CreateRDIB2d"        arg "HANDLE","STRING" ret "HANDLE" out 337

//double

 name "CreateDoubleDIB2d"   arg "HANDLE","STRING" ret "HANDLE" out 336
 name "CreateRDoubleDIB2d"  arg "HANDLE","STRING" ret "HANDLE" out 338
 name "GetDibPixel2d"  arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "COLORREF" out 474
 name "SetDibPixel2d"  arg "HANDLE","HANDLE","FLOAT","FLOAT","COLORREF" ret "FLOAT" out 1125

 name "GetDibObject2d"      arg "HANDLE","HANDLE" ret "HANDLE" out 744
 name "SetDibObject2d"      arg "HANDLE","HANDLE","HANDLE"  ret "FLOAT" out 745
 name "GetDDibObject2d"     arg "HANDLE","HANDLE" ret "HANDLE" out 746
 name "SetDDibObject2d"     arg "HANDLE","HANDLE","HANDLE"  ret "FLOAT" out 747
// shareable
 name "SetRDib2d" arg "HANDLE","HANDLE","STRING" ret "FLOAT" out 756
 name "SetRDoubleDib2d" arg "HANDLE","HANDLE","STRING" ret "FLOAT" out 757
 name "GetRDib2d" arg "HANDLE","HANDLE" ret "STRING" out 758
 name "GetRDoubleDib2d" arg "HANDLE","HANDLE" ret "STRING" out 759

//***************************************************//
//***************************************************//
//             O B J E C T 2 D  commmon              //
//***************************************************//
//***************************************************//

 name "IsObjectsIntersect2d" arg "HANDLE","HANDLE","HANDLE","FLOAT" ret "FLOAT" out 298
 name "SetCurrentObject2d" arg "HANDLE","HANDLE" ret "FLOAT" out 666
 name "GetCurrentObject2d" arg "HANDLE" ret "HANDLE"         out 667
 name "GetNextObject2d" arg "HANDLE","HANDLE" ret "HANDLE"   out 614

 // create & delete
 name "CreateObjectFromFile2d" arg "HANDLE","STRING","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 104
 name "DeleteObject2d"      arg "HANDLE","HANDLE"  ret "FLOAT" out 323

 name "GetObjectType2d"       arg "HANDLE","HANDLE"  ret "FLOAT" out 330

 // name
 name "GetObjectName2d"     arg "HANDLE","HANDLE" ret "STRING" out 378
 name "SetObjectName2d"     arg "HANDLE","HANDLE","STRING" ret "FLOAT" out 379
 name "GetObject2dByName"   arg "HANDLE","HANDLE","STRING" ret "HANDLE" out 224

 // origin & size
 name "SetObjectOrg2d"      arg "HANDLE","HANDLE","FLOAT","FLOAT"  ret "FLOAT" out 331
 name "GetObjectOrg2dx"     arg "HANDLE","HANDLE"  ret "FLOAT" out 324
 name "GetObjectOrg2dy"     arg "HANDLE","HANDLE"  ret "FLOAT" out 325

 name "SetObjectSize2d"     arg "HANDLE","HANDLE","FLOAT","FLOAT"  ret "FLOAT" out 332
 name "GetObjectWidth2d"    arg "HANDLE","HANDLE"  ret "FLOAT" out 328
 name "GetObjectHeight2d"    arg "HANDLE","HANDLE"  ret "FLOAT" out 329

 name "GetActualWidth2d"    arg "HANDLE","HANDLE"  ret "FLOAT" out 242
 name "GetActualHeight2d"   arg "HANDLE","HANDLE"  ret "FLOAT" out 243
 name "SaveRectArea2d"      arg "HANDLE","STRING","FLOAT","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 244

//rotate
 name "GetObjectAngle2d"    arg "HANDLE","HANDLE"  ret "FLOAT" out 327
 name "RotateObject2d"      arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT"  ret "FLOAT" out 382

//alpha
 name "SetObjectAlpha2d"    arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 1121
 name "GetObjectAlpha2d"    arg "HANDLE","HANDLE"  ret "FLOAT" out 1122

 // hide show
 name "SetShowObject2d"     arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 364
 name "ShowObject2d"        arg "HANDLE","HANDLE" out 383
 name "HideObject2d"        arg "HANDLE","HANDLE" out 384

//attributes
 name "GetObjectAttribute2d" arg "HANDLE","HANDLE" ret "FLOAT" out 359
 name "SetObjectAttribute2d" arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "FLOAT" out 358
 name "LockObject2d" arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 798

 name "GetObjectFromPoint2d"  arg "HANDLE","FLOAT","FLOAT" ret "HANDLE" out 380
 name "GetObjectFromPoint2dEx" arg "HANDLE","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 440

 name "GetLastPrimary2d"       ret "HANDLE" out 381

// Clipboard
 name "CopyToClipboard2d"     arg "HANDLE","HANDLE" ret "FLOAT" out 376
 name "PasteFromClipboard2d"  arg "HANDLE","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 377

//***************************************************//
//         Z   O R D E R                             //
//***************************************************//

 name "GetBottomObject2d"     arg "HANDLE" ret "HANDLE" out 347
 name "GetUpperObject2d"      arg "HANDLE","HANDLE" ret "HANDLE" out 348
 name "GetObjectFromZOrder2d" arg "HANDLE","FLOAT" ret "HANDLE"  out 349
 name "GetLowerObject2d"      arg "HANDLE","HANDLE" ret "HANDLE" out 350
 name "GetTopObject2d"        arg "HANDLE" ret "HANDLE"          out 351
 name "GetZOrder2d"           arg "HANDLE","HANDLE" ret "FLOAT"  out 352
 name "ObjectToBottom2d"      arg "HANDLE","HANDLE" ret "FLOAT"  out 353
 name "ObjectToTop2d"         arg "HANDLE","HANDLE" ret "FLOAT"  out 354
 name "SetZOrder2d"           arg "HANDLE","HANDLE","FLOAT" ret "FLOAT"  out 355
 name "SwapObject2d"          arg "HANDLE","HANDLE","HANDLE" ret "FLOAT" out 356


//***************************************************//
//             O B J E C T 2 D  /by type/            //
//***************************************************//

//line

 name "CreateLine2d"        arg "HANDLE","HANDLE","HANDLE","FLOAT","FLOAT" ret "HANDLE" out 300
 name "CreatePolyLine2d"    arg "HANDLE","HANDLE","HANDLE",["FLOAT","FLOAT"] ret "HANDLE" out 485
 name "AddPoint2d"          arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 301
 name "DelPoint2d"          arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 302
 name "GetBrushObject2d"    arg "HANDLE","HANDLE" ret "HANDLE" out 303
 name "GetPenObject2d"      arg "HANDLE","HANDLE" ret "HANDLE" out 304
 name "GetRGNCreateMode"    arg "HANDLE","HANDLE" ret "FLOAT" out 305
 name "GetVectorNumPoints2d" arg "HANDLE","HANDLE" ret "FLOAT" out 306
 name "GetVectorPoint2dx"   arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 307
 name "GetVectorPoint2dy"   arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 308

 name "SetBrushObject2d"    arg "HANDLE","HANDLE","HANDLE" ret "HANDLE" out 309
 name "SetPenObject2d"      arg "HANDLE","HANDLE","HANDLE" ret "HANDLE" out 310
//name "SetBrushObject2d"    arg "HANDLE","HANDLE","HANDLE" ret "HANDLE" out 780
//name "SetPenObject2d"      arg "HANDLE","HANDLE","HANDLE" ret "HANDLE" out 781


 name "SetRGNCreateMode"    arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 311
 name "SetVectorPoint2d"    arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 312
 name "SetPoints2d"         arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "FLOAT" out 334
 name "SetLineArrows2d"     arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 299

//allbitmaps
 name "SetBitmapSrcRect2d"      arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 357
 name "GetBitmapSrcRect2d"      arg "HANDLE","HANDLE",&"FLOAT",&"FLOAT",&"FLOAT",&"FLOAT" ret "FLOAT" out 754

//bitmap
 name "CreateBitmap2d"      arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "HANDLE" out 339


//doublebitmap
 name "CreateDoubleBitmap2d" arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "HANDLE" out 340

// ####  Control Objects ####

name "CreateControlObject2d" arg "HANDLE","STRING","STRING","FLOAT",
	 "FLOAT","FLOAT","FLOAT","FLOAT","FLOAT" ret "HANDLE"    out 390

name "GetControlTextLength2d" arg "HANDLE","HANDLE" ret "FLOAT"  out 1103
name "GetControlText2d" arg "HANDLE","HANDLE" ret "STRING"  out 391
name "GetControlText2d" arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "STRING"  out 1104

name "SetControlText2d" arg "HANDLE","HANDLE","STRING"  ret "FLOAT"    out 392  //**
name "AddControlText2d" arg "HANDLE","HANDLE","STRING"  ret "FLOAT"    out 1105
name "AddControlText2d" arg "HANDLE","HANDLE","STRING","FLOAT"  ret "FLOAT"    out 1106

name "SetControlFont2d" arg "HANDLE","HANDLE","HANDLE"  ret "FLOAT"    out 1101
name "SetControlTextColor2d" arg "HANDLE","HANDLE","COLORREF"  ret "FLOAT"    out 1102

name "GetControlText2ds" arg "HANDLE","HANDLE","HANDLE" ret "FLOAT"  out 713
name "SetControlText2ds" arg "HANDLE","HANDLE","HANDLE" ret "FLOAT"    out 714  //**


name "CheckDlgButton2d" arg "HANDLE","HANDLE","FLOAT"  ret "FLOAT"      out 393 //**
name "IsDlgButtonChecked2d" arg "HANDLE","HANDLE" ret "FLOAT" out 394
name "EnableControl2d" arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 395        //**
name "SetControlFocus2d" arg "HANDLE","HANDLE" out 1119        //**

name "SetControlStyle2d" arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out      460
name "GetControlStyle2d" arg "HANDLE","HANDLE" ret "FLOAT" out  461
name "LBAddString"       arg "HANDLE","HANDLE","STRING"  ret "FLOAT" out    462 //**
name "LBInsertString"    arg "HANDLE","HANDLE","STRING","FLOAT" ret "FLOAT" out 463
name "LBGetString"       arg "HANDLE","HANDLE","FLOAT" ret "STRING" out 464
name "LBClearList"       arg "HANDLE","HANDLE" ret "FLOAT" out  465             //**
name "LBDeleteString"    arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 466      //**
name "LBGetCount"        arg "HANDLE","HANDLE" ret "FLOAT" out  467
name "LBGetSelIndex"     arg "HANDLE","HANDLE" ret "FLOAT" out  468
name "LBGetSelIndexs"     arg "HANDLE","HANDLE" ret "HANDLE" out  1114
name "LBSetSelIndex"     arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out  469     //**
name "LBGetCaretIndex"   arg "HANDLE","HANDLE" ret "FLOAT" out  470
name "LBSetCaretIndex"   arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out      471 //**
name "LBFindString"      arg "HANDLE","HANDLE","STRING","FLOAT" ret "FLOAT" out 472
name "LBFindStringExact" arg "HANDLE","HANDLE","STRING","FLOAT" ret "FLOAT" out  473


//***************************************************//
//                G R O U P S                        //
//***************************************************//
 name "AddGroupItem2d"        arg "HANDLE","HANDLE","HANDLE" ret "FLOAT" out 366
 name "CreateGroup2d"         arg "HANDLE",["HANDLE"] ret "HANDLE" out 367
 name "DeleteGroup2d"         arg "HANDLE","HANDLE" ret "FLOAT" out 368
 name "DelGroupItem2d"        arg "HANDLE","HANDLE","HANDLE" ret "FLOAT" out  369
 name "GetGroupItem2d"        arg "HANDLE","HANDLE","FLOAT" ret "HANDLE" out 370
 name "GetGroupItemsNum2d"    arg "HANDLE","HANDLE" ret "FLOAT" out 372
 name "IsGroupContainObject2d" arg "HANDLE","HANDLE","HANDLE" ret "FLOAT" out 373
 name "SetGroupItem2d"        arg "HANDLE","HANDLE","FLOAT","HANDLE" ret "FLOAT" out 374
 name "SetGroupItems2d"       arg "HANDLE","HANDLE",["HANDLE"] ret "FLOAT" out 375
 name "GetObjectParent2d"     arg "HANDLE","HANDLE"  ret "HANDLE" out 326

//***************************************************//
//         S P A C E 2 D                             //
//***************************************************//

 name "GetSpaceOrg2dy"        arg "HANDLE" ret "FLOAT" out 341
 name "GetSpaceOrg2dx"        arg "HANDLE" ret "FLOAT" out 342
 name "SetSpaceOrg2d"         arg "HANDLE","FLOAT","FLOAT" ret "FLOAT" out 343
 name "SetScaleSpace2d"       arg "HANDLE","FLOAT" ret "FLOAT" out 344
 name "GetScaleSpace2d"       arg "HANDLE" ret "FLOAT" out 345
 name "SetSpaceLayers2d"      arg "HANDLE","FLOAT" ret "FLOAT" out 487
 name "GetSpaceLayers2d"      arg "HANDLE"  ret "FLOAT" out 488
 name "GetAngleByXY"          arg "FLOAT","FLOAT" ret "FLOAT" out 439

 name "GetActualSize2d" arg "HANDLE","HANDLE",&"FLOAT",&"FLOAT" ret "FLOAT" out 737

 name "SetHyperJump2d" arg "HANDLE","HANDLE","FLOAT",["STRING"] ret "FLOAT" out 755
