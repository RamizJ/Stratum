#include "Compiler.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <Sc2000DataStream.h>
#include <Class.h>
#include <Log.h>
#include <LibraryCollection.h>
#include <StandardTypes.h>

#include "Command.h"
#include "TplCursor.h"
#include "CommandCollection.h"
#include "CompilerVarCollection.h"
#include "MacroCollection.h"
#include "Macro.h"
#include "CompilerFunctions.h"
#include "CompileError.h"

#include <ClassVarInfo.h>
#include <Equation.h>
#include <PathManager.h>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

#ifdef QT_DEBUG
#include <DebugFunctions.h>
#endif

using namespace StData;

namespace StCompiler {

Compiler::Compiler(QObject* parent) :
    QObject(parent),
    m_class(nullptr),
    m_errX1(0),
    m_errX2(0),
    m_rootEquation(nullptr),
    m_lastError(nullptr),
    m_cursor(nullptr)
{
    m_compilerTplPath = PathManager::instance().templateDir().absoluteFilePath("COMPILER.TPL");

    m_operators = std::make_shared<CommandCollection>();
    m_functions = std::make_shared<CommandCollection>();
    m_constants = std::make_shared<MacroCollection>();
}

Compiler::~Compiler()
{}

//TODO коды ошибок
bool Compiler::initialize()
{
//#ifdef Q_OS_ANDROID
//    m_compilerTplPath = QString("assets:/template/COMPILER.TPL");
//#endif

    QFile tplFile(m_compilerTplPath);
    if(tplFile.open(QFile::ReadOnly))
    {
//        QMessageBox::information(nullptr, "Compiler", "tplFile opened");
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
            return errorResult(CompileError::Internal,
                               tr("Файл компилятора сompiler.tpl некорректен"));

        if (cursor.isEof())
            return false;

        if (cursor.string() == "OPERATORS")
            m_operators->load(cursor);
        else
            return errorResult(CompileError::Internal,
                               tr("Файл компилятора сompiler.tpl некорректен"));

        if (cursor.isEof())
            return errorResult(CompileError::Internal, tr("Файл компилятора сompiler.tpl некорректен"));

        if (cursor.string() == "FUNCTIONS")
            m_functions->load(cursor);
        else
            return errorResult(CompileError::Internal, tr("Файл компилятора сompiler.tpl некорректен"));

        if(!m_operators->searchByName("#", m_functionIndex))
            return errorResult(CompileError::Internal, tr("Не найден оператор #"));

        if(!m_operators->searchByName("un_minus", m_unaryIndex))
            return errorResult(CompileError::Internal, tr("Не найден оператор un_minus"));

        if(!m_functions->searchByName("VFunction", m_virtualFunctionIndex))
            return errorResult(CompileError::Internal, tr("Не найдена функция VFunction"));

        if(!m_functions->searchByName("DLLFunction", m_dllFunctionIndex))
            return errorResult(CompileError::Internal, tr("Не найдена функция DllFunction"));

        if(!m_functions->searchByName("GetElement", m_getElementIndex))
            return errorResult(CompileError::Internal, tr("Не найдена функция GetElement"));

        if(!m_functions->searchByName("SetElement", m_setElementIndex))
            return errorResult(CompileError::Internal, tr("Не найдена функция SetElement"));
        m_constants->load();
        //        TplLoader::loadConstants(m_constantsDictionary);
    }
    else
        return errorResult(CompileError::Internal, tr("Ошибка файла компилятора compiler.tpl. Не удалось открыть файл"));

    m_cursor = nullptr;
    return true;
}

bool Compiler::compile(Class* cls, const QString& newSourceCodeText, bool& isNeedRelinkVars)
{
    m_byteCodeBuffer.clear();
    bool result = false;
    CompileVarCollection newVars(cls);
    CompileVarCollection oldVars(newVars);

    if(newSourceCodeText.isEmpty())
    {
        result = true;
        cls->setText(QString());
        cls->clearCodeBytes();
        cls->deleteAllVariables();
        isNeedRelinkVars = newVars.isNeedModifyVars(cls, &oldVars);
    }
    else
    {
        newVars.deleteCompilerVars();
        St2000DataStream byteCodeStream(&m_byteCodeBuffer, QIODevice::ReadWrite);
        if(compile(cls, &newVars, newSourceCodeText, byteCodeStream))
        {
            cls->setText(newSourceCodeText);
            cls->setModify();
            cls->setCodeBytes(m_byteCodeBuffer);

#ifdef QT_DEBUG
            DebugFunctions::SaveDataToFile("d:\\newCode.txt", cls->vmCode());
#endif

            for(ClassVarInfo* newVar : newVars.variables())
            {
                for(ClassVarInfo* oldVar : oldVars.variables())
                {
                    if(newVar->varName() == oldVar->varName() && newVar->varType() == oldVar->varType())
                    {
                        if(newVar->defValue().isEmpty() && !oldVar->defValue().isEmpty())
                            newVar->setDefValue(oldVar->defValue());

                        if(newVar->info().isEmpty() && !oldVar->info().isEmpty())
                            newVar->setInfo(oldVar->info());
                    }
                }
                cls->addVarInfo(newVar);
            }

            if(cls->isProcedure() || cls->isStructure())
            {
                //TODO пересчет переменных класса : сохранение переменных в классе, если было изменение
            }
            isNeedRelinkVars = newVars.isNeedModifyVars(cls, &oldVars);
            if(isNeedRelinkVars)
            {
                cls->recreateObjectVars();
            }

            result = true;
        }
    }

    return result;
}

bool Compiler::compile(StData::Class* cls, CompileVarCollection* varData,
                       const QString newSourceCode, St2000DataStream& byteCodeStream)
{
    m_onePhaseMode = 0;
    m_eqMode = 0;
    m_fnMode = 0;
    m_class = cls;
    m_varData = varData;
    m_rootEquation = nullptr;
    m_lastError = nullptr;

    QTextStream stream(newSourceCode.toLatin1());
    TplCursor cursor(stream);
    m_cursor = &cursor;
    m_byteCodeStream = &byteCodeStream;

    if (m_cursor->read() == OperatorType::eof)
        goto end;

    //начинаем компиляцию
    do
    {
        if (!readString(State::Normal))
            goto err_end;
    }
    while (!m_cursor->isEof());


end:
    m_varData->fix();
    m_class->setEquation(m_rootEquation);
    m_cursor = nullptr;
    m_byteCodeStream = nullptr;
    return true;

err_end:
    m_varData->undo();
    m_indexStack.clear();
    m_varStack.clear();
    m_funcIndexStack.clear();
    m_equationStack.clear();
    m_funcNameStack.clear();
    delete m_rootEquation;
    m_cursor = nullptr;
    m_byteCodeStream = nullptr;
    return false;
}

bool Compiler::readString(Compiler::State state, qint16 label0, QStack<qint16>* plabelB)
{
    qDebug() << "readString()";

    m_newModificator = 0;
    qint16 label1, label2, label3;
    qint16 index;
    qint16 labelS = 0;
    //    qint16 sVarState = 0;

    QStack<qint16> labelB;

    if(state == State::If || state == State::While)
        label1 = setLabel();

    //выполняем выделение терминальных символов
    while (!m_cursor->isEof())
    {
//        QString str = m_cursor->string();

        //если в начале строки не стоят знак ( или ~, или нет операнда, или это оператор, то ошибка
        if(m_cursor->charAt(0) != '(' && m_cursor->charAt(0) != '~' &&
           m_cursor->type() != OperatorType::opnd &&
           m_operators->searchByName(m_cursor->string(), index))
        {
            return errorResult(CompileError::FirstVar, tr("Ожидался операнд"));
        }

        if (QString::compare(m_cursor->string(), "function", Qt::CaseInsensitive) == 0)
        {
            qDebug() << "parse " << "function";
            m_onePhaseMode = 1;
            m_fnMode = 1;
            m_class->setFlags(m_class->flags() | qint32(Class::Flag::Procedure));

            m_cursor->read();
            continue;
        }

        if (QString::compare(m_cursor->string(), "return", Qt::CaseInsensitive) == 0)
        {
            qDebug() << "parse " << "return";
            if (m_fnMode++ != 1)
                return errorResult(CompileError::Misplaced, tr("return"));

            if (!m_onePhaseMode)
                return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));

            //если после слова return нет операнда, то возвращаем ошибку
            if (m_cursor->read() != OperatorType::opnd)
                return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));

            //проверяем переменную после слова return
            qint16 i;
            ClassVarInfo* varInfo = m_varData->search(m_cursor->string(), i);
            if(!varInfo)
                return errorResult(CompileError::VarNotFound,
                                   tr("не найдена переменная: ") + m_cursor->string());

            varInfo->addFlags(ClassVarInfo::Return);

            m_cursor->read();
            continue;
        }

        if (QString::compare(m_cursor->string(), "switch", Qt::CaseInsensitive) == 0)
        {
            if(m_cursor->string()[0] == ';')
                m_cursor->read();

            m_cursor->read();
            if (m_cursor->string() != "case")
                return errorResult(CompileError::Wait, tr("После switch должен быть case"));

            //выполняем рекурсивный вызов функции readstring для лексического анализа
            //текста, стоящего после слова case.
            if (!readString(State::Switch))
                return false;

            m_cursor->read();
            continue;
        }

        if (QString::compare(m_cursor->string(), "case", Qt::CaseInsensitive) == 0)
        {
            if (state != State::Switch)
                return errorResult(CompileError::Misplaced, tr("Для case не найден switch"));

            if (labelS) // если есть предыдущий CASE
            {
                if (!analizeOperator(m_functionIndex, "JMP"))
                    return false;

                if (!checkStack(true))
                    return errorResult(CompileError::Internal, tr("break"));

                labelB.push(setLabel());
                linkLabel(labelS);
            }
            // переход
            if (!analizeOperator(m_functionIndex, "JZ"))
                return false;

            m_cursor->read();
            if (!readMString())
                return false;

            if (!checkStack(true))
                return errorResult(CompileError::Internal, tr("break"));

            labelS = setLabel();
            continue;
        }

        if(QString::compare(m_cursor->string(), "default", Qt::CaseInsensitive) == 0)
        {
            qDebug() << "parse " << "default";
            if (state != State::Switch)
                return errorResult(CompileError::Misplaced, tr("switch"));

            if (labelS) // если есть предыдущий CASE
            {
                if (!analizeOperator(m_functionIndex, "JMP"))
                    return false;

                if (!checkStack(true))
                    return errorResult(CompileError::Internal, tr("break"));

                labelB.push(setLabel());
                linkLabel(labelS);
            }
            labelS = 0;
            m_cursor->read();
            continue;
        }

        if(QString::compare(m_cursor->string(), "break", Qt::CaseInsensitive) == 0)
        {
            qDebug() << "parse " << "break";
            if (!analizeOperator(m_functionIndex, "JMP"))
                return false;

            if (!checkStack(true))
                return errorResult(CompileError::Internal, tr("break"));

            if(plabelB)
                plabelB->push(setLabel());
            else
                labelB.push(setLabel());

            m_cursor->read();
            while(m_cursor->string()[0] == ';')
                m_cursor->read();

            continue;
        }
        if (QString::compare(m_cursor->string(), "do", Qt::CaseInsensitive) == 0)
        {
            label3 = setLabel();
            m_cursor->read();
            if (!readString(State::Until, label3 + 2))
                return false;
            continue;
        }
        if (QString::compare(m_cursor->string(), "until", Qt::CaseInsensitive) == 0)
        {
            if (state != State::Until)
                return errorResult(CompileError::Misplaced, tr("until"));

            if (!analizeOperator(m_functionIndex, "JNZ"))
                return false;

            m_cursor->read();
            if (!readMString())
                return false;

            if (!checkStack(true))
                return errorResult(CompileError::Misplaced, tr("until"));

            setLabel(label0); // переход на DO
            while (labelB.size())
                linkLabel(labelB.pop()); // если есть break
            return true;
        }
        if (QString::compare(m_cursor->string(), "if", Qt::CaseInsensitive) == 0)
        {
            qDebug() << "parse " << "if";
            if (!analizeOperator(m_functionIndex, "JZ"))
                return false;

            m_cursor->read();
            if (!readMString())
                return false;

            if (!readString(State::If, 0, &labelB))
                return false;

            m_cursor->read();
            continue;
        }
        if (QString::compare(m_cursor->string(), "while", Qt::CaseInsensitive) == 0)
        {
            qDebug() << "parse " << "while";
            label3 = setLabel();
            if (!analizeOperator(m_functionIndex, "JZ"))
                return false;

            m_cursor->read();
            if (!readMString())
                return false;

            if (!readString(State::While, label3 + 2))
                return false;

            m_cursor->read();
            continue;
        }
        if (m_cursor->string()[0] == 'e' || m_cursor->string()[0] == 'E')
        {
            if (QString::compare(m_cursor->string(), "endswitch", Qt::CaseInsensitive) == 0)
            {
                if (state != State::Switch)
                    return errorResult(CompileError::Misplaced, tr("endswitch"));

                while (labelB.size())
                    linkLabel(labelB.pop()); // если есть break

                if (labelS)
                    linkLabel(labelS); // связываем предыдущий переход
                return true;
            }
            if (QString::compare(m_cursor->string(), "endwhile", Qt::CaseInsensitive) == 0)
            {
                qDebug() << "parse " << "endwhile";

                if (state != State::While)
                    return errorResult(CompileError::Misplaced, tr("endwhile"));

                if (!analizeOperator(m_functionIndex, "JMP"))
                    return false;

                if (!checkStack(true))
                    return errorResult(CompileError::Internal, tr("while"));

                setLabel(label0);
                while (labelB.size())
                    linkLabel(labelB.pop()); // если есть break

                if (!linkLabel(label1))
                    return errorResult(CompileError::Internal, tr("link"));

                return true;
            }
            if (QString::compare(m_cursor->string(), "else", Qt::CaseInsensitive) == 0)
            {
                if (state != State::If)
                    return errorResult(CompileError::Misplaced, tr("else"));

                if (!analizeOperator(m_functionIndex, "JMP"))
                    return false;

                if (!checkStack(true))
                    return errorResult(CompileError::Internal, tr("else"));

                label2 = setLabel();
                if (!linkLabel(label1))
                    return errorResult(CompileError::Internal, tr("link"));

                state = State::Else;
                m_cursor->read();
                continue;
            }
            if (QString::compare(m_cursor->string(), "endif", Qt::CaseInsensitive) == 0)
            {
                switch (state)
                {
                    case State::Else :
                        if (!linkLabel(label2))
                            return errorResult(CompileError::Internal, tr("link"));
                        break;

                    case State::If:
                        if (!linkLabel(label1))
                            return errorResult(CompileError::Internal, tr("link"));
                        break;

                    default :
                        return errorResult(CompileError::Misplaced, tr("endif"));
                }
                return true;
            }
        }// end...
        if (m_cursor->string()[0] == '?' && m_cursor->charAt(1) == 0)   // It's EQUATION string !
        {
            qDebug() << "parse " << "equ ?";
            if (!readVars(VF_EQVAR))
                return false;
            continue;
        }
        if (m_cursor->isEquation())
        {
            if (!analizeEquation())
                return false;
            continue;
        }

        //если после текущей лексемы стоит (, то значит это функция
        switch (m_cursor->getNextChar())
        {
            case '(':
            {
                qDebug() << "parse " << "(";
                if (!analizeOperator(m_functionIndex,m_cursor->string()))
                    return false;

                m_cursor->read();
                if (!readMString())
                    return false;
                else
                    continue;
            }
            case '[':                // It's array !
            {}
        }

        // Этап 2 - если следующий оператор ::, := или ++, обрабатываем их,
        // иначе считаем, что это строка определения переменных <тип> <модификатор> <имя1> <имя2>
        m_firstString =m_cursor->string();
        m_cursor->read();
        if (m_cursor->string()[0] == ':' &&m_cursor->getNextChar() == ':') // Спец строка - уравнение
        {
            qDebug() << "parse " << "equ :";
            m_eqMode = 1;   // first var
            insertVar(index, m_firstString, "FLOAT");

            Equation* eq = new Equation();
            eq->setCode(EC_VAR);
            eq->setIndex(index);
            eq->setFlag(1);
            m_equationStack.push(eq);

            m_cursor->read();

            if (!m_operators->searchByName("=", index))
                return errorResult(CompileError::Internal, tr("="));

            if (!analizeOperator(index))
                return false;

            if (!readMString(1))
                return false;

            m_eqMode = 0;
            m_isDiffer = 2;
            if (!checkEquationStack())
                return false;

            m_isDiffer = 0;
            continue;
        }

        if (m_cursor->string() == ":=")
        {
            qDebug() << "parse " << ":=";
            if (!m_operators->searchByName(m_cursor->string(), index))
                return errorResult(CompileError::Internal, tr("Не найден оператор ':='"));

            if (!analizeOperator(index))
                return false;

            if (!readMString())
                return false; //компилируем выражение, стоящиее после :=
            else
                continue;
        }
        if (m_cursor->string() == "++")
        {
            qDebug() << "parse " << "++";
            if (!m_operators->searchByName(m_cursor->string(), index))
                return errorResult(CompileError::Internal, tr("Не найден оператор '++'"));

            if (!analizeOperator(index))
                return false;

            m_cursor->read();
            while(m_cursor->string()[0] == ';')
                m_cursor->read();
            continue;
        }
        if (!readDefVars(m_firstString))
            return false;
        else
            continue;
    }
    while (labelB.size())
        linkLabel(labelB.pop()); // если есть break

    if (state != State::Normal)
        return errorResult(CompileError::Syntax, tr("Ошибка синтаксиса"));

    return true;
}

