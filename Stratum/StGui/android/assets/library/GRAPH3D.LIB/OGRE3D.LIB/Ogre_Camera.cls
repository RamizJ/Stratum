SB0  �      Ogre_Camera   m_Scene   #0 HANDLE    m_Name     STRING    m_SceneNode   #0 HANDLE    m_FOV  
 0,78539816 FLOAT    m_NearClipDistance   1 FLOAT    l_NearClipDistance     FLOAT�   m_FarClipDistance   1000 FLOAT    l_FarClipDistance     FLOAT�   l_Create     FLOAT�   l_SceneNode     HANDLE�   l_FOV     FLOAT�   l_PolygonMode     FLOAT�   l_ProjectionType     FLOAT�   m_Camera   #0 HANDLE�   m_PolygonMode   PM_SOLID FLOAT    m_ProjectionType   PT_PERSPECTIVE FLOAT   
 OHANDLE m_Scene
STRING m_Name
HANDLE m_SceneNode
FLOAT  m_FOV

FLOAT m_NearClipDistance
FLOAT local l_NearClipDistance

FLOAT m_FarClipDistance
FLOAT local l_FarClipDistance

FLOAT local l_Create

HANDLE local l_SceneNode
FLOAT local l_FOV
FLOAT local l_PolygonMode
FLOAT local l_ProjectionType

if(~!l_Create)
  if(~m_Scene)  
    m_Camera := Camera_Create(~m_Scene, ~m_Name)
    Movable_GetName(~m_Camera, ~m_Name)
    l_Create := 1
  endif
endif

if(~m_Camera)
  if(l_SceneNode != ~m_SceneNode)
    Movable_SetParent(~m_Camera, ~m_SceneNode)
    l_SceneNode := ~m_SceneNode
  endif
  
  if(l_FOV != ~m_FOV)
    Camera_SetFOV(~m_Camera, ~m_FOV)
    l_FOV := ~m_FOV
  endif
  
  if(l_PolygonMode != ~m_PolygonMode)
    Camera_SetPolygonMode(~m_Camera, ~m_PolygonMode)
    l_PolygonMode := ~m_PolygonMode
  endif
  
  if(l_ProjectionType != ~m_ProjectionType)
    Camera_SetProjectionType(~m_Camera, ~m_ProjectionType)
    l_ProjectionType := ~m_ProjectionType
  endif
  
  if(l_NearClipDistance != ~m_NearClipDistance)
    Camera_SetNearClipDistance(~m_Camera, ~m_NearClipDistance)
    l_NearClipDistance := ~m_NearClipDistance
  endif
  
  if(l_FarClipDistance != ~m_FarClipDistance)
    Camera_SetFarClipDistance(~m_Camera, ~m_FarClipDistance)
    l_FarClipDistance := ~m_FarClipDistance
  endif
endif   
 system.dbm �     - 5     o     y  �    � �       �?    o �  	   S 5 0     �   	  
   7 5 @     �   
     7 5 P     �        7 5 `     �        7 5 p     �        7 5 �     �       ov�    
 