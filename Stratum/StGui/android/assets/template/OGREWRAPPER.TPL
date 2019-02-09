name "AnimationState_GetTimePosition" arg "HANDLE" ret "FLOAT" out 900
name "AnimationState_SetTimePosition" arg "HANDLE", "FLOAT" out 901
name "AnimationState_GetLength" arg "HANDLE" ret "FLOAT" out 902
name "AnimationState_SetLength" arg "HANDLE", "FLOAT" out 903
name "AnimationState_GetWeight" arg "HANDLE" ret "FLOAT" out 904
name "AnimationState_SetWeight" arg "HANDLE", "FLOAT" out 905
name "AnimationState_AddTime" arg "HANDLE", "FLOAT" out 906
name "AnimationState_GetEnabled" arg "HANDLE" ret "FLOAT" out 907
name "AnimationState_SetEnabled" arg "HANDLE", "FLOAT" out 908
name "AnimationState_SetLoop" arg "HANDLE", "FLOAT" out 909
name "AnimationState_GetLoop" arg "HANDLE" ret "FLOAT" out 910
name "Bone_SetManuallyControlled" arg "HANDLE", "FLOAT" out 911
name "Bone_GetManuallyControlled" arg "HANDLE" ret "FLOAT" out 912
name "Bone_Reset" arg "HANDLE" out 913
name "Camera_Create" arg "HANDLE", "STRING" ret "HANDLE" out 914
name "Camera_Destroy" arg "HANDLE" out 915
name "Camera_SetFOV" arg "HANDLE", "FLOAT" out 916
name "Camera_GetFOV" arg "HANDLE" ret "FLOAT" out 917
name "Camera_SetAspectRatio" arg "HANDLE", "FLOAT" out 918
name "Camera_GetAspectRatio" arg "HANDLE" ret "FLOAT" out 919
name "Camera_SetNearClipDistance" arg "HANDLE", "FLOAT" out 920
name "Camera_GetNearClipDistance" arg "HANDLE" ret "FLOAT" out 921
name "Camera_SetFarClipDistance" arg "HANDLE", "FLOAT" out 922
name "Camera_GetFarClipDistance" arg "HANDLE" ret "FLOAT" out 923
name "Camera_SetProjectionType" arg "HANDLE", "FLOAT" out 924
name "Camera_GetProjectionType" arg "HANDLE" ret "FLOAT" out 925
name "Camera_SetPolygonMode" arg "HANDLE", "FLOAT" out 926
name "Camera_GetPolygonMode" arg "HANDLE" ret "FLOAT" out 927
name "Camera_SetFrustumOffset" arg "HANDLE", "FLOAT", "FLOAT" out 928
name "Camera_SetFocalLength" arg "HANDLE", "FLOAT" out 929
name "Collision_RayCast" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 930
name "Collision_Sort" out 931
name "Collision_GetResultCount" ret "FLOAT" out 932
name "Collision_GetDistance" arg "FLOAT" ret "FLOAT" out 933
name "Collision_GetObject" arg "FLOAT" ret "HANDLE" out 934
name "Entity_Create" arg "HANDLE", "STRING", "STRING" ret "HANDLE" out 935
name "Entity_Destroy" arg "HANDLE" out 936
name "Entity_SetMaterial" arg "HANDLE", "FLOAT", "STRING" out 937
name "Entity_GetAnimationState" arg "HANDLE", "STRING" ret "HANDLE" out 938
name "Entity_GetBone" arg "HANDLE", "STRING" ret "HANDLE" out 939
name "Light_Create" arg "HANDLE", "STRING" ret "HANDLE" out 940
name "Light_Destroy" arg "HANDLE" out 941
name "Light_SetAttenuation" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 942
name "Light_SetSpotlightRange" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 943
name "Light_SetDiffuseColour" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 944
name "Light_SetSpecularColour" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 945
name "Light_SetType" arg "HANDLE", "FLOAT" out 946
name "Light_SetPowerScale" arg "HANDLE", "FLOAT" out 947
name "Light_GetPowerScale" arg "HANDLE" ret "FLOAT" out 948
name "ManualObject_Create" arg "HANDLE", "STRING" ret "HANDLE" out 949
name "ManualObject_Destroy" arg "HANDLE" out 950
name "ManualObject_Clear" arg "HANDLE" out 951
name "ManualObject_EstimateVertexCount" arg "HANDLE", "FLOAT" out 952
name "ManualObject_EstimateIndexCount" arg "HANDLE", "FLOAT" out 953
name "ManualObject_SetDynamic" arg "HANDLE", "FLOAT" out 954
name "ManualObject_GetDynamic" arg "HANDLE" ret "FLOAT" out 955
name "ManualObject_Begin" arg "HANDLE", "STRING", "FLOAT" out 956
name "ManualObject_End" arg "HANDLE" out 957
name "ManualObject_Position" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 958
name "ManualObject_Normal" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 959
name "ManualObject_Colour" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 960
name "ManualObject_TextureCoord" arg "HANDLE", "FLOAT", "FLOAT" out 961
name "ManualObject_Index" arg "HANDLE", "FLOAT" out 962
name "ManualObject_ConvertToMesh" arg "HANDLE", "STRING" out 963
name "Material_Create" arg "STRING" ret "HANDLE" out 964
name "Material_Get" arg "STRING" ret "HANDLE" out 965
name "Material_GetBestTechnique" arg "HANDLE" ret "HANDLE" out 966
name "Material_GetTechniqueByName" arg "HANDLE", "STRING" ret "HANDLE" out 967
name "Material_GetTechniqueByIndex" arg "HANDLE", "FLOAT" ret "HANDLE" out 968
name "Movable_SetParent" arg "HANDLE", "HANDLE" out 969
name "Movable_GetParent" arg "HANDLE" ret "HANDLE" out 970
name "Movable_SetCastShadows" arg "HANDLE", "FLOAT" out 971
name "Movable_GetCastShadows" arg "HANDLE" ret "FLOAT" out 972
name "Movable_SetVisible" arg "HANDLE", "FLOAT" out 973
name "Movable_GetVisible" arg "HANDLE" ret "FLOAT" out 974
name "Node_SetPosition" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 975
name "Node_SetRotationEulerXYZ" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 976
name "Node_SetRotationEulerXZY" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 977
name "Node_SetRotationEulerYXZ" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 978
name "Node_SetRotationEulerYZX" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 979
name "Node_SetRotationEulerZXY" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 980
name "Node_SetRotationEulerZYX" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 981
name "Node_SetRotationAxis" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 982
name "Node_SetRotationQuaternion" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 983
name "Node_SetScale" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 984
name "Node_SetParent" arg "HANDLE", "HANDLE" out 985
name "Node_GetParent" arg "HANDLE" ret "HANDLE" out 986
name "Node_AddChild" arg "HANDLE", "HANDLE" out 987
name "Node_RemoveChild" arg "HANDLE", "HANDLE" out 988
name "Overlay_Get" arg "STRING" ret "HANDLE" out 989
name "Overlay_SetVisible" arg "HANDLE", "FLOAT" out 990
name "Overlay_GetVisible" arg "HANDLE" ret "FLOAT" out 991
name "OverlayElement_Get" arg "STRING" ret "HANDLE" out 992
name "OverlayElement_SetVisible" arg "HANDLE", "FLOAT" out 993
name "OverlayElement_GetVisible" arg "HANDLE" ret "FLOAT" out 994
name "OverlayElement_SetPosition" arg "HANDLE", "FLOAT", "FLOAT" out 995
name "OverlayElement_SetSize" arg "HANDLE", "FLOAT", "FLOAT" out 996
name "OverlayElement_SetColour" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 997
name "OverlayElement_SetCaption" arg "HANDLE", "STRING" out 998
name "OverlayElement_SetMaterialName" arg "HANDLE", "STRING" out 999
name "ParticleSystem_Create" arg "HANDLE", "STRING", "STRING" ret "HANDLE" out 1000
name "ParticleSystem_Destroy" arg "HANDLE" out 1001
name "Pass_Create" arg "HANDLE", "STRING" ret "HANDLE" out 1002
name "Pass_SetAmbient" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1003
name "Pass_SetDiffuse" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1004
name "Pass_SetSpecular" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1005
name "Pass_SetShininess" arg "HANDLE", "FLOAT" out 1006
name "Pass_SetSelfIllumination" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1007
name "Pass_SetSceneBlending" arg "HANDLE", "FLOAT", "FLOAT" out 1008
name "Pass_SetDepthCheckEnabled" arg "HANDLE", "FLOAT" out 1009
name "Pass_SetDepthWriteEnabled" arg "HANDLE", "FLOAT" out 1010
name "Pass_SetDepthFunction" arg "HANDLE", "FLOAT" out 1011
name "Pass_SetColourWriteEnabled" arg "HANDLE", "FLOAT" out 1012
name "Pass_SetCullingMode" arg "HANDLE", "FLOAT" out 1013
name "Pass_SetLightingEnabled" arg "HANDLE", "FLOAT" out 1014
name "Pass_SetShadingMode" arg "HANDLE", "FLOAT" out 1015
name "Pass_SetPolygonMode" arg "HANDLE", "FLOAT" out 1016
name "Pass_SetAlphaRejectSettings" arg "HANDLE", "FLOAT", "FLOAT" out 1017
name "Pass_GetTextureUnitStateByName" arg "HANDLE", "STRING" ret "HANDLE" out 1018
name "Pass_GetTextureUnitStateByIndex" arg "HANDLE", "FLOAT" ret "HANDLE" out 1019
name "RenderTexture_Create" arg "STRING", "FLOAT", "FLOAT" ret "HANDLE" out 1020
name "RenderTexture_Destroy" arg "HANDLE" out 1021
name "RenderWindow_Create" arg "HANDLE", "FLOAT", "FLOAT" ret "HANDLE" out 1022
name "RenderWindow_GetPrimary" ret "HANDLE" out 1023
name "RenderWindow_Destroy" arg "HANDLE" out 1024
name "RenderWindow_SetPosition" arg "HANDLE", "FLOAT", "FLOAT" out 1025
name "RenderWindow_SetSize" arg "HANDLE", "FLOAT", "FLOAT" out 1026
name "Root_Create" arg "STRING", "STRING", "STRING" out 1027
name "Root_Destroy" out 1028
name "Root_Initialise" out 1029
name "Root_IsInitialised" ret "FLOAT" out 1030
name "Root_RestoreConfig" ret "FLOAT" out 1031
name "Root_SaveConfig" out 1032
name "Root_ShowConfigDialog" ret "FLOAT" out 1033
name "Root_RenderOneFrame" ret "FLOAT" out 1034
name "Root_AddResourceLocationFromConfigFile" arg "STRING" out 1035
name "Root_AddResourceLocation" arg "STRING", "STRING", "STRING", "FLOAT" out 1036
name "Root_InitialiseAllResourceGroups" out 1037
name "Root_GetTime" ret "FLOAT" out 1038
name "Scene_Create" arg "STRING" ret "HANDLE" out 1039
name "Scene_Destroy" arg "HANDLE" out 1040
name "Scene_SetWorldGeometry" arg "HANDLE", "STRING" out 1041
name "Scene_GetRootSceneNode" arg "HANDLE" ret "HANDLE" out 1042
name "Scene_SetShadowTechnique" arg "HANDLE", "FLOAT" out 1043
name "Scene_GetShadowTechnique" arg "HANDLE" ret "FLOAT" out 1044
name "Scene_SetFog" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1045
name "Scene_SetAmbientLight" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1046
name "Scene_SetSkyBox" arg "HANDLE", "FLOAT", "STRING", "FLOAT" out 1047
name "Scene_GetCamera" arg "HANDLE", "STRING" ret "HANDLE" out 1048
name "Scene_GetEntity" arg "HANDLE", "STRING" ret "HANDLE" out 1049
name "Scene_GetLight" arg "HANDLE", "STRING" ret "HANDLE" out 1050
name "Scene_GetParticleSystem" arg "HANDLE", "STRING" ret "HANDLE" out 1051
name "Scene_GetSceneNode" arg "HANDLE", "STRING" ret "HANDLE" out 1052
name "Scene_Clear" arg "HANDLE" out 1053
name "Scene_Load" arg "HANDLE", "STRING" out 1054
name "SceneNode_Create" arg "HANDLE", "STRING" ret "HANDLE" out 1055
name "SceneNode_Destroy" arg "HANDLE" out 1056
name "SceneNode_SetAutoTracking" arg "HANDLE", "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1057
name "SceneNode_AttachObject" arg "HANDLE", "HANDLE" out 1058
name "SceneNode_DetachObject" arg "HANDLE", "HANDLE" out 1059
name "Technique_Create" arg "HANDLE", "STRING" ret "HANDLE" out 1060
name "Technique_GetPassByName" arg "HANDLE", "STRING" ret "HANDLE" out 1061
name "Technique_GetPassByIndex" arg "HANDLE", "FLOAT" ret "HANDLE" out 1062
name "TextureUnitState_Create" arg "HANDLE", "STRING" ret "HANDLE" out 1063
name "TextureUnitState_SetTexture" arg "HANDLE", "STRING", "FLOAT" out 1064
name "TextureUnitState_GetTexture" arg "HANDLE", "STRING", "HANDLE" out 1065
name "Viewport_Create" arg "HANDLE", "HANDLE", "FLOAT" ret "HANDLE" out 1066
name "Viewport_Destroy" arg "HANDLE" out 1067
name "Viewport_SetCamera" arg "HANDLE", "HANDLE" out 1068
name "Viewport_GetCamera" arg "HANDLE" ret "HANDLE" out 1069
name "Viewport_SetBackgroundColour" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1070
name "Viewport_SetDimensions" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1071
name "Viewport_SetOverlaysEnabled" arg "HANDLE", "FLOAT" out 1072
name "Viewport_GetOverlaysEnabled" arg "HANDLE" ret "FLOAT" out 1073
name "Viewport_GetRay" arg "HANDLE", "FLOAT", "FLOAT", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1074
name "RenderWindow_CreateEx" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" ret "HANDLE" out 1075
name "RenderWindow_ToggleFullscreen" arg "HANDLE" ret "FLOAT" out 1076
name "BillboardSet_Create" arg "HANDLE", "STRING", "FLOAT" ret "HANDLE" out 1077
name "BillboardSet_Destroy" arg "HANDLE" out 1078
name "BillboardSet_CreateBillboard" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" ret "HANDLE" out 1079
name "BillboardSet_GetNumBillboards" arg "HANDLE" ret "FLOAT" out 1080
name "BillboardSet_GetBillboard" arg "HANDLE", "FLOAT" ret "HANDLE" out 1081
name "BillboardSet_RemoveBillboard" arg "HANDLE", "HANDLE" out 1082
name "BillboardSet_SetBillboardType" arg "HANDLE", "FLOAT" out 1083
name "BillboardSet_GetBillboardType" arg "HANDLE" ret "FLOAT" out 1084
name "BillboardSet_SetCommonDirection" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1085
name "BillboardSet_GetCommonDirection" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1086
name "BillboardSet_SetCommonUpVector" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1087
name "BillboardSet_GetCommonUpVector" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1088
name "BillboardSet_SetDefaultDimensions" arg "HANDLE", "FLOAT", "FLOAT" out 1089
name "BillboardSet_GetDefaultDimensions" arg "HANDLE", &"FLOAT", &"FLOAT" out 1090
name "BillboardSet_SetMaterialName" arg "HANDLE", "STRING" out 1091
name "BillboardSet_GetMaterialName" arg "HANDLE", &"STRING" out 1092
name "Billboard_SetRotation" arg "HANDLE", "FLOAT" out 1093
name "Billboard_GetRotation" arg "HANDLE" ret "FLOAT" out 1094
name "Billboard_SetPosition" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1095
name "Billboard_GetPosition" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1096
name "Billboard_SetColour" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1097
name "Billboard_GetColour" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1098
name "Viewport_AddCompositor" arg "HANDLE", "STRING", "FLOAT" ret "HANDLE" out 1200
name "Viewport_RemoveCompositor" arg "HANDLE", "HANDLE" out 1201
name "Viewport_GetNumCompositors" arg "HANDLE" ret "FLOAT" out 1202
name "Viewport_GetCompositor" arg "HANDLE", "FLOAT" ret "HANDLE" out 1203
name "Compositor_SetEnabled" arg "HANDLE", "FLOAT" out 1204
name "Compositor_GetEnabled" arg "HANDLE" ret "FLOAT" out 1205
name "Node_GetName" arg "HANDLE", &"STRING" out 1206
name "Node_GetPosition" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1207
name "Node_GetScale" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1208
name "Node_GetRotationQuaternion" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1209
name "Node_GetNumChildren" arg "HANDLE" ret "FLOAT" out 1210
name "Node_GetChild" arg "HANDLE", "FLOAT" ret "HANDLE" out 1211
name "Movable_GetName" arg "HANDLE", &"STRING" out 1212
name "Movable_GetBoundingBox" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1213
name "SceneNode_GetNumObjects" arg "HANDLE" ret "FLOAT" out 1214
name "SceneNode_GetObject" arg "HANDLE", "FLOAT" ret "HANDLE" out 1215
name "SceneNode_SetVisible" arg "HANDLE", "FLOAT", "FLOAT" out 1216
name "SceneNode_GetScene" arg "HANDLE" ret "HANDLE" out 1217
name "ParticleSystem_CreateEx" arg "HANDLE", "STRING", "STRING" ret "HANDLE" out 1218

