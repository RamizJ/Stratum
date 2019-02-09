#include "Constant.h"

namespace StCompiler {

Constant::Constant(const QString name, double value) :
    m_name(name),
    m_value(value)
{}

Constant::~Constant()
{}

}
