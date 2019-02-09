#include "RasterTextGraphicsItem.h"
#include "GraphicsItemType.h"

#include <QCursor>
#include <QFontDatabase>
#include <QGraphicsSceneContextMenuEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QtMath>
#include <StringHelper.h>

#include <SpaceItem.h>
#include <Space.h>
#include <Tool2d.h>
#include <StDataGlobal.h>

#include <QDebug>
#include <QFontMetrics>

using namespace StData;

namespace StSpace {

RasterTextGraphicsItem::RasterTextGraphicsItem(TextItem2d* textItem2d, QGraphicsItem* parent) :
    SpaceGraphicsItem(textItem2d),
    QGraphicsTextItem(parent),
    m_textItem(textItem2d),
    m_space(textItem2d->space())
{
    m_document = new QTextDocument(this);
    m_document->setDocumentMargin(0);
    setDocument(m_document);
    m_cursor = QTextCursor(m_document);

    m_blockFormat = QTextBlockFormat();
    m_blockFormat.setLineHeight(-1, QTextBlockFormat::LineDistanceHeight);

    updateDocument();

    setPos(m_textItem->origin());
    setRotation(m_textItem->textAngle() / 10.0);

    setVisible(m_textItem->isVisible());
    setZValue(m_textItem->zOrder());

    RasterTextTool* rasterText = m_textItem->rasterText();
    connect(rasterText, &RasterTextTool::textFragmentAdded, this, &RasterTextGraphicsItem::textFragmentAdded);
    connect(rasterText, &RasterTextTool::textFragmentInserted, this, &RasterTextGraphicsItem::textFragmentInserted);
    connect(rasterText, &RasterTextTool::textFragmentRemoved, this, &RasterTextGraphicsItem::textFragmentRemoved);
    connect(rasterText, &RasterTextTool::allTextFragmentsRemoved, this, &RasterTextGraphicsItem::allTextFragmentsRemoved);

    connect(rasterText, &RasterTextTool::textFragmentFgColorChanged, this, &RasterTextGraphicsItem::textFragmentFgColorChanged);
    connect(rasterText, &RasterTextTool::textFragmentBgColorChanged, this, &RasterTextGraphicsItem::textFragmentBgColorChanged);
    connect(rasterText, &RasterTextTool::textFragmentFontChanged, this, &RasterTextGraphicsItem::textFragmentFontChanged);
    connect(rasterText, &RasterTextTool::textFragmentStringChanged, this, &RasterTextGraphicsItem::textFragmentStringChanged);
    //connect(rasterText, &RasterTextTool::textFragmentStringChanging, this, &RasterTextGraphicsItem::textFragmentStringChanging);
}

RasterTextGraphicsItem::~RasterTextGraphicsItem()
{}

QSizeF RasterTextGraphicsItem::renderSize()
{
    RasterTextTool* rasterText = m_textItem->rasterText();
    QList<TextFragment*> textFragmets = rasterText->textFragments();
    if(textFragmets.count() == 0)
        return QSizeF();

    if(textFragmets.count() == 1 && textFragmets[0]->text().isEmpty())
    {
        QFontMetrics fontMetrics(textFragmets[0]->fontTool()->font());
        int px = fontMetrics.height();
        return QSizeF(0, px);
    }

    QTextDocument doc;
    doc.setDocumentMargin(0);
    QTextCursor cursor(&doc);
    for(int i = 0; i < textFragmets.count(); i++)
    {
        TextFragment* textFragment = textFragmets.at(i);
        QTextCharFormat textFormat;
        QString str;
        setupTextFragmentParams(textFragment, str, textFormat);

        if(i == textFragmets.count()-1)
            str = StringHelper::TrimRight(str);

        cursor.setBlockFormat(m_blockFormat);
        cursor.insertText(cleanString(str), textFormat);
    }

    return doc.size();
}

int RasterTextGraphicsItem::type() const
{
    return UserType + static_cast<int>(GraphicsItemType::RichText);
}

QRectF RasterTextGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(), m_textItem->size());
}

