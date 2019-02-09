class TPopupTool:public TWindow{
 POINT size;
public:
 HWND tool;
 int refcount;
 int id;
 char * name;
 char   prefix[32];
 UINT32 style;
 TPopupTool(LPCREATESTRUCT,HWND);
 ~TPopupTool();
 virtual void EvSize(WORD sizeType,int sx,int sy);
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 void    SetChild(HWND,int flags); // 1 size from child
 BOOL static  GetPosVars(char*,char*,char*);
};
