#ifndef CONTROLGRAPHICSITEM_H
#define CONTROLGRAPHICSITEM_H

#include "SpaceGraphicsItem.h"
#include "stspace_global.h"
#include <QGraphicsProxyWidget>
#include <QStringListModel>

namespace StData {
class ControlItem2d;
class FontTool;
}

namespace StSpace {

class STSPACESHARED_EXPORT ControlGraphicsItem : public QGraphicsProxyWidget, public SpaceGraphicsItem
{
    Q_OBJECT

public:
    ControlGraphicsItem(StData::ControlItem2d* controlItem, QGraphicsItem* parent = nullptr);

    QGraphicsItem* graphicsItem() {return this;}
    StData::ControlItem2d* controlItem() const { return m_controlItem; }

    bool isControlEnabled();
    void setControlEnabled(bool enable);

    void setControlFocus();

    QString text() const;
    void setText(const QString& text);

    void setControlFont(StData::FontTool* fontInfo);

    int getControlStyle();
    void setControlStyle(int style);

    bool isButtonChecked();
    void setButtonChecked(int checked);

    bool listInsertItem(const QString& itemText, int index = -1);
    QString listGetItem(int index);
    void listClear();
    bool listRemoveItem(int index);
    int listItemsCount();
    int listCurrentIndex();
    QList<int> listSelectedIndexes();
    void listSetSelectedIndex(int index);
    int listFindItem(const QString& itemText, int startIndex, bool exact = false);

    // ISpaceGraphicsItem interface
public:
    void setSpaceItemSize(double w, double h);
    void rotateObject2d(const QPointF& base, const double& angle);

    void updateSize();

signals:
    void editTextChanged();
    void editFocusIn();
    void editFocusOut();
    void buttonClicked();

public slots:
    void onTextChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void onFontParamsChanged();

private:
    StData::ControlItem2d* m_controlItem;
    StData::FontTool* m_fontInfo;
    QFont m_defFont;

    QStringListModel* m_listModel;
};

}

#endif // CONTROLGRAPHICSITEM_H
