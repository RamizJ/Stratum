SB0  �      GraphicSpace   HSpace ���������� ������������   HANDLE�  
 WindowName ��� ���� Main Window STRING    FileName@ ��� VDR �����, ������������ � ������������ ���� ��� ��� ��������   STRING    Flags ����� ��� �������� ����   STRING    _WindowName     STRING�   x ���������� ����   FLOAT�   y ���������� ����   FLOAT�   x0V ���������� ������������ ��������������� ����� ������� ����� ��� ������� ������� � ����   FLOAT�   y0V ���������� ������������ ��������������� ����� ������� ����� ��� ������� ������� � ����   FLOAT�   Width7 ������ ������� ������� ������������ ������������ � ����   FLOAT�   Height7 ������ ������� ������� ������������ ������������ � ����   FLOAT�   Scale! ������� ������������ ������������   FLOAT�   _x     FLOAT�   _y     FLOAT�   _x0     FLOAT�   _y0     FLOAT�   _Width     FLOAT�   _Height     FLOAT�   _Scale     FLOAT�   _Exist     FLOAT�   Exist! ����  ����������� ���������� ���� 1 FLOAT    _enable! ����  ����������� ������� ������� 1 FLOAT    ret     FLOAT�   msg     FLOAT�   CanClose     FLOAT       ������������������������������	����������������������������	 ���������������������������	 ��������������������������	 �	�������������  ������������	  	�����������   ���������������	����������� {p��������������������������{{{ ������������ � �����������   ���x��������  ���x�������� ����wx������������wx�������������wwx� ����������wwx��  ��������wwwx����  �����wwwx������  ���wwwwx�����ww �wwwwx� ���wwww�wwwwwx��  p w�wwwwwx�w��wwww�wwwwwwx�www��w�wwwwwwx�wwwww��wwwwwwwx����������������
 uHANDLE HSpace
STRING WindowName,FileName,Flags
STRING local _WindowName
FLOAT x,y,x0,y0,Width,Height,Scale
FLOAT local _x,_y,_x0,_y0,_Width,_Height,_Scale,_Exist
FLOAT Exist,_enable
FLOAT local ret,msg,CanClose
if (msg==WM_SIZE)
Width   :=GetWindowWidth(WindowName)
Height  :=GetWindowHeight(WindowName)
_Width  :=~Width
_Height :=~Height
msg:=0
exit()
endif

if (msg==WM_SPACEDONE)
 HSpace := #0
 msg:=0
 exit()
endif

if (~WindowName != "" &&  _WindowName != ~WindowName)
  HSpace := GetWindowSpace(~WindowName)
  _WindowName := ~WindowName
  if (~HSpace)
   x0 := GetSpaceOrg2dx(~HSpace)
   y0 := GetSpaceOrg2dy(~HSpace)
   _x0 := ~x0; _y0 := ~y0
   x := GetWindowOrgX(~WindowName)
   y := GetWindowOrgY(~WindowName)
    _x := ~x; _y := ~y
   Width := GetClientWidth(~WindowName)
   Height := GetClientHeight(~WindowName)
   _Width := ~Width;   _Height := ~Height
   Scale := GetScaleSpace2d(~HSpace)
   _Scale := ~Scale
   exit()
 endif
endif

if (not(~HSpace))
 if (~WindowName == "" );  exit(); endif

 HSpace:= LoadSpaceWindow(~WindowName,~FileName,~Flags)

 if (~Exist == 0);  ret := ShowWindow(~WindowName,SW_HIDE); endif
 RegisterObject(~HSpace,HSpace,"",WM_SPACEDONE,280)
  RegisterObject(~HSpace,HSpace,"",WM_SIZE,280)
 if (~Scale)
  ret := SetScaleSpace2d(~HSpace,~Scale)   
 else
  Scale := 1
 endif  

 if (~x + ~y)
  ret :=  SetWindowOrg(WindowName,~x,~y)
 endif

 if (~x0 != 0 || ~y0 != 0)
   ret := SetSpaceOrg2d(~HSpace,~x0*~Scale,~y0*~Scale)
 else
   x0 :=  GetSpaceOrg2dx(~HSpace)
   y0 :=  GetSpaceOrg2dy(~HSpace)
 endif
 
 if (~Height + ~Width)
  ret := SetClientSize(~WindowName,~Width,~Height)
  else
  Width := GetClientWidth(~WindowName)
  Height := GetClientHeight(~WindowName)
 endif
 
 _Width := ~Width;   _Height := ~Height;    _Exist := ~Exist
 _Scale := Scale;   _x0 := ~x0; _y0 := ~y0;  _x := ~x; _y := ~y
else

  if (_Scale != ~Scale)
   ret := SetScaleSpace2d(~HSpace,~Scale)
   x0 :=  GetSpaceOrg2dx(~HSpace)
   y0 :=  GetSpaceOrg2dy(~HSpace)
   Width := GetClientWidth(~WindowName)
   Height := GetClientHeight(~WindowName)
   _x0 := ~x0;   _y0 := ~y0
   _Width := ~Width;   _Height := ~Height
   _Scale := ~Scale
  endif

   x0 := ~x0 - _x0 + GetSpaceOrg2dx(~HSpace)
   y0 := ~y0 - _y0 + GetSpaceOrg2dy(~HSpace)
   ret :=  SetSpaceOrg2d(~HSpace,~x0*~Scale,~y0*~Scale)
   _x0 := ~x0; _y0 := ~y0

  if (_Width != ~Width || _Height != ~Height)
   ret := SetClientSize(WindowName,~Width,~Height)
   _Width := ~Width;   _Height := ~Height
  endif

  if (_x != ~x || _y != ~y)
   ret :=  SetWindowOrg(WindowName,~x,~y)
    _y := ~y; _x := ~x
  endif

  if (_Exist != ~Exist)
   ret := BringWindowToTop(~WindowName)
   ret := ShowWindow(WindowName,SW_SHOW*~Exist+SW_HIDE*not(~Exist))
   _Exist := ~Exist
  endif

  if  (FileName != ~FileName);  ret :=  CloseWindow(~WindowName);  endif
endif 0 ���� ���������� ������������ ���������� �������. @�     
 SYSTEM.DBM H           @6 5 $ x  �  	 x  �  
  	    
                        �@6 5 ;                      y  z    � x  y  � + 5 � y  �    y  {     o �    V     U          y  �   y  �           y  �  	 y  �  
  	    
      Y           W 5 uy  z    � 5 �   y  y  y  �               6 5 � y           �         z          �@      �q@�       z           @      �q@�   5 �      X  3 �        �?       5 	x      �              7            7 , 5 -             W  3 7   V     U   
  	  5 Iy   	  
 �   3 Sy  �  	 y  �  
  	    
                           3 G    7 5 �     X     V     U  y  �  	 y  �  
          	    
               V           U                W             	 7    
 7 , 5 �x   	  
 �    	    
       7     7 , 5 x      �               7 5 ;y  �   x         @              -   �       x  y  � 5 Gy  �      �v|  a   i  
 �   