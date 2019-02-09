#include "OutputListModel.h"

#include <VmLog.h>
#include <Settings.h>

#include <QApplication>
#include <QStyle>

#include <QDebug>

using namespace StData;
using namespace StVirtualMachine;

OutputListModel::OutputListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_infoVisible(true),
    m_warningVisible(true),
    m_errorVisible(true),
    m_fetchItemsCount(0),
    m_fetchStep(50),
    m_lastCheckItemsCount(0)
{
    startTimer(1000);

    connect(&SystemLog::instance(), &SystemLog::messageWrited, this, &OutputListModel::onMessageWrited);
    connect(&VmLog::instance(), &VmLog::messageWrited, this, &OutputListModel::onMessageWrited);
}

int OutputListModel::rowCount(const QModelIndex&) const
{
    return m_fetchItemsCount;
}

QVariant OutputListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(index.isValid())
    {
        switch (role)
        {
            case Qt::EditRole:
            case Qt::DisplayRole:
                result = m_visibleMessageList[index.row()].message();
                break;

            case Qt::DecorationRole:
            {
                switch (m_visibleMessageList[index.row()].messageType())
                {
                    case MessageType::MT_Info:
                        result = QIcon(":/Message/16/Info");
                        break;

                    case MessageType::MT_Warning:
                        result = QIcon(":/Message/16/Warning.png");
                        break;

                    case MessageType::MT_Error:
                        result = QIcon(":/Message/16/Bug.png");
                        break;
                }
            }break;
        }
    }

    return result;
}

bool OutputListModel::setData(const QModelIndex& /*index*/, const QVariant& /*value*/, int /*role*/)
{
    return false;
}

Qt::ItemFlags OutputListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if(index.isValid())
        flags |= Qt::ItemIsEditable;

    return flags;
}

bool OutputListModel::canFetchMore(const QModelIndex& /*parent*/) const
{
    if(m_fetchItemsCount < m_visibleMessageList.count())
        return true;

    return false;
}

void OutputListModel::fetchMore(const QModelIndex&)
{
    int remainder = m_visibleMessageList.count() - m_fetchItemsCount;
    int itemsToFetch = qMin(m_fetchStep, remainder);

    beginInsertRows(QModelIndex(), m_fetchItemsCount, m_fetchItemsCount + itemsToFetch);
    {
        m_fetchItemsCount += itemsToFetch;
    }
    endInsertRows();
}

void OutputListModel::setInfoVisibility(bool isVisible)
{
    if(m_infoVisible == isVisible)
        return;

    m_infoVisible = isVisible;
    updateVisibleList();
}

void OutputListModel::setWarningVisibility(bool isVisible)
{
    if(m_warningVisible == isVisible)
        return;

    m_warningVisible = isVisible;
    updateVisibleList();
}

void OutputListModel::setErrorVisibility(bool isVisible)
{
    if(m_errorVisible == isVisible)
        return;

    m_errorVisible = isVisible;
    updateVisibleList();
}

void OutputListModel::onMessageWrited(const QString& message, int msgType)
{
//#ifdef QT_DEBUG
//#endif
    if(Settings::instance().isLogEnabled())
    {
        LogMessage logMessage(message, static_cast<MessageType>(msgType));
        m_allMessageList << logMessage;

        if(isMessageVisible(logMessage))
            m_visibleMessageList.append(logMessage);
    }
}

void OutputListModel::updateVisibleList()
{
    beginResetModel();
    {
        m_lastCheckItemsCount = 0;
        m_fetchItemsCount = 0;

        m_visibleMessageList.clear();
        for(LogMessage msg : m_allMessageList)
            if(isMessageVisible(msg))
                m_visibleMessageList.append(msg);
    }
    endResetModel();
}

void OutputListModel::timerEvent(QTimerEvent*)
{
    if(m_lastCheckItemsCount != m_visibleMessageList.count())
    {
        m_lastCheckItemsCount = m_visibleMessageList.count();
        emit dataChanged(QModelIndex(), QModelIndex());
    }
}

void OutputListModel::clear()
{
    beginResetModel();
    m_lastCheckItemsCount = 0;
    m_fetchItemsCount = 0;
    m_allMessageList.clear();
    m_visibleMessageList.clear();
    endResetModel();
}

bool OutputListModel::isMessageVisible(const LogMessage& message)
{
    bool isRowInserting = (message.messageType() == MessageType::MT_Info && m_infoVisible) ||
                          (message.messageType() == MessageType::MT_Warning && m_warningVisible) ||
                          (message.messageType() == MessageType::MT_Error && m_errorVisible);

    return isRowInserting;
}
