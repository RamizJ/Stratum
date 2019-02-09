#include "MainWindow.h"
#include "QtMsgFileLogger.h"
#include "RemoteLogger.h"

#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QFontDatabase>
#include <Settings.h>

#include <Core.h>
#include <Log.h>
#include <Error.h>
#include <QDebug>
#include <AndroidFunctions.h>
#include <StDataGlobal.h>

using namespace StCore;
using namespace StData;


int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.prepend(".");
    paths.prepend("./plugins");
    paths.prepend("./plugins/imageformats");
    paths.prepend("./plugins/platforms");
    QCoreApplication::setLibraryPaths(paths);

    initTextCodec();

    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    try
    {
        Core& core = Core::instance();
        core.initialize();
    }
    catch(std::exception& ex)
    {
        qCritical() << "Critical error: " << ex.what();
        QMessageBox::critical(nullptr, QObject::tr("Критическая ошибка"), ex.what());
        return 0;
    }

    MainWindow w;
    w.showMaximized();

    return a.exec();
}

