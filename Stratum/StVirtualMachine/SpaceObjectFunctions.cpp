#include "SpaceObjectFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"
#include "FileSystemFunctions.h"

#include <Object.h>
#include <SpaceWidget.h>


using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupSpaceObjectFunctions()
{
    operations[V_CREATEOBJECTFROMFILE] = createObjectFromFile2d;
    operations[DELETEOBJECT2D] = deleteObject2d;

    operations[GETOBJECTTYPE] = getObjectType;
    operations[GETOBJECTNAME2D] = getObjectName2d;
    operations[SETOBJECTNAME2D] = setObjectName2d;
    operations[GETOBJECTBYNAME] = getObject2dByName;

    operations[GETOBJECTORG2DX] = getObjectOrg2dX;
    operations[GETOBJECTORG2DY] = getObjectOrg2dY;
    operations[SETOBJECTORG2D] = setObjectOrg2d;

    operations[SETOBJECTSIZE2D] = setObjectSize2d;
    operations[GETOBJECTSIZE2DX] = getObjectWidth2d;
    operations[GETOBJECTSIZE2DY] = getObjectHeight2d;

    operations[VM_GETACTUALWIDTH] = getActualWidth2d;
    operations[VM_GETACTUALHEIGHT] = getActualHeight2d;
    operations[VM_GETACTUALSIZE] = getActualSize2d;

    operations[GETOBJECTANGLE2D] = getObjectAngle2d;
    operations[ROTATEOBJECT2D] = rotateObject2d;
    operations[VM_GETOBJECTALPHA2D] = getObjectAlpha2d;
    operations[VM_SETOBJECTALPHA2D] = setObjectAlpha2d;

    operations[SETSHOWOBJECT2D] = setShowObject2d;
    operations[SHOWOBJECT2D] = showObject2d;
    operations[HIDEOBJECT2D] = hideObject2d;

    operations[GETOBJECTFROMPOINT2D] = getObjectFromPoint2d;
    operations[GETOBJECTFROMPOINT2DEX] = getObjectFromPoint2dEx;
    operations[V_ISINTERSECT2D] = isObjectsIntersect2d;
    operations[GETLASTPRIMARY] = getLastPrimary;

    operations[VM_LOCKOBJECT2D] = lockObject2d;

    operations[GETOBJECTATTRIBUTE2D] = getObjectAttribute2d;
    operations[SETOBJECTATTRIBUTE2D] = setObjectAttribute2d;

    operations[VM_GETNEXTOBJECT] = getNextObject2d;
    operations[VM_GETCURRENTOBJ2D] = getCurrentObject2d;
    operations[VM_SETCURRENTOBJ2D] = setCurrentObject2d;

    operations[GETTOPOBJECT2D] = getTopObject2d;
    operations[GETBOTTOMOBJECT2D] = getBottomObject2d;
    operations[GETUPPEROBJECT2D] = getUpperObject2d;
    operations[GETLOWEROBJECT2D] = getLowerObject2d;
    operations[GETOBJECTFROMZORDER2D] = getObjectFromZOrder2d;

    operations[GETZORDER2D] = getZOrder2d;
    operations[SETZORDER2D] = setZOrder2d;

    operations[OBJECTTOBOTTOM2D] = objectToBottom2d;
    operations[OBJECTTOTOP2D] = objectToTop2d;
    operations[SWAPOBJECT2D] = swapObjects2d;
}

void createObjectFromFile2d()
{
    qint32 flags = valueStack->popDouble();
    float x = valueStack->popDouble();
    float y = valueStack->popDouble();
    QString path = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    QString absoluteFilePath = absolutePathByObject(executedObject, path);

    valueStack->pushInt32(0.0);
    //throw std::runtime_error("createObjectFromFile2d - function not implemented");
}

void deleteObject2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->deleteObject(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getObjectType()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectType(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getObjectName2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushString(w->getObjectName2d(objectHandle));
    else
        valueStack->pushString("");
}

void setObjectName2d()
{
    QString objectName = valueStack->popString();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setObjectName2d(objectHandle, objectName));
    else
        valueStack->pushDouble(0.0);
}

void getObject2dByName()
{
    QString objectName = valueStack->popString();
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(w->getObject2dByName(groupHandle, objectName));
    else
        valueStack->pushInt32(0);
}

void getObjectOrg2dX()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectOrgX(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getObjectOrg2dY()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectOrgY(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void setObjectOrg2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setObjectOrg2d(objectHandle, x, y));
    else
        valueStack->pushDouble(0.0);
}

