SB0  G      gst01   N     FLOAT    hmax     FLOAT�   min     FLOAT    max     FLOAT    HSpace     HANDLE    bars     HANDLE�   items     HANDLE�   next     FLOAT�   x0     FLOAT    y0     FLOAT    xc     FLOAT�   yc     FLOAT�   txt     STRING    Height     FLOAT�   Width     FLOAT�   h   20 FLOAT�   HBrush     HANDLE�   HPen     HANDLE�   HPen2     HANDLE�   HObj     HANDLE�   i     FLOAT�   step     FLOAT�   yy     FLOAT�   xx     FLOAT�   ret     FLOAT�   _enable   1 FLOAT�       ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�w wwpw  pp�pwwwppwppww�wwwwpwppwwp�wwwwwpwppwww�www  w ppw  �wwwwwww pwwp�wwwwpwppwwp�pwwwpwppww�w w pwppp�wwwwwwwwwwpwww�wwwwwwwwwwpwww�wwwwwwwwwwpwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww� Y      M  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            \@      P@     �I@      3@                    � 
 
 ��i                    Arial Cyr �3� �� �zm ��    ��� 
 
 ��j    create� 
 
 ��k   �       ���            End Of File <!>
 �FLOAT N,hmax,min,max
HANDLE HSpace,bars,items
FLOAT next,x0,y0,xc,yc
STRING txt

if (not(~HSpace)); exit(); endif
if (~N < 2); exit(); endif

 Height := GetClientHeight(GetWindowName(~HSpace))
 Width := GetClientWidth(GetWindowName(~HSpace))
 h := max(trunc(~Height/14),13)
 xc := ~h
 yc := GetSpaceOrg2dY(~HSpace) + ~Height - ~h
 hmax := ~Height - ~h - 1

 HBrush:=CreateBrush2d(~HSpace,0,0,Rgb(255,0,0),Handle(0),13)
 HPen:=CreatePen2d(~HSpace,0,0,Rgb(255,0,0),13)
 HPen2:=CreatePen2d(~HSpace,0,0,Rgb(192,192,192),13)

 HObj := CreatePolyLine2d(~HSpace,~HPen2,Handle(0),~xc,~yc,~xc,~yc-~hmax)
 HObj := CreatePolyLine2d(~HSpace,~HPen2,Handle(0),~xc,~yc,~xc+~Width-2*~h,~yc)

// HObj := DrawText(~HSpace,1,0,~xtxt,Rgb(0,0,0))
 HObj := DrawText(~HSpace,~Width-Length(~txt)*6-2,0,~txt,Rgb(0,0,0))
 HObj := DrawText(~HSpace,0,~yc,String(~min),Rgb(0,0,0))
 HObj := DrawText(~HSpace,~Width-Length(String(~max))*6-~h,~yc,String(~max),Rgb(0,0,0))
 
 i := 0
 step := (~Height-~h)/~10
 while(~i < 10)
  yy := ~yc-~step*~i
  HObj := CreatePolyLine2d(~HSpace,~HPen2,Handle(0),~xc,~yy,~xc-4,~yy)

  i := ~i + 1
 endwhile

 bars := New()
 items := New()
 i := 0
 step := (~width-2*~h)/~N
// bwidth := max(min(~step,~sk),1)
 while (~i < ~N)
  xx := ~xc+~step*~i
  HObj := CreatePolyLine2d(~HSpace,~HPen2,Handle(0),~xx,~yc,~xx,~yc+4)
//  HObj := CreatePolyLine2d(~HSpace,Handle(0),~HBrush,~xx,~yc,~xx+~bwidth,~yc,~xx+~bwidth,~yc-~h,~xx,~yc-~h)
  HObj := CreatePolyLine2d(~HSpace,~HPen,#0,~xx,~yc,~xx,~yc-~h)
  ret := vInsert(~bars,"HANDLE") 
  vSet(~bars,~i,"",~Hobj)
  ret := vInsert(~items,"FLOAT")
  i := ~i + 1
 endwhile

 next := 1
 _enable := 0 
 @B       W 5               @: 5      � �     � �            ,@ P        *@&      
   U                    �?                             �o@                  �                  *@M                            �o@                  �        *@9                             h@       h@       h@�        *@9                 
    
      �                  
    
            @      �       y  �        @          @          y                             � � DrawText   ������������                 �                            � � DrawText   ������������        � �        @         �                            � � DrawText   ������������                         $@            $@: 5 �                         
    
        @   �            �?   3 ��  �                       @               : 5 p 
                                      @ �                       �     z  HANDLE  �      z      �  z  FLOAT �           �?   3        �?                �f�  3   ;   �  
 