SB0       
 Ogre_Scene   m_SceneName     STRING    l_Create     FLOAT�   m_SkyBoxMaterialName     STRING    m_SceneManagerType   OctreeSceneManager STRING    m_WorldGeometry     STRING    m_ShadowTechnique   SHADOWTYPE_NONE FLOAT    l_ShadowTechnique     FLOAT�   m_AmbientLight   rgb(200,200,200) COLORREF    l_AmbientLight     COLORREF�  	 m_FogMode   FOG_NONE FLOAT    m_FogColour   rgb(200,200,200) COLORREF    m_FogExpDensity   0.01 FLOAT    m_FogLinearStart   10.0 FLOAT    m_FogLinearEnd   100.0 FLOAT   	 l_FogMode     FLOAT�   l_FogColour     COLORREF�   l_FogExpDensity     FLOAT�   l_FogLinearStart     FLOAT�   l_FogLinearEnd     FLOAT�   m_Scene   #0 HANDLE�   m_RootSceneNode   #0 HANDLE�  
 STRING m_SceneName

FLOAT local l_Create
  
STRING m_SkyBoxMaterialName
STRING m_SceneManagerType
STRING m_WorldGeometry
    
FLOAT m_ShadowTechnique
FLOAT local l_ShadowTechnique

COLORREF m_AmbientLight
COLORREF local l_AmbientLight

FLOAT m_FogMode
COLORREF m_FogColour
FLOAT m_FogExpDensity
FLOAT m_FogLinearStart 
FLOAT m_FogLinearEnd

FLOAT local l_FogMode
COLORREF local l_FogColour
FLOAT local l_FogExpDensity
FLOAT local l_FogLinearStart
FLOAT local l_FogLinearEnd

if(~!l_Create)
  if(Root_IsInitialised())
    if(RenderWindow_GetPrimary())
      m_Scene := Scene_Create(m_SceneManagerType)
  
      if(~m_SceneName != "")
        Scene_Load(~m_Scene, ~m_SceneName)
      endif

      if(~m_SkyBoxMaterialName != "")    
        Scene_SetSkyBox(~m_Scene, 1, ~m_SkyBoxMaterialName, 50)
      endif
      
      if(~m_WorldGeometry != "")    
        Scene_SetWorldGeometry(~m_Scene, ~m_WorldGeometry)
      endif
  
      m_RootSceneNode := Scene_GetRootSceneNode(~m_Scene)
  
      l_Create := 1
    endif
  endif
endif

if(~m_Scene)
  if(~l_ShadowTechnique != ~m_ShadowTechnique)
    Scene_SetShadowTechnique(~m_Scene, ~m_ShadowTechnique)
    l_ShadowTechnique := ~m_ShadowTechnique
  endif
  
  if(l_AmbientLight != ~m_AmbientLight)
    Scene_SetAmbientLight(~m_Scene, GetRValue(~m_AmbientLight) / 255, GetGValue(~m_AmbientLight) / 255, GetBValue(~m_AmbientLight) / 255)    
    l_AmbientLight := ~m_AmbientLight
  endif
  
  if(~l_FogMode != ~m_FogMode || ~l_FogColour != ~m_FogColour || ~l_FogExpDensity != ~m_FogExpDensity || ~l_FogLinearStart != ~m_FogLinearStart || ~l_FogLinearEnd != ~m_FogLinearEnd)
    Scene_SetFog(~m_Scene, ~m_FogMode, GetRValue(~m_FogColour) / 255, GetGValue(~m_FogColour) / 255, GetBValue(~m_FogColour) / 255, ~m_FogExpDensity, ~m_FogLinearStart, ~m_FogLinearEnd)    
    l_FogMode := ~m_FogMode
    l_FogColour := ~m_FogColour
    l_FogExpDensity := ~m_FogExpDensity
    l_FogLinearStart := ~m_FogLinearStart 
    l_FogLinearEnd := ~m_FogLinearEnd
  endif
endif 7   SCIENCE.DBM �     - 5 M 5 M �o M x    y   z    � 5    y   y  z    � 5 4          �?y         I@y  z    � 5 A   y             �?    o �     7 5 a             S 5 �     a      �o@   b      �o@   c      �o@        	 7    
 S ,     7 ,     7 ,     7 , 5 �    	  
 a      �o@  
 b      �o@  
 c      �o@        	    
                  w�    
 