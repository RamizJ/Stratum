SB0  !	      Ogre_Billboard   m_BillboardSet     HANDLE    m_Color   rgb(255,255,255) COLORREF    l_Color     COLORREF�   l_Alpha     FLOAT�   l_PositionX     FLOAT�   l_PositionY     FLOAT�   l_PositionZ     FLOAT�  
 l_Rotation     FLOAT�   l_Create     FLOAT�   m_PositionX   0 FLOAT    m_PositionY   0 FLOAT    m_PositionZ   0 FLOAT    m_Billboard   #0 HANDLE�  
 m_Rotation   0 FLOAT    m_Alpha   1 FLOAT   
 �HANDLE m_BillboardSet
COLORREF m_Color
COLORREF local l_Color
FLOAT local l_Alpha
FLOAT local l_PositionX
FLOAT local l_PositionY
FLOAT local l_PositionZ 
FLOAT local l_Rotation
FLOAT local l_Create

if(~!l_Create)
  if(~m_BillBoardSet)  
    m_Billboard := BillboardSet_CreateBillboard(~m_BillboardSet, ~m_PositionX, ~m_PositionY, ~m_PositionZ)
    
    Billboard_SetRotation(~m_Billboard, ~m_Rotation)
    Billboard_SetColour(~m_Billboard, GetRValue(~m_Color) / 255, GetGValue(~m_Color) / 255, GetBValue(~m_Color) / 255, ~m_Alpha)
    
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
    l_PositionZ := ~m_PositionZ
    
    l_Rotation := ~m_Rotation
    l_Color := ~m_Color
    
    l_Create := 1
  endif
endif

if(~m_BillBoard)
  if(l_PositionX != ~m_PositionX || l_PositionY != ~m_PositionY || l_PositionZ != ~m_PositionZ)
    Billboard_SetPosition(~m_Billboard, ~m_PositionX, ~m_PositionY, ~m_PositionZ)
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
    l_PositionZ := ~m_PositionZ
  endif
  
  if(l_Rotation != ~m_Rotation)
    Billboard_SetRotation(~m_Billboard, ~m_Rotation)
    l_Rotation := ~m_Rotation
  endif
  
  if(l_Color != ~m_Color || l_Alpha != ~m_Alpha)
    Billboard_SetColour(~m_Billboard, GetRValue(~m_Color) / 255, GetGValue(~m_Color) / 255, GetBValue(~m_Color) / 255, ~m_Alpha)
    l_Color := ~m_Color
    l_Alpha := ~m_Alpha
  endif
endif &  
 system.dbm �     - 5 T    o T     	  
   7      E    a      �o@   b      �o@   c      �o@   I 	    
                      �?    o �    	 7    
 7 ,     7 , 5 �    	  
   G 	    
           7 5 �     E        S     7 , 5 �     a      �o@   b      �o@   c      �o@   I           C�3  �  
 