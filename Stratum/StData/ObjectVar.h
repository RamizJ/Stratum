#ifndef OBJECTVAR
#define OBJECTVAR

#include "stdata_global.h"

#include <QString>
#include <memory>

namespace StData {

class ClassVarInfo;
class Class;

struct STDATASHARED_EXPORT VarData
{
    QString s;
    union
    {
        double d;
        qint32 i;
        qint16 w;
        quint8 b;
        void* v;
    };

    VarData()
    {
        reset();
    }

    inline void reset()
    {
        s = "";
        d = 0.0;
        i = 0;
        v = nullptr;
    }

    inline void setDouble(double value) { d = value; }
    inline void setInt(int value) { i = value; }
    inline void setString(const QString& value) { s = value; }
    inline void setValue(void* value) { v = value; }
};

typedef std::shared_ptr<VarData> VarDataPtr;

class STDATASHARED_EXPORT ObjectVar
{
public:
    explicit ObjectVar(ClassVarInfo* info);
    ~ObjectVar(){}

    ClassVarInfo* info() const {return m_varInfo;}

    void setData(const QString& data);
//    void setOldData(const QString& data);
    VarDataPtr varData() const {return m_varData;}
    void setVarData(const VarDataPtr& varData) {m_varData = varData;}

    VarData* savedVarData() { return &m_savedVarData; }

    quint8 byteValue() const {return m_varData->b;}
    qint16 wordValue() const {return m_varData->w;}
    qint16 handleValue() const {return m_varData->w;}
    qint32 intValue() const {return m_varData->i;}
    double doubleValue() const {return m_varData->d;}
    QString stringValue() const {return m_varData->s;}
    void* ptrValue() const {return m_varData->v;}

    quint8 savedByteValue() const {return m_savedVarData.b;}
    qint16 savedWordValue() const {return m_savedVarData.w;}
    qint16 savedHandleValue() const {return m_savedVarData.w;}
    qint32 savedIntValue() const {return m_savedVarData.i;}
    double savedDoubleValue() const {return m_savedVarData.d;}
    QString savedStringValue() const {return m_savedVarData.s;}

    void setDouble(double value);
    void setInt(qint32 value);
    void setString(const QString& str);
    void setVar(ObjectVar* var);

    void incDouble(double value = 1);
    void incInt(int value = 1);

    void setSavedDouble(double value);
    void setSavedInt(qint32 value);
    void setSavedString(const QString& str);

    void save();        //В старой версии CopyState(NEW_TO_OLD)
    void restore();     //В старой версии CopyState(OLD_TO_NEW)
    void saveAsStartup();

    Class* type();
    QString typeName();

    void toDefaultState();
    void clear();
    void toStartupState();
    QString toString();


private:
    void setData(VarData& varData, const QString& data);
//    void copyData(VarData& source, VarData& target);

private:
    ClassVarInfo* m_varInfo;

    VarData m_defaultData, m_savedVarData, m_startupData;
    VarDataPtr m_varData;

};

}

#endif // OBJECTVAR

