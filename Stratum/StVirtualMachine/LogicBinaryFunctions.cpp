#include "LogicBinaryFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

namespace StVirtualMachine {

void setupLogicBinaryFunctions()
{
    operations[V_AND] = v_and;
    operations[V_OR] = v_or;
    operations[V_XOR] = v_or;
    operations[V_NOT] = v_not;

    operations[V_ANDI] = v_andi;
    operations[V_ORI] = v_ori;
    operations[V_NOTI] = v_noti;

    operations[V_JMP] = v_jmp;
    operations[V_JNZ] = v_jnz;
    operations[V_JZ] = v_jz;
    operations[V_JNZ_HANDLE] = v_jnz_handle;
    operations[V_JZ_HANDLE] = v_jz_handle;

    operations[V_EQUAL] = v_equal;
    operations[V_NOTEQUAL] = v_notequal;
    operations[V_MORE] = v_more;
    operations[V_MOREorEQUAL] = v_moreorequal;
    operations[V_LOW] = v_low;
    operations[V_LOWorEQUAL] = v_loworequal;
    operations[V_EQUALI] = v_equali;
    operations[V_NOTEQUALI] = v_notequali;

    operations[S_EQUAL] = s_equal;
    operations[S_NOTEQUAL] = s_notequal;
    operations[S_MORE] = s_more;
    operations[S_MOREorEQUAL] = s_moreorequal;
    operations[S_LOW] = s_low;
    operations[S_LOWorEQUAL] = s_loworequal;

    operations[V_AND_BINARY] = v_and_binary;
    operations[V_OR_BINARY] = v_or_binary;
    operations[V_NOTbin] = v_notbin;
    operations[V_XORBIN] = v_xorbin;
    operations[V_SHR] = v_shr;
    operations[V_SHL] = v_shl;
}

void v_and()
{
    double value = valueStack->getDouble() && valueStack->getDouble(1);
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_or()
{
    double value = valueStack->getDouble() || valueStack->getDouble(1);
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_notbin()
{
    quint32 value = valueStack->getDouble();
    valueStack->setDouble(~value, 1);
}

void v_xorbin()
{
    quint32 v = valueStack->getDouble();
    quint32 v1 = valueStack->getDouble(1);

    valueStack->setDouble(v ^ v1, 1);
    valueStack->incPosition();
}

void v_xor()
{
    quint32 a = (valueStack->getDouble() != 0);
    quint32 b = (valueStack->getDouble(1) != 0);

    valueStack->setDouble(a ^ b);
    valueStack->incPosition();
}

void v_not()
{
    double value = valueStack->getDouble();
    valueStack->setDouble(value == 0 ? 1.0 : 0.0);
}

void v_andi()
{
    qint32 value = valueStack->getInt32() && valueStack->getInt32(1);
    valueStack->setInt32(value, 1);
    valueStack->incPosition();
}

void v_ori()
{
    qint32 value = valueStack->getInt32() || valueStack->getInt32(1);
    valueStack->setInt32(value, 1);
    valueStack->incPosition();
}

void v_noti()
{
    qint32 i = valueStack->popInt32();
    valueStack->pushDouble(i == 0 ? 1.0 : 0.0);
}

void v_jmp()
{
    qint16 jmpPos = codePointer->getCode();
    codePointer->setPosition(jmpPos);
}

void v_jz()
{
    double jm = valueStack->popDouble();
    qint16 ncp = codePointer->getCode();

    if(!jm)
        codePointer->setPosition(ncp);
    else
        codePointer->incPosition();
}

void v_jnz()
{
    double jm = valueStack->popDouble();
    qint16 ncp = codePointer->getCode();

    if(jm)
        codePointer->setPosition(ncp);
    else
        codePointer->incPosition();
}

void v_jz_handle()
{
    qint32 jm = valueStack->popInt32();
    qint16 ncp = codePointer->getCode();

    if(!jm)
        codePointer->setPosition(ncp);
    else
        codePointer->incPosition();
}

void v_jnz_handle()
{
    qint32 jm = valueStack->popInt32();
    qint16 ncp = codePointer->getCode();

    if(jm)
        codePointer->setPosition(ncp);
    else
        codePointer->incPosition();
}

void v_equal()
{
    bool value = (valueStack->getDouble(1) == valueStack->getDouble());
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_notequal()
{
    bool value = (valueStack->getDouble(1) != valueStack->getDouble());
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_more()
{
    double value = valueStack->getDouble(1) > valueStack->getDouble();
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_moreorequal()
{
    double value = valueStack->getDouble(1) >= valueStack->getDouble();
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_low()
{
    double value = valueStack->getDouble(1) < valueStack->getDouble();
    valueStack->setDouble(value, 1);
    valueStack->incPosition();}

void v_loworequal()
{
    double value = valueStack->getDouble(1) <= valueStack->getDouble();
    valueStack->setDouble(value, 1);
    valueStack->incPosition();
}

void v_equali()
{
    qint32 p1 = valueStack->popInt32();
    qint32 p2 = valueStack->popInt32();
    valueStack->pushDouble(p1 == p2);
}

void v_notequali()
{
    qint32 p1 = valueStack->popInt32();
    qint32 p2 = valueStack->popInt32();
    valueStack->pushDouble(p1 != p2);
}

void s_equal()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();
    valueStack->pushDouble(s1 == s2);
}

void s_notequal()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();
    valueStack->pushDouble(s1 != s2);
}

void s_more()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();
    valueStack->pushDouble(s1 > s2);
}

void s_moreorequal()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();
    valueStack->pushDouble(s1 >= s2);
}

void s_low()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();
    valueStack->pushDouble(s1 < s2);
}

void s_loworequal()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();
    valueStack->pushDouble(s1 <= s2);
}

void v_and_binary()
{
    quint32 v1 = (qint32)valueStack->getDouble(1);
    quint32 v = (qint32)valueStack->getDouble();
    valueStack->setDouble(v1 & v, 1);
    valueStack->incPosition();
}

void v_or_binary()
{
    quint32 v1 = (qint32)valueStack->getDouble(1);
    quint32 v = (qint32)valueStack->getDouble();
    valueStack->setDouble(v1 | v, 1);
    valueStack->incPosition();
}

void v_shl()
{
    quint32 p = valueStack->popDouble();
    quint8 b = valueStack->popDouble();
    p = p << b;
    valueStack->pushDouble(p);
}

void v_shr()
{
    quint32 p = valueStack->popDouble();
    quint8 b = valueStack->popDouble();
    p = p >> b;
    valueStack->pushDouble(p);
}


}
