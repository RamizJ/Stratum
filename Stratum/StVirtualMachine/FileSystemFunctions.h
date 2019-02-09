#ifndef FILESYSTEMFUNCTIONS_H
#define FILESYSTEMFUNCTIONS_H

#include <QString>

namespace StData {
class Object;
}

namespace StVirtualMachine {

extern void setupFileSystemFunctions();

extern void vm_createdir();
extern void vm_deletedir();
extern void vm_filedelete();

extern void vm_filerename();
extern void vm_filecopy();
extern void vm_fileexist();

extern void vm_getfilelist();
extern void getRomDriveNames();

extern void getclassdir();
extern void getprojectdir();
extern void getwindowsdir();
extern void getSystemDir();
extern void getTempDirectory();
extern void getdirfromfile();
extern void addslash();
extern void getstratumdir();

QString absolutePath(QString path);
QString absolutePathByObject(StData::Object* object, QString path);

}

#endif // FILESYSTEMFUNCTIONS_H
