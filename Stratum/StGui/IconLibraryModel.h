#ifndef ICONLIBRARYMODEL_H
#define ICONLIBRARYMODEL_H

#include <QAbstractListModel>

typedef QList<QIcon> IconList;

class IconLibraryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    IconLibraryModel(QObject* parent);
    ~IconLibraryModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

public slots:
    void setIconSet(const QString& iconSetName);

private:
    IconList m_currentIconList;
};

#endif // ICONLIBRARYMODEL_H
