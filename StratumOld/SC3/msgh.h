//class TMSGItems : public TCollection
//{
//    HWND HWindow;
//public:
//    HSP2D hsp;
//    int   lock;

//    TMSGItems(HWND _HWindow,HSP2D _hsp);
//    ~TMSGItems();

//    void  Update();

//    void  Register(TClassMSG * msg);

//    void  UnRegister(HOBJ2D obj,UINT16 code);

//    inline TClassMSG*  msg(C_TYPE i) { return (TClassMSG*)At(i); }

//    void virtual FreeItem(pointer p);

//    LRESULT WndProc(UINT message,WPARAM wParam,LPARAM lParam,BOOL &process);

//    BOOL RegisterObject(PObject po,HOBJ2D obj,INT16 code,UINT32 flags);

//    void DoneHandler(void*project,UINT msg=WM_DESTROY);
//};