//выполняет чтение переменных, перечисленных через запятую
bool Compiler::readVars(qint16 flag)   // <- ReadEQ Vars
{
    qDebug() << "readVars()";
    qint16 ix;
    ClassVarInfo* varInfo = NULL;
    do
    {
        //если последняя лексема не операнд, то ошибка
        if (m_cursor->read() != OperatorType::opnd)
            return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));

        //ищем индекс и описание операнда по его имени в списке переменных
        varInfo = m_varData->search(m_cursor->string(), ix);

        //если переменной с этим именем нет, то ошибка
        if(!varInfo)
            return errorResult(CompileError::VarNotFound,
                               tr("Не найдена переменная: ") +m_cursor->string());

        //устанавливаем флаг переменной
        varInfo->setFlags(flag);
        //читаем следующую лексему
        m_cursor->read();
    } while (m_cursor->string()[0] == ',');

    return true;
}

bool Compiler::analizeEquation()
{
    qDebug() << "analizeEquation()";
    QString buf;
    ClassVarInfo* varInfo = nullptr;
    qint16 functionIndex, index;

    if(m_cursor->scanChar('?')) // Переменные + Функция решить уравнение !
    {
        // ищем спец. функцию
        //        Command* equationCommand = m_functions->getCommand("equation");
        //        if(!equationCommand)
        //            return errorResult("ERR_EQUATION");
        if(!m_functions->searchByName("equation", functionIndex))
            return errorResult(CompileError::Internal, tr("Уравнения не поддерживаются"));

        if (m_cursor->type() != OperatorType::opnd)
            return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));

        goto mm1;

        do
        {
            if (m_cursor->read() != OperatorType::opnd)
                return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));
