SB0  7      Ogre_Entity   m_Scene   #0 HANDLE    m_Name     STRING   
 m_MeshName     STRING    l_Create     FLOAT�   m_SceneNode   #0 HANDLE    l_SceneNode     HANDLE�   m_CastShadows   1 FLOAT    l_CastShadows     FLOAT�  	 m_Visible   1 FLOAT   	 l_Visible     FLOAT�   m_Entity   #0 HANDLE�  
  HANDLE m_Scene
STRING m_Name
STRING m_MeshName

FLOAT local l_Create

HANDLE m_SceneNode
HANDLE local l_SceneNode

FLOAT m_CastShadows
FLOAT local l_CastShadows

FLOAT m_Visible
FLOAT local l_Visible

if(~!l_Create)
  if(~m_Scene)  
    m_Entity := Entity_Create(~m_Scene, ~m_Name, ~m_MeshName)
    Movable_GetName(~m_Entity, ~m_Name)
    l_Create := 1
  endif
endif

if(~m_Entity)
  if(l_SceneNode != ~m_SceneNode)
    Movable_SetParent(~m_Entity, ~m_SceneNode)
    l_SceneNode := ~m_SceneNode
  endif
  
  if(l_CastShadows != ~m_CastShadows)
    Movable_SetCastShadows(~m_Entity, ~m_CastShadows)
    l_CastShadows := ~m_CastShadows
  endif
  
  if(l_Visible != ~m_Visible)
    Movable_SetVisible(~m_Entity, ~m_Visible)
    l_Visible := ~m_Visible
  endif
endif -   BE_ICONS.DBM S     - 5     o     y  y  � 
  
 � �       �?   
 o R     S 5 2  
   �        7 5 B  
   �     	   7 5 R  
   �   	    7T�  K  
 