#include "SystemFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"
#include "Context.h"

#include <Class.h>
#include <Space.h>
#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <ObjectVar.h>
#include <Object.h>
#include <ClassVarInfo.h>
#include <StDataGlobal.h>

#include <QDesktopServices>
#include <QProcess>
#include <QDateTime>
#include <StandardTypes.h>
#include <QEvent>
#include <QKeyEvent>
#include <QInputMethod>
#include <QApplication>
#include <QScreen>
#include <SpaceView.h>

#include <HyperBase.h>
#include <QDebug>
#include <SpaceGraphicsItem.h>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#include <QWindow>
#include <HyperBase.h>
#include <SpaceGraphicsItem.h>
#endif

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupSystemFunctions()
{
    operations[V_SYSTEM] = system;
    operations[V_SYSTEMSTR] = systemStr;

    operations[GETAKEYSTATE] = getAsyncKeyState;
    operations[VM_GETKEYBOARDLAYOUT] = getKeyboardLayout;
    operations[JGETX] = joyGetX;
    operations[JGETY] = joyGetY;
    operations[JGETZ] = joyGetZ;
    operations[JGETBTN] = joyGetButtons;
    operations[VM_GETMOUSEPOS] = getMousePos;
    operations[VM_SHOWCURSOR] = showCursor;
    operations[VM_SETSTANDARTCURSOR] = setStandartCursorH;
    operations[VM_SETSTANDARTCURSOR_S] = setStandartCursorW;
    operations[VM_LOADCURSOR] = loadCursorH;
    operations[VM_LOADCURSOR_S] = loadCursorW;
    operations[GETTICKCOUNT] = getTickCount;
    operations[VM_GETTIME] = getTime;
    operations[VM_GETDATE] = getDate;
    operations[VM_SETHYPERJUMP] = setHyperJump2d;
    operations[STDHYPERJUMP] = stdHyperJump;
    operations[VM_GETSCREENWIDTH] = getScreenWidth;
    operations[VM_GETSCREENHEIGHT] = getScreenHeight;
    operations[VM_GETWORKAREAX] = getWorkAreaX;
    operations[VM_GETWORKAREAY] = getWorkAreaY;
    operations[VM_GETWORKAREAWIDTH] = getWorkAreaWidth;
    operations[VM_GETWORKAREAHEIGHT] = getWorkAreaHeight;
    operations[VM_GETTITLEHEIGHT] = getTitleHeight;
    operations[VM_GETSMALLTITLEHEIGHT] = getSmallTitleHeight;
    operations[VM_GETFIXEDFRAMEWIDTH] = getFixedFrameWidth;
    operations[VM_GETFIXEDFRAMEHEIGHT] = getFixedFrameHeight;
    operations[VM_GETSIZEFRAMEWIDTH] = getSizeFrameWidth;
    operations[VM_GETSIZEFRAMEHEIGHT] = getSizeFrameHeight;
    operations[WINEXECUTE] = winExecute;
    operations[WINSHELL] = shell;
    operations[VM_SHELLWAIT] = shellWait;
    operations[VM_SENDSMS] = sendSms;
    operations[VM_SENDMAIL] = sendMail;
    operations[VM_SCMESSAGE] = logMessage;
    operations[VFUNCTION] = vFunction;
}

//bool eventFilter(QObject* /*watchedObject*/, QEvent* event)
//{
//    if(event->type() == QEvent::KeyPress)
//    {
//        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
//        pressedKeys << keyEvent->nativeVirtualKey();
//    }
//    if(event->type() == QEvent::KeyRelease)
//    {
//        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
//        pressedKeys.remove(keyEvent->nativeVirtualKey());
//    }
//    if(event->type() == QEvent::MouseButtonPress)
//    {
//        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//        pressedKeys << mouseEvent->button();
//    }
//    if(event->type() == QEvent::MouseButtonRelease)
//    {
//        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//        pressedKeys.remove(mouseEvent->button());
//    }
//    return false;
//}

Class* getArgumentType()
{
    int typeCode = codePointer->getCode();
    codePointer->incPosition();

    if(typeCode < 0)
    {
        switch(typeCode)
        {
            case -1: return StandardTypes::floatType();
            case -2: return StandardTypes::stringType();
            case -3: return StandardTypes::handleType();
            case -4: return StandardTypes::colorRefType();
            default: return nullptr;
        }
    }
    else
    {
        if(typeCode)
        {
            QString className = codePointer->popString();
            return executedProject->getClassByName(className);
        }
        else
            return nullptr;
    }
}

