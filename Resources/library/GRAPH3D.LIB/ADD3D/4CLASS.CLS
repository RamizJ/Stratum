SB0  �     	 _rotate3d 
  hSpace3d     HANDLE    hObject     HANDLE    hBase ����    HANDLE    oldangle     FLOAT�   rez     FLOAT�   matrix M������ ��������   FLOAT�   AxisX     FLOAT    AxisY     FLOAT    AxisZ     FLOAT    angle     FLOAT        ���������������������������������������������������������������������������������������������������x������������������wwxx����������w�wh��x��x�����wwww�wwx�������xw�xx��xw��x����w�w�����xw������xw��x��xw��x���wwx��w������x����ww��x��xx��x��www���w����wwx����xw�wx��hw��x��wxw�xw����x�������h��x����x�����wwx�x��x��x�������xw�xx��xw�����wwwx��������������hx��x��xx������w����w�����������xx��h��gw�������w�x�w����������xw�xh�wxx�����������wwx������������xw�������������������������������x������������������������ W
      K
  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            H@      D@     �D@      >@                     �D@      >@    � 
 
 ��g   BM^	      6  (   )            (                       �  �   �� �   � � ��  ��� ��� �ʦ   @   `   �   �   �   �  @   @   @@  @`  @�  @�  @�  @�  `   `   `@  ``  `�  `�  `�  `�  �   �   �@  �`  ��  ��  ��  ��  �   �   �@  �`  ��  ��  ��  ��  �   �   �@  �`  ��  ��  ��  ��  �   �   �@  �`  ��  �  ��  �� @   @   @ @ @ ` @ � @ � @ � @ � @   @   @ @ @ ` @ � @ � @ � @ � @@  @@  @@@ @@` @@� @@� @@� @@� @`  @`  @`@ @`` @`� @`� @`� @`� @�  @�  @�@ @�` @�� @�� @�� @�� @�  @�  @�@ @�` @�� @�� @�� @�� @�  @�  @�@ @�` @�� @�� @�� @�� @�  @�  @�@ @�` @�� @� @�� @�� �   �   � @ � ` � � � � � � � � �   �   � @ � ` � � � � � � � � �@  �@  �@@ �@` �@� �@� �@� �@� �`  �`  �`@ �`` �`� �`� �`� �`� ��  ��  ��@ ��` ��� ��� ��� ��� ��  ��  ��@ ��` ��� ��� ��� ��� ��  ��  ��@ ��` ��� ��� ��� ��� ��  ��  ��@ ��` ��� �� ��� ��� �   �   � @ � ` � � � � � � � � �   �   � @ � ` � � � � � � � � �@  �@  �@@ �@` �@� �@� �@� �@� �`  �`  �`@ �`` �`� �`� �`� �`� ��  ��  ��@ ��` ��� ��� ��� ��� ��  ��  ��@ ��` ��� ��� ��� ��� ��  ��  ��@ ��` ��� ��� ��� ��� ���   �  �   �� �   � � ��  ��� �	�	�	�	�	�	�	�	�	�	�	�	�	�	�	�	�	�	�	�	�   ���������������	�������   �	�	�	�	���l�l��������				�	�	�	�	�	�   ����������	�	���	�	�������   �	�	�	��l��	�	�	�	��������	�	�	���   ���������\\\lkl\�\��������	�����   �	�l���l�[�[�[kY�s�[�[�[�l	�����\�\��   �����\l\l\l[ksks���ll\l\l\�	�����l��   �	�����[�\�[�[ksk�||�k�\�[�\�l��l	��   ���	��\\\�\lks���s���t�\\\�\l������   ���l�[�[�\�[�|�	[�t	�|�{�\�[�����[����   ���l\l\l\lk����ks�����������l\l��   ��	�[�\�[�s�|	�	�	k�t	�	�|�{��\�\�[��	�   ���\\\�\ks������ks�����|���\\\�\\��   ��l�[�[�[�|�|�	lkk�s��	��Y�k�k�[�[�[�	�   ��\l\l�k|���|�kk|�������k��ks��					�   ��l�[�\�Y��|�Y�|��||�s��[��	�	�	�	�	�   ���\\\�\k�	�kk������s�����ks�����   ����[�[�k�tks||�|�Y�||�{kst	�	�	�	�	�   ��ll\l[kkk������s�������s	����   �		[�\lY�|�	�k�t	��l�||���	�	�	�	�   ���\�k������	�k{�������s��������   �	�	[�||����Y���m���Yk|t����	�	�	�   ��	\lt���|	�k|�lk���lll�����   �	�	�		l�l�||�l�Y�t�kkk|�[���	�	�	�	�   ����		�\���|�tk{k[s|���\�������   �	�	�	�	�	����t|�{kk�|l���	�	�	�	�	�	�   ������	��s��	�������   �	�	�	�	�	�	�	�	�|�	�	�	�	�	�	�	�	�	�	�   ��������������������            End Of File <!>
 HANDLE hSpace3d,hObject,hBase
FLOAT local oldangle,rez

if(~matrix==0  && ed(~hSpace3d))
 matrix:=mcreate(0,1,2,1,3,1)
 rez:=Mput(~matrix,2,1,~AxisX,1)
 rez:=Mput(~matrix,2,2,~AxisY,1)
 rez:=Mput(~matrix,2,3,~AxisZ,1)
endif

if( ~matrix && ed( ~hObject  && ~hbase) )
if(~angle != oldangle)
 rez:=PushCrdSystem3d(~hSpace3d)
  rez:=SelectLocalCrd3d(~hSpace3d,~hbase)
  rez:=RotateObject3d(~hSpace3d,~hObject,~matrix,~angle-oldangle)
  rez:=PopCrdSystem3d(~hSpace3d)
  oldangle:=~angle
endif
endif

     �              6    T + 5 q                 �?        @       �?       @       �?<             @       �?         �?@             @        @         �?@             @       @         �?@         U T + 5 �  	   7 5 �    �       �          	    �     �   	      	�(        s  
 