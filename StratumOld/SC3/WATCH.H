
class TWatch
{
public:
    PObject object;
    char * name;
    char * info;
    INT16 index; // index peremennoi
    INT16 count;
    INT16 path[1];
    void UpdateVarIndex();
    BOOL Update(TProject*);
};
