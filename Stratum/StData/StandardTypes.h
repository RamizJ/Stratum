#ifndef STANDARDTYPES_H
#define STANDARDTYPES_H

#include <stdata_global.h>

namespace StData {

class Class;
class SystemLibrary;

class STDATASHARED_EXPORT StandardTypes
{
public:
    enum TypeCode {Undefined = -1, Float = 0, Handle, ColorRef, String};

public:
    static void initialize(QList<StData::SystemLibrary*> systemLibraries);
    static Class* loadClassByName(QList<StData::SystemLibrary*> systemLibraries, const QString& className);

    static Class* floatType() {return m_float;}
    static Class* integerType() {return m_integer;}
    static Class* wordType() {return m_word;}
    static Class* byteType() {return m_byte;}
    static Class* handleType() {return m_handle;}

    static Class* stringType() {return m_string;}
    static Class* colorRefType() {return m_colorRef;}

    static bool isFloat(Class* cl) {return cl == m_float;}
    static bool isInteger(Class* cl) {return cl == m_integer;}
    static bool isString(Class* cl) {return cl == m_string;}
    static bool isWord(Class* cl) {return cl == m_word;}
    static bool isByte(Class* cl) {return cl == m_byte;}
    static bool isHandle(Class* cl) {return cl == m_handle;}
    static bool isColorRef(Class* cl) {return cl == m_colorRef;}
    static bool isStandandType(Class* cl) {return typeCode(cl) >= 0;}

    static bool isNumeric(Class* type);

    static TypeCode typeCode(Class* type);

private:
    static Class* m_float;
    static Class* m_integer;
    static Class* m_string;
    static Class* m_word;
    static Class* m_byte;
    static Class* m_handle;
    static Class* m_colorRef;
};

}

#endif // STANDARDTYPES_H
