SB0  �      osc12 '  HSpace     HANDLE    HObject     HANDLE�   HPen     HANDLE�   HGroup     HANDLE�   HObj     HANDLE�   HPen2     HANDLE�   HFont     HANDLE�   HText     HANDLE�   regen   1 FLOAT�   x0     FLOAT    y0     FLOAT    type   1 FLOAT    xofs     FLOAT    yofs     FLOAT    ret     FLOAT�   _Width     FLOAT    _Height     FLOAT    stepX     FLOAT�   bstepX     FLOAT�   stepY     FLOAT�   bstepY     FLOAT�   _enable   1 FLOAT    Width     FLOAT�   Height     FLOAT�   xv     FLOAT�   xtxt     FLOAT�   yv     FLOAT�   ytxt     FLOAT�   x     FLOAT�   e     FLOAT�   s     FLOAT�   ScaleX     FLOAT    HString     HANDLE�   HRTxt     HANDLE�   y     FLOAT�   ScaleY     FLOAT    ButtonUp     FLOAT�  
 ButtonDown     FLOAT�   pressed     FLOAT        ����������������������������������������������������������������������������������������������������������������                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwpwwwwwwwwwwwpwwpwwwwwwwwwwpwwpwwwwwwwwwwwpwwpwwwwwwwwwwwwpwwpwwpwwwwwwwwwpwwpwpwwwwwwwwwpwwpwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp                         2D^   0                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��            0@      @@     @R@      9@         0@      @@      V@      @@      V@      L@      0@      L@      0@      @@             8@      @@     �I@      3@                    � 
 
  e           � 
 
  f   ���        � 
 
 ��i   ��      �   �"Arial   ���w�� w  ��Ȯ� � 
 
 ��j    ���� 
 
 ��k   �          �           End Of File <!>
 �HANDLE HSpace,HObject
HANDLE HPen,HGroup,HObj,HPen2,HFont,HText
FLOAT regen,x0,y0,type,xofs,yofs
FLOAT local ret
FLOAT _Width,_Height,stepX,bstepX,stepY,bstepY,_enable
FLOAT Width,Height

if (~regen)
// if (~Width==0); exit(); endif
 if (HGroup)
  ret := DeleteObject2d(HSpace,HGroup)
 else
  HFont := CreateFont2D(~HSpace,"MS SANS SERIF",8,0) // + 
  HPen:=CreatePen2d(~HSpace,0,0,Rgb(255,0,0),13)
  HPen2:=CreatePen2d(~HSpace,0,0,Rgb(192,192,192),13)
 endif

 Width := ~_Width*2
 Height := ~_Height*2

 HObject := CreateLine2d(~HSpace,~HPen,HANDLE(0),~xofs,~yofs)
 ret := ObjectToTop2d(~HSpace,~HObject)
 HGroup := CreateGroup2d(~HSpace,~HObject)

 stepX := 50
 bstepX := 100

 stepY := 50
 bstepY := 100

 if (~xofs > ~x0 && ~xofs < ~x0 + ~Width -~stepX)
  xv := 1;  xtxt := ~xofs + 8
 else
  xv := 0
  xtxt := ~x0 + (~xofs > ~x0) *(~Width - 50)  + 5
 endif

 if (~yofs > ~y0+~stepY  && ~yofs < ~y0 + ~Height )
  yv := 1; ytxt := ~yofs - 18
 else
  yv := 0
  ytxt := ~y0 + (~yofs > ~y0+~stepY) *(~Height - 50)  + 25
 endif


