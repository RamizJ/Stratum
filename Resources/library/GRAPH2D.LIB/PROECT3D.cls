SB0  ,,      Proection3D ) 
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    HSpace window handle   HANDLE�   HObject object handle   HANDLE�   HCamera8 ���������� ���������� ������ ����������� � ������ ������   HANDLE�   HSpace3d# ���������� ����������� ������������   HANDLE�   _ObjectName     STRING�   x     FLOAT�   y     FLOAT�   Width   400 FLOAT�   Height   400 FLOAT�   zOrder     FLOAT�   Show   1 FLOAT    _Width     FLOAT�   _Height     FLOAT�   _zOrder     FLOAT�   ret     FLOAT�   msg     FLOAT�   x0     FLOAT�   y0     FLOAT�   nWidth     FLOAT   nHeight     FLOAT   xPos     FLOAT   yPos     FLOAT   state     FLOAT�   rez     FLOAT�   _X     FLOAT�   _y     FLOAT�   _enable   1 FLOAT    bkColor  
 RGB(0,0,0) COLORREF    _xPos     FLOAT�   _yPos     FLOAT�   mode     FLOAT   hpanel     HANDLE   hprimary     HANDLE   control4 ����e����� ������ ��������� �������� �� ����� ������ 2 FLOAT    focus) �������� ���������� (0 - ��� �����������)   FLOAT    Extent   1000 FLOAT    _Vpx   1000 FLOAT    _Vpy   800 FLOAT    _Vpz   500 FLOAT             	   HSpace hSpace HObject hView x x y y Width sizex mode mode hpanel hObject HSpace3d HSpace3d HCamera hCamera           _enable _enable   	        HCamera hCamera HSpace3d HSpace3d   Camera3d  camera      \@      b@  StratumClass_f6310_411  panel     @[@     @g@      �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�xp�����������x�wp����������wx�wx�����������wx�wx�     �����wx�wx�
��������wx�wx�
���� ���wx�wx�
����"   �wx�wx�
����"��wx�wx�
����"���wx�wx�
����"���wx�wx�
����"���wx�wx�     "���wx�wx���������wx�wx��
�������wx�wx���    ���wx�wx�����������wx�wx�����������wx�wx�����������wx�wx�� �������wx�xx������������x�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�wxwwwwwwwwwwwwx�wwwwwwwwwwwwwwx����������������	 K      ?  2D�   �                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ��2           �Y@     �f@      F@      F@        @       @  � 
 
 	      �  
            @[@     @g@      @@      @@                      @@      @@       panel�                L@      g@      @@      @@                      @@      @@              R@      i@      H@                  R@      i@      ^@      i@            \@      b@      @@      @@                      @@      @@       camera�               `@      c@              D@          `@      h@      `@      c@  	          R@      d@      H@      D@          R@      i@      ^@      d@  � 
 
 ��e   �        e   �        e   �        � 
 
 ��o    default.dbmo    VARPOINT.DBM         End Of File <!> �      �  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
              j@      V@      @@      @@                      @@      @@    � 
 
 ��g   BMv      v   (                    �  �               �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�xp�����������x�wp����������wx�wx�����������wx�wx�     �����wx�wx�
��������wx�wx�
���� ���wx�wx�
����"   �wx�wx�
����"��wx�wx�
����"���wx�wx�
����"���wx�wx�
����"���wx�wx�     "���wx�wx���������wx�wx��
�������wx�wx���    ���wx�wx�����������wx�wx�����������wx�wx�����������wx�wx�� �������wx�xx������������x�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�wxwwwwwwwwwwwwx�wwwwwwwwwwwwwwx����������������        �    " HSpace,*HSpace;HSpace3d,*hSpace3d  �    "                        @       @    End Of File <!>
 �STRING WindowName,ObjectName
HANDLE HSpace,HObject,HCamera,HSpace3d
STRING WindowName,ObjectName
STRING local _ObjectName
FLOAT x,y,Width,Height,zOrder,Show
FLOAT local _Width,_Height,_zOrder,ret,msg,x0,y0,nWidth,nHeight,xPos,yPos,state,rez,_X,_y
FLOAT _enable
COLORREF bkColor
FLOAT local _x,_y,_xPos,_yPos,mode
HANDLE local hpanel,hprimary
if(msg)
 switch
  case (msg == WM_SPACEDONE)
   HSpace:=#0
   HObject:=#0
   ret:=SetVarsToDefault("#7")
   ret:=SetVarsToDefault("#2")
  case (msg == WM_SIZE)
   x0 := GetSpaceOrg2dx(~HSpace)
   y0 := GetSpaceOrg2dy(~HSpace)
   ret:=SetObjectOrg2d(HSpace,hObject,~x0,~y0)
   ret := SetObjectSize2d(HSpace,HObject,nWidth,nHeight)
   Width := nWidth; Height := nHeight
   _Width := ~Width; _Height := ~Height
   ret:=SetObjectOrg2d(HSpace,hPanel,~x0+~Width-32,~y0)
  case (msg == WM_LBUTTONDOWN)
   if(control&2)
   if(control&8)
    if(GetObject3dFromPoint2d(HSpace,hObject,xPos,yPos,hprimary,_xPos))
     msg:=0
     exit()
    endif
   endif
  SetCapture(HSpace,"",0)
  state:=1
  _xPos:=xPos
  _yPos:=yPos
  else
     msg:=0
     exit()
  endif
  case ((msg == WM_LBUTTONUP) && (state))
  ReleaseCapture()
  state:=0
  case ((msg == WM_MOUSEMOVE) && (state))
  if((xPos!=_xPos)||(yPos!=_yPos))
  switch
   case (mode==0)
   if(GetAsyncKeyState(16)<0)
    ret:= TransformCamera3d(HSpace,hObject,3,-(xPos-_xPos)/100,0,0)
    else
     ret:= TransformCamera3d(HSpace,hObject,1,-(xPos-_xPos)/100,0,0)
     ret:= TransformCamera3d(HSpace,hObject,2,(yPos-_yPos)/100,0,0)
    endif
   case (mode==1)
    ret:=TransformCamera3d(HSpace,hObject,40,_xPos-xPos,_yPos-yPos,0)
   case (mode==2)
     ret:=(_xPos-xPos);
     if (~ret>0.0)
      ret:=1.0+~ret/100.0
     else
      ret:=1.0/(1.0+abs(~ret/100.0))
     endif
    ret:= TransformCamera3d(HSpace,hObject,10,~ret,0,0)
  endswitch
   _xPos:=xPos
   _yPos:=yPos
  endif
 endswitch
 msg:=0
 exit()
endif

 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif

 if (~ObjectName != "" &&  _ObjectName != ~ObjectName)
  HObject := GetObject2dByName(~HSpace, #0,~ObjectName)
  if (~HObject)
   HCamera := GetActiveCamera3d(~HSpace,~HObject)
   HSpace3d:= GetSpace3d(~HSpace,~HObject)

   if(control&4)
   registerobject(~HSpace,~HObject,"",WM_SIZE,272)
     x0 := GetSpaceOrg2dx(~HSpace)
     y0 := GetSpaceOrg2dy(~HSpace)
     ret:=SetObjectOrg2d(~HSpace,~hObject,~x0,~y0)
     ret:=SetObjectSize2d(~HSpace,~HObject,GetClientWidth(GetWindowName(~HSpace)),GetClientHeight(GetWindowName(~HSpace)))
   endif
   x := GetObjectOrg2dx(~HSpace, ~HObject)
   y := GetObjectOrg2dy(~HSpace, ~HObject)
   Width := GetObjectWidth2d(~HSpace, ~HObject)
   Height := GetObjectHeight2d(~HSpace, ~HObject)
   _Width := ~Width; _Height := ~Height
   _X := ~X ; _Y := ~Y
   zOrder := GetZOrder2d(~HSpace,~HObject)
   _ObjectName := ~ObjectName

   if(control&2)
     SendMessage("panel","")
     registerobject(~HSpace,~HObject,"",WM_ALLMOUSEMESSAGE,273)
     registerobject(~HSpace,~HObject,"",WM_SPACEDONE,273)
   endif

   exit()
   endif
endif

if (~HObject)
  if (_X != ~X || _Y != ~Y)
   ret := SetObjectOrg2d(HSpace,HObject,~X,~Y)
   ret:=SetObjectOrg2d(HSpace,hPanel,~X+~Width-32,~y)
   _X := ~X ; _Y := ~Y
  endif

  if (_Width != ~Width || _Height != ~Height)
   ret := SetObjectSize2d(HSpace,HObject,~Width,~Height)
   ret:=SetObjectOrg2d(HSpace,hPanel,~X+~Width-32,~y)
   _Width := ~Width; _Height := ~Height
  endif

  if (_zOrder !=~zOrder)
   ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  endif

 zOrder := GetZOrder2d(~HSpace,~HObject);   _zOrder :=~zOrder
 ret:= SetShowObject2d(~HSpace,~HObject,~Show)
else
 if(~hSpace)
// �������� ����� ��������
 if(not(~hSpace3d))
   hSpace3d:=CreateSpace3d(~hSpace)
 endif

if (not(~hCamera))
 if(~focus); rez:=2;endif
 hCamera:=CreateCamera3dEx(~hSpace3d,"Auto",4+2048+32,~rez,~bkColor,~Extent,~Focus, 0,0,1,~_Vpx,~_Vpy,~_Vpz )
 rez:=SetObjectName2d(~hSpace,~hCamera,"DefaultCamera")
endif
// ~�������� ����� ��������
   HObject:=Create3dView2d(~hSpace3d,~hCamera,X,Y,Width,Height)
   zOrder := GetZOrder2d(~HSpace,~HObject)
   _ObjectName := ~ObjectName

   if(control&4)
   registerobject(~HSpace,~HObject,"",WM_SIZE,272)
     x0 := GetSpaceOrg2dx(~HSpace)
     y0 := GetSpaceOrg2dy(~HSpace)
     ret:=SetObjectOrg2d(~HSpace,~hObject,~x0,~y0)
     Width:=GetClientWidth(GetWindowName(~HSpace))
     Height:=GetClientHeight(GetWindowName(~HSpace))
     ret:=SetObjectSize2d(~HSpace,~HObject,~Width,~Height)
   endif
    _Width := ~Width; _Height := ~Height
    _X := ~X ; _Y := ~Y
    if(control&2);SendMessage("panel","");endif
    registerobject(~HSpace,~HObject,"",WM_ALLMOUSEMESSAGE,273)
    endif


endif
 � ���������� control 
2 - ����� ������ ��������� ������ ��������� : ����� ���������� ������.
4 - �������� ������������ � �������� ����
8 - ��������� �������� ������ ��� ������� �� ������ ������� PB   �    5         �@6 5 (               z  #7  �  z  #2  �  3          @6 5 x   V    U          K          L     	    
  	    
      !    	         @@   K  3         �@6 5 �  #         @i 5 �  #         @i 5 �         �" � (o �                z             �        �?          3 �              3         �@6   + 5 � �            3          �@6   + 5     7     7 , 5             6 5 �       0@�          : 5 @           @     q        Y@                   �  3 �           �?     q        Y@                   �              @            Y@                   �  3           �?6 5 �           D@                   �  3            @6 5                   8 5 �       �?         Y@    3 �       �?       �?         Y@ .                $@                    �                       x   z    �        R + 5 ,y   �   y  z    � x  y  � + 5        y  �     o     �      �   #        @i 5 �    z           @       q@�   V    U          K        � �   � � L      D      E      H 	     I 
  	    
               `  y  {   #         @i 5 z  panel z    X      z           �@      q@�     z          �@      q@�     o �    7     7 , 5 >        K     !    	         @@   K             	 7    
 7 , 5 r     	  
 L     !    	         @@   K   	    
       7 5 �      c      `            l  3 �  o �  W 5 �  �    W 5  $ 5 �        @    z  Auto         @       �@        @@      %  $                          �? &  '  ( �	 CreateCamera3dEx        z  DefaultCamera {           	  
 �      `  y  {   #        @i 5 j    z           @       q@�   V    U          K    � �  	   � �  
      	  
 L   	    
            #         @i 5 �z  panel z    X      z           �@      q@�    �N�       	 [   0  
 "   