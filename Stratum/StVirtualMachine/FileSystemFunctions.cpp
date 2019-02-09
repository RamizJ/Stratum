#include "FileSystemFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "ArrayManager.h"
#include "Array.h"
#include "VmLog.h"

#include <Class.h>
#include <LibraryCollection.h>
#include <PathManager.h>

#include <QObject>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QDateTime>
#include <QCoreApplication>
#include <PathManager.h>
#include <Object.h>
#include <Library.h>


#define tr(str) QObject::tr(str)

#ifdef Q_OS_WIN
#include <Library.h>
#include <Object.h>
#include <windows.h>
#endif

using namespace StData;

namespace StVirtualMachine {

enum FileAttributes
{
    Archive = 32, Compressed = 2048, Directory = 16, Hidden = 2, Normal = 128,
    Offline = 4096, Readonly = 1, System = 4, Temporary = 256
};

void setupFileSystemFunctions()
{
    operations[VM_CREATEDIR] = vm_createdir;
    operations[VM_DELETEDIR] = vm_deletedir;
    operations[VM_FILERENAME] = vm_filerename;
    operations[VM_FILECOPY] = vm_filecopy;
    operations[VM_FILEEXIST] = vm_fileexist;
    operations[VM_FILEDELETE] = vm_filedelete;
    operations[VM_GETFILELIST] = vm_getfilelist;
    operations[GETCLASSDIR] = getclassdir;
    operations[GETPROJECTDIR] = getprojectdir;
    operations[GETWINDOWSDIR] = getwindowsdir;
    operations[GETSYSTEMDIR] = getSystemDir;
    operations[VM_GETTEMPDIRECTORY] = getTempDirectory;
    operations[GETDIRFROMFILE] = getdirfromfile;
    operations[ADDSLASH] = addslash;
    operations[GETSTRATUMDIR] = getstratumdir;
    operations[VM_GETROMDRIVENAMES] = getRomDriveNames;
}

QString absolutePath(QString path)
{
    if(QFileInfo(path).isRelative())
    {
        path = PathManager::fromWinOsSeparators(path);
        path = QDir::cleanPath(executedProject->projectDir().absoluteFilePath(path));
    }

    return path;
}

QString absolutePathByObject(StData::Object* object, QString path)
{
    if(QFileInfo(path).isRelative())
    {

        path = PathManager::fromWinOsSeparators(path);
        path = QDir::cleanPath(object->cls()->library()->libraryDir().absoluteFilePath(path));
    }
    return path;
}


inline QString windowsDirPath()
{
    return QProcessEnvironment::systemEnvironment().value("WINDIR");
}

bool removeDirRecursive(const QDir& dir)
{
    bool result = true;
    if(dir.exists())
    {
        QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::System | QDir::Hidden;
        for(QFileInfo fileInfo : dir.entryInfoList(filter, QDir::DirsFirst))
        {
            if(fileInfo.isDir())
                result = removeDirRecursive(QDir(fileInfo.absoluteFilePath()));
            else
                result = QFile::remove(fileInfo.absoluteFilePath());

            if(!result)
                return result;
        }
        result = dir.rmdir(dir.absolutePath());
    }
    return result;
}

int getFileAttributes(const QFileInfo& fileInfo)
{
    int attributes = 0;

    if(fileInfo.isDir())
        attributes |= Directory;

    if(fileInfo.isHidden())
        attributes |= Hidden;

    if(fileInfo.isReadable())
        attributes |= Readonly;

    return attributes;
}

void vm_createdir()
{
    QString path = valueStack->popString();
    valueStack->pushDouble(executedProject->projectDir().mkpath(path));
}

void vm_deletedir()
{
    QString path = absolutePath(valueStack->popString());
    QDir dir(path);
    if(dir.exists())
    {
        removeDirRecursive(dir);
        valueStack->pushDouble(1.0);
    }
    else
        valueStack->pushDouble(0.0);
}

void vm_filerename()
{
    QString newName = absolutePath(valueStack->popString());
    QString oldName = absolutePath(valueStack->popString());

    valueStack->pushDouble(QFile::rename(oldName, newName));
}

void vm_filecopy()
{
    QString destFile = absolutePath(valueStack->popString());
    QString sourceFile = absolutePath(valueStack->popString());

    valueStack->pushDouble(QFile::copy(sourceFile, destFile));
}

void vm_fileexist()
{
    QString path = absolutePath(valueStack->popString());
    valueStack->pushDouble(QFile::exists(path));
}


void vm_filedelete()
{
    QString path = absolutePath(valueStack->popString());
    valueStack->pushDouble(QFile::remove(path));
}

void vm_getfilelist()
{
    int attribute = valueStack->popDouble();
    QString path = valueStack->popString();

    path = PathManager::instance().cleanPath(path);

    QFileInfo fi(path);
    QDir dir = fi.isDir() ? QDir(path) : fi.dir();

    bool isAssetsFolder = false;
#ifdef Q_OS_ANDROID
    isAssetsFolder = dir.path().startsWith("assets:", Qt::CaseInsensitive);
#endif

    if(dir.isRelative() && !isAssetsFolder)
    {
        QString absoutePath = executedProject->projectDir().absoluteFilePath(dir.path());
        dir = QDir(absoutePath);
    }

    if(dir.exists())
    {
        QString fileNamesFilter = fi.isDir() ? "*" : fi.fileName();

        int arrayHandle = arrayManager->createArray();
        Array* array = arrayManager->getArray(arrayHandle);

        Class* fileAttributeClass = LibraryCollection::getClassByName("FILE_ATTRIBUTE");
        if(!fileAttributeClass)
            std::runtime_error(tr("Ошибка в функции GetFileList. Не найден системный класс 'FILE_ATTRIBUTE'").toStdString());

        QFileInfoList entryList = dir.entryInfoList(QStringList() << fileNamesFilter,
                                                    QDir::AllEntries | QDir::Dirs | QDir::Hidden | QDir::System);

        if(fileNamesFilter.contains("*.*"))
        {
            QFileInfoList dirList = dir.entryInfoList(QDir::Dirs);

            for(QFileInfo dirInfo : dirList)
            {
                if(!entryList.contains(dirInfo))
                    entryList.append(dirInfo);
            }
        }


        for(QFileInfo fileInfo : entryList)
        {
            bool notFiltered = ((attribute & Directory) && !fileInfo.isDir()) ||
                               ((attribute & Hidden) && !fileInfo.isHidden()) ||
                               ((attribute & Readonly) && !fileInfo.isReadable());


            if(notFiltered)
                continue;

            ArrayItem* arrayItem = new ArrayItem(fileAttributeClass);
            array->insertItem(arrayItem);

            arrayItem->setVarS("Name", fileInfo.fileName());
            arrayItem->setVarF("Size", fileInfo.size());
            arrayItem->setVarF("Attr", getFileAttributes(fileInfo));

            QDateTime creationDateTime = fileInfo.created();
            arrayItem->setVarF("Year", creationDateTime.date().year());
            arrayItem->setVarF("Month", creationDateTime.date().month());
            arrayItem->setVarF("Day", creationDateTime.date().day());
            arrayItem->setVarF("DayOfWeek", creationDateTime.date().dayOfWeek());

            arrayItem->setVarF("Hour", creationDateTime.time().hour());
            arrayItem->setVarF("Minute", creationDateTime.time().minute());
            arrayItem->setVarF("Second", creationDateTime.time().second());
        }
        valueStack->pushInt32(arrayHandle);
        return;
    }
    valueStack->pushInt32(0);
}

void getclassdir()
{
    QString className = valueStack->popString();

    Class* cls = LibraryCollection::getClassByName(className);
    if(cls != nullptr)
    {
        QString path = QDir::toNativeSeparators(cls->fileInfo().absolutePath());
        if(path.right(1) != "\\")
            path.append('\\');
        valueStack->pushString(path);
    }
    else
        valueStack->pushString("");
}

void getprojectdir()
{
    valueStack->pushString(QDir::toNativeSeparators(executedProject->projectDir().absolutePath()));
}

void getwindowsdir()
{
    valueStack->pushString(QDir::toNativeSeparators(windowsDirPath()));
}

void getSystemDir()
{
    QString systemPath;
    QDir winDir(windowsDirPath());
    if(winDir.exists())
        systemPath = winDir.absoluteFilePath("System32");

    if(systemPath.right(1) != "\\")
        systemPath.append('\\');

    valueStack->pushString(QDir::toNativeSeparators(systemPath));
}

void getTempDirectory()
{
    QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
    QString path = pe.value("TEMP");
    if(path.right(1) != "\\")
        path.append('\\');
    valueStack->pushString(QDir::toNativeSeparators(path));
}

void getdirfromfile()
{
    QString path = valueStack->popString();
    QFileInfo fileInfo(path);
    valueStack->pushString(QDir::toNativeSeparators(fileInfo.dir().absolutePath()));
}

void addslash()
{
    QString path = valueStack->popString();
    path = QDir::toNativeSeparators(path);
    if(path.right(1) != "\\")
        path.append('\\');
    valueStack->pushString(path);
}

void getstratumdir()
{
    valueStack->pushString(QCoreApplication::applicationDirPath());
}

void getRomDriveNames()
{
    QString result;
#ifdef Q_OS_WIN
    const int bufSize = 100;
    char buf[bufSize];
    GetLogicalDriveStringsA(bufSize, buf);

    LPCSTR drivePath;
    for(char* s = buf; *s; s += strlen(s) + 1)
    {
        drivePath = (LPCSTR)s;
        if(GetDriveTypeA(drivePath) == DRIVE_CDROM)
        {
            if(result.length() > 0)
                result += ",";

            result += QString::fromLatin1(drivePath).left(1);
        }
    }
#endif
    valueStack->pushString(result);
}

}
