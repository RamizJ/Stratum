#include "Equation.h"
#include "Sc2000DataStream.h"
#include "Log.h"

namespace StData
{

Equation::Equation(QObject *parent) :
    QObject(parent),
    m_index(0),
    m_flag(0),
    m_value(0.0)
{}

Equation::~Equation()
{
    qDeleteAll(m_equations);
    qDeleteAll(m_childEquations);
}

QList<Equation*> Equation::equations() const
{
    return m_equations;
}

QList<Equation*> Equation::childEquations() const
{
    return m_childEquations;
}

void Equation::addEquation(Equation* equation)
{
    m_equations.append(equation);
}

void Equation::addEquations(QList<Equation*> equations)
{
    m_equations.append(equations);
}

void Equation::addChildEquation(Equation* equation)
{
    m_childEquations.append(equation);
}

void Equation::addChildEquations(QList<Equation*> equations)
{
    m_childEquations.append(equations);
}

void Equation::load(St2000DataStream& stream)
{
    qint16 code = stream.readInt16();
    if(code == Var)
    {
        m_index = stream.readInt16();
        m_flag = stream.readInt16();
    }
    else if(code == Const)
        m_value = stream.readDouble();

    quint8 byte = stream.readUint8();
    while(byte)
    {
        switch (byte)
        {
            case 1:
            {
                Equation* equ = new Equation();
                equ->load(stream);
                addChildEquation(equ);
            }break;

            case 2:
            {
                Equation* equ = new Equation();
                equ->load(stream);
                addEquation(equ);
            }break;
        }
        byte = stream.readUint8();
    }
}

void Equation::skip(St2000DataStream& stream)
{
    qint16 code = stream.readInt16();
    switch(code)
    {
        case DataCodes::Var:
            stream.skipRawData(4);
            break;

        case DataCodes::Const:
            stream.skipRawData(sizeof(double));
            break;
    }

    quint8 byte = stream.readUint8();
    while(byte)
    {
        if(byte == 1 || byte == 2)
            skip(stream);

        byte = stream.readUint8();
    }
}

}