name "Material_GetName" arg "HANDLE", &"STRING" out 1219
name "MovableText_Create" arg "STRING", "STRING", "STRING" ret "HANDLE" out 1220
name "MovableText_Destroy" arg "HANDLE" out 1221
name "MovableText_SetFontName" arg "HANDLE", "STRING" out 1222
name "MovableText_GetFontName" arg "HANDLE", &"STRING" out 1223
name "MovableText_SetCaption" arg "HANDLE", "STRING" out 1224
name "MovableText_GetCaption" arg "HANDLE", &"STRING" out 1225
name "MovableText_SetColor" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT", "FLOAT" out 1226
name "MovableText_GetColor" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1227
name "MovableText_SetCharacterHeight" arg "HANDLE", "FLOAT" out 1228
name "MovableText_GetCharacterHeight" arg "HANDLE" ret "FLOAT" out 1229
name "MovableText_SetSpaceWidth" arg "HANDLE", "FLOAT" out 1230
name "MovableText_GetSpaceWidth" arg "HANDLE" ret "FLOAT" out 1231
name "MovableText_SetTextAlignment" arg "HANDLE", "FLOAT", "FLOAT" out 1232
name "MovableText_GetTextAlignment" arg "HANDLE", &"FLOAT", &"FLOAT" out 1233
name "MovableText_SetGlobalTranslation" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1234
name "MovableText_GetGlobalTranslation" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1235
name "MovableText_SetLocalTranslation" arg "HANDLE", "FLOAT", "FLOAT", "FLOAT" out 1236
name "MovableText_GetLocalTranslation" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1237
name "Overlay_Create" arg "STRING" ret "HANDLE" out 1238
name "Overlay_GetName" arg "HANDLE", &"STRING" out 1239
name "Overlay_FindElementAt" arg "HANDLE", "FLOAT", "FLOAT" ret "HANDLE" out 1240
name "Overlay_SetZOrder" arg "HANDLE", "FLOAT" out 1241
name "Overlay_GetZOrder" arg "HANDLE" ret "FLOAT" out 1242
name "Overlay_SetScale" arg "HANDLE", "FLOAT", "FLOAT" out 1243
name "Overlay_GetScale" arg "HANDLE", &"FLOAT", &"FLOAT" out 1244
name "Overlay_SetScroll" arg "HANDLE", "FLOAT", "FLOAT" out 1245
name "Overlay_GetScroll" arg "HANDLE", &"FLOAT", &"FLOAT" out 1246
name "Overlay_SetRotate" arg "HANDLE", "FLOAT" out 1247
name "Overlay_GetRotate" arg "HANDLE" ret "FLOAT" out 1248
name "Overlay_AddChild" arg "HANDLE", "HANDLE" out 1249
name "Overlay_RemoveChild" arg "HANDLE", "HANDLE" out 1250
name "OverlayContainer_AddChild" arg "HANDLE", "HANDLE" out 1251
name "OverlayContainer_RemoveChild" arg "HANDLE", "HANDLE" out 1252
name "OverlayContainer_GetChild" arg "HANDLE", "FLOAT" ret "HANDLE" out 1253
name "OverlayContainer_GetChildCount" arg "HANDLE" ret "FLOAT" out 1254
name "OverlayElement_Create" arg "STRING", "STRING" ret "HANDLE" out 1255
name "OverlayElement_GetPosition" arg "HANDLE", &"FLOAT", &"FLOAT" out 1256
name "OverlayElement_GetSize" arg "HANDLE", &"FLOAT", &"FLOAT" out 1257
name "OverlayElement_GetColour" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1258
name "OverlayElement_GetCaption" arg "HANDLE", &"STRING" out 1259
name "OverlayElement_GetMaterialName" arg "HANDLE", &"STRING" out 1260
name "OverlayElement_SetMetricsMode" arg "HANDLE", "FLOAT" out 1261
name "OverlayElement_GetMetricsMode" arg "HANDLE" ret "FLOAT" out 1262
name "OverlayElement_SetAlignment" arg "HANDLE", "FLOAT", "FLOAT" out 1263
name "OverlayElement_GetAlignment" arg "HANDLE", &"FLOAT", &"FLOAT" out 1264
name "OverlayElement_IsContainer" arg "HANDLE" ret "FLOAT" out 1265
name "OverlayElement_GetParent" arg "HANDLE" ret "HANDLE" out 1266
name "Entity_GetMaterial" arg "HANDLE", "FLOAT", &"STRING" out 1267
name "Entity_GetSubEntityCount" arg "HANDLE" ret "FLOAT" out 1268
name "Pass_GetAmbient" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1269
name "Pass_GetDiffuse" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1270
name "Pass_GetSpecular" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1271
name "Pass_GetShininess" arg "HANDLE" ret "FLOAT" out 1272
name "Pass_GetSelfIllumination" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1273
name "Pass_GetSceneBlending" arg "HANDLE", &"FLOAT", &"FLOAT" out 1274
name "Pass_GetDepthCheckEnabled" arg "HANDLE" ret "FLOAT" out 1275
name "Pass_GetDepthWriteEnabled" arg "HANDLE" ret "FLOAT" out 1276
name "Pass_GetDepthFunction" arg "HANDLE" ret "FLOAT" out 1277
name "Pass_GetColourWriteEnabled" arg "HANDLE" ret "FLOAT" out 1278
name "Pass_GetCullingMode" arg "HANDLE" ret "FLOAT" out 1279
name "Pass_GetLightingEnabled" arg "HANDLE" ret "FLOAT" out 1280
name "Pass_GetShadingMode" arg "HANDLE" ret "FLOAT" out 1281
name "Pass_GetPolygonMode" arg "HANDLE" ret "FLOAT" out 1282
name "Pass_GetAlphaRejectSettings" arg "HANDLE", &"FLOAT", &"FLOAT" out 1283
name "Node_GetDerivedPosition" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1284
name "Node_GetDerivedScale" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1285
name "Node_GetDerivedRotationQuaternion" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1286
name "StringInterface_SetParameter" arg "HANDLE", "STRING", "STRING" out 1287
name "StringInterface_GetParameter" arg "HANDLE", "STRING", "STRING" out 1288
name "StringInterface_GetParameterCount" arg "HANDLE" ret "FLOAT" out 1289
name "StringInterface_GetParameterType" arg "HANDLE", "FLOAT" ret "FLOAT" out 1290
name "StringInterface_GetParameterName" arg "HANDLE", "FLOAT", "STRING" out 1291
name "StringInterface_GetParameterDescription" arg "HANDLE", "FLOAT", "STRING" out 1292
name "Entity_GetVertexCount" arg "HANDLE", "FLOAT" ret "FLOAT" out 1293
name "Entity_GetIndexCount" arg "HANDLE", "FLOAT" ret "FLOAT" out 1294
name "RenderWindow_GetCursorPosition" arg "HANDLE", &"FLOAT", &"FLOAT" out 1295
name "RenderWindow_GetCursorHovered" arg "HANDLE" ret "FLOAT" out 1296
name "RenderWindow_GetMouseButtonPressed" arg "HANDLE", "FLOAT" ret "FLOAT" out 1297
name "RenderWindow_GetKeyboardButtonPressed" arg "HANDLE", "FLOAT" ret "FLOAT" out 1298
name "RenderWindow_GetViewportCount" arg "HANDLE" ret "FLOAT" out 1299
name "RenderWindow_GetViewport" arg "HANDLE", "FLOAT" ret "HANDLE" out 1300
name "RenderWindow_GetCount" ret "FLOAT" out 1301
name "RenderWindow_Get" arg "FLOAT" ret "HANDLE" out 1302
name "RenderWindow_GetWheelPosition" arg "HANDLE" ret "FLOAT" out 1303
name "RenderWindow_Create2" arg "HANDLE", "STRING", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT" ret "HANDLE" out 1304
name "ParticleSystem_SetVisible" arg "HANDLE", "FLOAT" out 1305
name "ParticleSystem_GetVisible" arg "HANDLE" ret "FLOAT" out 1306
name "ParticleSystem_SetParent" arg "HANDLE", "HANDLE" out 1307
name "ParticleSystem_GetParent" arg "HANDLE" ret "HANDLE" out 1308
name "ParticleSystem_GetName" arg "HANDLE", "STRING" out 1309
name "Viewport_GetBackgroundColour" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT" out 1310
name "Viewport_GetDimensions" arg "HANDLE", &"FLOAT", &"FLOAT", &"FLOAT", &"FLOAT" out 1311
name "RenderWindow_GetPosition" arg "HANDLE", &"FLOAT", &"FLOAT" out 1312
name "RenderWindow_GetSize" arg "HANDLE", &"FLOAT", &"FLOAT" out 1313