SB0  �      Ogre_TextAreaOverlayElement !  m_OverlayElement/ ���������� ����������� ��� ���������� ��������.   HANDLE�  	 l_Visible     FLOAT�   l_PositionX     FLOAT�   l_PositionY     FLOAT�   m_PositionX+ ���������� ����� ������� �������� �� ��� X. 0 FLOAT    m_PositionY- ���������� ������� ������� �������� �� ��� Y. 0 FLOAT    l_Width     FLOAT�   l_Height     FLOAT�   m_Width ������ ��������. 1 FLOAT    m_Height ������ ��������. 1 FLOAT   	 l_Caption     STRING�  	 m_Caption ������������ �����.   STRING    m_Namek ��� ��������. ��� ������� ������ ���� ���������, ��� �������� ������� ������ ��������������� ����� �������.   STRING    m_Color ���� ��������. rgb(255,255,255 COLORREF    l_Color     COLORREF�   m_Alpha- ������������ ��������. �������� �� 0.0 �� 1.0 1 FLOAT    l_Alpha     FLOAT�   m_MetricsMode* ����� ������� ���������. ��. ������������. GMM_RELATIVE FLOAT�   l_MetricsMode     FLOAT�   m_HorizontalAlignment. �������������� ������������. ��. ������������. GHA_LEFT FLOAT    m_VerticalAlignment, ������������ ������������. ��. ������������. GVA_TOP FLOAT    l_HorizontalAlignment     FLOAT�   l_VerticalAlignment     FLOAT�   m_ParentContainerO ���������� ������������� ����������, � �������� ���� ���������� ���� ���������.   HANDLE    l_ParentContainer     HANDLE�  
 m_FontName9 ��� �������-������, ������������� ��� ����������� ������.   STRING   
 l_FontName     STRING�   m_CharHeight ������ ������ (������ �������).   FLOAT    l_CharHeight     FLOAT�   m_TextAlignment3 ������������ ������. ��������: left, center, right. left STRING    l_TextAlignment     STRING�   m_CreateManualo ���� �������� �������� �������. ���� ����� 1, �� ������� ����� ������, ����� ����� ������� �� ������� �� �����. 0 FLOAT   	 m_Visible ���� ���������. 1 FLOAT�  
 9HANDLE m_OverlayElement
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
STRING m_Name
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
HANDLE m_ParentContainer
HANDLE local l_ParentContainer
STRING m_FontName
STRING local l_FontName
FLOAT m_CharHeight
FLOAT local l_CharHeight
STRING m_TextAlignment
STRING local l_TextAlignment

if(~m_OverlayElement == #0)
  if(~m_Name != "" && RenderWindow_GetPrimary() != #0)
    if(RenderWindow_GetViewportCount(RenderWindow_GetPrimary()))
      if(~m_CreateManual)
        m_OverlayElement := OverlayElement_Create("TextArea", ~m_Name)
      else
        m_OverlayElement := OverlayElement_Get(~m_Name)
        
        m_Visible := OverlayElement_GetVisible(~m_OverlayElement)
        m_MetricsMode := OverlayElement_GetMetricsMode(~m_OverlayElement)
        OverlayElement_GetPosition(~m_OverlayElement, ~m_PositionX, ~m_PositionY)
        OverlayElement_GetSize(~m_OverlayElement, ~m_Width, ~m_Height)
        OverlayElement_GetCaption(~m_OverlayElement, ~m_Caption)
        OverlayElement_GetAlignment(~m_OverlayElement, ~m_HorizontalAlignment, ~m_VerticalAlignment)
        
        StringInterface_GetParameter(~m_OverlayElement, "font_name", ~m_FontName) 
//        StringInterface_GetParameter(~m_OverlayElement, "char_height", string(~m_CharHeight))  
        StringInterface_GetParameter(~m_OverlayElement, "alignment", ~m_TextAlignment)
      endif
    endif
  endif
endif

if(~m_OverlayElement != #0)
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
  
  if(~l_ParentContainer != ~m_ParentContainer)
    if(~l_ParentContainer != #0)
      OverlayContainer_RemoveChild(~l_ParentContainer, ~m_OverlayElement)
    endif
    
    if(~m_ParentContainer != #0)
      OverlayContainer_AddChild(~m_ParentContainer, ~m_OverlayElement)
    endif  
    
    l_ParentContainer := ~m_ParentContainer
  endif
  
  if(l_FontName != ~m_FontName)
    StringInterface_SetParameter(~m_OverlayElement, "font_name", ~m_FontName)
    l_FontName := ~m_FontName
  endif
  
  if(l_CharHeight != ~m_CharHeight)
    StringInterface_SetParameter(~m_OverlayElement, "char_height", string(~m_CharHeight))
    l_CharHeight := ~m_CharHeight
  endif
  
  if(l_TextAlignment != ~m_TextAlignment)
    StringInterface_SetParameter(~m_OverlayElement, "alignment", ~m_TextAlignment)
    l_TextAlignment := ~m_TextAlignment
  endif
endif
   
 system.dbm �          R 5 m y  z    � �     S + 5 m �5 m   5 , z  TextArea  y  �   3 m y  �      �      �     � � �   � �	 �   � �   � � �   z  font_name y     z  alignment y          S 5 �     7 5 �       �         7     7 , 5 �        �            7    	 7 , 5 �       	 �     	   x 
 y  � 5 �    y  �y  { 
     S     7 , 5      a      �o@   b      �o@   c      �o@   �            7 5      �        7     7 , 5 .       �            S 5 S       S 5 B     �       S 5 O     �    x  y  � 5 j   z  font_name y  y  {      7 5 �   z  char_height   �     x  y  � 5 �   z  alignment y  y  {     6]�  _  
 