void system()
{
    int count = codePointer->getCode();
    codePointer->incPosition();

    for(int i = 0; i < count; i++)
        valueStack->popDouble();

    int code = valueStack->popDouble();

    double result = 0;
    switch(code)
    {
        case 4:
            result = currentIterationNumber;
            break;
    }

    valueStack->pushDouble(result);

    //    VmLog::instance().warning("'system' function not implemented");
}

void systemStr()
{
    int code = valueStack->popDouble();
    QString s;
    switch (code)
    {
        default:
            break;
    }
    valueStack->pushString(s);
}

void getAsyncKeyState()
{
    int key = valueStack->popDouble();
    valueStack->pushDouble(pressedKeys.contains(key));
}

void getKeyboardLayout()
{
    double res = 0.0;
#ifdef Q_OS_WIN
    res = LOWORD(GetKeyboardLayout(0));
#else
    QInputMethod* inputMethod = QApplication::inputMethod();
    QLocale locale = inputMethod->locale();
    res = locale.language();
#endif
    valueStack->pushDouble(res);
}

void joyGetX()
{
    valueStack->popDouble();
    valueStack->pushDouble(0.0);

    VmLog::instance().warning("'joyGetX' - function not implemented");
}

void joyGetY()
{
    valueStack->popDouble();
    valueStack->pushDouble(0.0);

    VmLog::instance().warning("'joyGetY' - function not implemented");
}

void joyGetZ()
{
    valueStack->popDouble();
    valueStack->pushDouble(0.0);

    VmLog::instance().warning("'joyGetZ' - function not implemented");
}

void joyGetButtons()
{
    valueStack->popDouble();
    valueStack->pushDouble(0.0);

    VmLog::instance().warning("'joyGetButtons' - function not implemented");
}

void getMousePos()
{
    VarData* y = valueStack->popVarData();
    VarData* x = valueStack->popVarData();
    QString windowName = valueStack->popString();

    if(SpaceWidget* w = windowManager->getWidget(windowName))
    {
        QPointF pos = QCursor::pos();
        pos = w->spaceView()->mapFromGlobal(pos.toPoint());
        pos += w->getSpaceOrg();

        x->d = pos.x();
        y->d = pos.y();
        valueStack->pushDouble(1.0);
    }
    else
    {
        x->d = 0.0;
        y->d = 0.0;
        valueStack->pushDouble(0.0);
    }
}

void showCursor()
{
    int isVisible = valueStack->popDouble();

#ifdef Q_OS_WIN
    if(isVisible>0)
        while(ShowCursor(1) < 0);
    else
        while(ShowCursor(0) >= 0);
#else
    QCursor* cursor = QApplication::overrideCursor();
    bool isBlankCursor = (cursor != nullptr && cursor->shape() == Qt::BlankCursor);

    if(isVisible && isBlankCursor)
        QApplication::restoreOverrideCursor();

    else if(!isVisible && !isBlankCursor)
        QApplication::setOverrideCursor(Qt::BlankCursor);
#endif
}

void setStandartCursor(SpaceWidget* spaceWidget, int cursorType)
{
    if(!spaceWidget)
        return;

    switch(cursorType)
    {
        case 0:
            spaceWidget->setCursor(Qt::ArrowCursor);
            break;

        case 1:
            spaceWidget->setCursor(Qt::CrossCursor);
            break;

        case 2:
            spaceWidget->setCursor(Qt::IBeamCursor);
            break;

        case 3:
            spaceWidget->setCursor(Qt::ForbiddenCursor);
            break;

        case 4:
            spaceWidget->setCursor(Qt::SizeAllCursor);
            break;

        case 5:
            spaceWidget->setCursor(Qt::SizeBDiagCursor);
            break;

        case 6:
            spaceWidget->setCursor(Qt::SizeVerCursor);
            break;

        case 7:
            spaceWidget->setCursor(Qt::SizeFDiagCursor);
            break;

        case 8:
            spaceWidget->setCursor(Qt::SizeHorCursor);
            break;

        case 9:
            spaceWidget->setCursor(Qt::UpArrowCursor);
            break;

        case 10:
            spaceWidget->setCursor(Qt::BusyCursor);
            break;

        case 11:
            spaceWidget->setCursor(Qt::PointingHandCursor);
            break;

        case 12:
            spaceWidget->setCursor(Qt::WhatsThisCursor);
            break;
    }
}

void setStandartCursorH()
{
    int cursor = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    setStandartCursor(windowManager->getWidget(spaceHandle), cursor);
}

