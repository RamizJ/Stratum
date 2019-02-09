/*
Copyright (c) 1996 AAS
graphic library for windows

Module Name:
			gwobjt.h

 header file for gwin2d3d.dll

 object types 

*/
#ifndef __GRAPH_OBJ_TYPES
// internal object types

#define otGROUP                1
#define otCOLLECTION           1000
#define otPOINTCOLLECTION      1001
#define otPENCOLLECTION        1004
#define otBRUSHCOLLECTION      1005
#define otDIBCOLLECTION        1006
#define otDOUBLEDIBCOLLECTION  1007
#define otFONTCOLLECTION       1008
#define otSTRINGCOLLECTION     1009
#define otTEXTCOLLECTION       1010
#define otLOGTEXTCOLLECTION    1011
#define ot3DCOLLECTION         1012
#define otOBJECTCOLLECTION     1020
#define otPRIMARYCOLLECTION    1021
#define otDATAITEMS            1022
#define otMATERIALS3D          1024

#define otSPACE2D              17458
#define otSPACE3D              17459
#define otSPACE3Df             0x6444

// objecttypes. By GetObjectType(2/3)d

#define otGROUP2D         3
#define otRGROUP2D        4
#define otGROUP3D         5
#define otOBJECT3D        10
#define otCAMERA3D        11
#define otLIGHT3D         12
#define otLINE2D          20
#define otBITMAP2D        21
#define otDOUBLEBITMAP2D  22
#define otTEXT2D          23
#define otVIEW3D2D        24
#define otUSEROBJECT2D    25
#define otCONTROL2D       26
#define otEDITFRAME2D     50
#define otROTATECENTER2D  51
#define otFRAME3D         52
#define otAXIS3D          53



//Tool types. By GetToolType

#define ttPEN2D            101
#define ttBRUSH2D          102
#define ttDIB2D            103
#define ttDOUBLEDIB2D      104
#define ttFONT2D           105
#define ttSTRING2D         106
#define ttTEXT2D           107
#define ttREFTODIB2D       110
#define ttREFTODOUBLEDIB2D 111

#define ctINFONAME         200
#define ctINFOAUTOR        201
#define ctINFOINFO         202
#define ctCRDSYSTEM        203
#define stSPACEDATA        204
#define stOBJNAME          205
#endif