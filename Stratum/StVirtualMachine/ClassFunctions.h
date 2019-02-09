#ifndef CLASSFUNCTIONS_H
#define CLASSFUNCTIONS_H

#include <QString>

namespace StData {
class Class;
}

namespace StVirtualMachine {

extern void setupClassFunctions();

extern void getProjectClasses();
extern void createClass();
extern void deleteClass();
extern void getUniqueClassName();
extern void getClassName();
extern void getClassFile();
extern void getCalcOrder();
extern void setCalcOrder();
extern void createObject();
extern void deleteObject();
extern void getObjectCount();
extern void getHObject();
extern void getHObjectByName();
extern void getHObjectByNum();
extern void getObjectClass();
extern void createLink();
extern void setLinkVars();
extern void getLink();
extern void removeLink();
extern void setModelText();
extern void getModelText();
extern void getVarH();
extern void getVarF();
extern void getVarS();
extern void setVarH();
extern void setVarF();
extern void setVarS();
extern void getVarInfo();
extern void getVarInfoFull();
extern void getVarCount();
extern void setObjectName();
extern void getNameByHandle();
extern void saveObjectState();
extern void loadObjectState();
extern void setVarsToDefault();

extern void exit();
extern void stop();
extern void quit();
extern void closeAll();

extern bool canModifyClass(StData::Class* cls);
extern bool setClassText(StData::Class* cl, const QString& modelText);

}

#endif // CLASSFUNCTIONS_H