// ������   --------------------------------------------------------------------------------------------
  x := Round(~x0 + (~xofs-~x0)%~stepX,0) - ~stepX
  e := ~x0 + ~Width + ~stepX
  while (~x < ~e)
  
    if ((~x-~xofs)%~bstepX)
     s := 3
    else 
     s := 5
     HString := CreateString2D(~HSpace,String((~x - ~xofs)/~ScaleX)+" ") 
     HText := CreateText2D(~HSpace,~HFont,~HString,Rgb(0,0,0),Rgb(255,255,255))
     HRTxt := CreateRasterText2D(~HSpace,~HText,~x-~s*2,~ytxt,0)
     ret := AddGroupItem2d(~HSpace,~HGroup,~HRTxt)
    endif
   if (~yv)
    ret := AddPoint2d(~HSpace,~HObject,-1,~x ,~yofs)
    ret := AddPoint2d(~HSpace,~HObject,-1,~x ,~yofs - ~s)
    ret := AddPoint2d(~HSpace,~HObject,-1,~x ,~yofs + ~s)
    ret := AddPoint2d(~HSpace,~HObject,-1,~x ,~yofs)
   endif

   HObj := CreatePolyLine2d(~HSpace,~HPen2,#0,~x,~y0,~x,~y0 + ~Height)
   ret := AddGroupItem2d(~HSpace,~HGroup,~HObj)
   ret := ObjectToBottom2d(~HSpace,~HObj)

   x := ~x + ~stepX
  endwhile

 ret := AddPoint2d(~HSpace,~HObject,-1,~xofs,~yofs)

  // ����  --------------------------------------------------------------------------------------------
  y := Round(~y0 + (~yofs-~y0)%~stepY,0) - ~stepY*2

  e := ~y0 + ~Height + ~stepY
  while (~y < ~e)
   y := ~y + ~stepY
   if ((~y-~yofs)%~bstepY)
    s := 3
   else 
    s := 5
    HString := CreateString2D(~HSpace,String((~yofs-~y)/~ScaleY)+" ") 
    HText := CreateText2D(~HSpace,~HFont,~HString,Rgb(0,0,0),Rgb(255,255,255))
    HRTxt := CreateRasterText2D(~HSpace,~HText,~xtxt,~y-~s*1.4,0)
    ret := AddGroupItem2d(~HSpace,~HGroup,~HRTxt)
   endif
   if (~xv)
    ret := AddPoint2d(~HSpace,~HObject,-1,~xofs,~y)
    ret := AddPoint2d(~HSpace,~HObject,-1,~xofs - ~s,~y)
    ret := AddPoint2d(~HSpace,~HObject,-1,~xofs + ~s,~y)
    ret := AddPoint2d(~HSpace,~HObject,-1,~xofs ,~y)
   endif
   HObj := CreatePolyLine2d(~HSpace,~HPen2,#0,~x0,~y,~x0+~Width,~y)
   ret := AddGroupItem2d(~HSpace,~HGroup,~HObj)
   ret := ObjectToBottom2d(~HSpace,~HObj)
  endwhile

 regen := 0

endif

if (ButtonUp || ButtonDown)
 ret := SetShowObject2d(~HSpace,~HGroup,~pressed)
 ButtonUp := 0
 ButtonDown := 0
endif
  ��������� �����. @�   �    5 �  o       C  3 q    z  MS SANS SERIF         @         h                             �o@                  �        *@9                              h@       h@       h@�        *@9            @             @                      ,       b       o          I@         Y@         I@         Y@     	 8    	       : + 5 �        �?            @   3             	    	 8          I@          @      
    8    
    : + 5 -       �?           2@   3 P            
    
    8          I@          9@    	    	               )       	             : 5 g        5 �       @  3 �       @             � z    | i                                      �       �o@      �o@      �o@� j                   @             k !       ! n    5 8            �    -              �       -              �       -              �    -               
    
    �          n       a         3 p            �    -   
    
               )           @   "  
          "   : 5 � "     "  "       5 �       @  3 "       @        "   #  � z    | i                                      �       �o@      �o@      �o@� j          "    ffffff�?           k !       ! n    5 l            �   " -              �      " -              �      " -              �   " -             	  "  	     " �          n       a  3 �            $  % , 5 �      & l            $           %    �j�  o   w   �  
 \   