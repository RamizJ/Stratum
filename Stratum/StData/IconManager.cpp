#include "IconManager.h"

#include "Sc2000DataStream.h"
#include "GraphicsHelper.h"
#include "PathManager.h"

#include <QBitmap>
#include <QFile>
#include <QIcon>

namespace StData {


IconManager::IconManager() :
    m_defIconWidth(32)
{
    m_standardIconSets
            << "ANIMALS.DBM"
            << "ART.DBM"
            << "BE_ICONS.DBM"
            << "DEFAULT.DBM"
            << "DIAMONDS.DBM"
            << "FOLDERS.DBM"
            << "FOLDERS2.DBM"
            << "HARDWARE.DBM"
            << "PEOPLE.DBM"
            << "RECYCLE.DBM"
            << "SCIENCE.DBM"
            << "SIGNS.DBM"
            << "SYSTEM.DBM"
            << "WINDOWS.DBM";
}

QString IconManager::fullName(const QString& shortName)
{
    return PathManager::instance().iconsLibDir().absoluteFilePath(shortName.toUpper());
}

void IconManager::loadStandardIcons()
{
    for(QString iconSetName : m_standardIconSets)
    {
        QString fileName = fullName(iconSetName);
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly))
        {
            St2000DataStream stream(&file);

            QImage image = GraphicsHelper::imageFromData(&stream);
            QPixmap pixmap = QPixmap::fromImage(image);

            QImage maskImage = GraphicsHelper::imageFromData(&stream);
            QBitmap mask = QBitmap::fromImage(maskImage);
            pixmap.setMask(mask);

            m_iconSetDictionary[fileName] = pixmap;

            QList<QIcon> iconList;
            for(int i = 0; i < pixmap.height(); i += 32)
                for(int j = 0; j < pixmap.width(); j += 32)
                    iconList << QIcon(pixmap.copy(j, i, 32, 32));
            m_iconListDictionary[iconSetName] = iconList;
        }
    }
}

QPixmap IconManager::iconSetByName(const QString& name)
{
    return m_iconSetDictionary.value(fullName(name));
}

QList<QIcon> IconManager::iconListByName(const QString& name)
{
    return m_iconListDictionary[name.toUpper()];
}

bool IconManager::containsIconSet(const QString& name)
{
    return m_iconSetDictionary.contains(fullName(name));
}

QString IconManager::defaultIconSetName()
{
    return "SYSTEM.DBM";
}

QIcon IconManager::iconByIndex(const QString& iconSetName, int iconIndex)
{
    QIcon icon;

    QString listName = iconSetName.isEmpty() ? defaultIconSetName() : iconSetName;

    QList<QIcon> iconList = iconListByName(listName);
    if(iconList.isEmpty())
        return icon;

    if(iconIndex >= 0 && iconList.size() > iconIndex)
        icon = iconList[iconIndex];
    else
        icon = iconList[0];

    return icon;
}

IconManager& IconManager::instance()
{
    static IconManager instance;
    return instance;
}

}
