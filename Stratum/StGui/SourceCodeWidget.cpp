#include "SourceCodeWidget.h"
#include "ui_SourceCodeWidget.h"

#include <QtConcurrent/QtConcurrent>

#include <Compiler.h>
#include <CompileError.h>
#include <Project.h>
#include <Class.h>
#include <Core.h>
#include "GuiContext.h"

using namespace StData;
using namespace StCompiler;
using namespace StCore;
using namespace std;

SourceCodeWidget::SourceCodeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceCodeWidget)
{
    ui->setupUi(this);

    m_compileStatusText = "<html><head/><body><p>Статус: "
                          "<span style=\" font-weight:600; color:%1;\">%2</span></p></body></html>";
    ui->labelCompileStatus->setText(m_compileStatusText.arg("gray").arg("не выполнялась"));

    connect(&m_compileFutureWatcher, SIGNAL(finished()), SLOT(compileFinished()));
    connect(ui->pushButtonCompile, SIGNAL(clicked()), SLOT(startCompile()));
}

SourceCodeWidget::~SourceCodeWidget()
{
    delete ui;
}

void SourceCodeWidget::setClass(StData::Class* cls)
{
    if(m_class == cls)
        return;

    m_class = cls;
    if(m_class != nullptr)
        ui->textEditSourceCode->setText(m_class->text());
}

void SourceCodeWidget::timerEvent(QTimerEvent*)
{
    if(ui->labelCompileStatus->text().contains("Компиляция..."))
        ui->labelCompileStatus->setText(m_compileStatusText.arg("black").arg("Компиляция."));

    else if(ui->labelCompileStatus->text().contains("Компиляция.."))
        ui->labelCompileStatus->setText(m_compileStatusText.arg("black").arg("Компиляция..."));

    else if(ui->labelCompileStatus->text().contains("Компиляция."))
        ui->labelCompileStatus->setText(m_compileStatusText.arg("black").arg("Компиляция.."));
}

void SourceCodeWidget::startCompile()
{
    QString sourceCode = ui->textEditSourceCode->toPlainText();

    ui->pushButtonCompile->setEnabled(false);
    ui->labelCompileStatus->setText(m_compileStatusText.arg("black").arg("Компиляция."));
    m_timerId = startTimer(400);

    auto compileAsync = [](Class* cls, QString src) -> bool
    {
        bool relinkVars;
        bool result = StCore::Core::instance().compiler()->compile(cls, src, relinkVars);
        if(relinkVars)
        {
            Project* project = Core::instance().currentProject();
            project->setupLinks(true);
        }
        return result;
    };
    QFuture<bool> future = QtConcurrent::run(QThreadPool::globalInstance(),
                                             compileAsync, m_class, sourceCode);
    m_compileFutureWatcher.setFuture(future);
}

void SourceCodeWidget::compileFinished()
{
    Compiler* compiler = StCore::Core::instance().compiler();

    killTimer(m_timerId);
    ui->pushButtonCompile->setEnabled(true);
    ui->textEditSourceCode->setFocus();

    bool compileSuccess = m_compileFutureWatcher.result();
    if(compileSuccess)
    {
        ui->labelCompileStatus->setText(m_compileStatusText.arg("#009900").arg(tr("Успешно")));
    }
    else
    {
        QString errText = QString(tr("Ошибка: %1")).arg(compiler->lastError()->message());
        ui->labelCompileStatus->setText(m_compileStatusText.arg("red").arg(errText));

        QTextCursor cursor = ui->textEditSourceCode->textCursor();
        cursor.setPosition(compiler->lastError()->startPos());
        ui->textEditSourceCode->setTextCursor(cursor);
    }
}
