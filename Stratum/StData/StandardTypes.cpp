#include "StandardTypes.h"

#include "Class.h"
#include "Library.h"
#include "LibraryCollection.h"

namespace StData {

Class* StandardTypes::m_float = nullptr;
Class* StandardTypes::m_integer = nullptr;
Class* StandardTypes::m_string = nullptr;
Class* StandardTypes::m_word = nullptr;
Class* StandardTypes::m_byte = nullptr;
Class* StandardTypes::m_handle = nullptr;
Class* StandardTypes::m_colorRef = nullptr;

void StandardTypes::initialize(QList<StData::SystemLibrary*> systemLibraries)
{
    m_float = LibraryCollection::getSystemClassByName("FLOAT");
    m_integer = LibraryCollection::getSystemClassByName("INTEGER");
    m_string = LibraryCollection::getSystemClassByName("STRING");
    m_word = LibraryCollection::getSystemClassByName("WORD");
    m_byte = LibraryCollection::getSystemClassByName("BYTE");
    m_handle = LibraryCollection::getSystemClassByName("HANDLE");
    m_colorRef = LibraryCollection::getSystemClassByName("COLORREF");
}

bool StandardTypes::isNumeric(Class* type)
{
    return isByte(type) || isFloat(type) || isHandle(type) || isWord(type) || isInteger(type);
}

StandardTypes::TypeCode StandardTypes::typeCode(Class* type)
{
    if(isFloat(type))
        return Float;

    if(isHandle(type))
        return Handle;

    if(isColorRef(type))
        return ColorRef;

    if(isString(type))
        return String;

    return Undefined;
}

Class* StandardTypes::loadClassByName(QList<StData::SystemLibrary*> systemLibraries, const QString& className)
{
    for(SystemLibrary* sl : systemLibraries)
    {
        for(Class* cl : sl->classList())
            if(QString::compare(cl->originalName(), className, Qt::CaseInsensitive) == 0)
            {
                if(!cl->isLoaded())
                    cl->load();

                return cl;
            }
    }
}

}
