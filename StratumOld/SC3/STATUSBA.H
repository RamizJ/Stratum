class TStatus;
class TStatusBar:public TWindow{
 int sizey;
 int sizex;
 BOOL simple;
 char simpletext[256];
 TCollection *items;
 HBITMAP     icons;
 int         isizex,isizey;
 POINT       offset;
 int         status;
 public:

 TStatusBar(LPCREATESTRUCT,HWND);
 ~TStatusBar();
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 virtual void EvSize(WORD sizeType,int sx,int sy);
 void SetSimpleText(char*);
 void SetText(int id,char*);
 void SetIcon(int ,int);
 void Rebuild();
 virtual BOOL GetBallonStr(BALLONSTRUCT *bs);
 virtual void EvRButtonDown(WORD modKeys,int x,int y);
 virtual void EvLButtonDblClk(WORD modKeys,int x,int y);
 void SetStatusMode(BOOL mode);
 TStatus* GetStatusByXY(int x,int y);
 DoneIcons(); // Отгрузили иконки
 virtual void EvMouseMove(WORD modKeys,int x,int y);
 virtual void EvLButtonDown(WORD modKeys,int x,int y);
 virtual void EvLButtonUp(WORD modKeys,int x,int y);

private:
 TStatus *GetStatus(int id);
 void DrawProgressBar(HDC hdc,int i);
 void DoneItems();
 void Paint(HDC);
 void PaintFrame(HDC,int x,int size,int dy=0); // уменьшение Y
 void DrawText(HDC,int x,int size,char*,BOOL center);
 void PaintIcon(HDC,TStatus*,int id,BOOL center);
 void ReCalc();
 };
#define STATUS_STEPS      -1
#define STATUS_PERFOMANCE -2
#define STATUS_ERRICON    -3
