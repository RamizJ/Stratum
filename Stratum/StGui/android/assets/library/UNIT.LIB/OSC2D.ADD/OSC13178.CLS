SB0  Ћ      osc13   HSpace     HANDLE    HObject     HANDLE    regen     FLOAT    ms     FLOAT    ShiftUp     FLOAT  	 SHiftDown     FLOAT  	 ShiftLeft     FLOAT  
 ShiftRight     FLOAT   Catch     FLOAT    xofs     FLOAT    yofs     FLOAT    _enable   1 FLOAT    ret     FLOAT    Width     FLOAT    Height     FLOAT    x0     FLOAT   y0     FLOAT   xlast     FLOAT    ylast     FLOAT    xx     FLOAT    yy     FLOAT   	 kor_regen     FLOAT  	 osc_regen     FLOAT       џџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџ                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpww  ppwwwwwwwpwwpww pwwwwwwwpwwwpw pwwwwwwwpwwwpw p wwwwwwwpwwwpw pwwwwwwwpwwppw pwwwwwwwpwwp pppwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp                         2D^   0                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        џџџџ  § 
 
   ќ 
 
 џџ            H@      H@rЧqR@      9@         H@      H@rЧqЧ]@      H@rЧqЧ]@      R@      H@      R@      H@      H@             L@      H@      P@      0@                    ь 
 
  e           э 
 
  f   РРР        № 
 
 џџi   ѓџ         Ь"Arial   ЌЊЧ0wW!Ч0 w  ЇЊШЎ0 ё 
 
 џџj    ёфтшуђ 
 
 џџk   ѓ          џ       @   End Of File <!>
 HANDLE HSpace,HObject
FLOAT regen,ms,ShiftUp,SHiftDown,ShiftLeft,ShiftRight,Catch,xofs,yofs,_enable
FLOAT ret,Width,Height,x0,y0,xlast,ylast,xx,yy

if (~Catch)

 if (~xlast-~x0 < 0)
  x0 :=  ~xlast - ~Width/2
  kor_regen := 1

 endif

 if (~xlast-~x0 > ~Width)
  x0 := ~xlast - ~Width/2
  kor_regen := 1
 endif

 if (~ylast-~y0 < 0)
  y0 := ~ylast - ~Height/2
  kor_regen := 1
 endif

 if (~ylast-~y0 > ~Height)
  y0 :=  ~ylast - ~Height/2
  kor_regen := 1
 endif

endif

 if (~ShiftUp)
  y0 := ~y0 - ~Height/2
  osc_regen := 1
  kor_regen := 1
  ShiftUp := 0
 endif

 if (~ShiftDown)
  y0 := ~y0 + ~Height/2
  osc_regen := 1
  kor_regen := 1
  ShiftDown := 0
 endif

 if (~ShiftRight)
  x0 := ~x0 + ~Width/2
  osc_regen := 1
  kor_regen := 1
  ShiftRight := 0
 endif

 if (~ShiftLeft)
  x0 := ~x0 - ~Width/2
  osc_regen := 1
  kor_regen := 1
  ShiftLeft := 0
 endif


//  x0 := ~xlast - ~Width/2
//  y0 := ~ylast - ~Height/2
//  osc_regen := 1
//  kor_regen := 1
//  Catch := 0

  быѓцхсэћщ шьшфц. @Т       5                : 5 %             @           №?         8 5 C             @           №?                : 5 d             @           №?         8 5              @           №?    5 Ј             @           №?         №?               5 Ю             @           №?         №?               5 є             @           №?         №?               5             @           №?         №?                Z|     "   5  
 M
   