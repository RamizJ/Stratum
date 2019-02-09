// functions to work with Windows


//***************************************************//
//                Create & Close                     //
//***************************************************//
 name "LoadSpacewindow" arg "STRING","STRING","STRING" ret "HANDLE" out 200
 name "OpenSchemeWindow" arg "STRING","STRING","STRING" ret "HANDLE" out 201
 name "CreateWindowEx"  arg "STRING","STRING","STRING","FLOAT","FLOAT","FLOAT","FLOAT","STRING" ret "HANDLE" out 421

 name "CloseWindow" arg "STRING" ret "FLOAT" out 202

//***************************************************//
//        Name & Title & Space                       //
//***************************************************//

 name "GetWindowName"    arg "HANDLE" ret "STRING" out 203
 name "GetWindowSpace"   arg "STRING" ret "HANDLE" out 204

 name "SetWindowTitle"   arg "STRING","STRING" ret "FLOAT" out 206
 name "GetWindowTitle"   arg "STRING" ret "STRING" out 207

//***************************************************//
//        Size & Position                            //
//***************************************************//

 name "ShowWindow"       arg "STRING","FLOAT" ret "FLOAT" out 208
 name "SetWindowPos"     arg "STRING","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 209
 name "SetWindowOrg"     arg "STRING","FLOAT","FLOAT" ret "FLOAT" out 210
 name "SetWindowSize"    arg "STRING","FLOAT","FLOAT" ret "FLOAT" out 211
 name "SetClientSize"    arg  "STRING","FLOAT","FLOAT" ret "FLOAT" out 205
 name "GetClientWidth"   arg  "STRING"  ret "FLOAT" out 225
 name "GetClientHeight"  arg  "STRING"  ret "FLOAT" out 226

 name "IswindowVisible"  arg "STRING" ret "FLOAT" out 212
 name "IsIconic"         arg "STRING" ret "FLOAT" out 213
 name "IsWindowExist"    arg "STRING" ret "FLOAT" out 214
 name "BringWindowToTop" arg "STRING" ret "FLOAT" out 215
 name "CascadeWindows"   out 216
 name "Tile"             arg "FLOAT" out 217
 name "ArrangeIcons"     out 218
 name "GetWindowOrgX"    arg "STRING" ret "FLOAT" out 219
 name "GetWindowOrgY"    arg "STRING" ret "FLOAT" out 220
 name "GetWindowWidth"   arg "STRING" ret "FLOAT" out 221
 name "GetWindowHeight"   arg "STRING" ret "FLOAT" out 222

 name "SetScrollRange"   arg "STRING","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 241

 name "GetWindowProp" arg "STRING","STRING" ret "STRING" out 498
//***************************************************//
//    WINDOWS    Messages                            //
//***************************************************//

 name "RegisterObject" arg "HANDLE","HANDLE","STRING","FLOAT","FLOAT" out 153
 name "UnregisterObject" arg "HANDLE","STRING","FLOAT" out 154
 name "RegisterObject" arg "STRING","HANDLE","STRING","FLOAT","FLOAT" out 248
 name "UnregisterObject" arg "STRING","STRING","FLOAT" out 249

 name "SetCapture" arg "HANDLE","STRING","FLOAT" out 155
 name "ReleaseCapture" out 156
 name "StdHyperJump" arg "HANDLE","FLOAT","FLOAT","HANDLE","FLOAT" out 385

 name "Dialog" arg "STRING","STRING","STRING" ret "FLOAT" out 605
// title,vdr or class, object
 name "DialogEx" arg "STRING",["STRING","STRING","STRING"] ret "FLOAT" out 606
 name "DialogBox" arg "STRING","HANDLE" ret "FLOAT" out 608

 name "LoadMenu" arg "STRING","STRING","FLOAT" ret "FLOAT" out 246
 name "DeleteMenu" arg "STRING" ret "FLOAT" out 247
 name "CheckMenuItem" arg "STRING","FLOAT","FLOAT" ret "FLOAT" out 762
 name "EnableMenuItem" arg "STRING","FLOAT","FLOAT" ret "FLOAT" out 763

 name "GetMousePos" arg "STRING",&"FLOAT",&"FLOAT" ret "FLOAT" out 158
// title, Mich魥 Mercier,Jocelyne Mercier

 name "GetScreenWidth" ret "FLOAT" out 772
 name "GetScreenHeight" ret "FLOAT" out 773

 name "GetWorkAreaX" ret "FLOAT" out 774
 name "GetWorkAreaY" ret "FLOAT" out 775
 name "GetWorkAreaWidth" ret "FLOAT" out 776
 name "GetWorkAreaHeight" ret "FLOAT" out 777

 name "GetKeyboardLayout" ret "FLOAT" out 778

 name "SetWindowTransparent" arg "STRING", "FLOAT" ret "FLOAT" out 782
 name "SetWindowTransparent" arg "HANDLE", "FLOAT" ret "FLOAT" out 793
 name "SetWindowTransparentColor" arg "STRING", "COLORREF" ret "FLOAT" out 783
 name "SetWindowTransparentColor" arg "HANDLE", "COLORREF" ret "FLOAT" out 794

 name "SetWindowRegion" arg "STRING", "HANDLE" ret "FLOAT" out 784
 name "SetWindowRegion" arg "HANDLE", "HANDLE" ret "FLOAT" out 795


 name "GetTitleHeight" ret "FLOAT" out 785
 name "GetSmallTitleHeight" ret "FLOAT" out 786
 
 name "GetFixedFrameWidth" ret "FLOAT" out 787
 name "GetFixedFrameHeight" ret "FLOAT" out 788
 name "GetSizeFrameWidth" ret "FLOAT" out 789
 name "GetSizeFrameHeight" ret "FLOAT" out 790

 name "WindowInTaskBar" arg "HANDLE", "FLOAT" ret "FLOAT" out 791

 name "ShowCursor" arg "FLOAT" out 796
 name "SetStandartCursor" arg "HANDLE","FLOAT" out 863
 name "SetStandartCursor" arg "STRING","FLOAT" out 864
 name "LoadCursor" arg "HANDLE","STRING" out 865
 name "LoadCursor" arg "STRING","STRING" out 866


 name "ScreenShot" arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 797
 name "ScreenShot" arg "HANDLE","HANDLE" ret "HANDLE" out 842
 name "ScreenShot" arg "HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 844
 name "ScreenShot" arg "HANDLE" ret "HANDLE" out 845

 name "SetWindowOwner" arg "HANDLE","HANDLE" ret "FLOAT" out 1107
 name "SetWindowParent" arg "HANDLE","HANDLE" ret "FLOAT" out 1108
