#ifndef gr_tools__
#define gr_tools__

struct GETFILETYPE{
 char *file;
 char group[64];     // группа TABLES,RASTER
 char type[128];     // DBASE,SYBASE,PARADOX
 char advansed[64];  // разные параметры
 char filter[256];   // имя фильтра, если есть
 UINT32 flags;
 BOOL Export;
};

HOBJ2D ImportBMP(HSP2D hsp, char *s,int type=0,HOBJ2D* ddib=NULL);
HOBJ2D ImportDBM(HSP2D hsp,char *s,int type=0);
HOBJ2D ReadRTF(HSP2D hsp,TStream*st);
BOOL   WriteRTF(HSP2D hsp,HOBJ2D ht,TStream*st);
HOBJ2D MakeSingleGroup(HSP2D hsp,BOOL * created=NULL);
void   DeleteAllUnusedTool2d(HSP2D hsp);
HSP2D  GetDuplicateSpace(HSP2D);
BOOL   FullDeleteObject2d(HSP2D hsp,HOBJ2D obj);
BOOL   FullDeleteTool2d(HSP2D hsp,TOOL_TYPE,HOBJ2D obj);
BOOL   IsVideoFrame2d(HSP2D hsp,HOBJ2D obj);
HGLOBAL _CreateDIB2d(int sizex,int sizey,int bitCount,COLORREF bkcolor);
double GetScaleSpace2d(HSP2D hsp);
LPBITMAPINFOHEADER CreateScreenDIB(HWND hw, double x,double y,double w,double h, HSP2D hsp=0, int attr=0,char **bits=0);
BOOL   SaveSpaceImage2d(HSP2D hsp,char *file,int attr,double x,double y,double nWidth,double nHeight);
BOOL   SpaceSaveAs(HSP2D hsp,char*,int type);
BOOL   DefSpaceWindowProc(HSP2D hsp, HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);
BOOL   SpaceExchange(HSP2D &hsp,HOBJ2D &hobject,GETFILETYPE&ft);
BOOL   SimplyExchange(HSP2D&hsp,char *file,char *ext,BOOL export);
HSP2D  LdSpace2d(HWND hw,char *file);
HSP2D LdFromMemory2d(HWND hw,HGLOBAL bl,char *path);

void  GetTargetPoint3d(CAMERA3D&,POINT3D&p);
BOOL  TransformCamera3d(HSP2D,HOBJ2D,CAMERA3D&,int mode,double,double=0,double=0);

BOOL GetMaterialText3d(HSP3D hsp3d,HOBJ2D tool,char*ps,BOOL adduse=FALSE);
BOOL AddUseToolText2d(HSP2D hsp,TOOL_TYPE,HOBJ2D tool,char *ps);
class  TRestoreCrd{
HSP2D hsp;
public:
 TRestoreCrd(HSP2D _hsp);
 ~TRestoreCrd();
};

//BOOL  ApplyTexture(HSP3D,HOBJ2D,HOBJ2D,mapmode+flags,th,tv,tminc,tmaxc);
BOOL  ApplyTexture(HSP3D,HOBJ2D,HOBJ2D,HOBJ2D texture,int,double,double,double,double);
BOOL  RemoveTexture3d(HSP3D hsp3d,HOBJ2D object3d);
BOOL  ReplaceTexture3d(HSP3D hsp3d,HOBJ2D object3d,HOBJ2D hold,HOBJ2D hnew);
HOBJ2D ImportObject3d(HSP3D hsp3d,char * file,int type);
BOOL isRectEmpty(RECT2D r);
BOOL RectIntersect(RECT2D r,RECT2D r1);
void MakeRect(RECT2D&r,POINT2D&org,POINT2D&size);
enum CONTROLS{CNT_NONE,CNT_LISTBOX,CNT_COMBOBOX,CNT_BUTTON,CNT_EDIT};
CONTROLS _GetControlType(HWND hwnd);
#endif gr_tools__