mm1:
            varInfo = m_varData->search(m_cursor->string(), index);

            if (!varInfo)
                return errorResult(CompileError::VarNotFound,
                                   tr("Не найдена переменная: ") +m_cursor->string());

            // помещаем в код функцию и индекс переменной
            m_functions->at(functionIndex)->out(*m_byteCodeStream, index);
            varInfo->setFlags(ClassVarInfo::EqVar);
            m_cursor->read();
        }
        while (m_cursor->string()[0] == ',');

        m_cursor->read();
        if (m_cursor->string()[0] != '?')
            return errorResult(CompileError::Syntax, tr("Синтаксическая ошибка"));

        m_cursor->read();
        return 1;
    }

    m_eqMode = 1;
    m_isDiffer = 0;

    if (m_cursor->string()[0] == 'd' &&m_cursor->charAt(1) != 0)
    {
        // first var
        buf +=m_cursor->string()[1];
        if(!m_cursor->isNextOperator('/'))
            goto mm2;

        m_cursor->read();
        varInfo = insertVar(index, buf, "FLOAT");

        if (!varInfo)
            return false;

        Equation* equation = new Equation();
        equation->setCode(EC_VAR);
        equation->setIndex(index);
        equation->setFlag(1);

        m_equationStack.push(equation);

        // delimiter
        if(!m_operators->searchByName("/", index))
            return errorResult(CompileError::Internal, tr("Не найден оператор '/'"));

        if(!analizeOperator(index))
            return false;

        // twelb var
        m_cursor->read();
        if(m_cursor->charAt(0) != 'd' ||m_cursor->charAt(1) == 0)
            return errorResult(CompileError::Wait, tr("dt"));

        varInfo = insertVar(index, m_cursor->string().mid(1), "FLOAT");
        if(!varInfo)
        {
            delete equation;
            return false;
        }
        varInfo->setDefValue("0.01");

        equation = new Equation();
        equation->setCode(EC_VAR);
        equation->setIndex(index);
        equation->setFlag(1);

        m_equationStack.push(equation);

        // спец +
        if(!m_operators->searchByName("+", index))
            return errorResult(CompileError::Internal, tr("Не найден оператор '+'"));

        if(!analizeOperator(index))
            return false;

        // + Время
        varInfo = insertVar(index,m_cursor->string().mid(1), "FLOAT");

        if (!varInfo)
        {
            delete equation;
            return false;
        }

        equation = new Equation();
        equation->setCode(EC_VAR);
        equation->setIndex(index);
        equation->setFlag(1);
        m_equationStack.push(equation);

        m_cursor->read();
        if (m_cursor->string()[0] != '=')
            return errorResult(CompileError::Wait, tr("Ожидалось '='"));

        if (!m_operators->searchByName(m_cursor->string(), index))
            return errorResult(CompileError::Internal, tr("'='"));

        if (!analizeOperator(index))
            return false;

        if (!readMString(1))
            return false;

        m_eqMode = 0;
        m_isDiffer = 1;
        if (checkEquationStack())
            return false;
        m_isDiffer = 0;
        return true;
    }
