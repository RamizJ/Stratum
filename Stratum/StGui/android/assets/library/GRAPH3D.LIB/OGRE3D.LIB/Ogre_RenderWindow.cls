SB0  �      Ogre_RenderWindow   m_HSpace     HANDLE    l_PositionX     FLOAT�   l_PositionY     FLOAT�   l_Width     FLOAT�   l_Height     FLOAT�   m_FSAA   0 FLOAT    m_VSync   0 FLOAT    l_Create     FLOAT�   m_RenderWindow     HANDLE�   m_PositionX   0 FLOAT    m_PositionY   0 FLOAT    m_Width   640 FLOAT    m_Height   480 FLOAT   
 MHANDLE m_HSpace
FLOAT local l_PositionX  
FLOAT local l_PositionY
FLOAT local l_Width
FLOAT local l_Height

FLOAT m_FSAA
FLOAT m_VSync

FLOAT local l_Create

if(~!l_Create)
  if(Root_IsInitialised())
    if(~m_HSpace)
      m_RenderWindow := RenderWindow_Create(~m_HSpace, ~m_FSAA, ~m_VSync)
      
      Root_InitialiseAllResourceGroups()     
      
      l_Create := 1  
    endif
  endif
endif

if(~m_RenderWindow)
  if(l_PositionX != ~m_PositionX || l_PositionY != ~m_PositionY)
    RenderWindow_SetPosition(~m_RenderWindow, ~m_PositionX, ~m_PositionY)
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
  endif
  
  if(l_Width != ~m_Width || l_Height != ~m_Height)
    RenderWindow_SetSize(~m_RenderWindow, ~m_Width, ~m_Height)
    l_Width := ~m_Width
    l_Height := ~m_Height
  endif
endif A   WINDOWS.DBM Z     - 5  5     o         �         �?    o Y    	 7    
 7 , 5 =    	  
  	    
       7     7 , 5 Y                  At�  }  
 