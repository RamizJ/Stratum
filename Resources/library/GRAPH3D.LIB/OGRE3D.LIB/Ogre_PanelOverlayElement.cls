SB0  �      Ogre_PanelOverlayElement %  m_OverlayContainer     HANDLE�  	 l_Visible     FLOAT�   l_PositionX     FLOAT�   l_PositionY     FLOAT�   m_PositionX+ ���������� ����� ������� �������� �� ��� X. 0 FLOAT    m_PositionY- ���������� ������� ������� �������� �� ��� Y. 0 FLOAT    l_Width     FLOAT�   l_Height     FLOAT�   m_Width ������ ��������. 1 FLOAT    m_Height ������ ��������. 1 FLOAT    l_MaterialName     STRING�   m_MaterialName: ��� ���������, ������������� ��� ������� ������� ��������.   STRING    m_Namek ��� ��������. ��� ������� ������ ���� ���������, ��� �������� ������� ������ ��������������� ����� �������.   STRING    m_Color ���� ��������. rgb(255,255,255 COLORREF    l_Color     COLORREF�   m_Alpha- ������������ ��������. �������� �� 0.0 �� 1.0 1 FLOAT    l_Alpha     FLOAT�   m_MetricsMode* ����� ������� ���������. ��. ������������. GMM_RELATIVE FLOAT�   l_MetricsMode     FLOAT�   m_HorizontalAlignment. �������������� ������������. ��. ������������. GHA_LEFT FLOAT    m_VerticalAlignment, ������������ ������������. ��. ������������. GVA_TOP FLOAT    l_HorizontalAlignment     FLOAT�   l_VerticalAlignment     FLOAT�   m_ParentContainerO ���������� ������������� ����������, � �������� ���� ���������� ���� ���������.   HANDLE    l_ParentContainer     HANDLE�   m_ParentOverlayL ���������� ������������� �������, � �������� ���� ���������� ���� ���������.   HANDLE    l_ParentOverlay     HANDLE�   l_TexCoordLeft     FLOAT�   l_TexCoordTop     FLOAT�   l_TexCoordRight     FLOAT�   l_TexCoordBottom     FLOAT�   m_CreateManualo ���� �������� �������� �������. ���� ����� 1, �� ������� ����� ������, ����� ����� ������� �� ������� �� �����. 0 FLOAT   	 m_Visible ���� ���������. 1 FLOAT�   m_TexCoordLeft4 ���������� ���������� ����� ������� ������ �� ��� X. 0 FLOAT    m_TexCoordTop6 ���������� ���������� ������� ������� ������ �� ��� Y. 0 FLOAT    m_TexCoordRight6 ���������� ���������� ������� ������� ������ �� ��� X. 1 FLOAT    m_TexCoordBottom5 ���������� ���������� ������ ������� ������ �� ��� Y. 1 FLOAT   
 �HANDLE m_OverlayContainer
FLOAT local l_Visible
FLOAT local l_PositionX
FLOAT local l_PositionY
FLOAT m_PositionX
FLOAT m_PositionY
FLOAT local l_Width
FLOAT local l_Height
FLOAT m_Width
FLOAT m_Height
STRING local l_MaterialName
STRING m_MaterialName
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
HANDLE m_ParentOverlay
HANDLE local l_ParentOverlay

FLOAT local l_TexCoordLeft
FLOAT local l_TexCoordTop
FLOAT local l_TexCoordRight
FLOAT local l_TexCoordBottom

if(~m_OverlayContainer == #0)
  if(~m_Name != "" && RenderWindow_GetPrimary() != #0)
    if(RenderWindow_GetViewportCount(RenderWindow_GetPrimary()))
      if(~m_CreateManual)
        m_OverlayContainer := OverlayElement_Create("Panel", ~m_Name)      
      else
        m_OverlayContainer := OverlayElement_Get(~m_Name)
        
        m_Visible := OverlayElement_GetVisible(~m_OverlayContainer)
        m_MetricsMode := OverlayElement_GetMetricsMode(~m_OverlayContainer)
        OverlayElement_GetPosition(~m_OverlayContainer, ~m_PositionX, ~m_PositionY)
        OverlayElement_GetSize(~m_OverlayContainer, ~m_Width, ~m_Height)
        OverlayElement_GetMaterialName(~m_OverlayContainer, ~m_MaterialName)
        OverlayElement_GetAlignment(~m_OverlayContainer, ~m_HorizontalAlignment, ~m_VerticalAlignment)
      endif
    endif
  endif
endif

if(~m_OverlayContainer != #0)
  if(l_Visible != ~m_Visible)
    OverlayElement_SetVisible(~m_OverlayContainer, ~m_Visible)
    l_Visible := ~m_Visible
  endif

  if(l_PositionX != ~m_PositionX || l_PositionY != ~m_PositionY)
    OverlayElement_SetPosition(~m_OverlayContainer, ~m_PositionX, ~m_PositionY)
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
  endif

  if(l_Width != ~m_Width || l_Height != ~m_Height)
    OverlayElement_SetSize(~m_OverlayContainer, ~m_Width, ~m_Height)
    l_Width  := ~m_Width
    l_Height := ~m_Height
  endif

  if(~l_MaterialName != ~m_MaterialName)
    OverlayElement_SetMaterialName(~m_OverlayContainer, ~m_MaterialName)
    l_MaterialName  := ~m_MaterialName
  endif
  
  if(l_Color != ~m_Color || l_Alpha != ~m_Alpha)
    OverlayElement_SetColour(~m_OverlayContainer, GetRValue(~m_Color) / 255, GetGValue(~m_Color) / 255, GetBValue(~m_Color) / 255, ~m_Alpha)    
    l_Color := ~m_Color;
    l_Alpha := ~m_Alpha;
  endif
  
  if(l_MetricsMode != ~m_MetricsMode)
    OverlayElement_SetMetricsMode(~m_OverlayContainer, ~m_MetricsMode)
    l_MetricsMode := ~m_MetricsMode
  endif
  
  if(l_HorizontalAlignment != ~m_HorizontalAlignment || l_VerticalAlignment != ~m_VerticalAlignment)
    OverlayElement_SetAlignment(~m_OverlayContainer, ~m_HorizontalAlignment, ~m_VerticalAlignment)
    l_HorizontalAlignment := ~m_HorizontalAlignment
    l_VerticalAlignment := ~m_VerticalAlignment
  endif
  
  if(~l_ParentContainer != ~m_ParentContainer)
    if(~l_ParentContainer != #0)
      OverlayContainer_RemoveChild(~l_ParentContainer, ~m_OverlayContainer)
    endif
    
    if(~m_ParentContainer != #0)
      OverlayContainer_AddChild(~m_ParentContainer, ~m_OverlayContainer)
    endif  
    
    l_ParentContainer := ~m_ParentContainer
  endif
   
  if(~l_ParentOverlay != ~m_ParentOverlay)
    if(~l_ParentOverlay != #0)
      Overlay_RemoveChild(~l_ParentOverlay, ~m_OverlayContainer)
    endif
    
    if(~m_ParentOverlay != #0)
      Overlay_AddChild(~m_ParentOverlay, ~m_OverlayContainer)
    endif
    
    l_ParentOverlay := ~m_ParentOverlay
  endif
  
  if(l_TexCoordLeft != ~m_TexCoordLeft || l_TexCoordTop != ~m_TexCoordTop || l_TexCoordRight != ~m_TexCoordRight || l_TexCoordBottom != ~m_TexCoordBottom)
   StringInterface_SetParameter(~m_OverlayContainer, "uv_coords", string(~m_TexCoordLeft) + " " + string(~m_TexCoordTop) + " " + string(~m_TexCoordRight) + " " + string(~m_TexCoordBottom))
   l_TexCoordLeft := ~m_TexCoordLeft
   l_TexCoordTop := ~m_TexCoordTop
   l_TexCoordRight := ~m_TexCoordRight
   l_TexCoordBottom := ~m_TexCoordBottom
  endif
endif &  
 system.dbm �          R 5 S y  z    � �     S + 5 S �5 S   5 * z  Panel y  �   3 S y  �      �      �     � � �   � �	 �   � �   � � �        S 5 �     7 5 k       �         7     7 , 5 �        �            7    	 7 , 5 �       	 �     	   y 
 y  � 5 �    y  �y  { 
     S     7 , 5 �      a      �o@   b      �o@   c      �o@   �            7 5 �      �        7     7 , 5        �            S 5 9       S 5 (     �       S 5 5     �        S 5 ^       S 5 M     �       S 5 Z     �       ! 7    " 7 ,    # 7 ,    $ 7 , 5 �   z  uv_coords  ! � z    |  " � | z    |  # � | z    |  $ � |  !    "    #    $      ӱ�  �  
 