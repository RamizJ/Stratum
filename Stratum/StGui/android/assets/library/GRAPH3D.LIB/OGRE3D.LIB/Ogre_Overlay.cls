SB0  
      Ogre_Overlay  	 m_Overlay. ���������� ���������� ��� ����������� �������.   HANDLE�  	 l_Visible     FLOAT�   m_Name) ��� ����������� ��� ������������ �������.   STRING    m_ZOrder Z-������� �������. 0 FLOAT    l_ZOrder     FLOAT�   m_ScaleX ������� ������� �� ��� X. 1 FLOAT    m_ScaleY ������� ������� �� ��� Y. 1 FLOAT    l_ScaleX     FLOAT�   l_ScaleY     FLOAT�  	 m_ScrollX �������� ������� �� ��� X. 0 FLOAT   	 m_ScrollY �������� ������� �� ��� Y. 0 FLOAT   	 l_ScrollX     FLOAT�  	 l_ScrollY     FLOAT�   m_Rotate$ ���� �������� ������� (� ��������).  0 FLOAT    l_Rotate     FLOAT�   m_CreateManualn ���� �������� �������� �������. ���� ����� 1, �� ������� ����� ������, ����� ����� ������� �� ������� �� ����� 0 FLOAT   	 m_Visible ���� ���������. 1 FLOAT   
 AHANDLE m_Overlay
FLOAT local l_Visible

STRING m_Name

FLOAT m_ZOrder
FLOAT local l_ZOrder

FLOAT m_ScaleX
FLOAT m_ScaleY
FLOAT local l_ScaleX
FLOAT local l_ScaleY

FLOAT m_ScrollX
FLOAT m_ScrollY
FLOAT local l_ScrollX
FLOAT local l_ScrollY

FLOAT m_Rotate
FLOAT local l_Rotate

if(~m_Overlay == #0)
  if(~m_Name != "" && RenderWindow_GetPrimary() != #0)
    if(RenderWindow_GetViewportCount(RenderWindow_GetPrimary()))
      if(~m_CreateManual)
        m_Overlay := Overlay_Create(~m_Name)
      else
        m_Overlay := Overlay_Get(~m_Name)
      endif
    endif
  endif
endif

if(~m_Overlay)
  if(l_Visible != ~m_Visible)
    Overlay_SetVisible(~m_Overlay, ~m_Visible)
    l_Visible := ~m_Visible
  endif
  
  if(l_ZOrder != ~m_ZOrder)
    Overlay_SetZOrder(~m_Overlay, ~m_ZOrder)
    l_ZOrder := ~m_ZOrder
  endif

  if(l_ScaleX != ~m_ScaleX || l_ScaleY != ~m_ScaleY)
    Overlay_SetScale(~m_Overlay, ~m_ScaleX, ~m_ScaleY)
    l_ScaleX := ~m_ScaleX
    l_ScaleY := ~m_ScaleY
  endif

  if(l_ScrollX != ~m_ScrollX || l_ScrollY != ~m_ScrollY)
    Overlay_SetScroll(~m_Overlay, ~m_ScrollX, ~m_ScrollY)
    l_ScrollX := ~m_ScrollX
    l_ScrollY := ~m_ScrollY
  endif
  
  if(l_Rotate != ~m_Rotate)
    Overlay_SetRotate(~m_Overlay, ~m_Rotate)
    l_Rotate := ~m_Rotate
  endif
endif
 1 ����� ��� ��������� ������� ��� �������� �������.   
 system.dbm �           R 5 * y  z    � �     S + 5 * �5 *   5 % y  �   3 * y  �      o �     7 5 >      �        7 5 N      �        7     7 , 5 j        �           	 7    
 7 , 5 �     	  
 � 	    
       7 5 �      �       �h�  H  
 �   