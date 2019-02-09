#include "ControlGraphicsItem.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QTextEdit>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QtMath>

#include <Tool2d.h>
#include <FlagHelper.h>
#include <SpaceItem.h>

using namespace StData;

namespace StSpace {

ControlGraphicsItem::ControlGraphicsItem(ControlItem2d* controlItem, QGraphicsItem* parent) :
    QGraphicsProxyWidget(parent),
    SpaceGraphicsItem(controlItem),
    m_controlItem(controlItem),
    m_fontInfo(nullptr),
    m_listModel(nullptr)
{
    QWidget* widget = nullptr;

    int style = controlItem->style();
    switch(controlItem->classType())
    {
        case ControlItem2d::Button:
        {
            auto pushButton = new QPushButton(controlItem->text());
            connect(pushButton, &QPushButton::clicked, this, &ControlGraphicsItem::buttonClicked);
            //pushButton->installEventFilter(this);
            widget = pushButton;
            break;
        }
        case ControlItem2d::RadioButton:
            widget = new QRadioButton(controlItem->text());
            break;

        case ControlItem2d::CheckBox:
            widget = new QCheckBox(controlItem->text());
            break;

        case ControlItem2d::GroupBox:
            widget = new QGroupBox(controlItem->text());
            break;

        case ControlItem2d::Edit:
        {
            auto lineEdit = new QLineEdit(controlItem->text());

            if(!(style & ControlItem2d::WS_BORDER))
                lineEdit->setFrame(false);

            if(style & ControlItem2d::ES_PASSWORD)
                lineEdit->setEchoMode(QLineEdit::Password);

            if(style & ControlItem2d::ES_READONLY)
                lineEdit->setReadOnly(true);

            //            if(style & ControlItem2d::ES_LOWERCASE)
            //                lineEdit->setInputMethodHints(Qt::ImhPreferLowercase);

            //            if(style & ControlItem2d::ES_UPPERCASE)
            //                lineEdit->setInputMethodHints(Qt::ImhPreferUppercase);

            if(style & ControlItem2d::ES_CENTER)
                lineEdit->setAlignment(Qt::AlignCenter);

            else if(style & ControlItem2d::ES_RIGHT)
                lineEdit->setAlignment(Qt::AlignRight);

            widget = lineEdit;

            lineEdit->installEventFilter(this);
            connect(lineEdit, &QLineEdit::textChanged, this, &ControlGraphicsItem::onTextChanged);
        }break;

        case ControlItem2d::TextEdit:
        {
            QTextEdit* textEdit = new QTextEdit(controlItem->text());

            if(!(style & ControlItem2d::WS_BORDER))
                textEdit->setFrameStyle(QFrame::NoFrame);

            //            if(style & ControlItem2d::ES_PASSWORD)
            //                textEdit->setEchoMode(QLineEdit::Password);

            if(style & ControlItem2d::ES_READONLY)
                textEdit->setReadOnly(true);

            //            if(style & ControlItem2d::ES_LOWERCASE)
            //                textEdit->setInputMethodHints(Qt::ImhUppercaseOnly);

            //            if(style & ControlItem2d::ES_UPPERCASE)
            //            {}

            if(style & ControlItem2d::ES_CENTER)
                textEdit->setAlignment(Qt::AlignCenter);

            else if(style & ControlItem2d::ES_RIGHT)
                textEdit->setAlignment(Qt::AlignRight);

            widget = textEdit;

            textEdit->installEventFilter(this);
            connect(textEdit, &QTextEdit::textChanged, this, &ControlGraphicsItem::onTextChanged);
        }break;

        case ControlItem2d::Combobox:
        {
            QComboBox* comboBox = new QComboBox();

            QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(comboBox);
            proxyModel->setDynamicSortFilter(false);
            proxyModel->setSourceModel(comboBox->model());
            comboBox->model()->setParent(proxyModel);
            comboBox->setModel(proxyModel);

            if(style & ControlItem2d::CBS_DROPDOWN)
                comboBox->setEditable(true);

            widget = comboBox;
        }break;

        case ControlItem2d::Listbox:
        {
            QListView* listView = new QListView();
            listView->setEditTriggers(QListView::NoEditTriggers);

            QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(listView);
            m_listModel = new QStringListModel(proxyModel);
            proxyModel->setDynamicSortFilter(false);
            proxyModel->setSourceModel(m_listModel);
            listView->setModel(proxyModel);

            if((style & ControlItem2d::LBS_EXTENDEDSEL) == ControlItem2d::LBS_EXTENDEDSEL)
                listView->setSelectionMode(QListView::ExtendedSelection);
            else if((style & ControlItem2d::LBS_MULTIPLESEL) == ControlItem2d::LBS_MULTIPLESEL)
                listView->setSelectionMode(QListView::MultiSelection);

            widget = listView;
        }break;

        case ControlItem2d::Scrollbar:
            widget = new QScrollBar();
            break;

        default:
            break;
    }

    if(widget)
    {
        setMinimumSize(QSizeF(1, 1));
        widget->setMinimumSize(QSize(1, 1));

        setWidget(widget);

        m_defFont = widget->font();

        if(controlItem->classType() == ControlItem2d::Combobox)
        {
            int w = m_controlItem->size().toSize().width();
            resize(w, widget->height());
        }
        else
            resize(m_controlItem->size().toSize());

        setPos(m_controlItem->origin());
        setZValue(m_controlItem->zOrder());
    }
}

bool ControlGraphicsItem::isControlEnabled()
{
    return m_controlItem->isEnabled();
}

void ControlGraphicsItem::setControlEnabled(bool enable)
{
    m_controlItem->setEnabled(enable);
    widget()->setEnabled(enable);
}

void ControlGraphicsItem::setControlFocus()
{
    widget()->setFocus();
}

QString ControlGraphicsItem::text() const
{
    return m_controlItem->text();
}

void ControlGraphicsItem::setText(const QString& text)
{
    //    if(m_controlItem->text() != text)
    {
        m_controlItem->setText(text);

        if(auto pushButton = dynamic_cast<QPushButton*>(widget()))
            pushButton->setText(text);

        else if(auto radioButton = dynamic_cast<QRadioButton*>(widget()))
            radioButton->setText(text);

        else if(auto checkBox = dynamic_cast<QCheckBox*>(widget()))
            checkBox->setText(text);

        else if(auto groupBox = dynamic_cast<QGroupBox*>(widget()))
            groupBox->setTitle(text);

        else if(auto lineEdit = dynamic_cast<QLineEdit*>(widget()))
            lineEdit->setText(text);

        else if(auto textEdit = dynamic_cast<QTextEdit*>(widget()))
            textEdit->setText(text);

        else if(auto comboBox = dynamic_cast<QComboBox*>(widget()))
            comboBox->setCurrentText(text);

        else if(auto listView = dynamic_cast<QListView*>(widget()))
        {
            QAbstractItemModel* model = listView->model();
            QModelIndexList list = model->match(m_listModel->index(0,0), Qt::DisplayRole, text);
            if(!list.isEmpty())
            {
                QModelIndex index = list.first();
                if(index.isValid())
                    listView->setCurrentIndex(index);
            }
        }
    }
}

void ControlGraphicsItem::setControlFont(FontTool* fontInfo)
{
    if(fontInfo)
    {
        if(m_fontInfo)
            disconnect(m_fontInfo);

        m_fontInfo = fontInfo;
        connect(m_fontInfo, &FontTool::changed, this, &ControlGraphicsItem::onFontParamsChanged);

        QFont f = fontInfo->font();
        double fontSize = f.pointSizeF();
        if(fontSize != 8.25)
            f.setPointSizeF(f.pointSize()/2.0);
        else
            f.setPointSize(12);

        setFont(f);
        if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        {
            QFontMetrics defFontMetrics(m_defFont);
            QFontMetrics fontMetrics(f);
            if(fontMetrics.height() < defFontMetrics.height())
                comboBox->resize(comboBox->width(), defFontMetrics.height() + 8);
            else
                comboBox->resize(comboBox->width(), fontMetrics.height() + 8);
        }
    }
}

int ControlGraphicsItem::getControlStyle()
{
    return m_controlItem->style();
}

void ControlGraphicsItem::setControlStyle(int style)
{
    switch(m_controlItem->classType())
    {
        case ControlItem2d::Edit:
        {
            QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(widget());

            lineEdit->setFrame((style & ControlItem2d::WS_BORDER));

            if(style & ControlItem2d::ES_PASSWORD)
                lineEdit->setEchoMode(QLineEdit::Password);
            else
                lineEdit->setEchoMode(QLineEdit::Normal);

            lineEdit->setReadOnly(style & ControlItem2d::ES_READONLY);

            //            if(style & ControlItem2d::ES_LOWERCASE)
            //                lineEdit->setInputMethodHints(Qt::ImhPreferLowercase);

            //            if(style & ControlItem2d::ES_UPPERCASE)
            //                lineEdit->setInputMethodHints(Qt::ImhPreferUppercase);

            if(style & ControlItem2d::ES_CENTER)
                lineEdit->setAlignment(Qt::AlignCenter);

            else if(style & ControlItem2d::ES_RIGHT)
                lineEdit->setAlignment(Qt::AlignRight);

            else
                lineEdit->setAlignment(Qt::AlignLeft);
        }break;

        case ControlItem2d::TextEdit:
        {
            QTextEdit* textEdit = dynamic_cast<QTextEdit*>(widget());

            if(!(style & ControlItem2d::WS_BORDER))
                textEdit->setFrameStyle(QFrame::NoFrame);
            else
                textEdit->setFrameStyle(QFrame::Plain);

            //            if(style & ControlItem2d::ES_PASSWORD)
            //                textEdit->setEchoMode(QLineEdit::Password);

            textEdit->setReadOnly(style & ControlItem2d::ES_READONLY);

            //            if(style & ControlItem2d::ES_LOWERCASE)
            //                textEdit->setInputMethodHints(Qt::ImhUppercaseOnly);

            //            if(style & ControlItem2d::ES_UPPERCASE)
            //            {}

            if(style & ControlItem2d::ES_CENTER)
                textEdit->setAlignment(Qt::AlignCenter);

            else if(style & ControlItem2d::ES_RIGHT)
                textEdit->setAlignment(Qt::AlignRight);

            else
                textEdit->setAlignment(Qt::AlignLeft);
        }break;

        case ControlItem2d::Combobox:
        {
            QComboBox* comboBox = dynamic_cast<QComboBox*>(widget());
            comboBox->setEditable(style & ControlItem2d::CBS_DROPDOWN);
        }break;

        case ControlItem2d::Listbox:
        {
            QListView* listView = dynamic_cast<QListView*>(widget());
            if((style & ControlItem2d::LBS_EXTENDEDSEL) == ControlItem2d::LBS_EXTENDEDSEL)
                listView->setSelectionMode(QListView::ExtendedSelection);

            else if((style & ControlItem2d::LBS_MULTIPLESEL) == ControlItem2d::LBS_MULTIPLESEL)
                listView->setSelectionMode(QListView::MultiSelection);
        }break;

        default:
            break;
    }
}

bool ControlGraphicsItem::isButtonChecked()
{
    if(QAbstractButton* abstractButton = dynamic_cast<QAbstractButton*>(widget()))
        return abstractButton->isChecked();

    return false;
}

void ControlGraphicsItem::setButtonChecked(int checked)
{
    if(QAbstractButton* abstractButton = dynamic_cast<QAbstractButton*>(widget()))
        abstractButton->setChecked(checked == 1);

    if((m_controlItem->style() & ControlItem2d::BS_3STATE) ||
            (m_controlItem->style() & ControlItem2d::BS_AUTO3STATE))
    {
        if(QCheckBox* checkBox = dynamic_cast<QCheckBox*>(widget()))
        {
            switch(checked)
            {
                case 0:
                    checkBox->setCheckState(Qt::Unchecked);
                    break;

                case 1:
                    checkBox->setCheckState(Qt::Checked);
                    break;

                case 2:
                    checkBox->setCheckState(Qt::PartiallyChecked);
                    break;
            }
        }
    }
}

bool ControlGraphicsItem::listInsertItem(const QString& itemText, int index)
{
    QAbstractItemModel* model = nullptr;
    bool sort = false;
    bool isEmptyIndex = false;
    QComboBox* comboBox = nullptr;

    if(QListView* listView = dynamic_cast<QListView*>(widget()))
    {
        model = listView->model();
        sort = (m_controlItem->style() & ControlItem2d::LBS_SORT);
    }
    else if(comboBox = dynamic_cast<QComboBox*>(widget()))
    {
        model = comboBox->model();
        sort = (m_controlItem->style() & ControlItem2d::CBS_SORT);
        if(comboBox->currentIndex() == -1)
            isEmptyIndex = true;
    }

    if(model && index == -1)
        index = model->rowCount();

    if(model && (index >= 0 && index <= model->rowCount()))
    {
        model->insertRow(index);
        QModelIndex modelIndex = model->index(index, 0);
        model->setData(modelIndex, itemText);

        if(comboBox && isEmptyIndex)
            comboBox->setCurrentIndex(-1);

        if(sort)
        {
            auto proxyModel = static_cast<QSortFilterProxyModel*>(model);
            proxyModel->sort(0);
        }
        return true;
    }

    return false;
}

QString StSpace::ControlGraphicsItem::listGetItem(int index)
{
    QAbstractItemModel* model = nullptr;
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
        model = listView->model();

    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        model = comboBox->model();

    if(model && (index >= 0 && index < model->rowCount()))
    {
        QModelIndex modelIndex = model->index(index, 0);
        return modelIndex.data().toString();
    }
    return "";
}

void StSpace::ControlGraphicsItem::listClear()
{
    QAbstractItemModel* model = nullptr;
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
        model = listView->model();

    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        model = comboBox->model();

    if(model)
        model->removeRows(0, model->rowCount());
}

bool StSpace::ControlGraphicsItem::listRemoveItem(int index)
{
    QAbstractItemModel* model = nullptr;
    bool sort = false;
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
    {
        model = listView->model();
        sort = (m_controlItem->style() & ControlItem2d::LBS_SORT);
    }
    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
    {
        model = comboBox->model();
        sort = (m_controlItem->style() & ControlItem2d::CBS_SORT);
    }

    if(model && (index >= 0 && index < model->rowCount()))
    {
        model->removeRow(index);

        if(sort)
        {
            auto proxyModel = static_cast<QSortFilterProxyModel*>(model);
            proxyModel->sort(0);
        }
        return true;
    }
    return false;
}

int StSpace::ControlGraphicsItem::listItemsCount()
{
    QAbstractItemModel* model = nullptr;
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
        model = listView->model();

    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        model = comboBox->model();

    if(model)
        return model->rowCount();

    return 0;
}

int StSpace::ControlGraphicsItem::listCurrentIndex()
{
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
        if(listView->currentIndex().isValid())
            return listView->currentIndex().row() + 1;

    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        return comboBox->currentIndex() + 1;

    return 0;
}

QList<int> StSpace::ControlGraphicsItem::listSelectedIndexes()
{
    QList<int> result;

    if(QListView* listView = dynamic_cast<QListView*>(widget()))
        for(QModelIndex selectedIndex : listView->selectionModel()->selectedRows(0))
            result << selectedIndex.row();

    return result;
}

void StSpace::ControlGraphicsItem::listSetSelectedIndex(int index)
{
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
    {
        if(index >=0 && index < listView->model()->rowCount())
        {
            QModelIndex modelIndex = listView->model()->index(index, 0);
            listView->setCurrentIndex(modelIndex);
        }
    }
    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        comboBox->setCurrentIndex(index);
}

int StSpace::ControlGraphicsItem::listFindItem(const QString& itemText, int startIndex, bool exact)
{
    QAbstractItemModel* model = nullptr;
    if(QListView* listView = dynamic_cast<QListView*>(widget()))
        model = listView->model();

    else if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        model = comboBox->model();

    if(model)
    {
        for(int i = 0; i < model->rowCount(); i++)
        {
            QModelIndex modelIndex = model->index(i, 0);
            if(modelIndex.row() >= startIndex)
            {
                if(exact)
                {
                    if(modelIndex.data().toString().toLower().startsWith(itemText.toLower()))
                        return modelIndex.row();
                }
                else if(modelIndex.data().toString().compare(itemText, Qt::CaseInsensitive) == 0)
                    return modelIndex.row();
            }
        }
    }
    return -1;
}

void ControlGraphicsItem::setSpaceItemSize(double w, double h)
{
    QSizeF newSize(w, h);
    if(newSize != m_controlItem->size())
    {
        m_controlItem->setSize(newSize);
        resize(newSize);
    }
}

void ControlGraphicsItem::rotateObject2d(const QPointF& base, const double& angle)
{
    QPointF origin = m_controlItem->origin();
    rotatePoint(&origin, base, angle);
    m_controlItem->setOrigin(origin);

    double a = m_controlItem->angle();
    a += qRadiansToDegrees(angle);
    m_controlItem->setAngle(a);

    setRotation(a);
    setPos(origin);
}

void ControlGraphicsItem::updateSize()
{
    if(size() != m_controlItem->size())
        resize(m_controlItem->size());
}

void ControlGraphicsItem::onTextChanged()
{
    if(QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(sender()))
    {
        m_controlItem->setText(lineEdit->text());
        emit editTextChanged();
    }
    else if(QTextEdit* textEdit = dynamic_cast<QTextEdit*>(sender()))
    {
        m_controlItem->setText(textEdit->toPlainText());
        emit editTextChanged();
    }
}

void ControlGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(m_hyperJumpActivated)
        spaceItem()->invokeHyperJump();
    else
        QGraphicsProxyWidget::mousePressEvent(event);
}

bool ControlGraphicsItem::eventFilter(QObject* watched, QEvent* event)
{
    if(dynamic_cast<QLineEdit*>(watched) || dynamic_cast<QTextEdit*>(watched))
    {
        if(event->type() == QEvent::FocusIn)
            emit editFocusIn();

        else if(event->type() == QEvent::FocusOut)
            emit editFocusOut();
    }

    return false;
}

void ControlGraphicsItem::onFontParamsChanged()
{
    if(m_fontInfo)
    {
        setFont(m_fontInfo->font());
        if(QComboBox* comboBox = dynamic_cast<QComboBox*>(widget()))
        {
            QFontMetrics defFontMetrics(m_defFont);
            QFontMetrics fontMetrics(m_fontInfo->font());
            if(fontMetrics.height() < defFontMetrics.height())
                comboBox->resize(comboBox->width(), defFontMetrics.height() + 8);
            else
                comboBox->resize(comboBox->width(), fontMetrics.height() + 8);
        }
    }
}

}
