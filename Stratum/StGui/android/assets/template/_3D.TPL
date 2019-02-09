
 name "GetObjectBase3dM"   arg "HANDLE","HANDLE", "FLOAT" ret "FLOAT" out 649

 name "SetObjectBase3d"   arg "HANDLE","HANDLE", "FLOAT","FLOAT","FLOAT" ret "FLOAT" out 650
 name "GetObjectBase3d"   arg "HANDLE","HANDLE",&"FLOAT",&"FLOAT",&"FLOAT" ret "FLOAT" out 651
 name "GetObjectDimension3d"    arg "HANDLE","HANDLE", "FLOAT" ret "FLOAT" out 653
 name "SetObjectMatrix3d"   arg "HANDLE","HANDLE", "FLOAT" ret "FLOAT" out 645
 name "GetObjectMatrix3d"   arg "HANDLE","HANDLE", "FLOAT" ret "FLOAT" out 646

 name "RotateObject3d"          arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "FLOAT" out 655
 name "TransformObject3d"       arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 654
 name "RotateObjectPoints3d"    arg "HANDLE","HANDLE","FLOAT","FLOAT" ret "FLOAT" out 648
 name "TransformObjectPoints3d" arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 647

 name "SweepAndExtrude3d" arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT" ,"FLOAT","FLOAT","FLOAT","FLOAT","COLORREF","FLOAT" ret "HANDLE" out 297
 // HSP3D,HOBJECT
 // матрица с точками
 // матрица с вектором поворота+ угол поворота  +  матрица с векотром перемещения+ число шагов

 name "CreateSurface3d" arg "HANDLE","FLOAT","FLOAT","COLORREF" ret "HANDLE" out 668

 name "GetSpace3d" arg"HANDLE","HANDLE" ret "HANDLE" out 652

 name "Create3dView" arg"HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "HANDLE" out 662
 //  hSpace3d, hCamera,x,y,sizex,sizey //ret HObject

  name "CreateDefCamera3d" arg "HANDLE","FLOAT" ret "HANDLE" out 657
  name "SetCameraParam3d"    arg "HANDLE","HANDLE","FLOAT","FLOAT","STRING" ret "FLOAT" out 0
  name "GetCameraParam3d"    arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 0
  name "GetCameraParam3dstr" arg "HANDLE","HANDLE","FLOAT" ret "STRING" out 0

  name "SwitchToCamera3d" arg"HANDLE","HANDLE","HANDLE" ret "FLOAT" out 658

  name "_CameraProc3d" arg "STRING","HANDLE","HANDLE","FLOAT" ret "FLOAT" out 499
  name "GetActiveCamera" arg"HANDLE","HANDLE" ret "HANDLE" out 659


  name "CreateSpace3d" arg "HANDLE" ret "HANDLE" out 660
  name "DeleteSpace3d" arg "HANDLE" ret "FLOAT" out 661

  name "GetObjectPoints3d" arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 663
  name "SetObjectPoints3d" arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 664
  name "SetObjectColor3d"  arg "HANDLE","HANDLE","COLORREF" ret "FLOAT" out 656
  name "GetObjectColor3d"  arg "HANDLE","HANDLE" ret "COLORREF" out 665


  name "CreateObject3d"     arg "HANDLE" ret "HANDLE"   out 638
  name "DelPrimitive3d"     arg "HANDLE","HANDLE","FLOAT" ret "FLOAT"    out 639
  name "GetNumPrimitives3d" arg "HANDLE","HANDLE" ret "FLOAT"    out 640
  name "AddPrimitive3d"     arg "HANDLE","HANDLE","FLOAT","COLORREF",["FLOAT"] ret "FLOAT"  out 641
  name "DelPoint3d"         arg "HANDLE","HANDLE","FLOAT" ret "FLOAT" out 642
  name "GetNumPoints3d"     arg "HANDLE","HANDLE" ret "FLOAT"  out 643
  name "AddPoint3d"         arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT" ret "FLOAT"   out 644



//****** All Graphics

  name "FitToCamera3d" arg "HANDLE","HANDLE","HANDLE","FLOAT" ret "FLOAT" out 669

  name "SetCameraPoint3d" arg "HANDLE","HANDLE","FLOAT","FLOAT","FLOAT","FLOAT" ret "FLOAT" out 670
  // 0: Org
  // 1: Dir
  // 2: Up

// Для работы с системам координат

  name "PushCrdSystem3d"  arg "HANDLE" ret "FLOAT" out 671
  name "PopCrdSystem3d"   arg "HANDLE" ret "FLOAT" out 672
  name "SelectLocalCrd3d" arg "HANDLE","HANDLE" ret "FLOAT" out 673
  name "SelectWorldCrd3d" arg "HANDLE" ret "FLOAT" out 674
  name "SelectViewCrd3d"  arg "HANDLE","HANDLE" ret "FLOAT" out 675
