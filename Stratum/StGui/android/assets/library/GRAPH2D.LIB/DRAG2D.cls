SB0  2      DragObjects  
 WindowName ��� ���� Main Window STRING    HSpace ���������� ����   HANDLE�   HObject �������� ������   HANDLE�   xPos ������� ����������   FLOAT    yPos ������� ����������   FLOAT    _enable! ����  ����������� ������� ������� 1 FLOAT    orgx     FLOAT�   orgy     FLOAT�   flags     FLOAT�   rez     FLOAT�   msg     FLOAT�   init     FLOAT�       �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwx�     wwwx�wwwx�     wwwx�wwwx�     wwwx�wwwx�     wwwx�wwx��w����wwwx�wwx��w����wwwx�ww������wwwx�ww��w����wwwx�wx�w�����wwwx�wx�ww������wwwx�w�w������wwx�x�w��������wwx�x��������wwx�pw��������wwx�p�������wx�w ���w�w�wwx�ww���w�����wx�ww����w���x�ww�����px�ww������x�ww�����ppx�ww������wx�ww������w�wwx�ww�������wwx�ww�������wwx�ww�����	�wwx�wwwwwwwpwwwwwx�wwwwwwwwwwwwwwx���������������� o      c  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            V@      H@      @@      @@                      @@      @@    � 
 
 ��g   BMv      v   (                    �  �               �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwx�     wwwx�wwwx�     wwwx�wwwx�     wwwx�wwwx�     wwwx�wwx��w����wwwx�wwx��w����wwwx�ww������wwwx�ww��w����wwwx�wx�w�����wwwx�wx�ww������wwwx�w�w������wwx�x�w��������wwx�x��������wwx�pw��������wwx�p�������wx�w ���w�w�wwx�ww���w�����wx�ww����w���x�ww�����px�ww������x�ww�����ppx�ww������wx�ww������w�wwx�ww�������wwx�ww�������wwx�ww�����	�wwx�wwwwwwwpwwwwwx�wwwwwwwwwwwwwwx����������������         End Of File <!>
 �STRING WindowName
HANDLE HSpace, HObject
FLOAT xPos,yPos,_enable
FLOAT local orgx,orgy,flags,rez,msg,init

if(!init && (IsWindowExist(~WindowName) || ~HSpace!=#0))
  if(~WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
  if(~HSpace)
    RegisterObject(~HSpace,#0,"",WM_ALLMOUSEMESSAGE,16)
    RegisterObject(~HSpace,#0,"",WM_SPACEDONE,16)
    init:=1
  endif
endif

if(~flags && !GetAsyncKeyState(1) && ~msg!=WM_LBUTTONUP)
  msg:=WM_LBUTTONUP
endif

if(~msg)
  if(~msg==WM_LBUTTONDOWN)
    flags:=0
    HObject :=  GetObjectFromPoint2d(~HSpace,~xPos,~yPos)
    if (~HObject)
      flags:=1
      orgx:=~xPos-GetObjectOrg2dX(HSpace,~HObject)
      orgy:=~yPos-GetObjectOrg2dY(HSpace,~HObject)
      //SetCapture(HSpace,"",0)
    else
      exit()
    endif
  endif

  if(~msg==WM_MOUSEMOVE && flags)
    rez:=SetObjectOrg2d(HSpace,HObject,xPos-orgx,yPos-orgy)
  endif

  if(~msg==WM_LBUTTONUP && flags)
    flags:=0
    //ReleaseCapture()
    HObject:=#0
  endif
  
  if(~msg==WM_SPACEDONE)
    HObject:=#0
    HSpace:=#0
    flags:=0
    init:=0
  endif
  
  msg:=0
endif [ ����� ��������� ���������� ����� ����������� �������
� ��������� ����������� ������������. @B       - y   �        S , + 5 U y   z    �        R + 5 $ y   �     o U        z           �@       0@�        z          �@       0@�        �?           �?� - +  
       �@7 + 5 q       �@ 
  
 5  
       �@6 5 �                  |    o �        �?        D         E   3 �    
        �@6   + 5 �               K 	  
       �@6   + 5 �                    
       �@6 5 
                                              
    ɐ�  �   �     
 �   