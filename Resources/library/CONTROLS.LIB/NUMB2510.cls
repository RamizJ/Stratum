SB0  �     
 NumberView  
 WindowName ��� ���� Main Window STRING    HSpace$ ���������� ������������ ������������   HANDLE�   Value ������������ ����������   FLOAT    size ����� ������ ����� ������� 4 FLOAT    OrgX ���������� �����������    FLOAT    OrgY ���������� �����������    FLOAT    _Value     FLOAT�   fwKeys     FLOAT   _HObject     HANDLE�   HString     HANDLE�   obj     HANDLE�   msg     FLOAT�       ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww wwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwppwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww               ���������������� .      "  2D^                           Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��     text     �G@      R@      L@      0@                              �C@     �Q@      R@      4@                      R@      4@    � 
 
 ��g   BMF      v   (   H            �  �  �               �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �������������������������������������wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�                                  ������������������������������������� 
 
 ��i   ��      �   �"Arial   ,�8g�8 g  '�л79 � 
 
 ��j    0� 
 
 ��k   �          �      � �    "                        @       @     End Of File <!>
 �STRING WindowName
HANDLE HSpace
FLOAT Value,size
FLOAT OrgX,OrgY
FLOAT local _Value,fwKeys
HANDLE local _HObject,HString,obj

if (~HString)
if (_Value != ~Value)
  _Value := SetString2d(~HSpace,~HString,String(Round(~Value,~size)))
  _Value := ~Value
endif
 if (msg==WM_SPACEDONE)
 _HObject:=GetHObject()
 HSpace:=#0
 HString:=#0
 msg:=0
 exit()
 endif

else
 if (~WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
 if (~HSpace); else exit(); endif
 if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   _HObject := CreateObjectFromFile2D(~HSpace,AddSlash(GetClassDirectory(GetClassName("")))+"numberv.vdr",~OrgX,~OrgY,PFC_MOVEOBJECT)
 endif
 obj := GetObject2dByName(~HSpace,~_HObject,"text")
 HString := GetTextString2d(~HSpace,GetTextObject2d(~HSpace,~obj))
 registerobject(~HSpace,#0,"",WM_SPACEDONE,256)
 _Value:=SetString2d(~HSpace,~HString,String(Round(~Value,~size)))
  _Value := ~Value
endif

 F ����� ������������ ����������� ��������
�������� �������� ����������.  �   �    	 o =     7 5     	     ) � m              �@6 5 ; �                	              3 � y   z    �        R + 5 Q y   �     o W 3 X     � z  CLASSNAME �z  ..  �� 5 �   z    �� � z  numberv.vdr |            �@h       z  text  �  
      
 �� 	        z          �@       p@�    	     ) � m         Ox�  �   �   �  
 �   