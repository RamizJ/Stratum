SB0  �      DrawText 	  HSpace     HANDLE   x     FLOAT   y     FLOAT   text     STRING   Color     COLORREF   HFont     HANDLE�   HString     HANDLE�   HText     HANDLE�   HRTxt     HANDLE� 
 NFUNCTION
HANDLE parameter HSpace
FLOAT parameter x,y
STRING parameter text
COLORREF parameter Color


 HFont := CreateFont2D(HSpace,"MS SANS SERIF",8,0)
 HString := CreateString2D(HSpace,text) 
 HText := CreateText2D(HSpace,HFont,HString,Color,RgbEx(1,0,0,0))
 HRTxt := CreateRasterText2D(HSpace,HText,x,y,0)

RETURN HRTxt (     P      z  MS SANS SERIF         @         h     x  i                  �?                           � j                    k     �R�  �   
 