void getObjectWidth2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectWidth(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getObjectHeight2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectHeight(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getActualWidth2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getActualWidth(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getActualHeight2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getActualHeight(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void getActualSize2d()
{
    VarData* heightVar = valueStack->popVarData();
    VarData* widthVar = valueStack->popVarData();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    double width, height;
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        valueStack->pushDouble(w->getActualSize(objectHandle, &width, &height));
        widthVar->d = width;
        heightVar->d = height;
    }
    else
        valueStack->pushDouble(0.0);
}

void setObjectSize2d()
{
    double height = valueStack->popDouble();
    double width = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setObjectSize(objectHandle, width, height));
    else
        valueStack->pushDouble(0.0);
}

void getObjectAngle2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectAngle2d(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void rotateObject2d()
{
    double angle = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->rotateObject2d(objectHandle, x, y, angle));
    else
        valueStack->pushDouble(0.0);

}

void getObjectAlpha2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectAlpha2d(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void setObjectAlpha2d()
{
    double alpha = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setObjectAlpha2d(objectHandle, alpha));
    else
        valueStack->pushDouble(0.0);
}

void setShowObject2d()
{
    bool isVisible = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setShowObject2d(objectHandle, isVisible));
    else
        valueStack->pushDouble(0.0);
}

void showObject2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        w->showObject2d(objectHandle);
}

void hideObject2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        w->hideObject2d(objectHandle);
}

void getObjectFromPoint2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    lastPrimaryHandle = 0;
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        lastPrimaryHandle = w->getObjectFromPoint2d(x, y);
        valueStack->pushInt32(lastPrimaryHandle);
    }
    else
        valueStack->pushInt32(0.0);
}

void getObjectFromPoint2dEx()
{
    int layer = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    lastPrimaryHandle = 0;
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        lastPrimaryHandle = w->getObjectFromPoint2dEx(x, y, layer);
        valueStack->pushInt32(lastPrimaryHandle);
    }
    else
        valueStack->pushInt32(0.0);
}

void isObjectsIntersect2d()
{
    int flag = valueStack->popDouble();
    int object2Handle = valueStack->popInt32();
    int object1Handle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->isObjectsIntersect2d(object1Handle, object2Handle, flag));
    else
        valueStack->pushDouble(0.0);
}

void getLastPrimary()
{
    valueStack->pushInt32(lastPrimaryHandle);
}

void lockObject2d()
{
    /*int flag = */valueStack->popDouble();
    /*int objHandle = */valueStack->popInt32();
    /*int spaceHandle = */valueStack->popInt32();

    VmLog::instance().error("'lockObject2d' - function not implemented");

    valueStack->pushDouble(0.0);
}

void setObjectAttribute2d()
{
    double atrMode = valueStack->popDouble();
    int attrValue = valueStack->popDouble();
    int objHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setObjectAttribute(objHandle, atrMode, attrValue));
    else
        valueStack->pushDouble(0.0);
}

void getObjectAttribute2d()
{
    int objHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getObjectAttribute2d(objHandle));
    else
        valueStack->pushDouble(0.0);
}

void getNextObject2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        int nextObjectHandle = w->getNextObject(objectHandle);
//        qDebug() << "nextObjectHandle" << nextObjectHandle;
        valueStack->pushInt32(nextObjectHandle);
    }
    else
        valueStack->pushInt32(0);
}

void getCurrentObject2d()
{
    throw std::logic_error("getCurrentObject2d function not implemented");
}

void setCurrentObject2d()
{
    throw std::logic_error("setCurrentObject2d function not implemented");
}

void getBottomObject2d()
{
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(w->getBottomObject());
    else
        valueStack->pushInt32(0);
}

void getTopObject2d()
{
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(w->getTopObject());
    else
        valueStack->pushInt32(0);
}

void getUpperObject2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(w->getUpperObject(objectHandle));
    else
        valueStack->pushInt32(0);
}

void getLowerObject2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(w->getLowerObject(objectHandle));
    else
        valueStack->pushInt32(0);
}

void getObjectFromZOrder2d()
{
    int zOrder = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(w->getObjectFromZOrder(zOrder));
    else
        valueStack->pushInt32(0);
}

void getZOrder2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->getZOrder(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void objectToBottom2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->objectToBottom(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void objectToTop2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->objectToTop(objectHandle));
    else
        valueStack->pushDouble(0.0);
}

void setZOrder2d()
{
    int zOrder = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->setObjectZOrder(objectHandle, zOrder));
    else
        valueStack->pushDouble(0.0);
}

void swapObjects2d()
{
    int objectHandle2 = valueStack->popInt32();
    int objectHandle1 = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(w->swapObjectsZOrder(objectHandle1, objectHandle2));
    else
        valueStack->pushDouble(0.0);
}


}
