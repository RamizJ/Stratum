#include "TplCursor.h"
#include "Command.h"

#include <QTextCodec>
#include <QVector>
#include <StDataGlobal.h>

namespace StCompiler {

TplCursor::TplCursor(QTextStream& stream) :
    m_stream(stream),
    m_counter(-1),
    m_isEof(false),
    m_type(OperatorType::undef),
    m_lastType(OperatorType::undef),
    m_lineNumber(0),
    m_newLine(0)
{
    QString data = m_stream.readAll();
//    m_buffer = data.toLocal8Bit();
    m_buffer = StData::toWindows1251(data);

    m_charGroupDictionary = QList<quint8>(
    {   //0 1 2 3  4 5 6  7  8 9 a b  c  d  e f
        0,0,0,0, 0,0,0, 0, 0,0,9,0, 0, 0, 0,0,   	// 0
        0,0,0,0, 0,0,0, 0, 0,0,0,0, 0, 0, 0,0,   	// 1
        0,3,4,12,7,3,10,13,3,3,3,15,3,	16,6,5,  	// 2
        2,2,2,2, 2,2,2, 2, 2,2,8,3, 3, 3, 3,3,   	// 3       3b->14
        3,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 4
        1,1,1,1, 1,1,1, 1, 1,1,1,3, 3, 3, 3,1,   	// 5
        0,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 6
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 11,1, 3,0,  	// 7
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 8
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 9
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// a
        0,0,0,0, 0,0,0, 0, 0,0,0,0, 0, 0, 0,0,   	// b
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// c
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// d
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// e
        1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1    	// f
                                });
}

TplCursor::~TplCursor()
{}

OperatorType TplCursor::read()
{
    m_lastPos = m_counter;
    m_lastType = m_type;
    qint16 charGroup = 0;
    m_str = "";

begin:
    if (++m_counter >= m_buffer.size()) //проверяем дошли ли мы до конца текста
        goto eof;

    switch (getCharGroup(bufCharAt(m_counter))) //получаем номер группы символа
    {
        case 0:  // unknown
            goto begin;

        case 1: // symbol
            m_str += bufCharAt(m_counter);
            goto contin;

        case 2: // number
            m_str += bufCharAt(m_counter);
            goto dconstant;

        case 3: // operators
        {
            if (bufCharAt(m_counter) == '-')
            {
                if (m_lastType == OperatorType::optr && getCharGroup(m_buffer[m_counter+1]) == 2 ) // cifra
                {
                    if (bufCharAt(m_counter - 1) != ')' && bufCharAt(m_counter - 2) != ')') // если скобка впереди
                    {
                        m_str += m_buffer[m_counter];
                        goto dconstant;
                    }
                }
                if (m_buffer[m_counter + 1] == '>') // ->
                {
                    m_str += m_buffer[m_counter];
                    m_str += m_buffer[++m_counter];
                    return m_type = OperatorType::optr;
                }
            }
            if (m_buffer[m_counter] == '!' && m_buffer[m_counter+1] != '=')
            {
                m_str += m_buffer[m_counter];
                return (m_type = OperatorType::optr);
            }
            if (m_buffer[m_counter] != ')' && m_buffer[m_counter+1] == '=')
            {
                m_str += m_buffer[m_counter];
                m_str += m_buffer[++m_counter];
                return (m_type = OperatorType::optr);
            }
            m_str += m_buffer[m_counter];
            return (m_type = OperatorType::optr);
        }

        case 4:     // '"'
            goto string;

        case 5:     // comment '//'
        {
            if (m_buffer[m_counter+1] == '/')
                goto wait;

            if (m_buffer[m_counter+1] == '*')
                goto wait2;

            m_str += m_buffer[m_counter];
            return m_type = OperatorType::optr;
        }

        case 6: // '.'
        {
            m_str += m_buffer[m_counter];
            return m_type = OperatorType::optr;
        }

        case 7:  // $ - hconst
        {
            m_str += m_buffer[m_counter];
            goto hconstant;
        }

        case 8:  // ':'
        {
            m_str += m_buffer[m_counter];
            goto assign;
        }

        case 9:  //'LF'
        {
            m_lineNumber++;
            //Update progressbar
            m_newLine = 1;
            goto begin;
        }

        case 10: // '&'
            return handleOperator('&');

        case 11: // '|'
            return handleOperator('|');

        case 12:  // # - integer const
        {
            m_str += m_buffer[m_counter];
            goto iconstant;
        }
        case 13:  // '''  строковая константа 2
            goto string2;

        case 15: // '+'
            return handleOperator('+'); // инкремент ++

        case 16: // '-'
            return handleOperator('-'); // декремент --

        default : goto begin;
    }

assign: //нашли двоеточие
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::optr;

    if(bufCharAt(m_counter) == '=') //нашли оператор присвоения :=
    {
        m_str += bufCharAt(m_counter);
        return m_type = OperatorType::optr;
    }
    else
    {
        m_counter--;
        return m_type = OperatorType::optr;
    }

wait:  //комментарий до конца строки //...
    if (++m_counter >= m_buffer.size())
        goto eof;

    if (bufCharAt(m_counter) == 0x0a) //если дошли до конца строки
    {
        m_lineNumber++;
        m_newLine = 1;
        goto begin;
    }
    else
        goto wait;

wait2: // конкретный комментарий  /*...*/
    if (++m_counter >= m_buffer.size())
        goto eof;

    if (bufCharAt(m_counter) == '*' && bufCharAt(m_counter+1) == '/')
    {
        if (++m_counter >= m_buffer.size())
            goto eof;
        goto begin;
    }
    if (bufCharAt(m_counter) == 0x0a)
    {
        m_lineNumber++;
        m_newLine = 1;
    }
    goto wait2;

contin: //выделяем строковую лексему (последовательность букв и цифр)
    if (++m_counter >= m_buffer.size())//если дошли до конца текста, то выходим
        return m_type = OperatorType::opnd;

    charGroup = getCharGroup(bufCharAt(m_counter));//получаем группу символа
    if (charGroup == 1 || charGroup == 2) //если это буква или цифра, то повторяем проход
    {
        m_str += m_buffer[m_counter];
        goto contin;
    }
    else//если это не буква и не цифра, то выходим
    {
        m_counter--;
        return m_type = OperatorType::opnd;
    }

string: //выделяем строковую лексему в ковычках "
    if (m_counter++ >= m_buffer.size())
        return m_type = OperatorType::sconst;

    if (bufCharAt(m_counter) != 0x22 ) // "
    {
        m_str += m_buffer[m_counter];
        if (m_str.length() == 256)
            return m_type = OperatorType::sconst;
        else
            goto string;
    }
    else
        return m_type = OperatorType::sconst;

string2: //выделяем строковую лексему в апострофах '
    if (m_counter++ >= m_buffer.size())
        return m_type = OperatorType::sconst;

    if (bufCharAt(m_counter) != 0x27 ) // '
    {
        m_str += m_buffer[m_counter];

        if (m_str.length() == 256)
            return m_type = OperatorType::sconst;
        else
            goto string2;
    }
    else
        return m_type = OperatorType::sconst;

dconstant: //выделяем цифровую лексему
    if (++m_counter >= m_buffer.size())
    {
        return m_type = OperatorType::dconst;
    }
    //если среди цифр есть буква e или E, то переходим определению
    //вещественной лексемы с выделеной мантиссой и порядком
    if ( bufCharAt(m_counter) == 0x45 || bufCharAt(m_counter) == 0x65 )
    {
        m_str += bufCharAt(m_counter);
        goto fconstant4;
    }
    switch (getCharGroup(bufCharAt(m_counter)))
    {
        case 6 : // если среди цифр есть точка, то переходим к определнию лексемы с плавающей точкой
        {
            m_str += bufCharAt(m_counter);
            goto fconstant;
        }
        case 2 : //если символ опять цифра, то повторяем цикл
        {
            m_str += bufCharAt(m_counter);
            goto dconstant;
        }
        default:
        {
            m_counter--;
            return m_type = OperatorType::dconst;
        }
    }

iconstant: //выделяем целочисленную лексему #...
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::iconst;

    if ( bufCharAt(m_counter) == 0x45 || bufCharAt(m_counter) == 0x65 )   // e E
    {
        m_str += bufCharAt(m_counter);
        goto fconstant2;
    }
    switch (getCharGroup(bufCharAt(m_counter)))
    {
        case 6 : // .
        {
            m_str += m_buffer[m_counter];
            goto fconstant;
        }
        case 2 : // number
        {
            m_str += m_buffer[m_counter];
            goto iconstant;
        }
        default :
        {
            m_counter--;
            return m_type = OperatorType::iconst;
        }
    }

hconstant: //выделение шестнадцатеричной лексему $...
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::hconst;

    if ((bufCharAt(m_counter) >= 0x30 && bufCharAt(m_counter) <= 0x39) ||  //0..9
        (bufCharAt(m_counter) >= 0x41 && bufCharAt(m_counter) <= 0x46) ||  //A..F
        (bufCharAt(m_counter) >= 0x61 && bufCharAt(m_counter) <= 0x66))    //a..f
    {
        m_str += m_buffer[m_counter];
    }
    else
    {
        m_counter--;
        return m_type = OperatorType::optr;
    }

hconstant2:
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::hconst;

    if ((bufCharAt(m_counter) >= 0x30 && bufCharAt(m_counter) <= 0x39) ||
        (bufCharAt(m_counter) >= 0x41 && bufCharAt(m_counter) <= 0x46) ||
        (bufCharAt(m_counter) >= 0x61 && bufCharAt(m_counter) <= 0x66))
    {
        m_str += bufCharAt(m_counter);
        goto hconstant2;
    }
    else
    {
        m_counter--;
        return m_type = OperatorType::hconst;
    }

fconstant: //выделяем вещественную лексему
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::fconst;

    if (bufCharAt(m_counter) >= 0x30 && bufCharAt(m_counter) <= 0x39) // number
    {
        m_str += bufCharAt(m_counter);
        goto fconstant2;
    }
    else
    {
        m_counter -= 2;
        m_str = m_str.left(m_str.length() - 2);
//        *buf-- = 0;
//        *buf-- = 0;
        return m_type = OperatorType::dconst;
    }

fconstant2: //выделяем вещественную лексему с мантиссой и порядком
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::fconst;

    if (bufCharAt(m_counter) == 0x45 || bufCharAt(m_counter) == 0x65) // E e
    {
        m_str += bufCharAt(m_counter);
        goto fconstant3;
    }
    if (bufCharAt(m_counter) >= 0x30 && bufCharAt(m_counter) <= 0x39) //числа
    {
        m_str += bufCharAt(m_counter);
        goto fconstant2;
    }
    else
    {
        m_counter--;
        return m_type = OperatorType::fconst;
    }

fconstant3:
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::fconst;

    if (bufCharAt(m_counter) == 0x2d || bufCharAt(m_counter) == 0x2b)   // - +
    {
        m_str += bufCharAt(m_counter);
        goto fconstant4;
    }
    if ( bufCharAt(m_counter) >= 0x30 && bufCharAt(m_counter) <= 0x39) //числа
    {
        m_str += bufCharAt(m_counter);
        goto fconstant4;
    }
    else
    {
        m_counter--;
        return m_type = OperatorType::fconst;
    }

fconstant4:
    if (++m_counter >= m_buffer.size())
        return m_type = OperatorType::fconst;

    if (bufCharAt(m_counter) >= 0x30 && bufCharAt(m_counter) <= 0x39) //числа
    {
        m_str += bufCharAt(m_counter);
        goto fconstant4;
    }
    else
    {
        m_counter--;
        return m_type = OperatorType::fconst;
    }

eof:
    m_isEof = true;
    return m_type = OperatorType::eof;
}

//поиск символа с текущей позиции и до конца строки или текста
qint16 TplCursor::scanChar(char ch)
{
    int i = m_counter;
    while (m_buffer[i] != '\n' && i < m_buffer.count())
    {
        i++;
        if (m_buffer[i] == ch)
            return i - m_counter;
    }
    return 0;
}

void TplCursor::back()
{
    m_counter -= m_str.length();
    m_type = m_lastType;
}

uchar TplCursor::getNextChar()
{
    return bufCharAt(m_counter + 1);
}

bool TplCursor::isNextOperator(uchar ch)
{
    int i = m_counter + 1;
    while(i < m_buffer.length() && bufCharAt(i) == ' ')
        i++;

    return i < m_buffer.length() ? (bufCharAt(i) == ch) : false;
}

QString TplCursor::string()
{
//    return QString::fromLocal8Bit(m_str);
    return StData::fromWindows1251(m_str);
}

char TplCursor::charAt(int index) const
{
    return m_str.at(index);
}

char TplCursor::bufCharAt(int index) const
{
    return m_buffer.at(index);
}

OperatorType TplCursor::handleOperator(char ch)
{
    if (m_buffer[m_counter+1] == ch)
    {
        m_str += m_buffer[m_counter];
        m_str += m_buffer[++m_counter];
        return m_type = OperatorType::optr;
    }
    m_str += m_buffer[m_counter];
    return m_type = OperatorType::optr;
}

OperatorType TplCursor::type() const
{
    return m_type;
}

bool TplCursor::isEquation()
{
    qint16 isString = 0;
    int counter = m_counter;
    while (bufCharAt(counter++) != '\n' && bufCharAt(counter))
    {
        if (bufCharAt(counter) == '"' || bufCharAt(counter) == '\'')
            isString ^= 1;

        if (!isString)
        {
            if (bufCharAt(counter) == ':'  && bufCharAt(counter + 1) == '=')
                return false;

            if (bufCharAt(counter) == '/'  && bufCharAt(counter + 1) == '/')
                return false;

            if (bufCharAt(counter) == '='  &&
                bufCharAt(counter - 1) != '=' &&
                bufCharAt(counter - 1) != '!' &&
                bufCharAt(counter - 1) != '>' &&
                bufCharAt(counter - 1) != '<' &&
                bufCharAt(counter + 1) != '=')
                return true;
        }
    }
    return false;
}

qint16 TplCursor::getCharGroup(uchar ch)
{
    return m_charGroupDictionary[ch];
}

}
