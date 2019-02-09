#ifndef SPACEWIDGETMANAGER_H
#define SPACEWIDGETMANAGER_H

#include <HandleCollection.h>
#include <QObject>
#include <QPoint>
#include <QSize>
#include <functional>

namespace StData {
class Project;
class Space;
}

namespace StSpace {
class SpaceWidget;
class SpaceScene;
}

namespace StVirtualMachine {

class SpaceWindowItem;
class Array;

class SpaceWindowManager : public QObject
{
    Q_OBJECT
public:
    explicit SpaceWindowManager(QObject *parent = 0);
    ~SpaceWindowManager();

//    StData::Project* project() const { return m_project; }
//    void setProject(StData::Project* project) { m_project = pnroject; }

    QList<SpaceWindowItem*> windowItems() const {return m_windowItems.items();}

    int createWindowEx(const QString& windowName, const QString& parentWindowName,
                       const QString& contentSource, int hSpace,
                       int x, int y, int w, int h, const QString& style);

//    int loadSpaceWindow(const QString& windowName, const QString& className, const QString& style);
    int openSchemeWindow(const QString& windowName, const QString& className, const QString& style);
    int loadSchemeWindow(const QString& windowName, const QString& fileName, const QString& style);
    bool closeClassScheme(const QString& className);

    int closeWindow(const QString& windowName);
    QString getWindowName(int spaceHandle);
    int getWindowSpace(const QString& windowName);

    int getClientWidth(const QString& windowName);
    int getClientHeight(const QString& windowName);
    int setClientSize(const QString& windowName, int w, int h);

    int getWindowOrgX(const QString& windowName);
    int getWindowOrgY(const QString& windowName);
    int setWindowOrg(const QString& windowName, int x, int y);

    int getWindowWidth(const QString& windowName);
    int getWindowHeight(const QString& windowName);
    bool setWindowSize(const QString& windowName, int w, int h);
    bool setWindowPos(const QString& windowName, int x, int y, int w, int h);

    QString getWindowTitle(const QString& windowName);
    int setWindowTitle(const QString& windowName, const QString& title);

    int setWindowTransparentW(const QString windowName, int transparency);
    int setWindowTransparentH(int spaceHandle, int transparency);
    bool setWindowTransparentColorW(const QString& windowName, const QColor& color);
    bool setWindowTransparentColorH(int spaceHandle, const QColor& color);

    int setWindowRegionW(const QString& windowName, Array* array);
    int setWindowRegionH(int spaceHandle, Array* array);

    int showWindow(const QString windowName, int attribute);
    int bringWindowToTop(const QString windowName);

    int isIconic(const QString& windowName);
    int isWindowExist(const QString& windowName);
    int isWindowVisible(const QString& windowName);

    QString getWindowProp(const QString& windowName, const QString& propName);

    void clear(StData::Project* project);
    void clearAll();

private slots:
    void windowClosed();

public:
    StSpace::SpaceWidget* getWidget(const QString& windowName);
    StSpace::SpaceWidget* getWidget(int spaceHandle);
    StSpace::SpaceScene* getSpaceScene(int spaceHandle);

    SpaceWindowItem* getWindowItem(const QString& windowName);
    SpaceWindowItem* getWindowItem(int spaceHandle);

private:
    SpaceWindowItem* take(const QString& name);
    SpaceWindowItem* take(int handle);

    void remove(const QString& name);
    void remove(int handle);

    SpaceWindowItem* getItem(const QString& name);
    SpaceWindowItem* getItem(int spaceHandle);

    int insert(SpaceWindowItem* item);

    SpaceWindowItem* findByName(const QString& name);

    SpaceWindowItem* createWindowItem(const QString& windowName, SpaceWindowItem* parentItem,
                                      StData::Space* space, const QString& style,
                                      const QPoint& pos = QPoint(), const QSize& size = QSize());

    bool isSpaceInWindowMode(StData::Space* space);


private:
    StData::HandleCollection<SpaceWindowItem> m_windowItems;
//    QMultiMap<QString, SpaceWindowItem*> m_winItemForName;
//    StData::Project* m_project;
//    QMap<int, SpaceWidgetItem*> m_winItemForHandle;
};

}

#endif // SPACEWIDGETMANAGER_H
