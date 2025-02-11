SB0        Ogre_AnimationState   m_Entity     HANDLE    m_AnimationState     HANDLE�   m_AnimationName     STRING    l_AnimationName     STRING�   m_AnimationSpeed   1 FLOAT   	 l_Enabled     FLOAT�  	 m_Enabled   1 FLOAT    m_Weight   1.0 FLOAT    l_Weight     FLOAT�  	 l_CurTime     FLOAT�  	 l_OldTime     FLOAT�  
 �HANDLE m_Entity
HANDLE m_AnimationState

STRING m_AnimationName
STRING local l_AnimationName

FLOAT m_AnimationSpeed

FLOAT local l_Enabled
FLOAT m_Enabled

FLOAT m_Weight
FLOAT local l_Weight

FLOAT local l_CurTime
FLOAT local l_OldTime

if(l_AnimationName != ~m_AnimationName)
  if(~m_Entity)
    l_OldTime := GetTickCount()
    
    if(~m_AnimationState)
      AnimationState_SetEnabled(~m_AnimationState, 0.0)
    endif
    
    m_AnimationState := Entity_GetAnimationState(~m_Entity, ~m_AnimationName)
    
    if(~m_AnimationState)
      AnimationState_SetEnabled(~m_AnimationState, ~m_Enabled)
    endif
    
    l_AnimationName := ~m_AnimationName
  endif
endif

if(~m_AnimationState)
  if(l_Enabled != ~m_Enabled)
    AnimationState_SetEnabled(~m_AnimationState, ~m_Enabled)
    l_Enabled := ~m_Enabled
  endif
  
  if(l_Enabled != ~m_Enabled)
    AnimationState_SetWeight(~m_AnimationState, ~m_Weight)
    l_Weight := ~m_Weight
  endif
  
  if(~m_Enabled)
    l_CurTime := GetTickCount()
      
    AnimationState_AddTime(~m_AnimationState, m_AnimationSpeed * (l_CurTime - l_OldTime)/1000.0)
    
    l_OldTime := l_CurTime
  endif
endif   
 SYSTEM.DBM o   x  y  � 5 .    o . s  
   o             �   y  �    o *     �y  {    o n     7 5 B     �        7 5 R     �      5 n s  	      	  
         @�@ � 	  
    �  _  
 