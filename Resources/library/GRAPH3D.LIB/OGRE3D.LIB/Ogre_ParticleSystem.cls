SB0  5      Ogre_ParticleSystem 	  m_Name     STRING    m_TemplateName     STRING    m_Scene   #0 HANDLE    m_SceneNode   #0 HANDLE    l_SceneNode     HANDLE�   l_Create     FLOAT�  	 m_Visible   1 FLOAT   	 l_Visible     FLOAT�   m_ParticleSystem   #0 HANDLE�  
 BSTRING m_Name
STRING m_TemplateName

HANDLE m_Scene

HANDLE m_SceneNode
HANDLE local l_SceneNode

FLOAT local l_Create

FLOAT m_Visible
FLOAT local l_Visible

if(~!l_Create && ~m_TemplateName != "" && ~m_Scene != #0)
  m_ParticleSystem := ParticleSystem_Create(~m_Scene, ~m_Name, ~m_TemplateName)
    
  ParticleSystem_GetName(~m_ParticleSystem, ~m_Name)    
  l_Visible := ParticleSystem_GetVisible(~m_ParticleSystem)
  l_SceneNode := ParticleSystem_GetParent(~m_ParticleSystem)
  
  l_Create := 1
endif

if(~m_ParticleSystem)
  if(~l_SceneNode != ~m_SceneNode)
    ParticleSystem_SetParent(~m_ParticleSystem, ~m_SceneNode)
    l_SceneNode := ~m_SceneNode
  endif
  
  if(~l_Visible != ~m_Visible)
    ParticleSystem_SetVisible(~m_ParticleSystem, ~m_Visible)
    l_Visible := ~m_Visible
  endif
endif   	 SIGNS.DBM W     - y  z    � +        S + 5 2   y   y  �    y                  �?    o V     S 5 F             7 5 V            �{8  "  
 