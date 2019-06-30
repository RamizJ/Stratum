#include "StringFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <Log.h>
#include <StDataGlobal.h>
#include <StringHelper.h>

#include <QString>
#include <QDebug>
#include <QTextCodec>

using namespace StData;

namespace StVirtualMachine {

void setupStringFunctions()
{
    operations[SCHANGE] = schange;
    operations[PLUS_STRING] = plusString;
    operations[LEFT_STRING] = leftString;
    operations[RIGHT_STRING]= rightString;
    operations[SUBSTR_STRING] = substrString;
    operations[VM_SUBSTR1_STRING] = substr1String;
    operations[POS_STRING] = posString;
    operations[REPLICATE_STRING] = replicateString;
    operations[LOWER_STRING] = lowerString;
    operations[UPPER_STRING] = upperString;
    operations[ANSI_TO_OEM_STRING] = ansiToOem;
    operations[OEM_TO_ANSI_STRING] = oemToAnsi;
    operations[VM_COMPARE_STRING] = compareString;
    operations[VM_COMPAREI_STRING] = compareiString;
    operations[LENGTH_STRING] = lengthString;
    operations[LTRIM_STRING] = trimLeftString;
    operations[RTRIM_STRING] = trimRightString;
    operations[ALLTRIM_STRING] = trimAllString;

    operations[ASCII_STRING] = asciiString;
    operations[CHR_STRING] = chrString;

    operations[FLOAT_TO_STRING] = floatToString;
    operations[STRING_TO_FLOAT] = stringToFloat;

    operations[PLUS_STRING_FLOAT] = plusStringFloat;
    operations[PLUS_FLOAT_STRING] = plusFloatString;
}

void schange()
{
    QString newValue = valueStack->popString();
    QString oldValue = valueStack->popString();
    QString str = valueStack->popString();
    valueStack->pushString(str.replace(oldValue, newValue));
}

void plusString()
{
    QString s2 = valueStack->popString();
    QString s1 = valueStack->popString();

    valueStack->pushString(s1 + s2);
}

void leftString()
{
    int n = (int)valueStack->popDouble();
    QString s = valueStack->popString();

    valueStack->pushString(s.left(n));
}

void rightString()
{
    int n = (int)valueStack->popDouble();
    QString s = valueStack->popString();

    if(n <= 0)
        valueStack->pushString("");
    else
        valueStack->pushString(s.right(n));
}

void substrString()
{
    int n2 = (int)valueStack->popDouble();
    int n1 = (int)valueStack->popDouble();
    QString s = valueStack->popString();

    n1 = qMax(n1, 0);

    if(n1 >= s.length() || n2 <= 0)
    {
        valueStack->pushString("");
    }
    else
    {
        valueStack->pushString(s.mid(n1, n2));
    }
}

void substr1String()
{
    int n1 =valueStack->popDouble();
    QString s = valueStack->popString();

    n1 = qMax(n1, 0);
    if(n1 >= s.length())
        valueStack->pushString("");
    else
        valueStack->pushString(s.mid(n1));
}

void posString()
{
    int k = valueStack->popDouble();
    QString fr = valueStack->popString();
    QString str = valueStack->popString();

    int index = -1;
    for(int i = 0; i < k; i++)
        index = str.indexOf(fr, index + 1);
    valueStack->pushDouble(index);
}

void replicateString()
{
    int n = valueStack->popDouble();
    QString str = valueStack->popString();
    valueStack->pushString(str.repeated(n));
}

void lowerString()
{
    QString str = valueStack->popString();
    valueStack->pushString(str.toLower());
}

void upperString()
{
    QString str = valueStack->popString();
    valueStack->pushString(str.toUpper());
}

void ansiToOem()
{
    QString str = valueStack->popString();

    QTextCodec* ansiCodec = QTextCodec::codecForName("CP1251");
    QTextCodec* oemCodec = QTextCodec::codecForName("CP866");

    QByteArray ansiBytes = ansiCodec->fromUnicode(str);
    str = oemCodec->toUnicode(ansiBytes);

    valueStack->pushString(str);
}

void oemToAnsi()
{
    QString str = valueStack->popString();

    QTextCodec* ansiCodec = QTextCodec::codecForName("CP1251");
    QTextCodec* oemCodec = QTextCodec::codecForName("CP866");

    QByteArray oemBytes = oemCodec->fromUnicode(str);
    str = ansiCodec->toUnicode(oemBytes);

    valueStack->pushString(str);
}

void compareString()
{
    QString s1 = valueStack->getString(1);
    QString s2 = valueStack->getString();
    valueStack->incPosition(2);
    valueStack->pushDouble(s1 == s2);

}

void compareiString()
{
    double n = valueStack->popDouble();
    QString s1 = valueStack->popString();
    QString s2 = valueStack->popString();

    valueStack->pushDouble(n <= 0 ? 0 : s1.left(n-1) == s2.left(n-1));
}

void lengthString()
{
    double len = valueStack->getString().length();
    valueStack->incPosition();
    valueStack->pushDouble(len);
}

void trimLeftString()
{
    QString str = valueStack->popString();
    valueStack->pushString(StringHelper::TrimLeft(str));
}

void trimRightString()
{
    QString str = valueStack->popString();
    valueStack->pushString(StringHelper::TrimRight(str));
}

void trimAllString()
{
    QString str = valueStack->popString();
    valueStack->pushString(StringHelper::TrimAll(str));
}

void asciiString()
{
    QString s = valueStack->popString();
    valueStack->pushDouble(s.length() > 0 ? s[0].toLatin1() : 0);
}

void chrString()
{
    char n = (char)valueStack->popDouble();
    QString s = n == 0 ? "" : fromWindows1251(&n, 1);
//    QString s = n == 0 ? "" : QString::fromLocal8Bit(&n, 1);
    valueStack->pushString(s);
}

void floatToString()
{
    double value = valueStack->popDouble();
    valueStack->pushString(QString().number(value));
}

void stringToFloat()
{
    QString s = valueStack->popString();

    bool ok;
    double value = s.toDouble(&ok);
    if(ok)
    {
        valueStack->pushDouble(value);
    }
    else
    {
        value = 0.0;
        for(int i = 1; i < s.length(); i++)
        {
            double d = s.left(i).toDouble(&ok);
            if(ok)
                value = d;
            else
                break;
        }
        valueStack->pushDouble(value);
    }
}

void plusStringFloat()
{
    double d = valueStack->popDouble();
    QString s = valueStack->popString();
    valueStack->pushString(s + QString::number(d));
}

void plusFloatString()
{
    QString s = valueStack->popString();
    double d = valueStack->popDouble();
    valueStack->pushString(QString::number(d) + s);
}


}
