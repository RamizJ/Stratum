SB0  "	      Ogre_Viewport   m_RenderWindow     HANDLE    m_Camera     HANDLE    m_BackgroundColour   rgb(96,96,96) COLORREF    l_BackgroundColour     COLORREF�   l_Create     FLOAT�   m_OverlaysEnabled   1 FLOAT    l_OverlaysEnabled     FLOAT�   l_Left     FLOAT�   l_Top     FLOAT�   l_Width     FLOAT�   l_Height     FLOAT�   m_Left   0 FLOAT    m_Top   0 FLOAT    m_Width   1 FLOAT    m_Height   1 FLOAT    m_ZOrder   0 FLOAT    l_Camera     HANDLE�  
 m_Viewport     HANDLE�  
 �HANDLE m_RenderWindow
HANDLE m_Camera

COLORREF m_BackgroundColour
COLORREF local l_BackgroundColour

FLOAT local l_Create

FLOAT m_OverlaysEnabled
FLOAT local l_OverlaysEnabled

FLOAT local l_Left
FLOAT local l_Top
FLOAT local l_Width
FLOAT local l_Height

FLOAT m_Left
FLOAT m_Top
FLOAT m_Width
FLOAT m_Height

FLOAT m_ZOrder

HANDLE local l_Camera

if(~!l_Create)
  if(m_RenderWindow && m_Camera)
    m_Viewport := Viewport_Create(~m_RenderWindow, ~m_Camera, ~m_ZOrder)
    
    l_OverlaysEnabled := 1
    l_Create := 1
  endif
endif

if(~m_Viewport)
  if(l_BackgroundColour != ~m_BackgroundColour)
    Viewport_SetBackgroundColour(~m_Viewport, GetRValue(~m_BackgroundColour) / 255, GetGValue(~m_BackgroundColour) / 255, GetBValue(~m_BackgroundColour) / 255)    
    l_BackgroundColour := ~m_BackgroundColour
  endif
  
  if(l_Camera != ~m_Camera)
    Viewport_SetCamera(~m_Viewport, ~m_Camera)
    l_Camera := ~m_Camera
  endif
  
  if(l_OverlaysEnabled != ~m_OverlaysEnabled)
    Viewport_SetOverlaysEnabled(~m_Viewport, ~m_OverlaysEnabled)
    l_OverlaysEnabled := ~m_OverlaysEnabled
  endif
  
  if(l_Left != ~m_Left || l_Width != ~m_Width || l_Top != ~m_Top || l_Height != ~m_Height)
    Viewport_SetDimensions(~m_Viewport, ~m_Left, ~m_Top, ~m_Width, ~m_Height)
    l_Left := ~m_Left
    l_Width := ~m_Width
    l_Top := ~m_Top
    l_Height := ~m_Height
  endif
endif   
 system.dbm �     - 5 #      U o #        *         �?         �?    o �     S 5 P     a      �o@   b      �o@   c      �o@ .        S 5 `     ,        7 5 p     0        7  	   7 ,     7 ,  
   7 , 5 �           /       	        
    �s�    
 