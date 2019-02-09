#include "CompilerFunctions.h"

#include <QString>
#include <Sc2000DataStream.h>
#include <StDataGlobal.h>

using namespace StData;

namespace StCompiler {

bool CompilerFunctions::decConstantOut(StData::St2000DataStream& outStream, const QString& valueStr)
{
    return decConstantOut(outStream, valueStr.toDouble());
}

bool CompilerFunctions::decConstantOut(St2000DataStream& outStream, double value)
{
    outStream << value;
    return true;
}

bool CompilerFunctions::iConstantOut(St2000DataStream& outStream, const QString& str)
{
    ulong i = str.toULong();
    outStream << quint32(i);
    return true;
}

bool CompilerFunctions::strConstantOut(St2000DataStream& outStream, const QString& str, uint size)
{
    //zorocount - число нулей в старом стратуме в конце строки зависит от четности size
    int zeroCount = size % 2 == 0 ? 2 : 1;
    qint16 sizeOver2 = (size + zeroCount) >> 1;
    outStream << sizeOver2;

//    outStream.writeRawData(str.toLocal8Bit().constData(), size);
    outStream.writeRawData(toWindows1251(str).constData(), size);


    if(zeroCount == 2)
        outStream.writeInt16(0);
    else
        outStream.writeInt8(0);

    return true;
}

bool CompilerFunctions::strConstantOutEx(St2000DataStream& outStream, const QString& str)
{
    qint16 _size;
    qint16 size = _size = str.length();

    bool b = false;

    if (size & 1)
    {
        size++;
        b = true;
    }
    else
        size += 2;

    outStream << (size >> 1);
//    outStream.writeRawData(str.toLocal8Bit().constData(), str.length());
    outStream.writeRawData(toWindows1251(str).constData(), str.length());
    outStream.writeInt8(0);

    if(!b)
    {
        b = false;
        outStream << b;
    }
    return true;
}

bool CompilerFunctions::hexConstantOut(St2000DataStream& outStream, const QString& str)
{
    bool ok;
    ulong i = str.toULong(&ok, 16);
    outStream << quint32(i);
    return true;
}

}
