SB0  )     
 FileString   FileName	 ��� �����   STRING    Number ����� ������   FLOAT    Text ������   STRING�   ret     FLOAT�   _Number     FLOAT�   counter     FLOAT�   HStream     HANDLE�   _enable! ����  ����������� ������� �������   FLOAT   
 �STRING FileName
FLOAT Number
STRING Text
FLOAT local ret,_Number,counter
HANDLE local HStream
FLOAT _enable

if(FileName == "")
 exit()
else
 if (~HStream==#0)
   if(FileExist(~FileName))
     HStream:=CreateStream("FILE",~FileName,"READONLY")
   endif
 endif
endif

if (_Number != ~Number)
 if (~HStream)
  counter := 0
  ret := Seek(~HStream,0)
  while (~counter < ~Number)

   Text := GetLine(~HStream,255,Chr(13))
   counter := ~counter + 1
    
   if (Eof(~HStream))
    Text := Text
    exit()
   endif  

  endwhile
  text := Change(~text,Chr(10),"")
  text := Change(~text,Chr(13),"")
 endif
 _Number := ~Number
endif > ����� ��������� �� ���������� ����� ������ � �������� �������. @@   5  �   x   z    � 5    3 )        R 5 ) y   �5 ) z  FILE  y   z  READONLY  �       7 5 �   o �                       �       : 5 r         �o@       *@� � {           �?     � 5 p x  {    3 E y         $@� z    w {  y         *@� z    w {          o�    
 �   