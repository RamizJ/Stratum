SB0  �      Ogre_RenderWindowEx   m_HSpace= ���������� ������������ ��� ����������� ���� � ����� ��������   HANDLE    l_PositionX     FLOAT�   l_PositionY     FLOAT�   l_Width     FLOAT�   l_Height     FLOAT�   l_Create     FLOAT�   m_FSAAI ����� �������������� �����������. ����������� ��������: 0, 2, 4, 6, 8 ... 0 FLOAT    m_VSyncz ����� ������������ ������������� (������� ���������� ������ ����� ������� ���������� ��������). ����������� ��������: 0, 1 0 FLOAT    m_RenderWindow     HANDLE�   m_PositionX' ���������� ����� ������� ���� �� ��� X. 0 FLOAT    m_PositionY' ���������� ����� ������� ���� �� ��� Y. 0 FLOAT    m_Width ������ ����. 640 FLOAT    m_Height ������ ����. 480 FLOAT   
 (HANDLE m_HSpace
FLOAT local l_PositionX  
FLOAT local l_PositionY
FLOAT local l_Width
FLOAT local l_Height
FLOAT local l_Create
FLOAT m_FSAA
FLOAT m_VSync
  
if(~!l_Create)
  if(~m_HSpace != #0 && Root_IsInitialised())
    m_RenderWindow := RenderWindow_CreateEx(~m_HSpace, 0, ~m_FSAA, ~m_VSync)
    Root_InitialiseAllResourceGroups()
    l_Create := 1
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
endif A   WINDOWS.DBM b     - 5 %         S + 5 %                 3         �?    o a    	 7    
 7 , 5 E    	  
  	    
       7     7 , 5 a                  At�  �  
 