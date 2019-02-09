#define _USE_MATH_DEFINES

#include "MathFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"

#include <limits>
#include <ctime>
#include <math.h>

#include <Class.h>
#include <Object.h>
#include <QMessageBox>
#include <QObject>

#define tr(str) QObject::tr(str)
#define MIN_DOUBLE numeric_limits<double>::min()
#define MAX_DOUBLE numeric_limits<double>::max()

using namespace std;
using namespace StData;

namespace StVirtualMachine {

void setupMathFunctions()
{
    operations[VM_INCREMENT_AFTER] = incAfter;
    operations[VM_INC] = inc;
    operations[VM_INC_STEP] = incByValue;
    operations[VM_DEC] = dec;
    operations[VM_DEC_STEP] = decByValue;

    operations[V_ADD_F] = addF;
    operations[V_SUB_F] = subF;
    operations[V_MUL_F] = mulF;
    operations[V_DIV_F] = divF;

    operations[V_SIN] = sin_;
    operations[V_COS] = cos_;;
    operations[V_ASIN] = asin_;
    operations[V_ACOS] = acos_;
    operations[V_ATAN] = atan_;
    operations[V_TAN] = tan_;

    operations[V_EXP] = exp_;
    operations[V_SQRT] = sqrt_;
    operations[V_SQR] = sqr_;
    operations[V_LN] = ln_;
    operations[V_LG] = lg_;
    operations[V_LOG] = log_;
    operations[V_STEPEN] = pow_;

    operations[V_ED] = ed_;
    operations[V_EDI] = v_edi;
    operations[V_DELTA] = delta_;

    operations[V_MAX] = max_;
    operations[V_MIN] = min_;
    operations[VM_LIMIT] = limit_;
    operations[V_AVERAGE] = average_;
    operations[V_MOD] = mod_;
    operations[V_ROUND] = round_;
    operations[V_ROUND] = round_;
    operations[V_TRUNC] = trunk_;
    operations[V_ABS] = abs_;
    operations[V_SGN] = sgn_;

    operations[V_RAD] = rad_;
    operations[V_DEG] = deg_;

    operations[V_RANDOMIZE] = randomize_;
    operations[V_RANDOM] = rnd_;

    operations[V_UN_MINUS] = unMinus_;
    operations[VM_GETANGLEBYXY] = getAngleByXY;
}

void incAfter()
{
    int varIndex = codePointer->getCode();
    codePointer->incPosition();

    ObjectVar* var = variables->at(varIndex);
    var->setDouble(valueStack->getDouble() + 1);
    valueStack->incDouble();
}

void inc()
{
    VarData* varData = valueStack->popVarData();
    varData->d += 1;
}

void incByValue()
{
    double step = valueStack->popDouble();
    VarData* varData = valueStack->popVarData();
    varData->d += step;
}

void dec()
{
    VarData* varData = valueStack->popVarData();
    varData->d -= 1;
}

void decByValue()
{
    double step = valueStack->popDouble();
    VarData* varData = valueStack->popVarData();
    varData->d -= step;
}

void addF()
{
    double arg1 = valueStack->getDouble(1);
    double arg2 = valueStack->getDouble();
    valueStack->setDouble(arg1 + arg2, 1);
    valueStack->incPosition();
}

void subF()
{
    double arg1 = valueStack->getDouble(1);
    double arg2 = valueStack->getDouble();
    valueStack->setDouble(arg1 - arg2, 1);
    valueStack->incPosition();
}

void mulF()
{
    double arg1 = valueStack->getDouble(1);
    double arg2 = valueStack->getDouble();
    valueStack->setDouble(arg1 * arg2, 1);
    valueStack->incPosition();
}

void divF()
{
    double arg2 = valueStack->getDouble();
    if(arg2 == 0.0)
    {
//        QMessageBox::critical(nullptr,
//                              tr("Ошибка вычислений имиджа"),
//                              QString(tr("Деление на ноль в имидже: '%1'")).arg(executedObject->cls()->originalName()),
//                              );
        VmLog::instance().error(QString(tr("Деление на ноль в имидже: '%1'")).arg(executedObject->cls()->originalName()));
        valueStack->setDouble(0, 1);
        valueStack->incPosition();
        return;
        // throw std::invalid_argument(QString(tr("Деление на ноль")).toStdString());
    }
    double arg1 = valueStack->getDouble(1);
    valueStack->setDouble(arg1 / arg2, 1);
    valueStack->incPosition();
}

void sin_()
{
    double value = sin(valueStack->getDouble());
    valueStack->setDouble(value);
}

void cos_()
{
    double value = cos(valueStack->getDouble());
    valueStack->setDouble(value);
}

void tan_()
{
    double value = tan(valueStack->getDouble());
    valueStack->setDouble(value);
}

void asin_()
{
    double arg = valueStack->getDouble();
    double value = asin(arg);

    if(std::isnan(value))
        throwDomainError(QString(tr("Ошибка вычисления арксинуса для аргумента: %1")).arg(arg));

    valueStack->setDouble(value);
}

void acos_()
{
    double arg = valueStack->getDouble();
    double value = acos(arg);

    if(std::isnan(value))
        throwDomainError(QString(tr("Ошибка вычисления арккосинуса для аргумента: %1")).arg(arg));

    valueStack->setDouble(value);
}

void atan_()
{
    double value = atan(valueStack->getDouble());

    if(std::isnan(value))
        throwDomainError(QString(tr("Ошибка вычисления арктангенса для аргумента: %1")).arg(value));

    valueStack->setDouble(value);
}

void exp_()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value > 60 ? MAX_DOUBLE : exp(value));
}

