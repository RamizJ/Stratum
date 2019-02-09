#include "SourceCodeEditor.h"

#include "SyntaxHighlighter.h"
#include <QAction>

SourceCodeEditor::SourceCodeEditor(QWidget* parent) :
    SourceCodeEditor("", parent)
{
}

SourceCodeEditor::SourceCodeEditor(const QString& text, QWidget* parent) :
    QTextEdit(text, parent)
{
    setFont(QFont("Consolas", 11));
    setLineWrapMode(NoWrap);
    setAcceptRichText(true);

    QPalette textPalette = palette();
    textPalette.setColor(QPalette::Text, Qt::darkGreen);
    setPalette(textPalette);

    m_syntaxHightlighter = new SyntaxHighlighter(this);
    m_syntaxHightlighter->setDocument(document());

    QAction* undoAction = new QAction("Отмена", this);
    undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    addAction(undoAction);

    QAction* redoAction = new QAction("Повтор", this);
    redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    addAction(redoAction);

    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));
}

SourceCodeEditor::~SourceCodeEditor()
{}

