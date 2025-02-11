SB0  �%      LGSpace '  HSpace ���������� ������������   HANDLE�   HOwnerSpace     HANDLE    HObject; ���������� ������������ �������  (��� ����������� ��������)   HANDLE�  
 WindowName ��� ���� Main Window STRING   	 ClassName ��� ������    STRING�  
 ObjectName& ��� ������� (��� ����������� ��������)   STRING    Flags ����� ��� �������� ����   STRING    Exist ���� �������/������� ���� 1 FLOAT    _enable! ����  ����������� ������� ������� 1 FLOAT    x ���������� ���� 0 FLOAT�   y ���������� ���� 0 FLOAT�   x0V ���������� ������������ ��������������� ����� ������� ����� ��� ������� ������� � ����   FLOAT�   y0V ���������� ������������ ��������������� ����� ������� ����� ��� ������� ������� � ����   FLOAT�   Width7 ������ ������� ������� ������������ ������������ � ����   FLOAT�   Height7 ������ ������� ������� ������������ ������������ � ����   FLOAT�   Scale! ������� ������������ ������������ 1 FLOAT�   Transparent" ������� ������������ ���� (0..100) 100 FLOAT    SetTransparentColor  ���������� ���� TransparentColor 0 FLOAT�   TransparentColorh ���� ����, ������� �� ����� ������������, ���� ����� ��������� ���������� � ��� �����, ��� ��� ���� ���� rgb(255,255,255) COLORREF    _WindowName     STRING�   _x     FLOAT�   _y     FLOAT�   _x0     FLOAT�   _y0     FLOAT�   _Width     FLOAT�   _Height     FLOAT�   _Scale     FLOAT�   _Exist     FLOAT�   _Transparent     FLOAT�   _ShowInTaskbar     FLOAT   ret     FLOAT�   msg     FLOAT�   CanClose     FLOAT�   _HOwnerSpace     HANDLE�   NotCloseWindow   0 FLOAT    isMainWindow: ���� =1, �� ��� �������� ����� ���� ������ ��������������� 0 FLOAT    MoveToScreenCenter ����������� ���� � ����� ������ 0 FLOAT�   MoveToWorkAreaCenter� ����������� ���� � ����� ������� ������� ������ (� ������ ������ ������������, ������������� � ��� �����, ��������, ������ �����) 0 FLOAT�   r     FLOAT�       ��������������������	������������𙙙�����������	�����������𙙙�����������	����������𙙙�����������	����������������������� ������������� �����������������������wx������������wwww��������� �w���������� ��� ��������� ���� �������� ���������x��� �������x�������������wx������������wx������� �����wwx� ������ ���wwx��  ��������wwwx����  �����wwwx������  ���wwwwx�����ww �wwwwx� ���wwww�wwwwwx��  p w�wwwwwx�w��wwww�wwwwwwx�www��w�wwwwwwx�wwwww��wwwwwwwx����������������
 �HANDLE HSpace, HOwnerSpace
HANDLE HObject
STRING WindowName,ClassName,ObjectName,Flags
FLOAT Exist,_enable,x,y,x0,y0,Width,Height,Scale,Transparent,SetTransparentColor
COLORREF TransparentColor

STRING local _WindowName
FLOAT local _x,_y,_x0,_y0,_Width,_Height,_Scale,_Exist,_Transparent,_ShowInTaskbar,ret,msg,CanClose
handle local _HOwnerSpace