void sqrt_()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value >= 0.0 ? sqrt(value) : 0.0);
}

void sqr_()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value * value);
}

void ed_()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value > 0.0 ? 1.0 : 0.0);
}

void v_edi()
{
    qint32 value = valueStack->popInt32();
    valueStack->pushDouble(value > 0 ? 1.0 : 0.0);
}

void delta_()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value == 0.0 ? 1.0 : 0.0);
}

void ln_()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value > 0 ? log(value) : MIN_DOUBLE);
}

void lg_()
{
    double value = valueStack->popDouble();

    value = value > 0 ? log(value) : MIN_DOUBLE;
    valueStack->pushDouble(value/log(10));
}

void log_()
{
    double a = valueStack->popDouble();
    double n = valueStack->popDouble();
    double logA = a > 0 ? log(a) : MIN_DOUBLE;
    double logN = n > 0 ? log(n) : MIN_DOUBLE;
    valueStack->pushDouble(logA / logN);
}

void pow_()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    valueStack->pushDouble(pow(x, y));
}

void max_()
{
    double a = valueStack->getDouble();
    double b = valueStack->getDouble(1);
    valueStack->setDouble(a > b ? a : b, 1);
    valueStack->incPosition();
}

void min_()
{
    double a = valueStack->getDouble();
    double b = valueStack->getDouble(1);
    valueStack->setDouble(a < b ? a : b, 1);
    valueStack->incPosition();
}

void average_()
{
    double a = valueStack->getDouble();
    double b = valueStack->getDouble(1);
    valueStack->setDouble((a + b)/2, 1);
    valueStack->incPosition();
}

void limit_()
{
    double val_max = valueStack->popDouble();
    double val_min = valueStack->popDouble();
    double value = valueStack->popDouble();

    value = qMax(qMin(value, val_max), val_min);
    valueStack->pushDouble(value);
}

void mod_()
{
    double a = valueStack->getDouble(1);
    double b = valueStack->getDouble();
    valueStack->setDouble(fmod(a, b), 1);
    valueStack->incPosition();
}

void round_()
{
    double digit = floor(valueStack->popDouble());
    double a = valueStack->popDouble();
    double r;

    if(digit > 14)
        r = a;
    else
    {
        int i;
        for(i = 0; i < digit; i++)
            a *= 10;
        r=floor(a);

        if((a - r) > 0.5)
            r += 1.0;

        for(i = 0; i < digit; i++)
            r /= 10;
    }
    valueStack->pushDouble(r);
}

void trunk_()
{
    double r = floor(valueStack->popDouble());
    r = floor(r);
    valueStack->pushDouble(r);
}

void abs_()
{
    valueStack->setDouble(fabs(valueStack->getDouble()));
}

void sgn_()
{
    double a = valueStack->getDouble();
    if(a < 0)
        a = -1.0;
    else
    {
        if(a > 0)
            a = 1.0;
        else
            a = 0.0;
    }
    valueStack->setDouble(a);
}

void rad_()
{
    valueStack->setDouble(valueStack->getDouble() * M_PI / 180);
}

void deg_()
{
    valueStack->setDouble(180.0 * valueStack->getDouble() / M_PI);
}

void rnd_()
{
    double value = valueStack->getDouble();
    double rndValue = ((double)rand() / (double)RAND_MAX);
    valueStack->setDouble(value * rndValue);
}

void randomize_()
{
    double value = valueStack->popDouble();
    if(value < 0)
        srand(time(nullptr));
    else
        srand(value);
}

void unMinus_()
{
    valueStack->setDouble(-valueStack->getDouble());
}

void throwDomainError(const QString& str)
{
    throw domain_error(str.toStdString());
}

void getAngleByXY()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();

    double f=0.0;
    if (y==0) //защита от деления на 0.
        f = (x >= 0) ? 0.0 : M_PI;
    else if (y > 0)
        f = -atan(x / y) + M_PI_2;
    else
        f = -atan(x / y) + M_PI * 1.5;

    valueStack->pushDouble(f);
}

}
