#ifndef CLASSFACTORY_H
#define CLASSFACTORY_H

#include "ErrorHandler.h"

#include <QFileInfo>

class ClassInfo;
class Library;
class Class;

class STCORESHARED_EXPORT ClassFactory : public ErrorHandler
{
public:
    enum Codes
    {
        ClassSignature = 0x4253,
        PackSignature = 0x4C53
    };

    explicit ClassFactory();

    ClassInfo* loadClassInfo(Library* classLibrary, const QFileInfo& classFileInfo);
    Class* loadClass(ClassInfo* classInfo);
};

#endif // CLASSFACTORY_H
