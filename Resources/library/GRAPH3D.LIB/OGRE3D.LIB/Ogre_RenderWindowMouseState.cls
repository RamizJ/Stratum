SB0  �      Ogre_RenderWindowMouseState   m_RenderWindow? ���������� ���� ���������� ��� ���������� ��������� ����� ����.   HANDLE    m_CursorPositionX ���������� ������� �� ��� X.   FLOAT    m_CursorPositionY ���������� ������� �� ��� Y.   FLOAT    m_WheelPosition     FLOAT�   m_LeftButtonPressed0 ��������� ����� ������ ����. ������: 1, ����� 0.   FLOAT�   m_RightButtonPressed1 ��������� ������ ������ ����. ������: 1, ����� 0.   FLOAT�   m_MiddleButtonPressed2 ��������� ������� ������ ����. ������: 1, ����� 0.   FLOAT�  
 �HANDLE m_RenderWindow
if(~m_RenderWindow)
  RenderWindow_GetCursorPosition(~m_RenderWindow, ~m_CursorPositionX, ~m_CursorPositionY);
  m_WheelPosition := RenderWindow_GetWheelPosition(~m_RenderWindow);
  m_LeftButtonPressed := RenderWindow_GetMouseButtonPressed(~m_RenderWindow, 0);
  m_RightButtonPressed := RenderWindow_GetMouseButtonPressed(~m_RenderWindow, 1);
  m_MiddleButtonPressed := RenderWindow_GetMouseButtonPressed(~m_RenderWindow, 2);  
endif 8 ����� ��� ����������� ��������� ���� ��� ���� ����������   /      o .    � �                              �?             @     �e�  &  
 �   