mm2:
    // this normal equations
    m_cursor->back();
    if (!readMString())
        return false;

    m_eqMode = 0;
    if (!checkEquationStack())
        return false;
    return true;
}

bool Compiler::checkEquationStack()
{
    qDebug() << "checkEquationStack()";

    if (m_equationStack.size() != 1)
        return errorResult(CompileError::Syntax, tr("Синтаксическая ошибка"));

    Equation* tmp = m_equationStack.pop();

    if (m_isDiffer)
        tmp->setCode(EC_EQDIF);

    if (m_isDiffer == 2)
        tmp->setCode(EC_EQSTRING);


    if (!m_rootEquation)
        m_rootEquation = tmp;
    else
        m_rootEquation->addChildEquation(tmp);
    return true;
}

qint16 Compiler::readDefVars(const QString& typeName)
{
    qDebug() << "readDefVars()";
    qint16 ix;
    qint16 count, flags = 0;
    QString str2;
    QString errstr;

    //получаем информацию о типе по его названию
    Class* cl = LibraryCollection::getClassByName(typeName);
    if (!cl)
        return errorResult(CompileError::UnknownType, tr("Неизвестный тип ") + typeName);

    ClassVarInfo* var;
    //определяем область видимости переменной
    if(QString::compare(m_cursor->string(), "local", Qt::CaseInsensitive) ==0)
    {
        flags += VF_LOCAL;
        m_cursor->read();
    }
    if(QString::compare(m_cursor->string(), "global", Qt::CaseInsensitive) == 0)
    {
        flags += VF_GLOBAL;
        m_cursor->read();
    }
    if(QString::compare(m_cursor->string(), "classglobal", Qt::CaseInsensitive) == 0)
    {
        flags += VF_CLASSGLOBAL;
        m_cursor->string();
    }
    if(QString::compare(m_cursor->string(), "nosave", Qt::CaseInsensitive) == 0)
    {
        flags += VF_NOSAVE;
        m_cursor->read();
    }
    if(QString::compare(m_cursor->string(), "parameter", Qt::CaseInsensitive) == 0)
    {
        //если ранее не было найдено слово function, то ошибка
        if (!m_fnMode)
            return errorResult(CompileError::Misplaced, tr("parameter"));

        flags += VF_ARGUMENT;
        m_cursor->read();
    }
    //if (!lstrcmpi(id->str,"local")) { flags += VF_LOCAL; id->Read(); }
    //if (!lstrcmpi(id->str,"global")) { flags += VF_GLOBAL; id->Read(); }
    //if (!lstrcmpi(id->str,"classglobal")) { flags += VF_CLASSGLOBAL; id->Read(); }
    //if (!lstrcmpi(id->str,"nosave")) { flags += VF_NOSAVE; id->Read(); }
    goto l1;
    do
    {
        //если после типа или модификатора стоит не операнд, то ошибка
        if (m_cursor->read() != OperatorType::opnd)
            return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));
l1:
        str2 =m_cursor->string();
        m_cursor->read();
        count = 0;
        //если переменная является массивом
        if (m_cursor->string()[0] == '[')
        {
            count = 1;
            goto l2;
            do
            {
                m_cursor->read();
                if (m_cursor->string()[0] != '[')
                    return errorResult(CompileError::Misplaced, tr("["));
l2:
                //если после [ не стоит число, то ошибка
                if (m_cursor->read() != OperatorType::dconst)
                    return errorResult(CompileError::MbNum, tr("Ожидалось число"));

                count *= (qint16)m_cursor->string().toInt();
                m_cursor->string();
                if (m_cursor->string()[0] != ']')
                    return errorResult(CompileError::Misplaced, tr("]"));

                m_cursor->read();
            }
            while (m_cursor->string()[0] == ',');
        }
        //вставляем переменную в список переменных
        ix = m_varData->insert(str2, typeName, count);

        if (ix == -1)
            return errorResult(CompileError::UnknownType, tr("Неизвестный тип: ") + typeName);

        //если переменная уже существует, то ошибка
        if(ix == -2)
        {
            var = m_varData->search(str2, ix);
            if(var == nullptr)
                return errorResult(CompileError::VarExist,
                                   tr("Переменная уже существует под другим типом"));

            m_firstString = str2;
            errstr = str2 + " : " + var->varType()->originalName();
            return errorResult(CompileError::VarExist,
                               tr("Переменная уже существует под другим типом"));
        }
        //если был установлен модификатор переменной, то сохраняем его
        if (flags)
            m_varData->at(ix)->setFlags(flags);
        if (m_cursor->type() == OperatorType::eof)
            return true;
    }
    while (m_cursor->string()[0] == ',');
    return true;
}

