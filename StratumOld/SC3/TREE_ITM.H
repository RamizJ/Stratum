#define TREE_LIBRARY     1
#define TREE_CLASS       DATA_CLASS
#define TREE_OBJECT      3
#define TREE_VARSclass   4
#define TREE_VARSobj     5
#define TREE_CHILDSclass 6
#define TREE_CHILDSobj   7
#define TREE_VARclass    8
#define TREE_VARobj      9
#define TREE_STRATUM     10
#define TREE_SPACES      11
#define TREE_SPACE       12
#define TREE_VIDEOS      13
#define TREE_EMPTYCLASS  14

#define TREE_VIDEO       17

#define TREE_DBE         18
#define TREE_BASE        19
#define TREE_TABLE       20
#define TREE_SCFS        21   // sc file system
#define TREE_FILE        22

#define TREE_EQS         29
#define TREE_EQSinfo     30
#define TREE_EQSvar      31
#define TREE_EQSeq       32
#define TREE_EQSmx       33

#define TREE_MATMAN      50
#define TREE_MATRIX      51

#define TREE_TOOLS       52
#define TREE_PENS        53
#define TREE_BRUSHS      54
#define TREE_DIBS        55
#define TREE_DDIBS       56
#define TREE_FONTS       57
#define TREE_STRINGS     58
#define TREE_TEXTS       59
#define TREE_TOOL1       60
#define TREE_TEXTURE3D   61

#define TREE_WINMAN      70
#define TREE_WINITEM     71

#define TREE_OBJECT2D    72
#define TREE_OBJECTS2D   73
#define TREE_SPACE3D     74
#define TREE_OBJECT3D    75

#define TREE_VARinfoOBJ  80
#define TREE_VARinfoVAR  81

#define TREE_DYNOMEMORY  82
#define TREE_DYNOVAR     83
#define TREE_DYNOITEM    84
#define TREE_DYNOSUBITEM  85

#define TREE_PLUGINS     90
#define TREE_PLUGIN      91
#define TREE_DLLFUNC     92

#define TREE_ANIMATE     93
#define TREE_ANIITEM     94

#define TREE_MAIN       256
#define MESSAGE_WND     257


int __InsertObject2d(HWND tree,DWORD parent,HSP2D hsp,HOBJ2D obj);
int __InsertSpace3d2d(HWND tree,DWORD parent,HSP3D hsp3d);
