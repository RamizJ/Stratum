SB0        VideoStream   FileName     STRING    HVideo     HANDLE�   _disable     FLOAT�  
 u STRING FileName
HANDLE HVideo
if(length(FileName))
HVideo:= OpenVideo(FileName,16)
_disable:=ed(~HVideo)
endif
 @    I     x   � 5  x          0@�    T      os�  d   
 