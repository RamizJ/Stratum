#include "TextFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"

#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <Space.h>
#include <Tool2d.h>
#include <SpaceItem.h>
#include <StDataGlobal.h>
#include <FontHelper.h>

#include <QtMath>

#ifdef Q_OS_WIN
#include <FontHelper.h>
#include <QApplication>
#include <QScreen>
#include <windows.h>
#include <WinBase.h>
#endif

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupTextFunctions()
{
    //Font
    operations[CREATEFONT2D] = createFont2dPx;
    operations[VM_CREATEFONT2DPT] = createFont2dPt;

    operations[VM_GETFONTNAME2D] = getFontName2d;
    operations[VM_GETFONTSIZE2D] = getFontSize2d;
    operations[VM_GETFONTSTYLE2D] = getFontStyle2d;
    operations[VM_SETFONTNAME2D] = setFontName2d;
    operations[VM_SETFONTSIZE2D] = setFontSize2d;
    operations[VM_SETFONTSTYLE2D] = setFontStyle2d;
    operations[VM_GETFONTLIST] = getFontList;

    //String
    operations[CREATESTRING2D] = createString2d;
    operations[GETLOGSTRING2D] = getString2d;
    operations[SETLOGSTRING2D] = setString2d;

    //TEXT
    operations[CREATETEXT2D] = createText2d;
    operations[CREATERASTERTEXT2D] = createRasterText2d;
    operations[GETTEXTOBJECT2D] = getTextObject2d;
    operations[VM_GETTEXTCOUNT2D] = getTextCount2d;

    operations[GETTEXTFONT2D] = getTextFont2d;
    operations[VM_GETTEXTFONT2D_I] = getTextFont2d_i;

    operations[GETTEXTSTRING2D] = getTextString2d;
    operations[VM_GETTEXTSTRING2D_I] = getTextString2d_i;

    operations[GETTEXTFG2D] = getTextFgColor2d;
    operations[VM_GETTEXTFG2D_I] = getTextFgColor2d_i;

    operations[GETTEXTBK2D] = getTextBkColor2d;
    operations[VM_GETTEXTBG2D_I] = getTextBkColor2d_i;

    operations[VM_SETLOGTEXT2D] = setText2d;
    operations[VM_SETTEXT2D_I] = setText2d_i;

    operations[VM_SETTEXTFONT2D_I] = setTextFont2d;
    operations[VM_SETTEXTSTRING2D_I] = setTextString2d;
    operations[VM_SETTEXTFG2D_I] = setTextFgColor2d;
    operations[VM_SETTEXTBG2D_I] = setTextBkColor2d;

    operations[VM_ADDTEXT2D] = addText2d;
    operations[VM_INSERTTEXT2D] = insertText2d;
    operations[VM_REMOVETEXT2D] = removeText2d;
}

void createFont(bool sizeInPixels)
{
    int fontFlags = valueStack->popDouble();
    double fontSize = fabs(valueStack->popDouble());
    QString fontFamily = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    QFont font;
    font.setItalic(fontFlags & 1);
    font.setUnderline(fontFlags & 2);
    font.setStrikeOut(fontFlags & 4);
    font.setBold(fontFlags & 8);
    font.setFamily(FontHelper::getCleanName(fontFamily));

    if(sizeInPixels)
    {
        font.setPointSizeF(fontSize);
        QFontMetrics fm(font);
        double k = fontSize / fm.height();
        font.setPointSizeF(k*fontSize);

        VmLog::instance().info(QString("k*fontSize = ").arg(k*fontSize));
    }
    else
    {
        font.setPointSizeF(fontSize);
    }

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        valueStack->pushInt32(widget->space()->createFont(font, sizeInPixels));
        return;
    }
    valueStack->pushInt32(0);
}

void getTextFont(int index = 0)
{
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
            if(index >= 0 && index < rasterText->textFragments().count())
            {
                TextFragment* text = rasterText->textFragments().at(index);
                if(text->fontTool())
                {
                    valueStack->pushInt32(text->fontTool()->handle());
                    return;
                }
            }

    valueStack->pushInt32(0);
}

void getTextString(int index = 0)
{
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
            if(index >= 0 && index < rasterText->textFragments().count())
            {
                TextFragment* text = rasterText->textFragments().at(index);
                if(text->stringTool())
                {
                    valueStack->pushInt32(text->stringTool()->handle());
                    return;
                }
            }

    valueStack->pushInt32(0);
}

void getTextFgColor(int index = 0)
{
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            QList<TextFragment*> textInfoItems = rasterText->textFragments();
            if(index >= 0 && index < textInfoItems.count())
            {
                QColor color = textInfoItems.at(index)->fgColor();
                valueStack->pushInt32(colorToInt(color));
                return;
            }
        }
    }
    valueStack->pushInt32(0);
}

void getTextBgColor(int index = 0)
{
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            QList<TextFragment*> textInfoItems = rasterText->textFragments();
            if(index >= 0 && index < textInfoItems.count())
            {
                QColor color = textInfoItems.at(index)->bgColor();
                valueStack->pushInt32(colorToInt(color));
                return;
            }
        }
    }
    valueStack->pushInt32(0);
}

