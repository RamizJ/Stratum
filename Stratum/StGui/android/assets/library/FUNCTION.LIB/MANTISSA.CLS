SB0  �      mantissa   x     FLOAT� 
 � FUNCTION
FLOAT parameter x

if (abs(x) > 1)
 while (1)
 x := x/10  
 if (abs(x) < 1)
  exit()
 endif
 endwhile  
else
 while (1)
 x := x*10  
 if (abs(x) > 1)
  x := x/10  
  exit()
 endif
 endwhile  
endif

RETURN x       V      .        �?8 5 ,        �?5 *           $@ 
      .        �?: 5 (   3  3 U        �?5 U           $@ 
      .        �?8 5 S           $@ 
     3 ,    i`�  (   
 