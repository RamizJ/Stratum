SB0  9      osc10   HSpace     HANDLE    HObject     HANDLE    HGroup     HANDLE    xofs     FLOAT    yofs     FLOAT    Width     FLOAT    Height     FLOAT    ZoomIn2     FLOAT�   ZoomOut2     FLOAT�   regen     FLOAT    ScaleX     FLOAT�   ScaleY     FLOAT�   ZoomIn     FLOAT�   ZoomOut     FLOAT�   AutoZoom     FLOAT�   sregen   1 FLOAT    _enable   1 FLOAT    ret     FLOAT    w     FLOAT�   h     FLOAT�   _ScaleX     FLOAT�   _ScaleY     FLOAT�   xobj     FLOAT�   yobj     FLOAT�   kx     FLOAT�   ky     FLOAT�   r     FLOAT�   x0     FLOAT�   y0     FLOAT�  	 set_regen     FLOAT�       ����������������������������������������������������������������������������������������������������������������                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwww p  ppwpwwpwwppwwwppwwwpwwppwwwppwwwwpwwp wwwpppwwpwwpwwwppwwwwpwwpwwwppwwwwpwwpwp   wwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwpwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp                         2D^   0                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��            @@      D@B�AR@      9@         @@      D@B�A�Y@      D@B�A�Y@      P@      @@      P@      @@      D@             D@      D@      L@      3@                    � 
 
  e           � 
 
  f   ���        � 
 
 ��i   ��      �   �"Arial   ���w�� w  ��Ȯ� � 
 
 ��j    �������� 
 
 ��k   �          �      �"   End Of File <!>
 �HANDLE HSpace,HObject,HGroup
FLOAT xofs,yofs,Width,Height
FLOAT ZoomIn2,ZoomOut2
FLOAT regen,ScaleX,ScaleY,ZoomIn,ZoomOut,AutoZoom,sregen,_enable
FLOAT ret,Width,Height,w,h,_ScaleX,_ScaleY

 if (~ZoomIn)
   ScaleX := ScaleX*2
   ZoomIn := 0
 endif

 if (~ZoomOut)
   ScaleX := ScaleX/2
   ZoomOut := 0
 endif

 if (~ZoomIn2)
   ScaleY := ScaleY*2
   ZoomIn2 := 0
 endif

 if (~ZoomOut2)
   ScaleY := ScaleY/2
   ZoomOut2 := 0
 endif

 if (~AutoZoom)
 if (not(_ScaleX * _ScaleY))
  _ScaleX := ~ScaleX
  _ScaleY := ~ScaleY
 endif

  AutoZoom := 0
  w := GetObjectWidth2d(HSpace,HObject)
  h := GetObjectHeight2d(HSpace,HObject)
  if (NOT(~w && ~h)); exit(); endif
  ScaleX := ScaleX*~Width/~w
  ScaleY := ScaleY*~Height/~h
  ScaleX := round(mantissa(~ScaleX) - 0.02,1)* 10^order(~ScaleX)
  if (~ScaleX == 0); ScaleX := 1; endif
  ScaleY := round(mantissa(~ScaleY) - 0.02,1)* 10^order(~ScaleY)
  if (~ScaleY == 0); ScaleY := 1; endif

  xobj := GetObjectOrg2dx(HSpace,HObject)
  yobj := GetObjectOrg2dy(HSpace,HObject)

  kx := ~ScaleX/~_ScaleX
  ky := ~ScaleY/~_ScaleY

  w := ~w*~kx
  h := ~h*~ky

  xobj := ~xofs + ~kx*(~xobj-~xofs)
  yobj := ~yofs + ~ky*(~yobj-~yofs)

  r := SetObjectOrg2d(HSpace,HObject,~xobj,~yobj)
  r := SetObjectSize2d(HSpace,HObject,~w,~h)

  x0 := ~xobj
  y0 := ~yobj-30
      
  _ScaleX :=~ScaleX
  _ScaleY :=~ScaleY

  set_regen := 1
  exit()
 endif
 
if ((~_ScaleX != ~ScaleX) || (~_ScaleY != ~ScaleY)) // !!!!!!!!!

 if (not(~ScaleX * ~ScaleY)) 
  ScaleX := _ScaleX
  ScaleY := _ScaleY
  exit()
 endif

 if (not(_ScaleX * _ScaleY)) 
  _ScaleX := ~ScaleX
  _ScaleY := ~ScaleY
  exit()
 endif

// if (not(~_ScaleX * ~_ScaleY))
//  exit()
// endif
 
  w := GetObjectWidth2d(HSpace,HObject)
  h := GetObjectHeight2d(HSpace,HObject)

  xobj :=  GetObjectOrg2dx(HSpace,HObject)
  yobj :=  GetObjectOrg2dy(HSpace,HObject)

  kx := ~ScaleX/_ScaleX  
  ky := ~ScaleY/_ScaleY
  
  w := ~w*~kx
  h := ~h*~ky

  xobj := ~xofs + ~kx*(~xobj - ~xofs)
  yobj := ~yofs + ~ky*(~yobj - ~yofs)

  r := SetObjectOrg2d(HSpace,HObject,~xobj,~yobj)
  r := SetObjectSize2d(HSpace,HObject,~w,~h)

  _ScaleX :=~ScaleX
  _ScaleY :=~ScaleY

   set_regen := 1
endif
  ��������� �����. @�   1    5   
         @  
              5 *  
         @  
              5 ?           @                5 T           @                5 �     - 5 h  
                       H       I      + - 5 �    
        
            
 � mantissa   ���� {�G�z�?        �?)        $@ 
 � order  ����    
  
          6 5 �        �? 
   � mantissa   ���� {�G�z�?        �?)        $@  � order  ����                6 5        �?       D       E   
                                                              K           L               >@    
              �?       
 7     7 , 5 0 
    - 5 �   
            - 5 � 
              H       I       D       E   
                                                              K           L   
              �?     ej�  �   �   �  
 �   