#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include "stdata_global.h"

#include <QIcon>
#include <QMap>
#include <QPixmap>

namespace StData{

class STDATASHARED_EXPORT IconManager
{
public:
    static IconManager& instance();

    void loadStandardIcons();

    QPixmap iconSetByName(const QString& name);
    QList<QIcon> iconListByName(const QString& name);
    bool containsIconSet(const QString& name);

    QStringList standardIconSets() const {return m_standardIconSets;}

    QString defaultIconSetName();

    QIcon iconByIndex(const QString& iconSetName, int index);

private:
    IconManager();
    IconManager(const IconManager&);
    IconManager& operator=(const IconManager&);

    QString fullName(const QString& shortName);

private:
    QMap<QString, QPixmap> m_iconSetDictionary;
    QMap<QString, QList<QIcon>> m_iconListDictionary;

    QStringList m_standardIconSets;
    int m_defIconWidth;
};

}

#endif // ICONMANAGER_H
