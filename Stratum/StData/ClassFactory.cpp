#include "ClassFactory.h"

#include "ClassInfo.h"
#include "Context.h"
#include "Project.h"
#include "Sc2000DataStream.h"

#include <QFileInfo>
#include <QDir>
#include <QDataStream>

//#define CLASS_SIGNATURE 0x4253

ClassFactory::ClassFactory()
{}

ClassInfo* ClassFactory::loadClassInfo(Library* library, const QFileInfo& classFileInfo)
{
    cleanErrors();

    ClassInfo* result = NULL;
    if(classFileInfo.exists())
    {
        QFile classFile(classFileInfo.absoluteFilePath());
        if(classFile.open(QFile::ReadOnly))
        {
            Sc2000DataStream stream(&classFile);
            quint16 code = stream.readUint16();
            switch(code)
            {
                case ClassSignature:
                {
                    stream.device()->seek(14);
                    QString className = stream.readString();
                    Context::instance().log()->info(QObject::tr("Имя класса: ") + className);

                    if(!className.isEmpty())
                    {
                        result = new ClassInfo();
                        result->setFileName(classFileInfo.fileName());
                        result->setClassName(className);
                        result->setLibrary(library);
                    }
                }break;

                case PackSignature:
                    break;
            }
        }
        else
            addError(QObject::tr("Не удалось открыть файл: ") + classFileInfo.absoluteFilePath());

        classFile.close();
    }
    else
        addError(QObject::tr("Не найден путь к файлу класса: ") + classFileInfo.absoluteFilePath());

    return result;
}

Class* ClassFactory::loadClass(ClassInfo* classInfo)
{
    Class* result = NULL;
    return result;
}
