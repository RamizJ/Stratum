#ifndef STCOMPILER_H
#define STCOMPILER_H

#include "VarStack.h"
#include "stcompiler_global.h"

#include <QHash>
#include <QObject>
#include <QSet>
#include <QStack>
#include <QString>
#include <memory>

#define VF_LOCAL     2
#define VF_NOREAD    4
#define VF_NOWRITE   8
#define VF_STATIC    16
#define VF_ONSCHEME  32
#define VF_PARAMETER 64
#define VF_LEFT      128
#define VF_EQVAR     256
#define VF_ARGUMENT  512
#define VF_RETURN    1024
#define VF_NOSAVE    2048
#define VF_GLOBAL    4096
#define VF_CLASSGLOBAL 8192


namespace StData {
class Project;
class Class;
class St2000DataStream;
class SystemLog;
class ClassVarInfo;
class Equation;
}

namespace StCompiler {

class Command;
class TplCursor;
class CommandCollection;
class CompileVarCollection;
class MacroCollection;
class CompileError;

class STCOMPILERSHARED_EXPORT Compiler : public QObject
{
    Q_OBJECT

public:
    Compiler(QObject* parent = nullptr);
    ~Compiler();

    bool initialize();

    bool compile(StData::Class* cls, const QString& newSourceCodeText, bool& isNeedRelinkVars);
    bool compile(StData::Class* cls, CompileVarCollection* varData,
                 const QString newSourceCode, StData::St2000DataStream& byteCodeStream);

    std::shared_ptr<CompileError> lastError() const {return m_lastError;}

//    StCore::Project* project() const {return m_project;}
//    void setProject(StCore::Project* project) {m_project = project;}

private:
    enum class State {Normal, If, Else, While, Until, Switch, For};

private:
    bool readString(State state, qint16 label0 = 0, QStack<qint16>* plabelB = 0);
    bool readMString(qint16 chStack = 0);
    qint16 readDefVars(const QString& typeName);

    qint16 setLabel(qint16 dest = -1);
    bool linkLabel(qint16 position); // текущую позицию поставить в переход
    bool readVars(qint16 flag);

    bool analizeOperator(qint16 index, const QString& funcName = "");
    bool analizeEquation();

    qint16 checkStack(bool isErrorDisabled = false);
    qint16 checkVarStack();
    bool checkEquationStack();

    bool operatorOut(Command* cmd, qint16 ix = 0);
    bool functionOut();
    bool functionOut2(qint16 ix, QList<StData::Class*> Args, qint16 ArgCount);
    bool functionOut3(const QString& functionName, QList<StData::Class*> Args, qint16 ArgCount);
    bool functionOut4(const QString& functionName, const QList<StData::Class*>& Args, qint16 ArgCount);
    qint16 varOut(const QString varName);
    qint16 constantOut(const QString& type);

    StData::ClassVarInfo* insertVar(qint16& ix, QString varName, QString type = "");

    bool errorResult(int errorCode, const QString& errorMessage);

    void restoreVariables(StData::Class* cls, CompileVarCollection* varData);

private:
    QString funcSpecToStr(const QString& funcName, QList<StData::Class*> args,
                          bool hasOptionalArgs = true, bool argsReversed = false);

private:
//    StCore::Project* m_project;
    QString m_compilerTplPath;
    TplCursor* m_cursor;
    StData::St2000DataStream* m_byteCodeStream;
    StData::Class* m_class;
    QByteArray m_byteCodeBuffer;

    std::shared_ptr<Command> m_epilog;
    std::shared_ptr<CommandCollection> m_operators;
    std::shared_ptr<CommandCollection> m_functions;
    std::shared_ptr<MacroCollection> m_constants;

    qint16 m_onePhaseMode;
    qint16 m_eqMode;
    qint16 m_fnMode;
    qint16 m_newModificator;
    qint16 m_isDiffer;

    QString m_firstString;
    //индексы
    qint16 m_functionIndex, m_unaryIndex;
    qint16 m_errX1, m_errX2;
    qint16 m_virtualFunctionIndex, m_dllFunctionIndex;
    qint16 m_getElementIndex, m_setElementIndex;
    QString m_errText;

//    std::shared_ptr<Command> m_epilog;
//    QMultiHash<qint32, Command*> m_operatorDictionary;
//    QMultiHash<qint32, Command*> m_functionDictionary;
//    QHash<QString, double> m_constantsDictionary;

    QStack<qint16> m_indexStack;
    QStack<qint16> m_funcIndexStack;

    VarStack m_varStack;
    QStack<QString> m_funcNameStack;
    QStack<StData::Equation*> m_equationStack;
    QStack<QString> m_switchVarStack;

    CompileVarCollection* m_varData;
    StData::Equation* m_rootEquation;

    std::shared_ptr<CompileError> m_lastError;
};

}

#endif // STCOMPILER_H
