SB0  �	      WriteAviFile   HVideo! ���������� ���������� �����������   HANDLE�   HSpace# ���������� ����������� ������������   HANDLE�  
 WindowName ��� ���� Main Window STRING    FileName ��� ��������� avi �����   STRING   
 filedialog ������ ��� ���������� 1 FLOAT    showpos+ ���������� ������� ������� � ������ ������� 1 FLOAT    fps ������ � ������� 15 FLOAT    count3 ������� ����� ������ ������ (��� 0 ���� ����������) 20 FLOAT    ChoseOK     FLOAT�   rez     FLOAT�   j     FLOAT�  
 delay_tick. ���������� ����� ������� ����� �������� ������   FLOAT    pos ������� �������   FLOAT�   _enable ���������� ������� 1 FLOAT�  
 start_tick" ��������� ��� ������ ������� �����   FLOAT    OrgX< ���������� �������� ������ ���� ������������� ��������������   FLOAT    OrgY< ���������� �������� ������ ���� ������������� ��������������   FLOAT    Width$ ������� ������������� �������������� 320 FLOAT    Height$ ������� ������������� �������������� 240 FLOAT   
 �HANDLE HVideo,HSpace
STRING WindowName,FileName
FLOAT  filedialog,showpos,fps,count
FLOAT local ChoseOK,rez,j

if(HVideo)
  if(ChoseOK)
  if(delay_tick)
   if(j<0)
    j:=delay_tick
    else
    j:=~j-1
    exit()
   endif
  endif
  if(showpos)
   SetStatusText(1,"Writing frame :"+string(pos))
 endif
   rez:=WriteVideoFrame2d(HVideo)
   pos:=pos+1
    if(count && (~pos>count ))
     rez:=CloseVideo(HVideo)
     if(showpos)
       SetStatusText(1,"Completed")
     endif
     HVideo:=#0
     _enable:=0
    endif


   endif
 else
  j:=~j+1
  if(j>=start_tick)
  if (WindowName != "" && HSpace==#0);  HSpace := GetWindowSpace(~WindowName); endif
  if(~HSpace)
   HVideo:=BeginWriteVideo2d(~hSpace,filename,OrgX,OrgY,Width,Height,"/fps="+string(fps))
  if(filedialog)
   ChoseOK:=VideoDialog(~HVideo)
   ChoseOK:=1
  else
   ChoseOK:=1
  endif
  j:=delay_tick
  endif
  endif
endif
 @@   m   FOLDERS.DBM �      o |   5 z   5 '  
          : 5     
 3 '  
        �?  
     5 ?        �?z  Writing frame :   � | p    � 	          �?         8 + 5 z    � 	   5 n        �?z  Completed p                    3 �  
        �?  
  
   9 5 � x  z    �        R + 5 � y  �     o �   x          z  /fps=   � | �     5 �    �         �?  3 �        �?     
    ��.    
 