#include "MacroCollection.h"
#include "Macro.h"
#include "TplCursor.h"

#include <PathManager.h>
#include <QTextStream>
#include <QDebug>

namespace StCompiler {

MacroCollection::MacroCollection()
{}

MacroCollection::~MacroCollection()
{
    qDeleteAll(m_items);
    m_items.clear();
}

bool MacroCollection::load()
{
    QString constTplFileName = "constant.tpl";
    QString macroTplPath = PathManager::instance().templateDir().absoluteFilePath(constTplFileName.toUpper());
    QFile macroTplFile(macroTplPath);

    if(macroTplFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&macroTplFile);
        TplCursor cursor(stream);

        while(!cursor.isEof())
        {
            cursor.read();
            if(cursor.isEof())
                return true;

            Macro* macro = new Macro();
            macro->load(cursor);

            if(insert(macro) < 0)
            {
                qCritical() << QString("Duplicated constant (%1) in constant.tpl!").arg(macro->name());
                delete macro;
                return false;
            }
        }
    }
    else
    {
        qCritical() << "Can't open constant template file: " << macroTplPath;
        return false;
    }
    return true;
}

bool MacroCollection::search(const QString& name, qint16& index)
{
    qint8 c;
    for(int i = 0; i < m_items.count(); i++)
    {
        c = QString::compare(name, m_items.at(i)->name());
        if (c <= 0)
        {
            index = i;
            return c == 0;
        }
    }
    index = m_items.count();
    return 0;
}

int MacroComparer::compare(Macro* macro1, Macro* macro2)
{
    return QString::compare(macro1->name(), macro2->name());
}

}