void setStandartCursorW()
{
    int cursor = valueStack->popDouble();
    QString wndName = valueStack->popString();

    setStandartCursor(windowManager->getWidget(wndName), cursor);
}

void loadCursor(SpaceWidget* spaceWidget, const QString& fileName)
{
    if(!spaceWidget)
        return;

    QString absoluteFilePath = fileName;
    QFileInfo fi(fileName);
    if(fi.isRelative())
        absoluteFilePath = executedProject->projectDir().absoluteFilePath(fileName);

#ifdef Q_OS_WIN
    wchar_t cursorPath[1024];
    absoluteFilePath.toWCharArray(cursorPath);

    HCURSOR hCursor = LoadCursorFromFile(cursorPath);
    if(hCursor)
    {
        //        WId wId = spaceWidget->windowHandle()->winId();
    }
#else
    //    QPixmap pixmap(absoluteFilePath);
    //    spaceWidget->setCursor(QCursor(pixmap));
#endif
}

void loadCursorH()
{
    QString fileName = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    loadCursor(windowManager->getWidget(spaceHandle), fileName);
}

void loadCursorW()
{
    QString fileName = valueStack->popString();
    QString wndName = valueStack->popString();

    loadCursor(windowManager->getWidget(wndName), fileName);
}

void getTickCount()
{
    valueStack->pushDouble(environmentTime.elapsed());
}

void getTime()
{
    VarData* msecs = valueStack->popVarData();
    VarData* secs = valueStack->popVarData();
    VarData* mins = valueStack->popVarData();
    VarData* hours = valueStack->popVarData();

    QTime currentTime = QTime::currentTime();

    int hund = currentTime.msec() / 10;
    msecs->setDouble(hund);
    secs->setDouble(currentTime.second());
    mins->setDouble(currentTime.minute());
    hours->setDouble(currentTime.hour());
}

void getDate()
{
    VarData* day = valueStack->popVarData();
    VarData* month = valueStack->popVarData();
    VarData* year = valueStack->popVarData();

    QDate currentDate = QDate::currentDate();

    day->setDouble(currentDate.day());
    month->setDouble(currentDate.month());
    year->setDouble(currentDate.year());
}

void setHyperJump2d()
{
    bool result = false;
    QList<QString> args;

    int count = codePointer->getCode();
    codePointer->incPosition();

    for(int i = 0; i < count; i++)
        args.insert(0, valueStack->popString());

    int mode = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(mode >= 0)
    {
        SpaceItemHyperKeyPtr key = std::make_shared<SpaceItemHyperKey>();
        key->setOpenMode(mode);

        for(int i = 0; i < args.count(); i++)
        {
            switch(i)
            {
                case 0:
                    key->setTarget(args[i]);
                    break;

                case 1:
                    key->setWindowName(args[i]);
                    break;

                case 2:
                    key->setObject(args[i]);
                    break;

                case 3:
                    if(mode == 2)
                        key->setParams(args[i]);
                    else
                        key->setEffect(args[i]);
                    break;

                case 4:
                    key->setParams(args[i]);
                    break;
            }

        }

        if(auto w = windowManager->getWidget(spaceHandle))
            if(auto item = w->spaceScene()->getSpaceGraphicsItem(objectHandle))
            {
                item->setHyperKey(key);
                hyperJumpManager->registerItem(w, item);
                result = true;
            }
    }
    valueStack->pushDouble(result);
}

void stdHyperJump()
{
    int fwKeys = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();

    double y = valueStack->popDouble();
    double x = valueStack->popDouble();

    int spaceHandle = valueStack->popInt32();

    auto winMan = windowManager;
    if(auto w = windowManager->getWidget(spaceHandle))
    {
        if(objectHandle <= 0)
            objectHandle = w->getObjectFromPoint2d(x, y);

        w->spaceScene()->stdHyperJump(objectHandle);
    }
}

void getScreenWidth()
{
    QScreen* screen = QApplication::primaryScreen();
    double width = screen->size().width();
    valueStack->pushDouble(width);
}

void getScreenHeight()
{
    QScreen* screen = QApplication::primaryScreen();
    valueStack->pushDouble(screen->size().height());
}

void getWorkAreaX()
{
    QScreen* screen = QApplication::primaryScreen();
    valueStack->pushDouble(screen->availableGeometry().left());
}

void getWorkAreaY()
{
    QScreen* screen = QApplication::primaryScreen();
    valueStack->pushDouble(screen->availableGeometry().top());
}

void getWorkAreaWidth()
{
    QScreen* screen = QApplication::primaryScreen();
    valueStack->pushDouble(screen->availableGeometry().width());
}

