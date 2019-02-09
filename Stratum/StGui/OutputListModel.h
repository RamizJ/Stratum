#ifndef LOGMESSAGELISTMODEL_H
#define LOGMESSAGELISTMODEL_H

#include <QAbstractListModel>
#include <Log.h>

//namespace StData {
//class LogMessage;
//enum MessageType;
//}

class OutputListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit OutputListModel(QObject *parent = 0);

    int rowCount(const QModelIndex&) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool canFetchMore(const QModelIndex& /*parent*/) const;
    void fetchMore(const QModelIndex& /*index*/);

    bool isInfoVisible() const {return m_infoVisible;}
    bool isWarningVisible() const {return m_warningVisible;}
    bool isErrorVisible() const {return m_errorVisible;}

    bool isMessageVisible(const StData::LogMessage& message);

public slots:
    void clear();
    void setInfoVisibility(bool visible);
    void setWarningVisibility(bool isVisible);
    void setErrorVisibility(bool isVisible);

protected:
    void timerEvent(QTimerEvent*);

private slots:
    void onMessageWrited(const QString& message, int msgType);
    void updateVisibleList();

private:
    QList<StData::LogMessage> m_allMessageList;
    QList<StData::LogMessage> m_visibleMessageList;
    bool m_infoVisible, m_warningVisible, m_errorVisible;
    int m_fetchItemsCount;
    int m_fetchStep;
    int m_lastCheckItemsCount;
};

#endif // LOGMESSAGELISTMODEL_H
