#include "Builder.h"
#include "TplCursor.h"
#include "Command.h"
#include "CommandCollection.h"
#include "CompilerVarCollection.h"
#include "Compiler.h"

#include <PathManager.h>

#include <Class.h>
#include <QFile>
#include <QTextStream>

#include <QDebug>

using namespace StCore;

namespace StCompiler {

Builder::Builder(QObject* parent) :
    QObject(parent)
{
    m_compilerTplPath = PathManager::instance().templateDir().absoluteFilePath("compiler.tpl");

    m_compiler = std::make_shared<Compiler>(m_operators, m_functions, m_constants);
}

Builder::~Builder()
{}

bool Builder::initialize()
{
    QFile tplFile(m_compilerTplPath);

    if(tplFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&tplFile);
        TplCursor cursor(stream);

        cursor.read();
        if(cursor.string() == "EPILOG")
        {
            cursor.read();
            if(!cursor.isEof())
            {
                m_epilog = std::make_shared<Command>();
                m_epilog->initialize(cursor);
            }
        }
        else
            return errorResult(tr("Файл компилятора сompiler.tpl некорректен"));

        if (cursor.isEof())
            return false;

        //Формируем список операторов
        if (cursor.string() == "OPERATORS")
            m_operators->load(cursor);
        else
            return errorResult(tr("Файл компилятора сompiler.tpl некорректен"));

        if (cursor.isEof())
            return errorResult(tr("Файл компилятора сompiler.tpl некорректен"));

        //формируем список функций
        if (cursor.string() == "FUNCTIONS")
            m_functions->load(cursor);
        else
            return errorResult(tr("Файл компилятора сompiler.tpl некорректен"));

        if(!m_operators->searchByName("#", m_functionIndex))
            return errorResult(tr("Не найден оператор #"));

        if(!m_operators->searchByName("un_minus", m_unaryIndex))
            return errorResult(tr("Не найден оператор un_minus"));

        if(!m_functions->searchByName("VFunction", m_virtualFunctionIndex))
            return errorResult(tr("Не найдена функция VFunction"));

        if(!m_functions->searchByName("DLLFunction", m_dllFunctionIndex))
            return errorResult(tr("Не найдена функция DllFunction"));

        if(!m_functions->searchByName("GetElement", m_getElementIndex))
            return errorResult(tr("Не найдена функция GetElement"));

        if(!m_functions->searchByName("SetElement", m_setElementIndex))
            return errorResult(tr("Не найдена функция SetElement"));

        m_constants->load();
    }
    else
        return errorResult(tr("Ошибка файла компилятора compiler.tpl. Не удалось открыть файл"));

    return true;
}

bool Builder::build(Class* cls, const QString& newSourceCode)
{
    qDebug() << "build()";

    m_byteCodeBuffer.clear();
    Sc2000DataStream byteCodeStream(m_byteCodeBuffer, QIODevice::ReadWrite);

    CompileVarCollection newVars(cls);
    CompileVarCollection oldVars(newVars);
    newVars.deleteCompilerVars();

    if(m_compiler->compile(cls, &newVars, newSourceCode, byteCodeStream))
    {
        cls->setText(newSourceCode);
        cls->setModify();

        for(ClassVarInfo* newVar : newVars.variables())
            for(ClassVarInfo* oldVar : oldVars.variables())
                if(newVar->name() == oldVar->name() && newVar->varType() == oldVar->varType())
                {
                    if(newVar->defValue().isEmpty() && !oldVar->defValue().isEmpty())
                        newVar->setDefValue(oldVar->defValue());

                    if(newVar->info().isEmpty() && !oldVar->info().isEmpty())
                        newVar->setInfo(oldVar->info());
                }

        if(cls->isProcedure() || cls->isStructure())
        {
            //TODO пересчет переменных класса : сохранение переменных в классе, если было изменение
        }
        //TODO IfNeedModifyVar(_class, vardata, &old);
        //TODO сохранить класс в файл
        return true;
    }
    return false;
}

bool Builder::errorResult(const QString& errorMessage)
{
    m_lastErrorText = errorMessage;
    return false;
}

}