void getWorkAreaHeight()
{
    QScreen* screen = QApplication::primaryScreen();
    valueStack->pushDouble(screen->availableGeometry().height());
}

int titleHeight()
{
#ifdef Q_OS_WIN
    return GetSystemMetrics(SM_CYCAPTION);
#else
    VmLog::instance().warning("'GetTitleHeight' - function not implemented");
    return 0;
#endif
}

int smallTitleHeight()
{
#ifdef Q_OS_WIN
    return GetSystemMetrics(SM_CYSMCAPTION);
#else
    VmLog::instance().warning("'GetSmallTitleHeight' - function not implemented");
    return 0;
#endif
}

int fixedFrameWidth()
{
#ifdef Q_OS_WIN
    return GetSystemMetrics(SM_CXFIXEDFRAME);
#else
    VmLog::instance().warning("'GetFixedFrameWidth' - function not implemented");
    return 0;
#endif
}

int fixedFrameHeight()
{
#ifdef Q_OS_WIN
    return GetSystemMetrics(SM_CYFIXEDFRAME);
#else
    VmLog::instance().warning("'GetFixedFrameHeight' - function not implemented");
    return 0;
#endif
}

int sizeFrameWidth()
{
#ifdef Q_OS_WIN
    return GetSystemMetrics(SM_CXSIZEFRAME);
#else
    VmLog::instance().warning("'GetSizeFrameWidth' - function not implemented");
    return 0;
#endif
}

int sizeFrameHeight()
{
#ifdef Q_OS_WIN
    return GetSystemMetrics(SM_CYSIZEFRAME);
#else
    VmLog::instance().warning("'GetSizeFrameHeight' - function not implemented");
    return 0;
#endif
}

void getTitleHeight()
{
    valueStack->pushDouble(titleHeight());
}

void getSmallTitleHeight()
{
    valueStack->pushDouble(smallTitleHeight());
}

void getFixedFrameWidth()
{
    valueStack->pushDouble(fixedFrameWidth());
}

void getFixedFrameHeight()
{
    valueStack->pushDouble(fixedFrameHeight());
}

void getSizeFrameWidth()
{
    valueStack->pushDouble(sizeFrameWidth());
}

void getSizeFrameHeight()
{
    valueStack->pushDouble(sizeFrameHeight());
}

void winExecute()
{
    int showFlag = valueStack->popDouble();
    QString command = valueStack->popString();
#ifdef Q_OS_WIN
    wchar_t commandData[256];
//    std::mbstowcs(commandData, command.toLocal8Bit().data(), command.length());
    std::mbstowcs(commandData, toWindows1251(command).data(), command.length());

//    int res = WinExec(command.toLocal8Bit().data(), showFlag);
    int res = WinExec(toWindows1251(command).data(), showFlag);
    valueStack->pushDouble(res);
#else
    valueStack->pushDouble(QProcess::execute(command) >= 0);
#endif
}

void shell()
{
    int cmdShow = valueStack->popDouble();
    QString dir = valueStack->popString();
    QString paramsStr = valueStack->popString();
    QString command = valueStack->popString();

    bool res = shellExecute(command, paramsStr, dir, cmdShow);
    valueStack->pushDouble(res);
}

void shellWait()
{
    int cmdShow = valueStack->popDouble();
    QString dir = valueStack->popString();
    QString params = valueStack->popString();
    QString command = valueStack->popString();

#ifdef Q_OS_WIN
    wchar_t dirWChar[1024];
    wchar_t paramsWChar[1024];
    wchar_t commandWChar[1024];

    int dirLen = dir.toWCharArray(dirWChar);
    int paramsLen = params.toWCharArray(paramsWChar);
    int commandLen = command.toWCharArray(commandWChar);

    dirWChar[dirLen >= 0 ? dirLen : 0] = 0;
    paramsWChar[paramsLen >= 0 ? paramsLen : 0] = 0;
    commandWChar[commandLen >= 0 ? commandLen : 0] = 0;

    SHELLEXECUTEINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.hwnd = NULL;

    info.lpFile = commandWChar;
    info.lpParameters = paramsWChar;
    info.lpDirectory = dirWChar;
    info.nShow = cmdShow;

    if(!ShellExecuteEx(&info))
    {
        valueStack->pushDouble(0);
        return;
    }
    if(info.hProcess == 0)
    {
        valueStack->pushDouble(0);
        return;
    }
    WaitForSingleObjectEx(info.hProcess, INFINITE, false);
    CloseHandle(info.hProcess);

    valueStack->pushDouble(1);
#else
    QProcess* process = new QProcess();
    process->setProgram(command);
    process->setArguments(params.split(' '));
    process->setWorkingDirectory(dir);

    QObject::connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));

    process->waitForFinished(-1);
    process->start();
    valueStack->pushDouble(1.0);
