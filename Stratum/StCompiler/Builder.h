#ifndef BUILDER_H
#define BUILDER_H

#include <QByteArray>
#include <QObject>
#include <memory>

namespace StCore {
class Class;
}

namespace StCompiler {

class Compiler;
class Command;
class CommandCollection;
class MacroCollection;

class Builder : public QObject
{
    Q_OBJECT

public:
    Builder(QObject* parent = nullptr);
    ~Builder();

    bool initialize();
    bool build(StCore::Class* cls, const QString& newSourceCode);

private:
    bool errorResult(const QString& errorMessage);

private:
    QString m_compilerTplPath;

    std::shared_ptr<Compiler> m_compiler;

    std::shared_ptr<Command> m_epilog;
    std::shared_ptr<CommandCollection> m_operators;
    std::shared_ptr<CommandCollection> m_functions;
    std::shared_ptr<MacroCollection> m_constants;

    QByteArray m_byteCodeBuffer;

    QString m_lastErrorText;
};

}

#endif // BUILDER_H
