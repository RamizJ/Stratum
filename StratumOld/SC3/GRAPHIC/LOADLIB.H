
typedef struct tagRENDERLIB{
 char Name[RENDER_NAME_SIZE];
 HINSTANCE lib;
 INT32 (WINAPI *MessageProc)(INT16,INT32,INT32);
}RENDERLIB;

class TRenders3d: public TSortedCollection{
 public:
 TRenders3d():TSortedCollection(2,2){};
 virtual int  Compare(pointer key1,pointer key2);
 virtual void FreeItem(pointer );
 RENDERLIB * GetLib(char*);
};

void *GetRenderByName(char*);