void setText(int spaceHandle, int rasterTextHandle, int index,
             int fontHandle, int stringHandle, int bgRgba, int fgRgba)
{
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
            if(index >= 0 && index < rasterText->textFragments().count())
            {
                FontTool* font = widget->space()->getFont(fontHandle);
                StringTool* string = widget->space()->getString(stringHandle);

                TextFragment* text = rasterText->textFragments().at(index);
                text->setFont(font);
                text->setString(string);
                text->setFgColor(intToColor(fgRgba));
                text->setBgColor(intToColor(bgRgba));
            }
}

void insertText(int spaceHandle, int rasterTextHandle, int index, int fontHandle, int stringHandle,
                int bgRgba, int fgRgba)
{
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        QColor bgColor = intToColor(bgRgba);
        QColor fgColor = intToColor(fgRgba);
        bool result = widget->space()->insertText(rasterTextHandle, index, fontHandle, stringHandle, bgColor, fgColor);
        valueStack->pushDouble(result);
        return;
    }
    valueStack->pushDouble(0.0);
}

void createFont2dPx()
{
    createFont(true);
}

void createFont2dPt()
{
    createFont(false);
}

void getFontName2d()
{
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(FontTool* fontInfo = widget->space()->getFont(fontHandle))
        {
            valueStack->pushString(fontInfo->family());
            return;
        }
    }
    valueStack->pushString("");
}

void getFontSize2d()
{
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(FontTool* fontInfo = widget->space()->getFont(fontHandle))
            if(!fontInfo->sizeInPixels())
            {
                valueStack->pushDouble(fontInfo->font().pointSize());
                return;
            }
    }
    valueStack->pushDouble(0.0);
}

void getFontStyle2d()
{
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(FontTool* fontInfo = widget->space()->getFont(fontHandle))
        {
            valueStack->pushDouble(fontInfo->style());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setFontName2d()
{
    QString fontFamily = valueStack->popString();
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(FontTool* fontInfo = widget->space()->getFont(fontHandle))
        {
            fontInfo->setFamily(fontFamily);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setFontSize2d()
{
    int fontSize = valueStack->popDouble();
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(FontTool* fontInfo = widget->space()->getFont(fontHandle))
        {
            fontInfo->setSizeInPixels(false);
            fontInfo->setSize(fontSize);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setFontStyle2d()
{
    int fontStyle = valueStack->popDouble();
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(FontTool* fontInfo = widget->space()->getFont(fontHandle))
        {
            fontInfo->setStyle(fontStyle);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void getFontList()
{
    int arrayHandle = arrayManager->createArray();
    int i = 0;
    for(QString family : fontDataBase->families())
    {
        arrayManager->insertItem(arrayHandle, "string");
        arrayManager->setVarS(arrayHandle, i, "string", family);
        i++;
    }
    valueStack->pushInt32(arrayHandle);
}

void createString2d()
{
    QString str = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        valueStack->pushInt32(widget->space()->createString(str));
        return;
    }
    valueStack->pushInt32(0);
}

void getString2d()
{
    int stringHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(StringTool* strTool = widget->space()->getString(stringHandle))
        {
            valueStack->pushString(strTool->string());
            return;
        }
    }
    valueStack->pushString("");
}

void setString2d()
{
    QString stringData = valueStack->popString();
    int stringHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(StringTool* strTool = widget->space()->getString(stringHandle))
        {
            strTool->setString(stringData);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void createText2d()
{
    int bgRgba = valueStack->popInt32();
    int fgRgba = valueStack->popInt32();
    int stringHandle = valueStack->popInt32();
    int fontHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        QColor bgColor = intToColor(bgRgba);
        QColor fgColor = intToColor(fgRgba);
        valueStack->pushInt32(widget->space()->createText(fontHandle, stringHandle, bgColor, fgColor));
    }
    else
        valueStack->pushInt32(0);
}

void createRasterText2d()
{
    double angle = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int textHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->createRasterText(textHandle, x, y, angle));
    else
        valueStack->pushInt32(0);
}

void getTextObject2d()
{
    int textItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        SpaceItem* spaceItem = widget->space()->getSpaceItem(textItemHandle);
        if(TextItem2d* textItem = dynamic_cast<TextItem2d*>(spaceItem))
            if(textItem->rasterText())
            {
                valueStack->pushInt32(textItem->rasterText()->handle());
                return;
            }
    }
    valueStack->pushInt32(0);
}

void getTextCount2d()
{
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            valueStack->pushDouble(rasterText->textFragments().count());
            return;
        }

    valueStack->pushDouble(0.0);
}

void getTextFont2d()
{
    getTextFont();
}

void getTextString2d()
{
    getTextString();
}

void getTextFgColor2d()
{
    getTextFgColor();
}

void getTextBkColor2d()
{
    getTextBgColor();
}

bool isRasterTextEqual(RasterTextTool* rasterText, int fontHandle, int stringHandle, int bgRgba, int fgRgba)
{
    if(rasterText->textFragments().count() != 1)
        return false;

    if(TextFragment* textFragment = rasterText->textFragmentAt(0))
    {
        if(textFragment->fontHandle() != fontHandle)
            return false;

        if(textFragment->stringHandle() != stringHandle)
            return false;

        if(textFragment->bgColor() != intToColor(bgRgba))
            return false;

        if(textFragment->fgColor() != intToColor(fgRgba))
            return false;
    }
    else
        return false;

    return true;
}

void setText2d()
{
    int bgRgba = valueStack->popInt32();
    int fgRgba = valueStack->popInt32();
    int stringHandle = valueStack->popInt32();
    int fontHandle = valueStack->popInt32();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    bool result = false;
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            if(!isRasterTextEqual(rasterText, fontHandle, stringHandle, bgRgba, fgRgba))
            {
                rasterText->clear();
                Space* space = widget->space();

                TextFragment* textFragment = new TextFragment();
                textFragment->setFont(space->getFont(fontHandle));
                textFragment->setString(space->getString(stringHandle));
                textFragment->setFgColor(intToColor(fgRgba));
                textFragment->setBgColor(intToTextColor(bgRgba));

                rasterText->append(textFragment);
                result = true;
            }
        }
    }
    valueStack->pushDouble(result);
}

void setText2d_i()
{
    int bgRgba = valueStack->popInt32();
    int fgRgba = valueStack->popInt32();
    int stringHandle = valueStack->popInt32();
    int fontHandle = valueStack->popInt32();
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    bool result = false;
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
            if(index >= 0 && index < rasterText->textFragments().count())
            {
                FontTool* font = widget->space()->getFont(fontHandle);
                StringTool* string = widget->space()->getString(stringHandle);

                TextFragment* text = rasterText->textFragments().at(index);
                text->setFont(font);
                text->setString(string);
                text->setFgColor(intToColor(fgRgba));
                text->setBgColor(intToTextColor(bgRgba));

                result = true;
            }
    }
    valueStack->pushDouble(result);
}

void setTextFont2d()
{
    int fontHandle = valueStack->popInt32();
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    bool result = false;
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            FontTool* font = widget->space()->getFont(fontHandle);
            if(font && index >= 0 && index < rasterText->textFragments().count())
            {
                TextFragment* text = rasterText->textFragments().at(index);
                text->setFont(font);
                result = true;
            }
        }
    }
    valueStack->pushDouble(result);
}