ClassVarInfo* Compiler::insertVar(qint16& ix, QString varName, QString type)
{
    qDebug() << "insertVar(" << varName << type << ")";
    ix = m_varData->insert(varName, type);

    //если тип неизвестен, то ошибка
    if (ix == -1)
    {
        errorResult(CompileError::Internal, tr("Неизвестный тип: ") + type);
        return nullptr;
    }
    //если переменная с таким именем уже есть, но с другим типом, то ошибка
    if (ix == -2)
    {
        ClassVarInfo* var = m_varData->search(varName, ix);
        if (var == nullptr)
        {
            errorResult(CompileError::Internal, tr("Переменная не найдена: ") + varName);
            return nullptr;
        }

        QString errstr = m_firstString + " : " + var->varType()->originalName();
        errorResult(CompileError::VarExist, "Переменная существует под другим типом " + errstr);
        return nullptr;
    }
    //ищем описание на эту переменную в списке переменных
    ClassVarInfo* var = m_varData->search(varName, ix);
    //добавляем тип переменной и ее позицию в outcode в стек переменных
    m_varStack.push(var->varType(), (qint16)m_byteCodeStream->pos());
    return var;
}

//определяем операторы, функции и константы
bool Compiler::readMString(qint16 chStack)
{
    qDebug() << "readMString()";

    m_newModificator = 0;

    OperatorType lastIndex = OperatorType::optr;
    m_cursor->setNewLine(0);

    qint16 index;

cicle:
    m_cursor->read();
    if (m_cursor->newLine() &&m_cursor->type() != OperatorType::eof) // end of string !!
    {
        m_cursor->setNewLine(0);
        if (!checkStack(chStack))
            return false;

        lastIndex = OperatorType::optr;
        return true;
    }
    switch (m_cursor->type())
    {
        case OperatorType::optr:   //нашли оператор
        {
            //если оператор есть в списке
            if (m_operators->searchByName(m_cursor->string(), index))
            {
                switch (m_cursor->charAt(0))
                {
                    case ';' : // end of mstring
                    {
                        m_cursor->read();
                        goto end;
                    }
                    case '~' : // tilda !
                    {
                        if (m_newModificator)
                            return errorResult(CompileError::Misplaced, tr("~"));

                        m_newModificator = 1;
                        goto cicle;
                    }
                    case '-' :// unary minus ?
                    {
                        if (lastIndex == OperatorType::optr)
                        {
                            if (analizeOperator(m_unaryIndex))
                                goto cicle;
                            else
                                return false;
                        }
                    }
                    case '!' :// unary ! ?
                    {
                        if (lastIndex == OperatorType::optr &&m_cursor->string()[1] != '=')
                        {
                            if (analizeOperator(index))
                                goto cicle;
                            else
                                return false;
                        }
                        else
                            break;
                    }
                    case '+' ://
                    {
                        if (lastIndex == OperatorType::optr &&m_cursor->string()[1] != '+')
                        {
                            if (analizeOperator(index))
                                goto cicle;
                            else
                                return false;
                        }
                        else break;
                    }
                    case ':' : // недолжно быть
                    {
                        if (m_cursor->string()[1] == '=')
                            return errorResult(CompileError::Syntax, tr("':'"));
                    }
                }
                //если идут подряд 2 оператора, то ошибка
                if (m_cursor->string()[0] != '(' &&m_cursor->string()[0] != ')') // syntax check !
                {
                    if(lastIndex == OperatorType::optr)
                        return errorResult(CompileError::Misplaced, m_cursor->string());
                    else
                        lastIndex = OperatorType::optr;
                }
                if (analizeOperator(index))
                    goto cicle;
                else
                    return false;
            }
            else
                return errorResult(CompileError::NfOptr, tr("Не найден оператор: ") +m_cursor->string());
        }
        case OperatorType::opnd: //нашли операнд
        {
            //если следующая лексема - оператор (, то этот операнд должен быть именем функции
            if(m_cursor->isNextOperator('('))
            {
                //если этот операнд - имя функции
                //и у него нет возвращаемого значения, то ошибка
                if(m_functions->searchByName(m_cursor->string(), index))
                {
                    //if (((TOperator*)Functions->At(ix))->ret == 0)
                    //{ SetError(ERR_MISPLACED,((TOperator*)Functions->At(ix))->Name); return 0; };
                }
                if (analizeOperator(m_functionIndex,m_cursor->string()))
                {
                    m_cursor->read();
                    if(m_cursor->getNextChar() == ')')
                        lastIndex = OperatorType::opnd;
                    else
                        lastIndex = OperatorType::optr;
                    goto cicle;
                }
                else
                    return false;
            }
            //	case '[' :                // It's Array !
            //	{
            //	 if (AnalizeOPTR(FunFlag_ix,"GetElement")) { id->Read(); LastIndex = optr; goto cicle; }
            //	  else return 0;
            //	}
            //	case '.' :                // It's Structure !
            //	{
            //	 return 0;
            //	}
            //   }
            // если перед этим операндом стоял тоже операнд, то ошибка
            if(lastIndex == OperatorType::opnd)
                return errorResult(CompileError::MbOptr, tr("Ожидался оператор: ") +m_cursor->string());
            else
                lastIndex = OperatorType::opnd;

            // It's Macro ?
            if (m_constants->search(m_cursor->string(), index)) // It's Macro !!!
            {
                if (!constantOut("FLOAT"))
                    return false;
                //double d = (((TMacro*)Constants->At(ix))->Number);
                if (!m_eqMode)
                    CompilerFunctions::decConstantOut(*m_byteCodeStream, m_constants->at(index)->value());
                goto cicle;
            }
            // It's Operand !!!
            if (varOut(m_cursor->string()))
                goto cicle;

            else return 0;
        }
        case OperatorType::dconst : // нашли вещественную константу
        case OperatorType::fconst :
        {
            // если перед константой стоял операнд, то ошибка
            if (lastIndex == OperatorType::opnd)
                return errorResult(CompileError::MbOptr, tr("Ожидался оператор: ") + m_cursor->string());
            else
                lastIndex = OperatorType::opnd;

            if (!constantOut("FLOAT"))
                return false;

            if (!m_eqMode)
                CompilerFunctions::decConstantOut(*m_byteCodeStream, m_cursor->string());
            goto cicle;
        }
        case OperatorType::iconst : // integer constant :
        {
            // syntax check !
            if (lastIndex == OperatorType::opnd)
                return errorResult(CompileError::MbOptr, tr("Ожидался оператор: ") +m_cursor->string());

            else
                lastIndex = OperatorType::opnd;
            if (!constantOut("HANDLE"))
                return false;

            if (!m_eqMode)
                CompilerFunctions::iConstantOut(*m_byteCodeStream, m_cursor->string());
            goto cicle;
        }
        case OperatorType::sconst : // string constant :
        {
            if (m_eqMode)
                return errorResult(CompileError::Syntax, tr("Ошибка синтаксиса"));

            if (lastIndex == OperatorType::opnd)
                return errorResult(CompileError::MbOptr, tr("Ожидался оператор: ") +m_cursor->string());
            else
                lastIndex = OperatorType::opnd;
            if (!constantOut("STRING"))
                return false;

            CompilerFunctions::strConstantOut(*m_byteCodeStream, m_cursor->string(), qint16(m_cursor->string().length()));
            goto cicle;
        }
        case OperatorType::hconst : // hex constant :
        {
            if (lastIndex == OperatorType::opnd)
                return errorResult(CompileError::MbOptr, tr("Ожидася оператор: ") +m_cursor->string());
            else
                lastIndex = OperatorType::opnd;

            if (!constantOut("INTEGER"))
                return false;

            CompilerFunctions::hexConstantOut(*m_byteCodeStream, m_cursor->string());
            goto cicle;
        }
        case OperatorType::eof : // end of file
            break;
    }

end:
    if (!checkStack(chStack))
        return false;
    return true;
}

