SB0  n      ColorBar  
 WindowName ��� ���� Main Window STRING    HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� ���������� �������   HANDLE�   OrgX
 ����������   FLOAT    OrgY
 ����������   FLOAT    SizeX ������ �� X 100 FLOAT    SizeY ������ �� Y 32 FLOAT    levels ���������� �������� ������ 20 FLOAT    Mode@ 1-������������/ 2- �������������� / 0 ������������ �������������   FLOAT�   Color1 ������ ���� rgb(0,0,255) COLORREF    Color2 ������ ���� rgb(255,0,0) COLORREF    HBrush     HANDLE�   a     FLOAT�   _a     FLOAT�   i     FLOAT�   w     FLOAT�   r     FLOAT�   _Color1     COLORREF�   _Color2     COLORREF�   Show ����  ��������� 1 FLOAT    _enable! ����, ����������� ������� ������� 1 FLOAT        �����������������wwwwwwwwwwwwwwx�wtwwwwwwwwwtwwx�wwxwwwwwwwxwwwx�wt����������wwx�wwxwwwwwwwxwwwx�wtwwwwwwwwwtwwx�wwwwwwwwwwwwwwx�wp          ttx�wp������D@w�x�wp��������@w�x�wp������D@x�x�wp��������@w�x�wp������D@w�x�wp��������@w�x�wp������D@w�x�wp��������@w�x�wp������D@w�x�wp��������@x�x�wp������D@w�x�wp��������@w�x�tt          ttx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx���������������� o      c  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��           Pr@     Pc@      @@      @@                      @@      @@    � 
 
 ��g   BMv      v   (                    �  �               �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �����������������wwwwwwwwwwwwwwx�wtwwwwwwwwwtwwx�wwxwwwwwwwxwwwx�wt����������wwx�wwxwwwwwwwxwwwx�wtwwwwwwwwwtwwx�wwwwwwwwwwwwwwx�wp          ttx�wp������D@w�x�wp��������@w�x�wp������D@x�x�wp��������@w�x�wp������D@w�x�wp��������@w�x�wp������D@w�x�wp��������@w�x�wp������D@w�x�wp��������@x�x�wp������D@w�x�wp��������@w�x�tt          ttx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx����������������         End Of File <!>
 �STRING WindowName
HANDLE HSpace,HObject
FLOAT OrgX,OrgY,SizeX,SizeY,levels,Mode
COLORREF Color1,Color2
HANDLE local HBrush
FLOAT local a,_a,i,w,r
COLORREF local _Color1,_Color2
FLOAT Show
FLOAT _enable

if(hObject)
if ((_Color1!=~Color1)||(_Color2!=~Color2))
 i:=0
 while(~i<levels)
  _a:=~i/(levels)
  a:=1-~_a
  HBrush:=GetBrushObject2d(~HSpace,GetGroupItem2d(HSpace,HObject,~i))
  _a:=SetBrushColor2d(~HSpace,~HBrush,RGB(GetRValue(~color1)*~a+GetRValue(~color2)*~_a,GetGValue(~color1)*~a+GetGValue(~color2)*~_a,GetBValue(~color1)*~a+GetBValue(~color2)*~_a))
  i:=~i+1
  endwhile
   _Color1:=~Color1
   _Color2:=~Color2

endif
  r := SetShowObject2d(~HSpace,~HObject,~Show)
else
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace)
  if(not(mode))
   if(SizeY>SizeX);mode:=1;else mode:=2;endif
  endif
   if(~mode==2)
   w:=SizeX/levels
   else
    w:=SizeY/levels
   endif
   while(~i<levels)

 _a:=~i/(levels)
  a:=1-~_a
  
  HBrush:=CreateBrush2d(~HSpace,0,0,RGB(GetRValue(color1)*~a+GetRValue(color2)*~_a,GetGValue(color1)*~a+GetGValue(color2)*~_a,GetBValue(color1)*~a+GetBValue(color2)*~_a),#0,13)
  
if(~mode==2)
  HBrush := CreatePolyLine2d(~HSpace,#0,~hBrush, OrgX+sizex*~_a,OrgY, OrgX+sizex*~_a+~w,OrgY, OrgX+sizex*~_a+~w,OrgY +SizeY, OrgX+sizex*~_a,OrgY+SizeY)
else
  HBrush := CreatePolyLine2d(~HSpace,#0,~hBrush, OrgX,OrgY+sizey*~_a, OrgX+sizex,OrgY+sizey*~_a, OrgX+sizex,OrgY+sizey*~_a+~w, OrgX,OrgY+sizey*~_a+~w)
endif
  if(not(~HObject))
  HObject:= CreateGroup2d(~HSpace)
  endif
  _a:=AddGroupItem2d(~HSpace,~HObject,~HBrush)
   i:=~i+1
   endwhile
   _Color1:=~Color1
   _Color2:=~Color2
  else 
  exit()
 endif
endif
 @B      FOLDERS2.DBM �    o �    	 S    
 S , 5                 : 5 w               �?             r/       	 a    
 a     	 b    
 b     	 c    
 c    � �           �?   3   	    
         l  3 �x   z    �        R + 5 � x   �     o �  - 5 �     8 5 �        �?  3 �         @            @6 5 �        3 �            : 5 �              �?                          	 a    
 a     	 b    
 b     	 c    
 c    �             *@M            @6 5 z                                                             �   3 �                                                             �     W 5 �  o          n           �?   3 �  	    
   3 �     �x:  .   6   �  
 