SB0  T      AudioControl   FileName ��� ������������ �����-�����   STRING    Sound  ���������� ��������� �����-����� #0 HANDLE�   r     FLOAT   t     FLOAT�   _Repeat     FLOAT�   _Volume     FLOAT�   _Balance     FLOAT�   _Tone     FLOAT�  	 OpenSound ������� �����-���� 0 FLOAT�   Repeat~ ���� ������������ ��������������� �������: ���� 1, �� ��� ���������� ����� ������� ����������� �������������� ������� � ������ 0 FLOAT    Volume ������� ��������� 1 FLOAT    Balance ������ 0 FLOAT    Tone ����������� 1 FLOAT    Length2 ����� ������� (�����, ���������� �� ������� �����) 0 FLOAT�   Pos ������� ������� 0 FLOAT�   Play ������ �������������� ������� 0 FLOAT�   Stop ���������� ��������������� 0 FLOAT�   Reset �������� ������� � ������ 0 FLOAT�  	 ChangePosH �������� ������� �������, ������� ����� �������� �������� ���������� Pos 0 FLOAT�  
 �STRING FileName
handle Sound

float local r,t,_Repeat,_Volume,_Balance,_Tone


if(~OpenSound)
  OpenSound:=0
  if(alltrim(~FileName)=="");exit();endif
  if(!FileExist(~FileName));exit();endif
  if(~Sound!=#0)
    AudioStop(~Sound)
  endif
  Sound:=AudioOpenSound(~FileName)
  AudioSetRepeat(~Sound,~Repeat)
  AudioSetVolume(~Sound,~Volume)
  AudioSetBalance(~Sound,~Balance)
  AudioSetTone(~Sound,~Tone)
  Length:=AudioGetLength(~Sound)
  Pos:=0
endif

if(~Play)
  Play:=0
  AudioPlay(~Sound)
  t:=gettickcount()-50;
endif

if(~Stop)
  Stop:=0
  AudioStop(~Sound)
  Pos:=AudioGetPosition(~Sound)
endif

if(~Reset)
  Reset:=0
  AudioReset(~Sound)
  Pos:=0
endif

if(~_Volume!=~Volume)
  _Volume:=~Volume
  AudioSetVolume(~Sound,~Volume)
endif

if(~_Balance!=~Balance)
  _Balance:=~Balance
  AudioSetBalance(~Sound,~Balance)
endif

if(~_Tone!=~Tone)
  _Tone:=~Tone
  AudioSetTone(~Sound,~Tone)
endif

if(~_Repeat!=~Repeat)
  _Repeat:=~Repeat
  AudioSetRepeat(~Sound,~Repeat)
endif

if(GetTickCount()-~t>=50)
  if(AudioIsPlaying(~Sound))
    Pos:=AudioGetPosition(~Sound)
  endif
  t:=GetTickCount()
  exit()
endif

if(~ChangePos)
  ChangePos:=0
  AudioSetPosition(~Sound,~Pos)
endif

 ������ ������������ ��� ���������� ���������������� �����-������
(������ �������������� �������� ��. � �������, ������� AudioOpenSound).
����� ������������ �������� � ����� ��������� ������� AudioControl �
������ ����� �������� �� ��������������� ���������� �����-�����,
��� ���� ��������������� ����� ����������� ����������� � ����������.
��� ������� �����-����� �������� ���������� Repeat, Volume, Balance,Tone
����� �������� ����������.    FOLDERS.DBM �     5 L            y   � z    � 5    y   �- 5           S 5 '   $y   "     	 '   
 )    +    -  2               5 c              #s        I@     5 v              $  1    5 �              &              
 7 5 �  
      
 )    7 5 �         +    7 5 �         -   	 7 5 �  	      	 's           I@9 5 �   %5 �   1  s       5 �                0   CP�  �  
 �   