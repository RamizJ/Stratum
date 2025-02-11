SB0  H      Ogre_OverlayElement   m_OverlayElement     HANDLE�  	 l_Visible     FLOAT�   l_PositionX     FLOAT�   l_PositionY     FLOAT�   m_PositionX   0 FLOAT    m_PositionY   0 FLOAT    l_Width     FLOAT�   l_Height     FLOAT�   m_Width   1 FLOAT    m_Height   1 FLOAT   	 l_Caption     STRING�  	 m_Caption     STRING    l_MaterialName     STRING�   m_MaterialName     STRING    m_Name     STRING    m_CreateTypeb ��� ������������ ��������, ���� ����� ������ ������, �� ������� ����� ������� �� ������� �� �����.   STRING    m_Color   rgb(255,255,255 COLORREF    l_Color     COLORREF�   m_Alpha   1 FLOAT    l_Alpha     FLOAT�   m_MetricsMode   GMM_RELATIVE FLOAT�   l_MetricsMode     FLOAT�   m_HorizontalAlignment   GHA_LEFT FLOAT    m_VerticalAlignment   GVA_TOP FLOAT    l_HorizontalAlignment     FLOAT�   l_VerticalAlignment     FLOAT�  	 m_Visible   1 FLOAT�  
 �HANDLE m_OverlayElement
FLOAT local l_Visible

FLOAT local l_PositionX
FLOAT local l_PositionY
  
FLOAT m_PositionX
FLOAT m_PositionY

FLOAT local l_Width
FLOAT local l_Height
  
FLOAT m_Width
FLOAT m_Height

STRING local l_Caption
STRING m_Caption

STRING local l_MaterialName
STRING m_MaterialName

STRING m_Name
STRING m_CreateType

COLORREF m_Color
COLORREF local l_Color
FLOAT m_Alpha
FLOAT local l_Alpha

FLOAT m_MetricsMode
FLOAT local l_MetricsMode

FLOAT m_HorizontalAlignment
FLOAT m_VerticalAlignment
FLOAT local l_HorizontalAlignment
FLOAT local l_VerticalAlignment

if(~m_OverlayElement == #0)
  if(~m_Name != "" && RenderWindow_GetPrimary() != #0)
    
    if(~m_CreateType != "")
      m_OverlayElement := OverlayElement_Create(~m_CreateType, ~m_Name)
    else
      m_OverlayElement := OverlayElement_Get(~m_Name)
      
      m_Visible := OverlayElement_GetVisible(~m_OverlayElement)
      m_MetricsMode := OverlayElement_GetMetricsMode(~m_OverlayElement)
      OverlayElement_GetPosition(~m_OverlayElement, ~m_PositionX, ~m_PositionY)
      OverlayElement_GetSize(~m_OverlayElement, ~m_Width, ~m_Height)
      OverlayElement_GetCaption(~m_OverlayElement, ~m_Caption)
      OverlayElement_GetMaterialName(~m_OverlayElement, ~m_MaterialName)
      OverlayElement_GetAlignment(~m_OverlayElement, ~m_HorizontalAlignment, ~m_VerticalAlignment)
    endif    
  endif
endif

if(~m_OverlayElement)
  if(l_Visible != ~m_Visible)
    OverlayElement_SetVisible(~m_OverlayElement, ~m_Visible)
    l_Visible := ~m_Visible
  endif
  
  if(l_PositionX != ~m_PositionX || l_PositionY != ~m_PositionY)
    OverlayElement_SetPosition(~m_OverlayElement, ~m_PositionX, ~m_PositionY)
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
  endif
  
  if(l_Width != ~m_Width || l_Height != ~m_Height)
    OverlayElement_SetSize(~m_OverlayElement, ~m_Width, ~m_Height)
    l_Width  := ~m_Width
    l_Height := ~m_Height
  endif
  
  if(l_Caption != ~m_Caption)
    OverlayElement_SetCaption(~m_OverlayElement, ~m_Caption)
    l_Caption := ~m_Caption
  endif
  
  if~l_MaterialName != ~m_MaterialName)
    OverlayElement_SetMaterialName(~m_OverlayElement, ~m_MaterialName)
    l_MaterialName  := ~m_MaterialName
  endif
  
  if(l_Color != ~m_Color || l_Alpha != ~m_Alpha)
    OverlayElement_SetColour(~m_OverlayElement, GetRValue(~m_Color) / 255, GetGValue(~m_Color) / 255, GetBValue(~m_Color) / 255, ~m_Alpha)    
    l_Color := ~m_Color;
    l_Alpha := ~m_Alpha;
  endif
  
  if(l_MetricsMode != ~m_MetricsMode)
    OverlayElement_SetMetricsMode(~m_OverlayElement, ~m_MetricsMode)
    l_MetricsMode := ~m_MetricsMode
  endif
  
  if(l_HorizontalAlignment != ~m_HorizontalAlignment || l_VerticalAlignment != ~m_VerticalAlignment)
    OverlayElement_SetAlignment(~m_OverlayElement, ~m_HorizontalAlignment, ~m_VerticalAlignment)
    l_HorizontalAlignment := ~m_HorizontalAlignment
    l_VerticalAlignment := ~m_VerticalAlignment
  endif
endif
 6 ������������� ����� ��� ���������� ���������� �������.   
 system.dbm #          R 5 U y  z    � �     S + 5 U y  z    � 5 ' y  y  �   3 U y  �      �     �     � � �   � �	 �   � �   � �   � � �   o "    7 5 i      �        7     7 , 5 �        �            7    	 7 , 5 �       	 �     	   x 
 y  � 5 �    y  �y  { 
 x  y  � 5 �    y  �y  {      S     7 , 5 �      a      �o@   b      �o@   c      �o@   �            7 5      �        7     7 , 5 "       �           �p�  �  
 �   