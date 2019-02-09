
#define SYNTAX_ITEMS_C 15
#define SYNTAX_ITEMS_G 10
class TSColor
{
public:
    COLORREF bkgrndsyntax, textsyntax;
    int      groups;
    BOOL     modifyed;
    char * file;

    COLORREF fg[SYNTAX_ITEMS_C];
    COLORREF bk[SYNTAX_ITEMS_C];
    COLORREF _fg[SYNTAX_ITEMS_G];
    COLORREF _bk[SYNTAX_ITEMS_G];
    BYTE      style[SYNTAX_ITEMS_C];
    BYTE     _style[SYNTAX_ITEMS_G];

public:
    TSColor();
    TSColor(TSColor&);
    ~TSColor();

    void Rebuild(char *);
    void FromFile(char*);
    void ToFile(char*);

    void SetAuto();
    void GetColor(char*,COLORREF&f,COLORREF&b,BYTE&);
    void Set(HWND);
private:
    void Reset();
    void WriteStr(char*file,char*group,COLORREF f,COLORREF b,BYTE);
};