void RasterTextGraphicsItem::setSpaceItemSize(double w, double h)
{
    QSizeF newSize(w, h);
    if(newSize != m_textItem->size())
    {
        prepareGeometryChange();
        m_textItem->setSize(newSize);
    }
}

void RasterTextGraphicsItem::rotateObject2d(const QPointF& base, const double& angle)
{
    QPointF origin = m_textItem->origin();
    rotatePoint(&origin, base, angle);
    m_textItem->setOrigin(origin);
    setPos(origin);

    double oldAngle = m_textItem->textAngle();
    double degrees = qRadiansToDegrees(angle);
    m_textItem->setTextAngle(oldAngle + degrees);
    setRotation(oldAngle + degrees);
}

void RasterTextGraphicsItem::updateSize()
{
    if(boundingRect().size() != m_textItem->size())
        prepareGeometryChange();
}

void RasterTextGraphicsItem::textFragmentAdded()
{
    TextFragment* textFragmentTool = m_textItem->rasterText()->textFragments().last();
    QTextCharFormat textFormat;
    QString str;
    setupTextFragmentParams(textFragmentTool, str, textFormat);

    QTextBlock block = m_document->begin();
    seek(block.length());
    m_cursor.insertText(cleanString(str), textFormat);
}

void RasterTextGraphicsItem::textFragmentInserted(int index)
{
    RasterTextTool* rasterText = m_textItem->rasterText();
    TextFragment* textFragment = rasterText->textFragmentAt(index);

    QTextCharFormat textFormat;
    QString str;
    setupTextFragmentParams(textFragment, str, textFormat);

    int textPos = rasterText->getTextPos(index);
    seek(textPos);
    m_cursor.insertText(cleanString(str), textFormat);
}

void RasterTextGraphicsItem::textFragmentRemoved(int index, TextFragment* textFrament)
{
    int pos = m_textItem->rasterText()->getTextPos(index);

    seek(pos);
    seek(pos + getValidLength(textFrament->text()), QTextCursor::KeepAnchor);

    m_cursor.removeSelectedText();
}

void RasterTextGraphicsItem::textFragmentBgColorChanged(TextFragment* tool)
{
    int textPos = m_textItem->rasterText()->getTextPos(tool);
    seek(textPos + 1);
    QTextCharFormat format = m_cursor.charFormat();

    format.setBackground(QBrush(tool->bgColor()));
    seek(textPos);
    seek(textPos + getValidLength(tool->text()), QTextCursor::KeepAnchor);
    m_cursor.setCharFormat(format);
}

void RasterTextGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsTextItem::mousePressEvent(event);
    if(m_hyperJumpActivated)
        spaceItem()->invokeHyperJump();
}

void RasterTextGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if(m_hyperJumpActivated)
        setCursor(Qt::PointingHandCursor);
}

void RasterTextGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if(m_hyperJumpActivated)
        setCursor(Qt::ArrowCursor);
}

void RasterTextGraphicsItem::textFragmentFgColorChanged(TextFragment* tool)
{
    int textPos = m_textItem->rasterText()->getTextPos(tool);
    seek(textPos + 1);

    QTextCharFormat format = m_cursor.charFormat();
    format.setForeground(QBrush(tool->fgColor()));

    m_cursor.setPosition(textPos);
    m_cursor.setPosition(textPos + getValidLength(tool->text()), QTextCursor::KeepAnchor);
    m_cursor.setCharFormat(format);
}

void RasterTextGraphicsItem::textFragmentFontChanged(TextFragment* tool)
{
    int textPos = m_textItem->rasterText()->getTextPos(tool);
    seek(textPos + 1);

    QTextCharFormat format = m_cursor.charFormat();
    format.setFont(tool->fontTool()->font());

    seek(textPos);
    seek(textPos + getValidLength(tool->text()), QTextCursor::KeepAnchor);

    m_cursor.setCharFormat(format);
}