void setTextString2d()
{
    int stringHandle = valueStack->popInt32();
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            StringTool* string = widget->space()->getString(stringHandle);
            if(string && index >= 0 && index < rasterText->textFragments().count())
            {

                TextFragment* text = rasterText->textFragments().at(index);
                text->setString(string);
                valueStack->pushDouble(1.0);
                return;
            }
        }

    valueStack->pushDouble(0.0);
}

void setTextFgColor2d()
{
    int fgRgba = valueStack->popInt32();
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            if(index >= 0 && index < rasterText->textFragments().count())
            {
                TextFragment* textFragment = rasterText->textFragments().at(index);
                textFragment->setFgColor(intToColor(fgRgba));
                valueStack->pushDouble(1.0);
                return;
            }
        }
    }
    valueStack->pushDouble(0.0);
}

void setTextBkColor2d()
{
    int bgRgba = valueStack->popInt32();
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(RasterTextTool* rasterText = widget->space()->getRasterText(rasterTextHandle))
        {
            if(index >= 0 && index < rasterText->textFragments().count())
            {
                TextFragment* textInfo = rasterText->textFragments().at(index);
                textInfo->setBgColor(intToColor(bgRgba));
                valueStack->pushDouble(1.0);
                return;
            }
        }
    }
    valueStack->pushDouble(0.0);
}

void addText2d()
{
    int bgRgba = valueStack->popInt32();
    int fgRgba = valueStack->popInt32();
    int stringHandle = valueStack->popInt32();
    int fontHandle = valueStack->popInt32();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(spaceHandle == 0)
        return;

    int index = -1;
    insertText(spaceHandle, rasterTextHandle, index, fontHandle, stringHandle, bgRgba, fgRgba);
}

void insertText2d()
{
    int bgRgba = valueStack->popInt32();
    int fgRgba = valueStack->popInt32();
    int stringHandle = valueStack->popInt32();
    int fontHandle = valueStack->popInt32();
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    insertText(spaceHandle, rasterTextHandle, index, fontHandle, stringHandle, bgRgba, fgRgba);
}

void removeText2d()
{
    int index = valueStack->popDouble();
    int rasterTextHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        bool result = widget->space()->removeText(rasterTextHandle, index);
        valueStack->pushDouble(result);
        return;
    }
    valueStack->pushDouble(0.0);
}

void getTextFont2d_i()
{
    int index = valueStack->popDouble();
    getTextFont(index);
}

void getTextString2d_i()
{
    int index = valueStack->popDouble();
    getTextString(index);
}

void getTextFgColor2d_i()
{
    int index = valueStack->popDouble();
    getTextFgColor(index);
}

void getTextBkColor2d_i()
{
    int index = valueStack->popDouble();
    getTextBgColor(index);
}

}
