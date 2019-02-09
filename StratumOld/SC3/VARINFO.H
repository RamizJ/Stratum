class _TObj;

class TObjVarData
{
public:
    PObject object;
    INT16 index;
    _TObj* root;
    void* var;

    TObjVarData(PObject po,INT16 _index);
};

class _TVar
{
public:
    _TObj*o;
    INT16 index;
    _TVar*next;
};

class _TObj
{
public:
    _TObj*next;
    _TObj*items;
    _TVar*vars;

    TObjVarData*data;
    PObject object;

    _TObj();
    void Insert(_TObj*);
    void InsertVar(INT16);
    ~_TObj();
};
