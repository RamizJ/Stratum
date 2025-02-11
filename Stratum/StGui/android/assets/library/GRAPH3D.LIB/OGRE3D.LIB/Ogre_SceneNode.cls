SB0  U      Ogre_SceneNode   m_Scene     HANDLE    m_ParentNode     HANDLE    m_Name     STRING    l_Create     FLOAT�   l_PositionX     FLOAT�   l_PositionY     FLOAT�   l_PositionZ     FLOAT�   l_RotationX     FLOAT�   l_RotationY     FLOAT�   l_RotationZ     FLOAT�   l_ScaleX     FLOAT�   l_ScaleY     FLOAT�   l_ScaleZ     FLOAT�   m_AutoTrackTarget     HANDLE    l_AutoTrackTarget     HANDLE�   l_ParentNode     HANDLE�   m_SceneNode     HANDLE�   m_PositionX     FLOAT    m_PositionY     FLOAT    m_PositionZ     FLOAT    m_RotationX     FLOAT    m_RotationY     FLOAT    m_RotationZ     FLOAT    m_ScaleX   1 FLOAT    m_ScaleY   1 FLOAT    m_ScaleZ   1 FLOAT   
 �HANDLE m_Scene
HANDLE m_ParentNode
STRING m_Name

FLOAT local l_Create

FLOAT local l_PositionX
FLOAT local l_PositionY
FLOAT local l_PositionZ

FLOAT local l_RotationX
FLOAT local l_RotationY
FLOAT local l_RotationZ

FLOAT local l_ScaleX
FLOAT local l_ScaleY
FLOAT local l_ScaleZ

HANDLE m_AutoTrackTarget
HANDLE local l_AutoTrackTarget

HANDLE local l_ParentNode

if(~!l_Create)
  if(~m_Scene)
    m_SceneNode := SceneNode_Create(~m_Scene, ~m_Name)
    Node_GetName(~m_SceneNode, ~m_Name)
    l_Create := 1
  endif
endif

if(~m_SceneNode)
  if(l_ParentNode != ~m_ParentNode)
    Node_SetParent(~m_SceneNode, ~m_ParentNode)
    l_ParentNode := ~m_ParentNode
  endif

  if(l_PositionX != ~m_PositionX || l_PositionY != ~m_PositionY || l_PositionZ != ~m_PositionZ)
    Node_SetPosition(~m_SceneNode, ~m_PositionX, ~m_PositionY, ~m_PositionZ)
    l_PositionX := ~m_PositionX
    l_PositionY := ~m_PositionY
    l_PositionZ := ~m_PositionZ
  endif
  
  if(l_RotationX != ~m_RotationX || l_RotationY != ~m_RotationY || l_RotationZ != ~m_RotationZ)
    Node_SetRotationEulerYXZ(~m_SceneNode, ~m_RotationX, ~m_RotationY, ~m_RotationZ)
    l_RotationX := ~m_RotationX
    l_RotationY := ~m_RotationY
    l_RotationZ := ~m_RotationZ
  endif
  
  if(l_ScaleX != ~m_ScaleX || l_ScaleY != ~m_ScaleY || l_ScaleZ != ~m_ScaleZ)
    Node_SetScale(~m_SceneNode, ~m_ScaleX, ~m_ScaleY, ~m_ScaleZ)
    l_ScaleX := ~m_ScaleX
    l_ScaleY := ~m_ScaleY
    l_ScaleZ := ~m_ScaleZ
  endif
  
  if(l_AutoTrackTarget != ~m_AutoTrackTarget)
    SceneNode_SetAutoTracking(~m_SceneNode, ~m_AutoTrackTarget, 0.0, 0.0, -1.0)
    l_AutoTrackTarget := ~m_AutoTrackTarget
  endif
endif   
 system.dbm �     - 5     o     y      � �       �?    o �     S 5 0     �        7     7 ,     7 , 5 X         �                7     7 ,  	   7 , 5 �         �           	  
   7     7 ,     7 , 5 �         �   
             S 5 �                              �?q !       �c9  �  
 