#ifndef EQUATION_H
#define EQUATION_H

#include "stdata_global.h"

#include <QObject>
#include <memory>

#define V_ADD_F             18
#define V_SUB_F             19
#define V_DIV_F             20
#define V_MUL_F             21
#define V_MOD               22
#define V_UN_MINUS          113
#define V_STEPEN            32    // 2

#define EC_CONST            0
#define EC_EQSTRING         16383
#define EC_VAR              16384
#define EC_EQDIF            16385
#define EC_UNMINUS          V_UN_MINUS
#define EC_PLUS             V_ADD_F
#define EC_MINUS            V_SUB_F
#define EC_MUL              V_MUL_F
#define EC_DIV              V_DIV_F
#define EC_STEPEN           V_STEPEN

namespace StData
{

class St2000DataStream;
class Equation;

class STDATASHARED_EXPORT Equation : public QObject
{
    Q_OBJECT

public:
    explicit Equation(QObject *parent = 0);
    ~Equation();

    qint32 code() const {return m_code;}
    void setCode(const qint32& code) {m_code = code;}

    qint16 index() const {return m_index;}
    void setIndex(const qint16& index) {m_index = index;}

    qreal value() const {return m_value;}
    void setValue(const qreal& value) {m_value = value;}

    qint16 flag() const {return m_flag;}
    void setFlag(const qint16& flag) {m_flag = flag;}

    QList<Equation*> equations() const;
    QList<Equation*> childEquations() const;

    void addEquation(Equation* equation);
    void addEquations(QList<Equation*> equations);

    void addChildEquation(Equation* equation);
    void addChildEquations(QList<Equation*> equations);

    void load(St2000DataStream& stream);
    static void skip(St2000DataStream& stream);

private:
    enum DataCodes {Var = 16384, Const = 0};

private:
    qint32 m_code;

    qint16 m_index;
    qreal m_value;
    qint16 m_flag;

    QList<Equation*> m_equations;
    QList<Equation*> m_childEquations;
};

}
#endif // EQUATION_H
