SB0  �     
 Ogre_Light    l_Create     FLOAT�   m_Scene   #0 HANDLE    m_Name     STRING    m_SceneNode   #0 HANDLE    l_SceneNode     HANDLE�   l_PowerScale     FLOAT�  	 m_Diffuse   rgb(255,255,255) COLORREF   	 l_Diffuse     COLORREF�  
 m_Specular  
 rgb(0,0,0) COLORREF   
 l_Specular     COLORREF�   m_Type   LT_POINT FLOAT    l_Type     FLOAT�   m_AttenuationRange   100000 FLOAT    m_AttenuationConstant   1 FLOAT    m_AttenuationLinear   0 FLOAT    m_AttenuationQuadratic   0 FLOAT    l_AttenuationRange     FLOAT�   l_AttenuationConstant     FLOAT�   l_AttenuationLinear     FLOAT�   l_AttenuationQuadratic     FLOAT�   m_SpotlightInnerAngle   2.0 FLOAT    m_SpotlightOuterAngle   3.0 FLOAT    m_SpotlightFalloff   1.0 FLOAT    l_SpotlightInnerAngle     FLOAT�   l_SpotlightOuterAngle     FLOAT�   l_SpotlightFalloff     FLOAT�   m_CastShadows   1 FLOAT    l_CastShadows     FLOAT�  	 m_Visible   1 FLOAT   	 l_Visible     FLOAT�   m_Light   #0 HANDLE�   m_PowerScale   0 FLOAT   
 �FLOAT local l_Create

HANDLE m_Scene
STRING m_Name
HANDLE m_SceneNode

HANDLE local l_SceneNode
FLOAT local l_PowerScale

COLORREF m_Diffuse
COLORREF local l_Diffuse

COLORREF m_Specular
COLORREF local l_Specular

FLOAT m_Type
FLOAT local l_Type

FLOAT m_AttenuationRange
FLOAT m_AttenuationConstant
FLOAT m_AttenuationLinear
FLOAT m_AttenuationQuadratic

FLOAT local l_AttenuationRange
FLOAT local l_AttenuationConstant
FLOAT local l_AttenuationLinear
FLOAT local l_AttenuationQuadratic

FLOAT m_SpotlightInnerAngle 
FLOAT m_SpotlightOuterAngle 
FLOAT m_SpotlightFalloff 

FLOAT local l_SpotlightInnerAngle 
FLOAT local l_SpotlightOuterAngle 
FLOAT local l_SpotlightFalloff 

FLOAT m_CastShadows
FLOAT local l_CastShadows

FLOAT m_Visible
FLOAT local l_Visible

if(~!l_Create)
  if(~m_Scene)  
    m_Light := Light_Create(~m_Scene, ~m_Name)
    Movable_GetName(~m_Light, ~m_Name)
    l_Create := 1
  endif
endif

if(~m_Light)
  if(l_SceneNode != ~m_SceneNode)
    Movable_SetParent(~m_Light, ~m_SceneNode)
    l_SceneNode := ~m_SceneNode
  endif
  
  if(l_Diffuse != ~m_Diffuse)
    Light_SetDiffuseColour(~m_Light, GetRValue(~m_Diffuse) / 255, GetGValue(~m_Diffuse) / 255, GetBValue(~m_Diffuse) / 255)    
    l_Diffuse := ~m_Diffuse
  endif
  
  if(l_Specular != ~m_Specular)
    Light_SetSpecularColour(~m_Light, GetRValue(~m_Specular) / 255, GetGValue(~m_Specular) / 255, GetBValue(~m_Specular) / 255)    
    l_Specular := ~m_Specular
  endif
  
  if(l_Type != ~m_Type)
    Light_SetType(~m_Light, ~m_Type)    
    l_Type := ~m_Type
  endif
  
  if(l_PowerScale != ~m_PowerScale)
    Light_SetPowerScale(~m_Light, ~m_PowerScale)
    l_PowerScale := ~m_PowerScale
  endif
  
  if(l_AttenuationRange != ~m_AttenuationRange || l_AttenuationConstant != ~m_AttenuationConstant || l_AttenuationLinear != ~m_AttenuationLinear || l_AttenuationQuadratic != ~m_AttenuationQuadratic)
    Light_SetAttenuation(~m_Light, ~m_AttenuationRange, ~m_AttenuationConstant, ~m_AttenuationLinear, ~m_AttenuationQuadratic)
    l_AttenuationRange := ~m_AttenuationRange
    l_AttenuationConstant := ~m_AttenuationConstant
    l_AttenuationLinear := ~m_AttenuationLinear
    l_AttenuationQuadratic := ~m_AttenuationQuadratic
  endif
  
  if(~m_Type == LT_SPOTLIGHT)
    if(l_SpotlightInnerAngle != ~m_SpotlightInnerAngle || l_SpotlightOuterAngle != ~m_SpotlightOuterAngle || l_SpotlightFalloff != ~m_SpotlightFalloff)
      Light_SetSpotlightRange(~m_Light, ~m_SpotlightInnerAngle, ~m_SpotlightOuterAngle, ~m_SpotlightFalloff)
      l_SpotlightInnerAngle := ~m_SpotlightInnerAngle
      l_SpotlightOuterAngle := ~m_SpotlightOuterAngle 
      l_SpotlightFalloff    := ~m_SpotlightFalloff
    endif
  endif
  
  if(l_CastShadows != ~m_CastShadows)
    Movable_SetCastShadows(~m_Light, ~m_CastShadows)
    l_CastShadows := ~m_CastShadows
  endif
  
  if(l_Visible != ~m_Visible)
    Movable_SetVisible(~m_Light, ~m_Visible)
    l_Visible := ~m_Visible
  endif
endif t  )     - 5    o    y  �    � �       �?     o (    S 5 0     �        S 5 Y     a      �o@   b      �o@   c      �o@ �     	   S 5 �     a      �o@   b      �o@   c      �o@ �   	    
 7 5 �    
 � 
       7 5 �     �        7     7 ,     7 ,     7 , 5 �           �                 
         @6 5     7     7 ,     7 , 5         �                7 5     �        7 5 (    �       �|�  F  
 