if(~msg)
  if(~msg==WM_SIZE)
    Width := GetClientWidth(~WindowName)
    Height := GetClientHeight(~WindowName)
    _Width  :=~Width
    _Height :=~Height
  endif

  if(~msg==WM_MOVE)
    x := GetWindowOrgX(~WindowName)
    y := GetWindowOrgY(~WindowName)
    _y := ~y; _x := ~x
  endif
  
  if(~msg==WM_CANCLOSE)
    if(NotCloseWindow)
      CanClose:=0
    endif
  endif
  
  if(~msg==WM_SPACEDONE)
    if(~isMainWindow)
      CloseAll(); exit()
    endif
  endif
  
  msg:=0
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
    if(HSpace==#0)
      RegisterObject(~HSpace,#0,"",WM_SPACEDONE,16)
      RegisterObject(~HSpace,#0,"",WM_SIZE,16)
      RegisterObject(~HSpace,#0,"",WM_MOVE,16)
      RegisterObject(~HSpace,HSpace,"",WM_CANCLOSE,16)
    endif
    exit()
  endif
endif

if(not(~HSpace))
  if (~WindowName == "" );  exit(); endif
  if (~ClassName == "" );  ClassName := GetClassName(".."); endif
  HSpace := OpenSchemeWindow(~WindowName,~ClassName,"WS_BYSPACE|"+~Flags)
  _x:=29999;
  _y:=~_x
  ret :=  SetWindowOrg(~WindowName,~_x,~_y)
  if (~ObjectName != "");  HObject := GetObject2dByName(~HSpace,Handle(0),~ObjectName); endif
  
  if (!~Scale)
    Scale := 1
  endif
  
  ret := SetScaleSpace2d(~HSpace,~Scale)
  
  if (~HObject)
    x0 := GetObjectOrg2dx(~HSpace,~HObject)
    y0 := GetObjectOrg2dy(~HSpace,~HObject)
    Width := GetObjectWidth2d(~HSpace,~HObject)
    Height := GetObjectHeight2d(~HSpace,~HObject)
    ret := SetSpaceOrg2d(~HSpace,~x0,~y0)
  endif

  if(~x0 != 0 || ~y0 != 0)
    ret := SetSpaceOrg2d(~HSpace,~x0*~Scale,~y0*~Scale)
  else
    x0 :=  GetSpaceOrg2dx(~HSpace)
    y0 :=  GetSpaceOrg2dy(~HSpace)
  endif

  if(~Width + ~Height)
    ret := SetClientSize(~WindowName,~Width,~Height)
  else
    Width := GetClientWidth(~WindowName)
    Height := GetClientHeight(~WindowName)
  endif

  if (~Exist == 0);  
    ret := ShowWindow(~WindowName,SW_HIDE); 
  endif
  RegisterObject(~HSpace,HSpace,"",WM_SPACEDONE,16)
  RegisterObject(~HSpace,HSpace,"",WM_SIZE,16)
  RegisterObject(~HSpace,HSpace,"",WM_MOVE,16)
  RegisterObject(~HSpace,HSpace,"",WM_CANCLOSE,16)

  _Exist := ~Exist
  _x0 := ~x0; _y0 := ~y0;  
  //_x := ~x; _y := ~y
  _Width := ~Width;   _Height := ~Height
  _Scale := Scale
else
  if (_Scale != ~Scale)
    ret := SetScaleSpace2d(~HSpace,~Scale)
    x0 :=  GetSpaceOrg2dx(~HSpace)
    y0 :=  GetSpaceOrg2dy(~HSpace)
    //Width := GetClientWidth(~WindowName)
    //Height := GetClientHeight(~WindowName)
    _x0 := ~x0;   _y0 := ~y0
    //_Width := ~Width;   _Height := ~Height
    _Scale := ~Scale
  endif

  if ((~x0!=_x0)||(~y0!=_y0))
    ret :=  SetSpaceOrg2d(~HSpace,~x0*~Scale,~y0*~Scale)
    _x0 := ~x0; _y0 := ~y0
  endif

  if (_Width != ~Width || _Height != ~Height)
    ret := SetClientSize(WindowName,~Width,~Height)
    _Width := ~Width;   _Height := ~Height
  endif

  if (~MoveToScreenCenter)
    MoveToScreenCenter:=0
    x:=GetScreenWidth()/2-~Width/2
    y:=GetScreenHeight()/2-~Height/2
  endif

  if(~MoveToWorkAreaCenter)
    MoveToWorkAreaCenter:=0
    x:=GetWorkAreaX() + GetWorkAreaWidth()/2-~Width/2
    y:=GetWorkAreaY() + GetWorkAreaHeight()/2-~Height/2
  endif

  if (_x != ~x || _y != ~y)
    ret :=  SetWindowOrg(~WindowName,~x,~y)
    _y := ~y; _x := ~x
  endif

  if (_Exist != ~Exist)
    ret := BringWindowToTop(~WindowName)
    ret := ShowWindow(WindowName,SW_SHOW*~Exist+SW_HIDE*not(~Exist))
    _Exist := ~Exist
  endif

  if (_Transparent != ~Transparent)
    ret:=SetWindowTransparent(~WindowName,~Transparent)
    ret:=SetWindowTransparent(~WindowName,~Transparent)//��� ��� ��������� ������
    _Transparent := ~Transparent
  endif
  
  if (~SetTransparentColor)
    SetTransparentColor:=0
    ret:=SetWindowTransparentColor(~WindowName,~TransparentColor)
  endif
  
  //if (~_ShowInTaskbar != ~ShowInTaskbar)
  //  ret:=WindowInTaskBar(~HSpace,~ShowInTaskbar)
  //  _ShowInTaskbar := ~ShowInTaskbar
  //endif
  
  if(_HOwnerSpace!=~HOwnerSpace)
    _HOwnerSpace:=~HOwnerSpace
    r:=SetWindowOwner(HSpace,~HOwnerSpace);
  endif

  if  (ClassName != ~ClassName)
    ret :=  CloseWindow(~WindowName)
  endif
endif

 Y ����� ������� ��������� ����������� ������������
� ������������ ����� ���������� ������. @�     
 SYSTEM.DBM �    5 h          @6 5   y  �   y  �                    @6 5 < y  �  	 y  �  
  
    	           �@6 5 Q  " 5 Q                     �@6 5 a  # 5 a Q              y  z    � x  y  � + 5 y  �    y  {     o    V     U          y  �  	 y  �  
  	    
   y  �   y  �              Y              R 5         z          �@       0@�         z           @       0@�         z           @       0@�       z          �@       0@�      W 5 jy  z    � 5 $  y  z    � 5 3z  ..  �{  y  y  z  WS_BYSPACE| y  | �         �K�@      y      �   y  z    � 5 n             y  �     - 5 z       �?       X    o �     D       E       H       I         W             7            7 , 5 �             W  3 �   V     U       5 �y      �   3 �y  �   y  �              6 5 y           �         z          �@       0@�       z           @       0@�       z           @       0@�       z          �@       0@�                         3 �    7 5 �     X     V     U                  7     7 , 5 �             W              7     7 , 5 �x      �            $ 5 �          $         @           @   	         @           @   
  % 5 2          %         @            @   	 	        @            @   
    	 7    
 7 , 5 Py   	  
 �    
    	       7 5 wy  �   x         @              -   �           7 5 �y      y            5 �           y       !   S 5 �   !      S & x  y  � 5 �y  �      ��  �   �	  
 �   