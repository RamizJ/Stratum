#include "SpaceFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"
#include "Clipboard.h"

#include <JsonSerializer.h>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QPointF>
#include <Space.h>
#include <SpaceItem.h>
#include <SpaceScene.h>
#include <SpaceWidget.h>
#include <Tool2d.h>

using namespace StData;
using namespace StSpace;
using namespace StSerialization;

namespace StVirtualMachine {

#define MIME_TYPE_SPACEITEM "Stratum SpaceItem"

void setupSpaceFunctions()
{
    operations[GETSPACEORGX] = getSpaceOrg2dX;
    operations[GETSPACEORGY] = getSpaceOrg2dY;
    operations[SETSPACEORG2D] = setSpaceOrg2d;

    operations[GETSCALESPACE2D] = getSpaceScale2d;
    operations[SETSCALESPACE2D] = setSpaceScale2d;

    operations[EMPTYSPACE2D] = emptySpace2d;
    operations[VM_SETCRDSYSTEM] = setCrdSystem2d;
    operations[VM_SAVEIMAGE] = saveRectArea2d;

    operations[VM_GETBKBRUSH] = getBkBrush2d;
    operations[VM_SETBKBRUSH] = setBkBrush2d;

    operations[V_LOCK2D] = lockSpace2d;

    operations[V_GETSPACELAYERS] = getSpaceLayers2d;
    operations[V_SETSPACELAYERS] = setSpaceLayers2d;
    operations[COPYTOCLIPBOARD2D] = copyToClipboard;
    operations[PASTEFROMCLIPBOARD2D] = pasteFromClipboard;
}

void getSpaceOrg2dX()
{
    int hSpace = valueStack->popInt32();

    QPointF org;
    if(SpaceWidget* window = windowManager->getWidget(hSpace))
        org = window->getSpaceOrg();

    valueStack->pushDouble(org.x());
}

void getSpaceOrg2dY()
{
    int hSpace = valueStack->popInt32();

    QPointF org;
    if(SpaceWidget* window = windowManager->getWidget(hSpace))
        org = window->getSpaceOrg();

    valueStack->pushDouble(org.y());
}

void setSpaceOrg2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int hSpace = valueStack->popInt32();

    if(SpaceWidget* window = windowManager->getWidget(hSpace))
    {
        window->setSpaceOrg(x, y);
        valueStack->pushDouble(1);
    }
    else
        valueStack->pushDouble(0);
}

void getSpaceScale2d()
{
    int hSpace = valueStack->popInt32();

    double scale = 0.0;
    if(SpaceWidget* window = windowManager->getWidget(hSpace))
        scale = window->getSpaceScale();

    valueStack->pushDouble(scale);
}

void setSpaceScale2d()
{
    double scale = valueStack->popDouble();
    int hSpace = valueStack->popInt32();

    SpaceWindowItem* wndItem = windowManager->getWindowItem(hSpace);
    if(wndItem && scale > 0.01 && scale <= 50)
    {
        wndItem->setSpaceScale(scale);
        valueStack->pushDouble(1.0);
    }
    else
        valueStack->pushDouble(0.0);
}

void emptySpace2d()
{
    throw std::logic_error("emptySpace2d function not implemented");
}

void setCrdSystem2d()
{
    throw std::logic_error("setCrdSystem2d function not implemented");
}

void saveRectArea2d()
{
    double height = valueStack->popDouble();
    double width = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int colorBits = valueStack->popDouble();
    QString fileName = valueStack->popString();
    int hSpace = valueStack->popInt32();

    SpaceWidget* widget = windowManager->getWidget(hSpace);
    if(widget)
    {
        if(QFileInfo(fileName).isRelative())
            fileName = executedProject->projectDir().absoluteFilePath(fileName);

        double res = widget->saveSpaceArea(fileName, colorBits, x, y, width, height);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void getBkBrush2d()
{
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        if(BrushTool* brushTool = w->spaceScene()->bkBrush())
        {
            valueStack->pushInt32(brushTool->handle());
            return;
        }
    }
    valueStack->pushInt32(0);
}

void setBkBrush2d()
{
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        if(BrushTool* brushTool = w->space()->getBrush(brushHandle))
        {
            w->spaceScene()->setBkBrush(brushTool);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void lockSpace2d()
{
    int lock = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        w->lockSceneUpdate(lock);
        valueStack->pushDouble(1.0);
    }
    else
        valueStack->pushDouble(0.0);
}

void getSpaceLayers2d()
{
    int hSpace = valueStack->popInt32();

    int spaceLayers = 0;
    if(SpaceWidget* spaceWidget = windowManager->getWidget(hSpace))
        spaceLayers = spaceWidget->spaceLayers();

    valueStack->pushDouble(spaceLayers);
}

void setSpaceLayers2d()
{
    int spaceLayers = valueStack->popDouble();
    int hSpace = valueStack->popInt32();

    if(SpaceWidget* window = windowManager->getWidget(hSpace))
    {
        window->setSpaceLayers(spaceLayers);
        valueStack->pushDouble(1.0);
    }
    else
        valueStack->pushDouble(0.0);
}

void copyToClipboard()
{
    int spaceItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    double result = 0.0;
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        if(SpaceItem* item = w->space()->getSpaceItem(spaceItemHandle))
        {
#ifdef _DEBUG
            QByteArray data = JsonSerializer(false).serialize(item);
#else
            QByteArray data = JsonSerializer(true).serialize(item);
#endif


#ifdef Q_OS_ANDROID
            Clipboard::setData(data);
#else
            QMimeData* mimeData = new QMimeData();
            mimeData->setData(MIME_TYPE_SPACEITEM, data);
            VmLog::instance().info(QString("Item data: %1").arg(QString(data)));

            QClipboard* c = QApplication::clipboard();
            c->setMimeData(mimeData);
#endif
            result = 1.0;

//            //Test
//            QFile file("SpaceItem.txt");
//            if(file.open(QFile::WriteOnly))
//            {
//                QTextStream stream(&file);
//                stream << data;
//                stream.flush();
//            }
//            file.close();
        }
    }
    valueStack->pushDouble(result);
}

void pasteFromClipboard()
{
    double flags = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    int copyHandle = 0;
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        QByteArray itemData;

#ifdef Q_OS_ANDROID
        itemData = Clipboard::getData();
#else
        QClipboard* clipboard = QApplication::clipboard();
        if(const QMimeData* mimeData = clipboard->mimeData())
        {
            itemData = mimeData->data(MIME_TYPE_SPACEITEM);
        }
#endif

        if(!itemData.isEmpty())
        {
            QList<Tool*> copyTools;
            SpaceItem* copyItem = JsonSerializer().deserialize(itemData, copyTools);
            if(copyItem != nullptr)
            {
                w->spaceScene()->pasteItem(copyItem, copyTools);

                w->spaceScene()->setObjectOrg2d(copyItem->handle(), x, y);
                copyHandle = copyItem->handle();
            }
        }
    }
    valueStack->pushInt32(copyHandle);
}

}
