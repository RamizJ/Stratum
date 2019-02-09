#ifndef MACRO_H
#define MACRO_H

#include <QString>
#include <QtGlobal>

namespace StCompiler {

class Constant
{
public:
    Constant(const QString name, double value);
    ~Constant();

private:
    QString m_name;
    double m_value;
};

}

#endif // MACRO_H
