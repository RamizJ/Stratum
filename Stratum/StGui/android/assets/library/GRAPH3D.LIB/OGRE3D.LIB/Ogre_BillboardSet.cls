SB0  �      Ogre_BillboardSet   m_Scene   #0 HANDLE    m_Name     STRING    m_SceneNode   #0 HANDLE    m_MaterialName     STRING    l_SceneNode     HANDLE�   l_CastShadows     FLOAT�  	 l_Visible     FLOAT�   l_Create     FLOAT�   l_DefaultWidth     FLOAT�   l_DefaultHeight     FLOAT�   l_MaterialName     STRING�   l_BillboardType     FLOAT�   l_CommonDirectionX     FLOAT�   l_CommonDirectionY     FLOAT�   l_CommonDirectionZ     FLOAT�   l_CommonUpVectorX     FLOAT�   l_CommonUpVectorY     FLOAT�   l_CommonUpVectorZ     FLOAT�  
 m_PoolSize   20 FLOAT    m_BillboardSet   #0 HANDLE�   m_DefaultWidth   10 FLOAT    m_DefaultHeight   10 FLOAT    m_BillboardType  	 BBT_POINT FLOAT    m_CommonDirectionX   0 FLOAT    m_CommonDirectionY   0 FLOAT    m_CommonDirectionZ   1 FLOAT    m_CommonUpVectorX   0 FLOAT    m_CommonUpVectorY   0 FLOAT    m_CommonUpVectorZ   1 FLOAT    m_CastShadows   0 FLOAT   	 m_Visible   1 FLOAT   
 xHANDLE m_Scene
STRING m_Name
HANDLE m_SceneNode
STRING m_MaterialName

HANDLE local l_SceneNode
FLOAT local l_CastShadows
FLOAT local l_Visible

FLOAT local l_Create

FLOAT local l_DefaultWidth
FLOAT local l_DefaultHeight
STRING local l_MaterialName
FLOAT local l_BillboardType

FLOAT local l_CommonDirectionX
FLOAT local l_CommonDirectionY
FLOAT local l_CommonDirectionZ

FLOAT local l_CommonUpVectorX
FLOAT local l_CommonUpVectorY
FLOAT local l_CommonUpVectorZ

if(~!l_Create)
  if(~m_Scene)  
    m_BillboardSet := BillboardSet_Create(~m_Scene, ~m_Name, ~m_PoolSize)
    BillboardSet_SetDefaultDimensions(~m_BillboardSet, ~m_DefaultWidth, ~m_DefaultHeight)
    
    if(~m_MaterialName != "")
      BillboardSet_SetMaterialName(~m_BillboardSet, ~m_MaterialName)
    endif
    
    BillboardSet_SetBillboardType(~m_BillboardSet, ~m_BillboardType)
    
    BillboardSet_SetCommonDirection(~m_BillboardSet, ~m_CommonDirectionX, ~m_CommonDirectionY, ~m_CommonDirectionZ)
    BillboardSet_SetCommonUpVector(~m_BillboardSet, ~m_CommonUpVectorX, ~m_CommonUpVectorY, ~m_CommonUpVectorZ)
    
    l_DefaultWidth := ~m_DefaultWidth
    l_DefaultHeight := ~m_DefaultHeight
    l_MaterialName := ~m_MaterialName
    l_BillboardType := ~m_BillboardType
    
    l_CommonDirectionX := ~m_CommonDirectionX
    l_CommonDirectionY := ~m_CommonDirectionY
    l_CommonDirectionZ := ~m_CommonDirectionZ

    l_CommonUpVectorX := ~m_CommonUpVectorX
    l_CommonUpVectorY := ~m_CommonUpVectorY
    l_CommonUpVectorZ := ~m_CommonUpVectorZ
    
    l_Create := 1
  endif
endif

if(~m_BillboardSet)
  if(l_SceneNode != ~m_SceneNode)
    Movable_SetParent(~m_BillboardSet, ~m_SceneNode)
    l_SceneNode := ~m_SceneNode
  endif
  
  if(l_CastShadows != ~m_CastShadows)
    Movable_SetCastShadows(~m_BillboardSet, ~m_CastShadows)
    l_CastShadows := ~m_CastShadows
  endif
  
  if(l_Visible != ~m_Visible)
    Movable_SetVisible(~m_BillboardSet, ~m_Visible)
    l_Visible := ~m_Visible
  endif
  
  if(l_DefaultWidth != ~m_DefaultWidth || l_DefaultHeight != ~m_DefaultHeight)
    BillboardSet_SetDefaultDimensions(~m_BillboardSet, ~m_DefaultWidth, ~m_DefaultHeight)    
    l_DefaultWidth := ~m_DefaultWidth
    l_DefaultHeight := ~m_DefaultHeight
  endif
  
  if(l_MaterialName != ~m_MaterialName)
    BillboardSet_SetMaterialName(~m_BillboardSet, ~m_MaterialName)
    l_MaterialName := ~m_MaterialName
  endif
  
  if(l_BillboardType != ~m_BillboardType)
    BillboardSet_SetBillboardType(~m_BillboardSet, ~m_BillboardType)
    l_BillboardType := ~m_BillboardType
  endif
  
  if(l_CommonDirectionX != ~m_CommonDirectionX || l_CommonDirectionY != ~m_CommonDirectionY || l_CommonDirectionZ != ~m_CommonDirectionZ)
    BillboardSet_SetCommonDirection(~m_BillboardSet, ~m_CommonDirectionX, ~m_CommonDirectionY, ~m_CommonDirectionZ)    
    l_CommonDirectionX := ~m_CommonDirectionX
    l_CommonDirectionY := ~m_CommonDirectionY
    l_CommonDirectionZ := ~m_CommonDirectionZ
  endif
  
  if(l_CommonUpVectorX != ~m_CommonUpVectorX || l_CommonUpVectorY != ~m_CommonUpVectorY || l_CommonUpVectorZ != ~m_CommonUpVectorZ)
    BillboardSet_SetCommonUpVector(~m_BillboardSet, ~m_CommonUpVectorX, ~m_CommonUpVectorY, ~m_CommonUpVectorZ)
    l_CommonUpVectorX := ~m_CommonUpVectorX
    l_CommonUpVectorY := ~m_CommonUpVectorY
    l_CommonUpVectorZ := ~m_CommonUpVectorZ
  endif
endif  SCIENCE.DBM -    - 5 l    o l    y    5        Ay  z    � 5 &   y  C    ;        =        ?       	 y  { 
                                    �?    o ,    S 5 �     �        7 5 �     �        7 5 �     �        7  	   7 , 5 �       A       	 x 
 y  � 5 �   y  Cy  { 
     7 5 �     ;        7     7 ,     7 , 5         =                7     7 ,     7 , 5 ,        ?               �3    
 