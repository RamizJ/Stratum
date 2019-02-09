#include "GroupFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <Space.h>
#include <SpaceItem.h>
#include <SpaceScene.h>
#include <SpaceWidget.h>

using namespace StSpace;
using namespace StData;

namespace StVirtualMachine {

void setupGroupFunctions()
{
    operations[CREATEGROUP2D] = createGroup2d;
    operations[DELETEGROUP2D] = deleteGroup2d;

    operations[ADDGROUPITEM2D] = addGroupItem2d;
    operations[DELGROUPITEM2D] = delGroupItem2d;
    operations[GETGROUPITEMSNUM2D] = getGroupItemsNum2d;

    operations[GETGROUPITEM2D] = getGroupItem2d;
    operations[SETGROUPITEM2D] = setGroupItem2d;
    operations[SETGROUPITEMS2D] = setGroupItems2d;

    operations[ISGROUPCONTAINOBJECT2D] = isGroupContainObject2d;
    operations[GETOBJECTPARENT2D] = getObjectParent2d;
}

void createGroup2d()
{
    qint16 objectsCount = codePointer->getCode();
    codePointer->incPosition();

    QVector<int> objectHandles(objectsCount);
    for(int i = objectsCount - 1; i >= 0; i--)
        objectHandles[i] = valueStack->popInt32();

    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->createGroup(objectHandles));
    else
        valueStack->pushInt32(0);
}

void deleteGroup2d()
{
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->deleteGroup(groupHandle));
    else
        valueStack->pushDouble(0.0);
}

void addGroupItem2d()
{
    int itemHandle = valueStack->popInt32();
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->addItemToGroup(groupHandle, itemHandle));
    else
        valueStack->pushDouble(0.0);
}

void delGroupItem2d()
{
    int itemHandle = valueStack->popInt32();
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->deleteGroupItem(groupHandle, itemHandle));
    else
        valueStack->pushDouble(0.0);
}

void getGroupItemsNum2d()
{
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->space()->getGroupItemsCount(groupHandle));
    else
        valueStack->pushDouble(0.0);
}

void getGroupItem2d()
{
    int index = valueStack->popDouble();
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->space()->getGroupItem(groupHandle, index));
    else
        valueStack->pushInt32(0);
}

void setGroupItem2d()
{
    int newItemHandle = valueStack->popInt32();
    int replacingItemindex = valueStack->popDouble();
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->setGroupItem2d(groupHandle, replacingItemindex, newItemHandle));
    else
        valueStack->pushDouble(0.0);
}

void setGroupItems2d()
{
    int objectsCount = codePointer->getCode();
    codePointer->incPosition();

    QVector<int> objectHandles(objectsCount);
    for(int i = objectsCount - 1; i >= 0; i--)
        objectHandles[i] = valueStack->popInt32();

    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->setGroupItems2d(groupHandle, objectHandles));
    else
        valueStack->pushDouble(0.0);
}

void getObjectParent2d()
{
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(SpaceItem* spaceItem = widget->space()->getSpaceItem(objectHandle))
        {
            int parentHandle = spaceItem->ownerItem() ? spaceItem->ownerItem()->handle() : 0;
            valueStack->pushInt32(parentHandle);
            return;
        }
    }
    valueStack->pushInt32(0);
}

void isGroupContainObject2d()
{
    int objectHandle = valueStack->popInt32();
    int groupHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    double res = 0.0;
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        GroupItem* groupItem = dynamic_cast<GroupItem*>(widget->space()->getSpaceItem(groupHandle));
        if(SpaceItem* spaceItem = widget->space()->getSpaceItem(objectHandle))
            res = groupItem->indexOf(spaceItem) + 1;
    }
    valueStack->pushDouble(res);
}

}
