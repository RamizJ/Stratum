SB0  Џ      HGauge1  
 WindowName УВ  ЬЖьЯ Main Window STRING    HSpace$ СтыЖ­У№ЫЬ­ с­ЯЗУэтыЖЬсЬ №­ЬыЫ­ЯьыЫРЯ   HANDLEђ   _HObject     HANDLEѓ   HObj     HANDLEѓ   HString     HANDLEѓ   obj     HANDLEѓ   HBrush     HANDLEѓ   Value ЬЫЬр­ЯТЯтВЯ  №т­тВтььЯ    FLOAT    min ВУьУВЯвЧьЬт уьЯэтьУт   FLOAT    max ВЯЖыУВЯвЧьЬт уьЯэтьУт  1 FLOAT    Height     FLOATѓ   Width     FLOATѓ   v     FLOATѓ   Color ШРтЫ УьСУЖЯЫЬ­Я rgb(255,0,0) COLORREF    ret     FLOATѓ   _Value     FLOATѓ   x0     FLOATѓ   y0     FLOATѓ   y1     FLOATѓ   y2     FLOATѓ   OrgX ЖЬЬ­СУьЯЫч УуЬр­ЯТтьУ     FLOAT    OrgY ЖЬЬ­СУьЯЫч УуЬр­ЯТтьУ     FLOAT                                                                                                                                                       ­               ­               ­               ­               ­               ­               ­      ­­      ­              ­              ­       ­     ­       ­      ­             ­             ­      ­­       ­               ­               ­               ­               ­               ­               ­               ­               ­                 '        2D^   4                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\              § 
 
   Ч 
 
      ђ       @Q@     └X@#    ^@`├┌яЗ9@    Ђ   ђg@№д   └X@     ђg@п├░Х7§^@     @Q@У░Х7§^@#   @Q@     └X@Ђ   ђg@№д   └X@    ђ text      ^@     @Z@      @@      3@                    В 
 
   e           ­ 
 
   i   ш       љ    "Arial ░ыб ЧdС п­б ~~m ░ыб    С­ы 
 
   j    00%Ы 
 
   k   з                  @ ■    "                        @       @     End Of File <!>
 STRING WindowName
HANDLE HSpace
HANDLE  local  _HObject,HObj,HString,obj,HBrush
FLOAT Value,min,max
FLOAT local Height,Width,v
COLORREF Color
FLOAT local ret,_Value,x0,y0,y1,y2
FLOAT OrgX,OrgY

if (~HObj)

 if (_Value != ~Value)
   v := ~Value
   if (~Value < min); v := min; endif
   if(~Value > max); v := max; endif
   ret := SetVectorPoint2d(HSpace,HObj,1,x0+(~v-min)/(max-min) * Width,y1)
   ret := SetVectorPoint2d(HSpace,HObj,2,x0+(~v-min)/(max-min) * Width,y2)
   ret := SetString2d(HSpace,HString,String(Round(~Value,2)*100) + "%")
   _Value := Value
 endif

else
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace); else exit(); endif
 if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   _HObject := CreateObjectFromFile2D(~HSpace,AddSlash(GetClassDirectory(GetClassName("")))+GetClassName("")+".vdr", OrgX,OrgY,PFC_MOVEOBJECT)
 endif

 Width := GetObjectWidth2d(~HSpace,~_HObject) - 2
 Height := GetObjectHeight2d(~HSpace,~_HObject)
 x0 := GetObjectOrg2dx(~HSpace,~_HObject)
 y0 := GetObjectOrg2dy(~HSpace,~_HObject)

 x0 := ~x0 + 1
 y1 := ~y0 + 1
 y2 := ~y0 + ~Height - 1

 HBrush:=CreateBrush2d(~HSpace,0,0,~Color,#0,13)
 HObj := CreatePolyLine2d(~HSpace,#0,~HBrush,~x0,~y1,~x0+1,~y1,~x0+1,~y2,~x0,~y2,~x0,~y1)
 ret := ObjectToTop2d(~HSpace,~HObj)

 obj := GetObject2dByName(~HSpace,~_HObject,"text")
 ret := ObjectToTop2d(~HSpace,~obj)
 HString := GetTextString2d(~HSpace,GetTextObject2d(~HSpace,~obj))
endif
 ќ ╚ВУСТ Ьрты№тэУРЯтЫ с­ЯЗУэтыЖз■ УьСУЖЯШУ■
ЫтЖзщтсЬ ыЬыЫЬ ьУ  эУывтььЬж №т­тВтььЬж Р
уЯСЯььЬВ СУЯ№ЯуЬьт.
пЯс УуВтьтьУ  зыЫЯьЯРвУРЯтЫы  ЯРЫЬВЯЫУэтыЖУ.  ѓ   |    o Ѓ     7 5 Ђ         : 5         	 8 5 %  	              ­?        	           8              @        	           8                @)        Y@ ј z  % | m      3 {x   z    Љ        R + 5 Ќ x   ╠     o Ю 3 ъ     ╦ z  CLASSNAME Ыz  ..  јЉ 5 л   z    јж Ь z    ј| z  .vdr  |            Я@h       H        @       I 
     D      E           ­?            ­?      
         ­?                                     *@M                        ­?            ­?           т
       b      z  text  Я       b        ьЬ     =xќ  И   └   ж  
 ш   