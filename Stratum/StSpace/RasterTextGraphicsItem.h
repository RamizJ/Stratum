#ifndef RICHTEXTGRAPHICSITEM_H
#define RICHTEXTGRAPHICSITEM_H

#include "stspace_global.h"
#include "SpaceGraphicsItem.h"

#include <QFontDatabase>
#include <QGraphicsTextItem>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <memory>

namespace StData{
class TextItem2d;
class Space;
class RasterTextTool;
class TextFragment;
class FontTool;
class StringTool;
}

namespace StSpace {

typedef std::shared_ptr<QTextDocument> QTextDocumentPtr;

class STSPACESHARED_EXPORT RasterTextGraphicsItem : public QGraphicsTextItem, public SpaceGraphicsItem
{
public:
    RasterTextGraphicsItem(StData::TextItem2d* textItem, QGraphicsItem* parent = 0);
    ~RasterTextGraphicsItem();

    QGraphicsItem* graphicsItem() {return this;}

    StData::TextItem2d* textItem() const {return m_textItem;}
    void setTextItem2d(StData::TextItem2d* textItem2d) {m_textItem = textItem2d;}

    QTextDocument* document() const { return m_document; }
    QSizeF renderSize();

public:
    int type() const;
    QRectF boundingRect() const;

    // ISpaceGraphicsItem interface
public:
    void setSpaceItemSize(double w, double h);
    void rotateObject2d(const QPointF& base, const double& angle);
    void updateSize();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private slots:
    void textFragmentAdded();
    void textFragmentInserted(int index);
    void textFragmentRemoved(int pos, StData::TextFragment* textFrament);

    void textFragmentBgColorChanged(StData::TextFragment* textFragmentTool);
    void textFragmentFgColorChanged(StData::TextFragment* tool);
    void textFragmentFontChanged(StData::TextFragment* tool);
    void textFragmentStringChanging(StData::TextFragment* textFragment, const QString& oldStr, const QString& newStr);
    void textFragmentStringChanged(StData::TextFragment* textFragment, const QString& oldStr, const QString& newStr);
    void allTextFragmentsRemoved();
//    void textChanged();

private:
    void updateDocument();
    void setupTextFragmentParams(StData::TextFragment* tool, QString& str, QTextCharFormat& format);
    int getValidLength(const QString& text);
    void seek(int index, QTextCursor::MoveMode moveMode = QTextCursor::MoveAnchor);
    QString cleanString(QString str);

private:
    StData::Space* m_space;
    StData::TextItem2d* m_textItem;

    QTextDocument* m_document;
    QTextCursor m_cursor;
    QFontDatabase m_fontDatabase;

    QTextBlockFormat m_blockFormat;

//    QMap<StData::TextFragmentTool*, QTextFragment> m_textFragmentMap;
    // QGraphicsItem interface
};

}

#endif // RICHTEXTGRAPHICSITEM_H
