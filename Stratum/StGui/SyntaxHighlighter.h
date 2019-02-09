#ifndef SINTAXHIGHLIGHTER_H
#define SINTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit SyntaxHighlighter(QObject *parent);
    ~SyntaxHighlighter();

    // QSyntaxHighlighter interface
protected:
    void highlightBlock(const QString& text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_highlightingRules;

    QRegExp m_commentStartExpression;
    QRegExp m_commentEndExpression;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_functionFormat;
    QTextCharFormat m_quotationFormat;
    QTextCharFormat m_singleLineCommentFormat;
    QTextCharFormat m_multiLineCommentFormat;
};

#endif // SINTAXHIGHLIGHTER_H
