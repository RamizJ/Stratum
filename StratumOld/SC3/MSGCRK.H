#ifdef WIN32
#define EV_COMMAND \
		 HWND hwndCtl = (HWND) lParam;\
       int wNotifyCode = HIWORD(wParam);\
       int command = LOWORD(wParam);

#define EV_COMMANDnoVar \
		  hwndCtl = (HWND) lParam;\
        wNotifyCode = HIWORD(wParam);\
        command = LOWORD(wParam);

#define EV_COMMANDsimply \
       int command = LOWORD(wParam);
#define EV_COMMANDsimplyNoVar \
		command = LOWORD(wParam);

#else
#define EV_COMMAND \
		 HWND hwndCtl = (HWND) LOWORD(lParam);\
		 int wNotifyCode=HIWORD(lParam);\
		 int command=wParam;
#define EV_COMMANDnoVar \
		 hwndCtl = (HWND) LOWORD(lParam);\
		 wNotifyCode=HIWORD(lParam);\
		 command=wParam;

#define EV_COMMANDsimply \
	   int command = wParam;
#define EV_COMMANDsimplyNoVar \
	    command = LOWORD(wParam);	  
#endif

