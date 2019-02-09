#ifndef VMGLOBAL
#define VMGLOBAL

#include "ArrayManager.h"
#include "CodePointer.h"
#include "VarValueStack.h"

#include "SpaceWindowManager.h"
#include "MatrixManager.h"
#include "MessageManager.h"
#include "StreamManager.h"
#include "HyperJumpManager.h"

#include <QVector>
#include <QStack>
#include <QFontDatabase>
#include <QSet>

#include <ObjectVar.h>
#include <Project.h>
#include <Compiler.h>

#include <memory>

namespace StVirtualMachine {

extern std::shared_ptr<StCompiler::Compiler> compiler;

typedef void(*vm_operation)();
struct Context;
typedef std::shared_ptr<Context> ContextPtr;

extern QVector<vm_operation> operations;
extern QList<StData::ObjectVar*>* variables;
extern QStack<ContextPtr> contextStack;

extern StData::Project* executedProject;
extern StData::Object* executedObject;
extern CodePointer* codePointer;
extern VarValueStack* valueStack;

extern ArrayManager* arrayManager;
extern MatrixManager* matrixManager;
extern SpaceWindowManager* windowManager;
extern StreamManager* streamManager;
extern MessageManager* messageManager;
extern HyperJumpManager* hyperJumpManager;

extern VirtualMachine* virtualMachine;

extern QFontDatabase* fontDataBase;
extern QSet<int> pressedKeys;
extern QTime environmentTime;
extern int lastPrimaryHandle;
extern quint64 currentIterationNumber;

#define EF_RUNNING      1
#define EF_MUSTSTOP     2   // после шага все чистится ClearAll
#define EF_STEPACTIVE   4   // Сейчас идет шаг
#define EF_EQUACTIVE    8   // Сейчас идет вычисление уравнений
#define EF_MSGACTIVE    16  // Сейчас идет обработка сообщений
#define EF_BYTIMER      32  // Вычисления идут по таймеру
#define EF_ONDIALOG     64  // Сейчас идет обработка диалога
#define EF_JUMPING     128  // Идет гиперпереход
extern quint32 execFlags;

/*----------------------------------------------------------------------*/
extern void execute(StData::Object* object, bool executeDisabled = false);
extern void executeObjectCode(StData::Object* object);
extern StData::Object* getRootObject();
extern void funcNotImplemented(const QString& funcName);
//extern void vmStop();

}

#endif // VMGLOBAL

