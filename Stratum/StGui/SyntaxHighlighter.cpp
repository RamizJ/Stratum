#include "SyntaxHighlighter.h"

#include <QRegExp>

SyntaxHighlighter::SyntaxHighlighter(QObject* parent) :
    QSyntaxHighlighter(parent)
{
    QStringList keywordPatterns;
    keywordPatterns << "\\blocal\\b" << "\\bif\\b" << "\\belse\\b" << "\\bendif\\b" << "\\bwhile\\b" << "\\bendwhile\\b"
                    << "\\bswitch\\b" << "\\bendswitch\\b" << "\\bcase\\b" << "\\breturn\\b" << "\\bfunction\\b" << "\\bparameter\\b";

    QStringList typePatterns;
    typePatterns << "\\bfloat\\b" << "\\bhandle\\b" << "\\bstring\\b" << "\\bcolorref\\b";

    m_keywordFormat.setForeground(QBrush(QColor(100, 100, 255)));
    m_typeFormat.setForeground(Qt::darkRed);
    m_functionFormat.setForeground(QBrush(QColor(150, 0, 0)));
    m_singleLineCommentFormat.setForeground(Qt::gray);
    m_multiLineCommentFormat.setForeground(Qt::gray);
    m_quotationFormat.setForeground(QBrush(QColor(200, 90, 0)));

    HighlightingRule rule;

    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\s*\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);

    for(QString& pattern : keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }
    for(QString& pattern : typePatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_typeFormat;
        m_highlightingRules.append(rule);
    }

//    rule.pattern = QRegExp("\".*\"");
    rule.pattern = QRegExp("\"[\\w\\d\\s_]*\"");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);

    rule.pattern = QRegExp("//[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    m_commentStartExpression = QRegExp("/\\*");
    m_commentEndExpression = QRegExp("\\*/");
}

SyntaxHighlighter::~SyntaxHighlighter()
{}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for(HighlightingRule &rule : m_highlightingRules)
    {
        QRegExp expression(rule.pattern);
        expression.setCaseSensitivity(Qt::CaseInsensitive);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = m_commentStartExpression.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = m_commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + m_commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = m_commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

