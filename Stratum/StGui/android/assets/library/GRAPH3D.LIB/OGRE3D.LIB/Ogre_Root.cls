SB0  E     	 Ogre_Root 	 
 m_RenderOK     FLOAT�  
 m_ConfigOK     FLOAT�   l_Create     FLOAT�   m_PluginsFileName   Plugins.cfg STRING    m_ConfigFileName   Ogre.cfg STRING    m_LogFileName   Ogre.log STRING    m_ResourceConfigFileName     STRING    m_ShowConfigDialog   1 FLOAT    i     FLOAT�  
 (FLOAT local m_RenderOK
FLOAT local m_ConfigOK

FLOAT local l_Create

STRING m_PluginsFileName
STRING m_ConfigFileName
STRING m_LogFileName
STRING m_ResourceConfigFileName

FLOAT m_ShowConfigDialog

if(~!l_Create)
  Root_Create("Plugins.cfg", "Ogre.cfg", "Ogre.log")
  
  if(~m_ShowConfigDialog)
    m_ConfigOK := Root_ShowConfigDialog();
    
    if(~!m_ConfigOK)
      m_ConfigOK := Root_RestoreConfig();
    endif
  else
    m_ConfigOK := Root_RestoreConfig();
    
    if(~!m_ConfigOK)
      m_ConfigOK := Root_ShowConfigDialog();
    endif
  endif
  
  Root_Initialise()
  
  if(~m_ResourceConfigFileName != "")
    Root_AddResourceLocationFromConfigFile(~m_ResourceConfigFileName)
  endif
     
  l_Create := 1  
endif

m_RenderOk := Root_RenderOneFrame();
i:=~i+1 G   DIAMONDS.DBM Y     - 5 K z  Plugins.cfg z  Ogre.cfg  z  Ogre.log    5 - 	    - 5 +   3 8     - 5 8 	  y  z    � 5 D y         �?  
            �?      �~[  E  
 