#include "MainWindow.h"
#include <QApplication>

#include <QMessageBox>
#include <QFontDatabase>
#include <QDebug>
#include <QDir>
#include <Log.h>
#include <Settings.h>
#include <StDataGlobal.h>

#include "Core.h"
#include "Log.h"
#include "Error.h"

using namespace StCore;
using namespace StData;

void ImportFontsFromAssets();

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

    try
    {
        ImportFontsFromAssets();

        Core::instance().initialize();
    }
    catch(std::exception& ex)
    {
        QMessageBox::critical(nullptr, QObject::tr("Критическая ошибка"), ex.what());
        return 0;
    }

    MainWindow mainWindow;
    mainWindow.show();

    mainWindow.initialize();

    return a.exec();
}

void ImportFontsFromAssets()
{
    QDir fontDir("assets:/fonts");

    QStringList nameFilters;
    nameFilters << "*.ttf";
    QStringList fontFiles = fontDir.entryList(nameFilters , QDir::Files);

    for(QString fontFile : fontFiles)
        QFontDatabase::addApplicationFont("assets:/fonts/" + fontFile);
}