#endif
}

void sendSms()
{
    throw std::runtime_error("'SendSms' - function not implemented");
}

void sendMail()
{
    throw std::runtime_error("'SendMail' - function not implemented");
}

void logMessage()
{
    QString msg = valueStack->popString();
    VmLog::instance().info(msg);
}

void vFunction()
{
    QString functionName = codePointer->popString();

    if(Class* functionClass = executedProject->getClassByName(functionName))
    {
        int count = codePointer->getCode();
        codePointer->incPosition();

        //        QVector<ObjectVar*> functionVars;
        int functionsVarsCount = functionClass->varInfoList().count();

        if(count <= functionsVarsCount)
        {
            Object* functionObject = Object::createByClass(executedProject, functionClass);
            int retIndex = -1;
            for(int i = 0; i < functionsVarsCount; i++)
            {
                //                functionVars[i] = new ObjectVar(functionClass->varInfoByIndex(i));
                if(functionClass->varInfoByIndex(i)->flags() & ClassVarInfo::Return)
                    retIndex = i;
            }

            for(int i = count - 1; i >= 0; i--)
            {
                int code = codePointer->getCode();
                Class* varType = getArgumentType();

                if(varType && functionClass->varInfoByIndex(i)->varType() != varType)
                {
                    delete functionObject;
                    //                    qDeleteAll(functionVars);
                    throw std::runtime_error(QString("Type error in user function '%1'").arg(functionName).toStdString());
                }

                ObjectVar* var = functionObject->varByIndex(i);
                switch(code)
                {
                    case -1:
                        var->setDouble(valueStack->popDouble());
                        break;

                    case -2:
                        var->setString(valueStack->popString());
                        break;

                    case -3:
                    case -4:
                        var->setInt(valueStack->popInt32());
                        break;

                    default:
                        throw std::runtime_error("Load var for complex type not implemented");
                }
            }
            functionObject->saveState();
            Class* retVarType = getArgumentType();

            VarData* retVarData = nullptr;
            if(pushContext(true))
            {
                execute(functionObject, true);
            }
            popContext();

            if(retVarType)
            {
                ObjectVar* retVar = functionObject->varByIndex(retIndex);
                VarData* retVarData = retVar->savedVarData();
                //VarData* retVarData = valueStack->getVarData(-1);

                if(StandardTypes::isString(retVarType))
                    valueStack->pushString(retVarData->s);

                else if(StandardTypes::isFloat(retVarType))
                    valueStack->pushDouble(retVarData->d);

                else if(StandardTypes::isNumeric(retVarType) || StandardTypes::isColorRef(retVarType))
                    valueStack->pushInt32(retVarData->i);
            }

            if(functionObject)
                delete functionObject;
        }
    }
    else
    {
        QString errorMsg = QString("Class function '%1' not found.").arg(functionName);
        throw std::runtime_error(errorMsg.toStdString());
    }
}

bool shellExecute(const QString& command, const QString& paramsStr, const QString& dir, int cmdShow)
{
#ifdef Q_OS_WIN
    wchar_t dirWChar[1024];
    wchar_t paramsWChar[1024];
    wchar_t commandWChar[1024];
    int dirLen = dir.toWCharArray(dirWChar);
    int paramsLen = paramsStr.toWCharArray(paramsWChar);
    int commandLen = command.toWCharArray(commandWChar);

    dirWChar[dirLen >= 0 ? dirLen : 0] = 0;
    paramsWChar[paramsLen >= 0 ? paramsLen : 0] = 0;
    commandWChar[commandLen >= 0 ? commandLen : 0] = 0;

    auto hi = ShellExecute(0, NULL, commandWChar, paramsWChar, dirWChar, cmdShow);
    return hi > HINSTANCE(32);
#else
    QStringList params = paramsStr.split(' ', QString::SkipEmptyParts);
    if(dir.isEmpty() && params.isEmpty())
    {
        QDesktopServices::openUrl(QUrl(command));
    }
    else
    {
        QProcess* process = new QProcess();
        process->setProgram(command);
        process->setArguments(params);

        if(!dir.isEmpty())
            process->setWorkingDirectory(dir);

        QObject::connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));

        process->start(QProcess::ReadOnly);
    }
    return true;
#endif
}

}
