SB0  �     
 VideoFrame 
  HVideo     HANDLE    HFrame     HANDLE�   HSpace     HANDLE    OldPos     FLOAT�   Pos     FLOAT    OrgX     FLOAT    OrgY     FLOAT    Width   -1 FLOAT�   Height   -1 FLOAT�   Flags   1 FLOAT   
 fHANDLE HVideo,HFrame,HSpace
FLOAT local OldPos

if(HVideo && HSpace )
  if(HFrame)
  if(~Pos!=OldPos)
  OldPos:=FrameSetPos2d(hSpace,HFrame,~Pos)
  OldPos:=~Pos
 endif
else
 HFrame:=CreateVideoFrame2d( hSpace, HVideo, OrgX,OrgY,Width,Height,Flags)
 Width:=GetObjectWidth2d(hSpace,~HFrame)
 Height:=GetObjectHeight2d(hSpace,~HFrame)
 endif
endif *  A        U o @   o !     7 5        �      3 @               	 �      H      I     ��.  �   
 