qint16 Compiler::setLabel(qint16 dest)
{
    qDebug() << "setLabel()";
    if (dest >= 0)
    {
        m_byteCodeStream->seek(m_byteCodeStream->device()->size() - 2);
        *m_byteCodeStream << qint16(dest / 2);
        return 0;
    }
    return qint16(m_byteCodeStream->pos() - 2);
}

bool Compiler::analizeOperator(qint16 index, const QString& funcName)
{
    qDebug() << "analizeOperator()" << funcName;
    //получаем описание оператора по его индексу в списке
    Command* command = m_operators->at(index);

    if(!m_indexStack.isEmpty())
    {
        while(command->endPriority() <= m_operators->at(m_indexStack.top())->startPriority())
        {
            if(!operatorOut(m_operators->at(m_indexStack.pop())))
                return false;

            if (m_indexStack.isEmpty())
                break;
        }
    }

    switch (command->name()[0].toLatin1())
    {
        case '#':
        {
            m_funcNameStack.push(funcName);
            m_indexStack.push(index);
            m_funcIndexStack.push(m_varStack.count());
            return true;
        }
        case ',':
        {
            if (m_funcIndexStack.isEmpty())
                return errorResult(CompileError::Misplaced, tr(","));

            return true;
        }
        case '(' :
        {
            m_indexStack.push(index);
            return true;
        }
        case ')':
        {
            //если перед ) не было ( и других операторов, то ошибка
            if (m_indexStack.isEmpty())
                return errorResult(CompileError::Misplaced, tr("Не хватает закрывающей скобки"));

            switch(m_operators->at(m_indexStack.pop())->name()[0].toLatin1())
            {
                case '(':
                    return true;

                case '#':   //если между ( и ) есть имя функции, то проверяем функцию
                    return functionOut();

                default:
                    return errorResult(CompileError::Misplaced, tr("Не хватает закрывающей скобки"));
            }
        }
        case '+':
        {
            if(m_cursor->string() == "++")
            {
                if (!operatorOut(m_operators->at(index)))
                    return false;
                return true;
            }
        }
    }
    m_indexStack.push(index);
    return true;
}

qint16 Compiler::checkStack(bool isErrorDisabled)
{
    qDebug() << "checkStack()";
    Command* op_st;
    while (!m_indexStack.isEmpty())
    {
        op_st = m_operators->at(m_indexStack.pop());
        if (op_st->name()[0] == '#')
        {
            if (isErrorDisabled)
            {
                functionOut();
                continue;
            }
            else
                return errorResult(CompileError::Misplaced, tr("Не хватает закрывающей скобки"));
        }

        if(op_st->name()[0] == '(')
            return errorResult(CompileError::Misplaced, tr("("));

        else if(!operatorOut(op_st))
            return false;
    }
    return checkVarStack();
}

qint16 Compiler::checkVarStack()
{
    if (m_varStack.isEmpty())
        return true;

    return errorResult(CompileError::Syntax, tr("Синтаксическая ошибка"));
}

bool Compiler::linkLabel(qint16 position)
{
    qint16 i = (qint16)(m_byteCodeStream->pos() / 2);
    m_byteCodeStream->seek(position);
    *m_byteCodeStream << i;
    m_byteCodeStream->seek(m_byteCodeStream->device()->size());
    return true;
}

//проверяет типы оператора,
//добавляет переменную перед оператором присваивания
//добавляет номер оператора в outcode
bool Compiler::operatorOut(Command* cmd, qint16 index)
{
    QList<Class*> buf;
    qint16 popVarIndex;

    Equation* pequ;
    //если это оператор присваивания, то добавляем переменную перед ним в список переменных
    //firststring - имя предыдущего операнда, если он не был декларирован,
    //то VarStack.GetTop()->GetClassName() вернет 0
    if (cmd->name()[0] == ':' && !m_eqMode)
    {
        if (m_varStack.isEmpty())
            return errorResult(CompileError::Syntax, tr("Синтаксическая ошибка"));

        ClassVarInfo* varInfo = insertVar(popVarIndex, m_firstString, m_varStack.top()->originalName());
        if (!varInfo)
            return false;

        varInfo->setFlags(VF_LEFT);
    }
    if (cmd->name() == "++")
    {
        //if (VarStack.Empty()) { SetError(ERR_SYNTAX,0,1);  return 0; }
        ClassVarInfo* varInfo = insertVar(popVarIndex, m_firstString, "FLOAT");
        if (!varInfo)
            return false;
        varInfo->setFlags(VF_LEFT);
    }
    //проверяем типы аргументов оператора
    if (cmd->argsCount())
    {
        //копируем аргументы в буфер
        for (qint16 i = 0; i < cmd->argsCount(); i++)
        {
            if(m_varStack.isEmpty())
                return errorResult(CompileError::Syntax, tr("Синтаксическая ошибка"));

            int pos;
            buf << m_varStack.pop(pos);
        }
        //если типы аргументов не совпадают, то ошибка
        if (!m_operators->searchBySpec(index, cmd->name(), buf))
            return errorResult(CompileError::NfOptr,
                               funcSpecToStr(cmd->name(), cmd->args(), false, true));
    }
    if (m_eqMode) // EQUATION !!!
    {
        pequ = new Equation();
        for (qint16 i = 0; i < cmd->argsCount(); i++)
        {
            if (m_equationStack.isEmpty())
                return errorResult(CompileError::EqInternal, tr(""));

            pequ->addChildEquation(m_equationStack.pop());
        }
        pequ->setCode(m_operators->at(index)->outNum());
        m_equationStack.push(pequ);

        if(cmd->returnType() != nullptr)
            m_varStack.push(m_operators->at(index)->returnType());

        return true;
    }

    //если это оператор присваивания, то добавляем индекс переменной перед ним в outcode
    if (cmd->name()[0] == ':' ||m_cursor->string() == "++")
    {
        m_operators->at(index)->out(*m_byteCodeStream, popVarIndex);
        return true;
    }
    else
        m_operators->at(index)->out(*m_byteCodeStream, index);

    if (cmd->returnType() != nullptr)
        m_varStack.push(m_operators->at(index)->returnType());

    return true;
}

