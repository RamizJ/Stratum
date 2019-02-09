#ifndef SOURCECODEEDITOR_H
#define SOURCECODEEDITOR_H

#include <QTextEdit>

class SyntaxHighlighter;

class SourceCodeEditor : public QTextEdit
{
public:
    explicit SourceCodeEditor(QWidget* parent = 0);
    SourceCodeEditor(const QString& text, QWidget* parent = 0);
    ~SourceCodeEditor();

private:
    SyntaxHighlighter* m_syntaxHightlighter;
};

#endif // SOURCECODEEDITOR_H
