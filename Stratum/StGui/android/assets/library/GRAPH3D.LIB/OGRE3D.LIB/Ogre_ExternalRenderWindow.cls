SB0  �	      Ogre_ExternalRenderWindow   l_PositionX     FLOAT�   l_PositionY     FLOAT�   l_Width     FLOAT�   l_Height     FLOAT�   l_Fullscreen     FLOAT�   l_Create     FLOAT�   m_FSAAI ����� �������������� �����������. ����������� ��������: 0, 2, 4, 6, 8 ...   FLOAT    m_VSyncz ����� ������������ ������������� (������� ���������� ������ ����� ������� ���������� ��������). ����������� ��������: 0, 1   FLOAT    m_Title ��������� ����. Ogre window STRING    m_Width ������ ����. 640 FLOAT    m_Height ������ ����. 480 FLOAT    m_Fullscreen ���� �������������� ������. 0 FLOAT    m_Popup" ���� ���������� ����� ������ ����. 0 FLOAT    m_RenderWindow& ���������� ���������� ���� ����������.   HANDLE�   m_PositionX' ���������� ����� ������� ���� �� ��� X. 0 FLOAT    m_PositionY' ���������� ����� ������� ���� �� ��� Y. 0 FLOAT   
 �FLOAT local l_PositionX  
FLOAT local l_PositionY
FLOAT local l_Width
FLOAT local l_Height
FLOAT local l_Fullscreen
FLOAT local l_Create
FLOAT m_FSAA
FLOAT m_VSync

STRING m_Title
   
if(~!l_Create)
  if(Root_IsInitialised())
    m_RenderWindow := RenderWindow_Create2(#0, ~m_Title, ~m_Width, ~m_Height, ~m_Fullscreen, ~m_FSAA, ~m_VSync, ~m_Popup)
    l_Fullscreen := ~m_Fullscreen;    
    l_Width := ~m_Width
    l_Height := ~m_Height
      
    if(~m_RenderWindow == RenderWindow_GetPrimary())
      Root_InitialiseAllResourceGroups()
    endif
    
    l_Create := 1  
  endif
endif

if(~m_RenderWindow) 
  if(l_PositionX != ~m_PositionX || l_PositionY != ~m_PositionY)
    RenderWindow_SetPosition(~m_RenderWindow, ~m_PositionX, ~m_PositionY)
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
  endif
  
  if(~l_Width != ~m_Width || ~l_Height != ~m_Height)
    RenderWindow_SetSize(~m_RenderWindow, ~m_Width, ~m_Height)
    l_Width := ~m_Width
    l_Height := ~m_Height
  endif
  
  if(l_Fullscreen != ~m_Fullscreen)
    l_Fullscreen := RenderWindow_ToggleFullscreen(~m_RenderWindow)
  endif
endif � ������� ���� ����������. ���������� �� ����������� � ����� ���, ���
������������ ������� � ������������� �����. ����� ��������� 
����������� ����� ��������� �����. B   WINDOWS.DBM      - 5 6 5 6      y   	  
                	    
     �R 5 /        �?    o ~      7     7 , 5 V                   	 7    
 7 , 5 r    	  
  	    
       7 5 ~   4     �}�  |  
    