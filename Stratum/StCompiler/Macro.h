#ifndef MACRO_H
#define MACRO_H

#include <QString>

namespace StCompiler {

class TplCursor;

class Macro
{
public:
    Macro(const QString& name = "", double value = 0.0);
    ~Macro();

    QString name() const {return m_name;}
    double value() const {return m_value;}

public:
    void load(TplCursor& cursor);

private:
    QString m_name;
    double m_value;
};

}

#endif // MACRO_H