//проверка синтаксиса функции
bool Compiler::functionOut()
{
    qint16 ix, index = 0;
    QList<Class*> args;
    QList<qint32> argsPos;

    Equation* equation;
    QString functionName;

    if (m_funcIndexStack.isEmpty())
        return errorResult(CompileError::Internal, tr(""));

    if (m_funcNameStack.isEmpty())
        return errorResult(CompileError::Internal, tr(""));

    functionName = m_funcNameStack.pop();

    //получаем количество аргументов в стеке
    qint16 argCount = m_varStack.size() - m_funcIndexStack.pop();

    //формируем список типов аргументов из стека в линейный массив Args
    //и сохраняем позицию этого аргумента в выходном коде в массив ArgsPos
    for(qint16 i = 0; i < argCount; i++)
    {
        qint32 pos;
        args << m_varStack.pop(pos);
        argsPos << pos;
        //char str[MAX_STRING];
        //wsprintf(str, "%s, %d, %s",fname,ArgsPos[i],Args[i]->GetClassName());
        //MessageBox(0,str,"",0);
    }

    //формируем строку с описанием текущей функции и ее аргументов - имя(аргумент1,...)
    QString fSpec = funcSpecToStr(functionName, args, false, true);

    //ищем оператор с именем fname и аргументами Args
    Command* op;
    while(m_functions->searchByName(functionName, ix, index))
    {
        index = ix + 1;
        op = m_functions->at(ix);

        if (!op->optionalArgs().isEmpty())  //это функция с переменным числом аргументов !
        {
            if(functionOut2(ix, args, argCount))
                return true;
            else
                continue;
        }

        //если типы аргументов не совпадают, то ошибка
        //иначе выходим из цикла
        if(m_functions->searchBySpec(ix, op->name(), args))
        {
            index = m_functions->count() + 1;
            op = m_functions->at(ix);
            break;
        }
        else
            return errorResult(CompileError::FuncNotFound, tr("Не найдена функция: ") + fSpec);
    }

    //если не нашли функцию fname
    if(index <= m_functions->count() && ix == m_functions->count())
        return functionOut3(functionName, args, argCount); // может это функция имидж или DLL ?

    //проверка на указательность (доделать сообщение об ошибке !)
    for(int i = argCount - 1; i >= 0 ; i--)
        m_varStack.push(args[i], argsPos[i]);

    for(int i = 0; i < op->argsCount(); i++)
    {
        if(op->argIsRef(op->argsCount() - i - 1))
            if(!m_varStack.setPtr(*m_byteCodeStream))
                return errorResult(CompileError::MbVar, tr("Ожидалась переменная"));

        args[i] = m_varStack.pop(argsPos[i]);
    }

    //если это уравнение
    if (m_eqMode) // EQUATION !!!
    {
        equation = new Equation;
        for (qint16 i = 0; i < op->argsCount(); i++)
        {
            if (m_equationStack.isEmpty())
                return errorResult(CompileError::Internal, tr("Ошибка в уравнении"));

            equation->addChildEquation(m_equationStack.pop());
        }
        equation->setCode(m_functions->at(ix)->outNum());
        m_equationStack.push(equation);

        if(op->returnType())
            m_varStack.push(op->returnType());

        return true;
    }

    m_functions->at(ix)->out(*m_byteCodeStream, ix);

    if (op->returnType())
        m_varStack.push(op->returnType());

    return true;
}

//обрабатка функций с переменным числом параметров
bool Compiler::functionOut2(qint16 ix, QList<Class*> Args, qint16 ArgCount)
{
    if(m_eqMode)
        return errorResult(CompileError::Internal, tr("Уравнения не поддерживаются"));

    Command* op = m_functions->at(ix);

    QList<Class*> MainArgs, DopArgs;
    qint16 _vars2;
    qint16 vars2 = _vars2 = ArgCount - op->argsCount(); // дополнительных

    // проверки
    if (vars2 % op->optionalArgsCount()) //если количество доп.переменных в стеке не кратно количеству доп.переменных функции, то ошибка.
        return errorResult(CompileError::Arg,
                           funcSpecToStr(op->name(), op->args(), true));

    // достаем основные переменные
    //    for (qint16 i = _vars2; i < ArgCount; i++)
    //        MainArgs[i - _vars2] = Args[i];
    for (qint16 i = _vars2; i < ArgCount; i++)
        MainArgs << Args[i];

    // проверяем основные переменные
    if(!m_functions->searchBySpec(ix, op->name(), MainArgs, op->argsCount()))
        return errorResult(CompileError::FuncNotFound, tr("Не найдена функция: ") +
                           funcSpecToStr(op->name(), MainArgs, true, true));

    // проверяем дополнительные переменные
    while((vars2 -= op->optionalArgsCount()) >= 0)
        for(qint16 i = 0; i < op->optionalArgsCount(); i++)
            if(Args[vars2 + op->optionalArgsCount() - i - 1] != op->optionalArgs()[i])
                return errorResult(CompileError::Arg, funcSpecToStr(op->name(), op->args(), true));

    // достаем дополнительные переменные
    if(_vars2 > 0)
    {
        for(qint16 i = 0; i < op->optionalArgsCount(); i++)
            DopArgs << Args[i];

        if(!m_functions->searchBySpec(ix, op->name(), MainArgs, 0, DopArgs))
            return errorResult(CompileError::FuncNotFound, tr("Не найдена функция: ") +
                               funcSpecToStr(op->name(), MainArgs, true, true));
    }

    // наконец пишем код функции
    (m_functions->at(ix))->out(*m_byteCodeStream, ix);
    if (op->returnType())
        m_varStack.push(op->returnType());
    //	char s[200];
    //	wsprintf(s,"arg=%d, arg2=%d, a2cnt=%d, ret=%d",op->arg,op->arg2,_vars2,op->ret->isFLOAT());
    //	MessageBox(0,s,op->Name,0);

    // и число дополнительных аргументов
    *m_byteCodeStream << _vars2;
    return true;
}

