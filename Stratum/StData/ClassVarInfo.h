#ifndef CLASSVARINFO_H
#define CLASSVARINFO_H

#include "stdata_global.h"
#include "FlagHelper.h"

#include <QString>

namespace StData {

class Class;
class St2000DataStream;

class STDATASHARED_EXPORT ClassVarInfo
{
public:
    enum VarFlags
    {
        Local = SHL(1), NoRead = SHL(2), NoWrite = SHL(3), Static = SHL(4), OnScheme = SHL(5),
        Parameter = SHL(6), Left = SHL(7), EqVar = SHL(8), Argument = SHL(9), Return = SHL(10),
        NoSave = SHL(11), Global = SHL(12), ClassGlobal = SHL(13),
        NotFixed = 0x10000l, ByCompiler = 0x20000l
    };
    Q_DECLARE_FLAGS(ClassVarFlags, VarFlags)

public:
    explicit ClassVarInfo(const QString& varName = "", Class* varType = nullptr);
    ClassVarInfo(const ClassVarInfo& varInfo);
    ~ClassVarInfo(){}

    QString varName() const {return m_varName;}
    void setVarName(const QString& varName) {m_varName = varName;}

    QString info() const {return m_info;}
    void setInfo(const QString& info) {m_info = info;}

    QString defValue() const {return m_defValue;}
    void setDefValue(const QString& defValue) {m_defValue = defValue;}

    quint32 flags() const {return m_flags;}
    void setFlags(const quint32& flags) {m_flags = flags;}
    void addFlags(const quint32& flags) {m_flags != flags;}

    void load(St2000DataStream& stream, bool hasDefValue);
    static void skip(St2000DataStream& stream, bool hasDefValue);

    Class* varType() const { return m_varType; }
    void setVarType(Class* varType) { m_varType = varType; }

    bool isEnableVar() const;
    bool isDisableVar() const;

private:
    QString m_varName;
    QString m_info;
    QString m_defValue;

    Class* m_varType;

    quint32 m_flags;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ClassVarInfo::ClassVarFlags)

}

#endif // CLASSVARINFO_H
