SB0        ChangeModel 
 	 ClassName ��� ������, ��� �������� ������   STRING    Number ����� ������   FLOAT    Type ���: 0 - �������, 1 - ������   FLOAT    Text ����� ���������� ������   STRING    tmp     STRING�   ret     FLOAT�   counter     FLOAT�   HStream     HANDLE�   HStream2     HANDLE�   _enable
 ����������   FLOAT�       ������������������������������������������������������    ����������� ���� ��������������������� �����������������������������������������L��LL����������D��������  ����L��LLL@���������DD������D������DDDL���L������DDD@�����������D�DH�����������D��@�����������O�����������U�O���������U�U�����������UUU�������_�����UU�����UX����]]]P����������������������  ��]]�����������������U����������P��_����������������� �������� ����������������������� ���� ����������    ���������������������� V      J  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��   �) \\Solar\d\Vlad archive\ICONS\CONVERT2.ICO     @r@     Pc@      @@      @@                      @@      @@    � 
 
 ��h   BMv      v   (                                         �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ���                       ����          ������        �    ���       � ���� ��     ���������     ������������  ������������� ������������L �LL����������D� ������  ����L� LLL@    ����� DD��   D����� DDDL  L����� DDD@�  ������ D�DH�   ����� D��@��   ���� O������   ��U O������   U�U �������  UUU  ������P  ���UU ����UX����]]]P ������������ �������  ��]]  ��������������  �U����������P  P  ����������       ��������        �������          ����                                      BM�       >   (                 �                      ��� ��������� ��� ?�  �  �  �   �   �   �   �� �� �� �� �� �� � � � � �����  �           x  �  ?�  �����������    �    End Of File <!>
 �STRING ClassName
FLOAT Number,Type
STRING Text
STRING local tmp
FLOAT local ret,counter
HANDLE local HStream,HStream2
FLOAT _enable
     
HStream := CreateStream("FILE","tmp.txt","CREATE")
HStream2 := CreateStream("FILE","tmp1.txt","CREATE")
ret := GetModelText(~ClassName,~HStream)

if (not(~HStream)); exit(); endif

  counter := 0
  ret := Seek(~HStream,0)
  while (not(Eof(~HStream)))

   tmp := GetLine(~HStream,255,Chr(13))
   tmp := Change(~tmp,Chr(10),"")
   tmp := Change(~tmp,Chr(13),"")

   
   if (~counter == ~Number)
    if (Type)
     ret := Writeln(~HStream2,~Text)   
    else
     ret := Writeln(~HStream2,~tmp)  
     ret := Writeln(~HStream2,~Text)   
    endif
   else
    ret := Writeln(~HStream2,~tmp)  
   endif
 
   counter := ~counter + 1
  endwhile


ret := SetModelText(~ClassName,~HStream2)
ret := CloseStream(~HStream)
ret := CloseStream(~HStream2)

ret := FileDelete("tmp.txt")
ret := FileDelete("tmp2.txt")

_enable := 0 @B   �   z  FILE  z  tmp.txt z  CREATE  �   z  FILE  z  tmp1.txt  z  CREATE  �   y     �    W 5 6                         �     � - 5 �         �o@       *@� � {  y         $@� z    w {  y         *@� z    w {      6 5 �   5 �   y  �   3 �   y  �     y  �   3 �   y  �            �?   3 G y     �    �     �   z  tmp.txt �  z  tmp2.txt  �            	    t��  j   r   �  
 