//обрабатка функции-имиджа
bool Compiler::functionOut3(const QString& fname,
                            QList<Class*> Args, qint16 ArgCount)
{
    // а не в уравнении ли мы ?
    if (m_eqMode)
        return errorResult(CompileError::Internal, tr("Не поддерживается в уравнении"));

    QList<Class*> buf;
    QString str;

    //    qint16 args = 0;

    // есть ли имидж с таким именем ?
    Class* _class = LibraryCollection::getClassByName(fname);
    if (!_class)
        return functionOut4(fname, Args, ArgCount); //может это функция DLL

    if(!_class->isLoaded())
        _class->load();

    // он функция ?
    if ((_class->flags() & qint32(Class::Flag::Procedure)) == 0 )
        return functionOut4(fname, Args, ArgCount); //может это функция DLL

    //достать параметры
    for(qint16 i=0; i < _class->varInfoList().count(); i++)
        if(_class->varInfoList().at(i)->flags() & ClassVarInfo::Argument)
            buf << _class->varInfoList()[i]->varType();
    //    args = buf.count();

    str = funcSpecToStr(fname, Args, false, true);
    if (ArgCount > buf.count())        //аргументов больше, чем должно быть
        return errorResult(CompileError::ExtraPrm, tr("Не найдена функция. Слишком много аргументов: ") + str);

    if (ArgCount < buf.count())        //аргументов меньше, чем должно быть
        return errorResult(CompileError::FewPrm,tr("Не найдена функция. Слишком мало аргументов: ") + str);

    for (qint16 i = buf.count() - 1; i >= 0; i--)
        if (buf[i] != Args[buf.count() - 1 - i])    // аргументы не совпали !
            return errorResult(CompileError::FuncNotFound,
                               tr("Не найдена функция. Некорректные типы аргументов: ") + str);

    // пишем код функции
    m_functions->at(m_virtualFunctionIndex)->out(*m_byteCodeStream, m_virtualFunctionIndex);
    // пишем имя функции
    CompilerFunctions::strConstantOutEx(*m_byteCodeStream, fname);
    // пишем число аргументов
    *m_byteCodeStream << qint16(buf.count());
    // пишем типы аргументов
    for (int i = buf.count() - 1; i >= 0 ; i--)
    {
        if (StandardTypes::isFloat(buf[i]))
            *m_byteCodeStream << qint16(-1);

        else if(StandardTypes::isString(buf[i]))
            *m_byteCodeStream << qint16(-2);

        else if(StandardTypes::isHandle(buf[i]))
            *m_byteCodeStream << qint16(-3);

        else if(StandardTypes::isColorRef(buf[i]))
            *m_byteCodeStream << qint16(-4);

        else
            CompilerFunctions::strConstantOutEx(*m_byteCodeStream, buf[i]->originalName());
    }

    // возвращаемое значение
    Class* varType = nullptr;
    for(int i = 0; i < _class->varInfoList().count(); i++)
        if(_class->varInfoList()[i]->flags() & ClassVarInfo::Return)
        {
            varType = _class->varInfoList()[i]->varType();
            m_varStack.push(varType);
            break;
        }

    // пишем возвращаемое значение
    if (varType)
    {
        if (StandardTypes::isFloat(varType))
            *m_byteCodeStream << qint16(-1);

        else if (StandardTypes::isString(varType))
            *m_byteCodeStream << qint16(-2);

        else if (StandardTypes::isHandle(varType))
            *m_byteCodeStream << qint16(-3);

        else if (StandardTypes::isColorRef(varType))
            *m_byteCodeStream << qint16(-4);

        else
            CompilerFunctions::strConstantOutEx(*m_byteCodeStream, varType->originalName());
    }
    else
        *m_byteCodeStream << qint16(0);

    return true;
}

//обработка dll-функции
bool Compiler::functionOut4(const QString& functionName,
                            const QList<Class*>& Args, qint16 /*ArgCount*/)
{
    return errorResult(CompileError::FuncNotFound,
                       funcSpecToStr(functionName, Args, false, true));
}

qint16 Compiler::varOut(const QString varName)
{
    qint16 fix;
    qint16 ix;

    ClassVarInfo* varInfo = insertVar(ix, varName);
    if (!varInfo)
        return false;

    QString operatorName;
    if (m_eqMode) // EQUATION !!!
    {
        Equation* equation = new Equation();
        equation->setCode(EC_VAR);
        if (m_newModificator)
        {
            equation->setFlag(2);
            m_newModificator = 0;
        }
        else
            equation->setFlag(0);

        equation->setIndex(ix);
        m_equationStack.push(equation);
        return true;
    }
    //если перед переменной стоит знак ~
    if (m_newModificator)
    {
        //если компилируется имидж-функция
        operatorName = m_onePhaseMode ? "push" : "push_new";
        m_newModificator = 0;
    }
    else
        operatorName = "push";

    if (m_functions->searchByRetSpec(operatorName, varInfo->varType(), fix))
        m_functions->at(fix)->out(*m_byteCodeStream, ix);
    else
        return errorResult(CompileError::NsType, varInfo->varType()->originalName());

    return true;
}

qint16 Compiler::constantOut(const QString& type)
{
    qint16 ix;
    Class* cl = LibraryCollection::getClassByName(type);
    if (!cl)
        return errorResult(CompileError::UnknownType, type);

    m_varStack.push(cl);
    if (m_eqMode) // EQUATION !!!
    {
        Equation* equation = new Equation();
        equation->setCode(EC_CONST);
        equation->setValue(m_cursor->string().toDouble());
        m_equationStack.push(equation);
        return true;
    }
    if(m_functions->searchByRetSpec("push_cst", cl, ix))
        m_functions->at(ix)->out(*m_byteCodeStream, 0);
    else
        return errorResult(CompileError::NsType, cl->originalName());

    return true;
}

bool Compiler::errorResult(int errorCode, const QString& errorMessage)
{
    CompileError::ErrorCode errCode = static_cast<CompileError::ErrorCode>(errorCode);
    m_lastError = m_cursor == nullptr ?
                std::make_shared<CompileError>(errCode, errorMessage) :
                std::make_shared<CompileError>(*m_cursor, errCode, errorMessage);
    SystemLog::instance().error(tr("Compile error. "));
    return false;
}

void Compiler::restoreVariables(Class* cls, CompileVarCollection* varData)
{
    cls->deleteAllVariables();

    QList<ClassVarInfo*> vars = varData->variables();
    for (int i = 0; i < vars.count(); ++i)
        cls->addVarInfo(vars[i]);
}

QString Compiler::funcSpecToStr(const QString& funcName, QList<Class*> args, bool hasOptionalArgs, bool argsReversed)
{
    QString str = funcName + "(";

    if(argsReversed)
    {
        for(qint16 i = args.count() - 1; i > 0; i--)
            str += args[i]->originalName() + ", ";

        if(args.count() >= 1)
            str += args.last()->originalName();
    }
    else
    {
        for(qint16 i = 0; i < args.count() - 1; i++)
            str += args[i]->originalName() + ", ";

        if(args.count() >= 1)
            str += args.first()->originalName();
    }

    str += hasOptionalArgs ? ",[...])" : ")";
    return str;
}

}
