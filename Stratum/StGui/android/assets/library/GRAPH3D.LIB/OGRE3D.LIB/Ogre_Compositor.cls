SB0  �      Ogre_Compositor   l_Create     FLOAT�  
 m_Viewport     HANDLE    m_CompositorName     STRING   	 l_Enabled     FLOAT�  
 m_Position   -1 FLOAT    m_Compositor     HANDLE�  	 m_Enabled     FLOAT   
  FLOAT local l_Create
HANDLE m_Viewport
STRING m_CompositorName

FLOAT local l_Enabled

if(~!l_Create && m_CompositorName != "")
  if(~m_Viewport)  
    m_Compositor := Viewport_AddCompositor(~m_Viewport, ~m_CompositorName, ~m_Position)
    Compositor_SetEnabled(~m_Compositor, ~m_Enabled)
    l_Enabled := ~m_Enabled
    l_Create := 1
  endif
endif

if(~m_Compositor)
  if(l_Enabled != ~m_Enabled)
    Compositor_SetEnabled(~m_Compositor, ~m_Enabled)
    l_Enabled := ~m_Enabled
  endif
endif 5  	 SIGNS.DBM >      - x  z    � + 5 )   o )   y    �      �           �?     o =     7 5 =     �       7|4  �   
 