//void RasterTextGraphicsItem::textFragmentStringChanging(TextFragment* textFragment, const QString& /*oldStr*/, const QString& newStr)
//{
//    int textPos = m_textItem->rasterText()->getTextPos(textFragment);
//    seek(textPos + 1);

//    QTextCharFormat format = m_cursor.charFormat();

//    m_cursor.clearSelection();
//    seek(textPos);
//    seek(textPos + getTextLength(textFragment), QTextCursor::KeepAnchor);

//    m_cursor.removeSelectedText();
//    m_cursor.insertText(cleanString(newStr), format);
//}

void RasterTextGraphicsItem::textFragmentStringChanged(TextFragment* textFragment, const QString& oldStr, const QString& newStr)
{
    int textPos = m_textItem->rasterText()->getTextPos(textFragment);

    QTextCharFormat format;
    if(oldStr.isEmpty())
    {
        QString tmp;
        setupTextFragmentParams(textFragment, tmp, format);
    }
    else
    {
        seek(textPos + 1);
        format = m_cursor.charFormat();
    }

    m_cursor.clearSelection();
    seek(textPos);
    seek(textPos + getValidLength(oldStr), QTextCursor::KeepAnchor);

    m_cursor.removeSelectedText();
    m_cursor.insertText(cleanString(newStr), format);
}

void RasterTextGraphicsItem::allTextFragmentsRemoved()
{
    m_document->clear();
}

void RasterTextGraphicsItem::updateDocument()
{
    m_document->clear();
    m_cursor.setPosition(0);

    RasterTextTool* rasterText = m_textItem->rasterText();
    QList<TextFragment*> textFragmets = rasterText->textFragments();
    for(int i = 0; i < textFragmets.count(); i++)
    {
        TextFragment* textFragment = textFragmets.at(i);
        QTextCharFormat textFormat;
        QString str;
        setupTextFragmentParams(textFragment, str, textFormat);

        m_cursor.setBlockFormat(m_blockFormat);

        m_cursor.insertText(cleanString(str), textFormat);
    }
}

void RasterTextGraphicsItem::setupTextFragmentParams(TextFragment* textFragment, QString& str, QTextCharFormat& format)
{
    format.setForeground(QBrush(textFragment->fgColor()));
    format.setBackground(QBrush(textFragment->bgColor()));

    str = textFragment->stringTool()->string();
    if(FontTool* fontTool = textFragment->fontTool())
    {
        QFont font = fontTool->font();
        if(m_fontDatabase.writingSystems(font.family()).contains(QFontDatabase::Symbol))
        {
            QByteArray data = toWindows1251(str);
            str = QString::fromLatin1(data);
//            str = QString::fromLatin1(str.toLocal8Bit());
        }

        format.setFont(font);
//        format.setFontLetterSpacing(-0.2);
//        format.setFontLetterSpacingType(QFont::AbsoluteSpacing);
    }
}

int RasterTextGraphicsItem::getValidLength(/*TextFragment* tool,*/const QString& text)
{
//    int textLength = tool->textLength();
    int textLength = text.length();
    int docLength = m_document->characterCount();

    if(docLength == 0)
        textLength = 0;

    else if(textLength >= docLength)
        textLength = docLength - 1;

    return textLength;
}


void RasterTextGraphicsItem::seek(int index, QTextCursor::MoveMode moveMode)
{
    //int length = m_textItem->rasterText()->length();
    int length = m_document->characterCount();

    if(length == 0)
        index = 0;

    else if(index >= length)
        index = length - 1;

    m_cursor.setPosition(index, moveMode);
}

QString RasterTextGraphicsItem::cleanString(QString str)
{
    return str.replace("\r", "");
}

}
