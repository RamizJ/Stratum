#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtGlobal>

namespace StData {
class St2000DataStream;
}

namespace StCompiler {

class CompilerFunctions
{
public:
    static bool decConstantOut(StData::St2000DataStream& outStream, const QString& valueStr);
    static bool decConstantOut(StData::St2000DataStream& outStream, double value);
    static bool iConstantOut(StData::St2000DataStream& outStream, const QString& str);
    static bool strConstantOut(StData::St2000DataStream& outStream, const QString& str, uint size);
    static bool strConstantOutEx(StData::St2000DataStream& outStream, const QString& str);
    static bool hexConstantOut(StData::St2000DataStream& outStream, const QString& str);
};

}

#endif // GLOBALFUNCTIONS_H
