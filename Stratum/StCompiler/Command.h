#ifndef OPERATOR_H
#define OPERATOR_H

#include <QString>
#include <QTextStream>
#include <memory>

namespace StData {
class Class;
class St2000DataStream;
}

namespace StCompiler {

class TplCursor;

class Command
{
public:
    Command(const QString& name = "",
            const QList<StData::Class*>& args = QList<StData::Class*>(),
            const QList<StData::Class*>& optionalArgs = QList<StData::Class*>(),
            StData::Class* returnType = nullptr);
    ~Command();

    QString name() const {return m_name;}
    qint16 outNum() const {return m_id;}

    qint16 startPriority() const {return m_startPriority;}
    qint16 endPriority() const {return m_endPriority;}

    QList<StData::Class*> args() const {return m_args;}
    QList<StData::Class*> optionalArgs() const {return m_optionalArgs;}
    StData::Class* returnType() const{return m_returnType;}

    int argsCount() {return m_args.count();}
    int optionalArgsCount() {return m_optionalArgs.count();}

    bool initialize(TplCursor& cursor);
    bool out(StData::St2000DataStream& stream, qint16 ix);
    void changeName(const QString& name);

//    QList<qint8> refArgsNumbers() const { return m_refArgsNumbers; }

    qint32 refArgsNumbers() const { return m_refArgsNumbers; }
    bool argIsRef(qint16 argIndex);

private:
    bool readOptionalArgs(TplCursor& cursor);
    bool readPriority(TplCursor& cursor);
    bool readArgs(TplCursor& cursor);
    bool readReturn(TplCursor& cursor);

    bool errorResult(const QString& msg);

private:
    QString m_name;                 //имя функции/оператора
    qint16 m_id;                    //(out) номер элемента в списке виртуальных команд (commands в wmachine.cpp)

    qint16 m_startPriority;         //(imp) начальный приоритет оператора в диапазоне
    qint16 m_endPriority;           //(imp2) конечный приоритет оператора в диапазоне

    StData::Class* m_returnType;    //(ret) ссылка на тип возвращаемого значения
//    QList<qint8> m_refArgsNumbers;  //(pointers) хранит номера аргументов оператора, являющихся ссылками
    qint32 m_refArgsNumbers;

    bool m_isOut;
    bool m_isIndex;
    bool m_isName;

    QList<StData::Class*> m_args;
    QList<StData::Class*> m_optionalArgs;
};

enum class OperatorType
{
    optr, opnd, dconst, iconst, hconst, fconst, sconst, eof, undef
};

}

#endif // OPERATOR_H
