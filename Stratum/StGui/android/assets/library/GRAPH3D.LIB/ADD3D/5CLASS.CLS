SB0        StratumClass_f6310_411   _enable   0 FLOAT    hSpace     HANDLE    hView     HANDLE    _h     HANDLE�   hActive     HANDLE�   HSpace3d     HANDLE    hCamera     HANDLE    msg     FLOAT�   xPos     FLOAT    yPos     FLOAT    s     STRING�   mode     FLOAT�   bx     FLOAT    by     FLOAT    bcx     FLOAT    bcy     FLOAT    rez     FLOAT�   state     FLOAT�   h     HANDLE�   x     FLOAT    sizex     FLOAT    y     FLOAT    hObject     HANDLE�  
 SFLOAT _enable
HANDLE hSpace,hView ,_h,hActive,HSpace3d,hCamera
if(msg)
switch
 case (msg==WM_LBUTTONDOWN)
 _h:=GetObjectFromPoint2d(~hSpace,xPos,yPos)
 _h:=GetLastPrimary2d()
 s:=GetObjectName2d(~hSpace,~_h)
 switch
  case (~s=='r')
   mode:=0
  case (~s=='p')
   mode:=1
  case (~s=='s')
   mode:=2
  case (substr(~s,0,1)=="_")
   rez:=GetBitmapSrcRect2d(~hSpace,~_h,~bx,~by,~bcx,~bcy)
   rez:=SetBitmapSrcRect2d(~hSpace,~_h,32,~by,~bcx,~bcy)
   state:=2
   h:=~_h
   SetCapture(~hSpace,"",0)
   msg:=0
   exit()
 endswitch

  if(~_h!=hActive)
   rez:=GetBitmapSrcRect2d(~hSpace,hActive,bx,by,bcx,bcy)
   rez:=SetBitmapSrcRect2d(~hSpace,hActive,0,by,bcx,bcy)
   rez:=GetBitmapSrcRect2d(~hSpace,~_h,bx,by,bcx,bcy)
   rez:=SetBitmapSrcRect2d(~hSpace,~_h,32,by,bcx,bcy)
   hActive:=~_h
  endif

 case ((msg==WM_MOUSEMOVE) && (state))
 _h:=GetObjectFromPoint2d(hSpace,xPos,yPos)
 _h:=GetLastPrimary2d()
// rez:=GetBitmapSrcRect(~hSpace,~h,bx,by,bcx,bcy)
 if(~_h==h)
   state:=2
   rez:=SetBitmapSrcRect2d(hSpace,h,32,by,bcx,bcy)
 else
   state:=1
   rez:=SetBitmapSrcRect2d(hSpace,h,0,by,bcx,bcy)
 endif

 case ((msg==WM_LBUTTONUP) && (state))
 rez:=SetBitmapSrcRect2d(~hSpace,h,0,by,bcx,bcy)
 ReleaseCapture()
 if(state==2)
  s:=GetObjectName2d(~hSpace,~h)
  switch
   case (~s=="_c")
   rez:=System(22,float(HSpace3d),Float(hCamera))
   case (~s=="_h")
    SetVar("..\camera","update",1)
    SendMessage("..\camera","")
    SetVar("..\camera","update",0)
  endswitch
 endif
 state:=0
endswitch
msg:=0
exit()
endif
if(~hSpace && ~hView)  

 hObject:=CreateObjectFromFile2d(~hSpace,GetClassDirectory(GetClassName(""))+"\cambtn.vdr",~x+~sizex-32,~y,PFC_MOVEOBJECT)
 hActive:=GetObject2dByName(~hSpace,~HObject,"r")
 registerobject(~HSpace,~HObject,"",WM_ALLMOUSEMESSAGE,273)
 SendMessage("..\camera","")
endif 5 ��������� ����� ��� �������� ����������� ������������ @@   5    5 �        �@6 5 �      	 |  }      z{ 
 y 
 z  r � 5 2            3 � y 
 z  p � 5 C        �?  3 � y 
 z  s � 5 T         @  3 � y 
                 �? z  _ � 5 �     � � � � �             @@      e          @        z             �                  S 5 �     � � � � �                     e      � � � � �             @@      e      3 �         �@6   + 5 K     	 |  }      R 5 0        @             @@      e  3 I       �?                     e  3 �        �@6   + 5 �                   e  �           @6 5 �    z{ 
 y 
 z  _c  � 5 �       6@      �    3 �y 
 z  _h  � 5 �z  ..\camera z  update         �?�z  ..\camera z    X  z  ..\camera z  update           �                            U o 4  z    �� z  \cambtn.vdr |             @@          �@h       z  r �       z           �@      q@� z  ..\camera z    X     �x�    
 g	   