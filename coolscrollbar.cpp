#include "coolscrollbar.h"
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTextBlock>
#include <QtGui/QTextLayout>
#include <QtGui/QTextDocumentFragment>

#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QDebug>

#include <texteditor/basetexteditor.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditorconstants.h>

#include "coolscrollbarsettings.h"

CoolScrollBar::CoolScrollBar(TextEditor::BaseTextEditorWidget* edit,
                             QSharedPointer<CoolScrollbarSettings>& settings) :
    m_parentEdit(edit),
    m_settings(settings),
    m_yAdditionalScale(1.0),
    m_highlightNextSelection(false)
{
    m_parentEdit->viewport()->installEventFilter(this);

    m_internalDocument = originalDocument().clone();
    applySettingsToDocument(internalDocument());
    connect(m_parentEdit, SIGNAL(textChanged()), SLOT(onDocumentContentChanged()));
    connect(m_parentEdit, SIGNAL(selectionChanged()), SLOT(onDocumentSelectionChanged()));
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);

    p.fillRect(rect(), Qt::white);

    int lineHeight = calculateLineHeight();

    qreal documentHeight = qreal(lineHeight * m_internalDocument->lineCount());
    documentHeight *= settings().m_yDefaultScale;

    if(documentHeight > size().height())
    {
        m_yAdditionalScale = size().height() / documentHeight;
    }

    p.scale(getXScale(), getYScale());

    drawPreview(p);
    drawViewportRect(p);
    p.end();
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::unfoldedLinesCount() const
{
    Q_ASSERT(m_parentEdit);
    int res = 0;
    QTextBlock b = originalDocument().firstBlock();
    while(b != originalDocument().lastBlock())
    {
        if(b.isVisible())
        {
            res += b.lineCount();
        }
        b = b.next();
    }
    return res;
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::linesInViewportCount() const
{
    return (2 * m_parentEdit->document()->lineCount() - unfoldedLinesCount() - maximum());
}
////////////////////////////////////////////////////////////////////////////
QSize CoolScrollBar::sizeHint() const
{
    return QSize(settings().m_scrollBarWidth, 0);
}
////////////////////////////////////////////////////////////////////////////
QSize CoolScrollBar::minimumSizeHint() const
{
    return QSize(settings().m_scrollBarWidth, 0);
}
////////////////////////////////////////////////////////////////////////////
const QTextDocument & CoolScrollBar::originalDocument() const
{
    return *m_parentEdit->document();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::onDocumentContentChanged()
{
    internalDocument().setPlainText(originalDocument().toPlainText());
    applySettingsToDocument(internalDocument());
    update();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::onDocumentSelectionChanged()
{
    if(m_highlightNextSelection)
    {
        // clear previous highlight
        highlightEntryInDocument(internalDocument(), m_stringToHighlight, Qt::white);
        m_stringToHighlight = m_parentEdit->textCursor().selection().toPlainText();
        // highlight new selection
        highlightEntryInDocument(internalDocument(), m_stringToHighlight,
                                 settings().m_selectionHighlightColor);

        update();
    }
}
////////////////////////////////////////////////////////////////////////////
bool CoolScrollBar::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_parentEdit->viewport())
    {
        m_highlightNextSelection = (e->type() == QEvent::MouseButtonDblClick);
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::getXScale() const
{
    return settings().m_xDefaultScale;
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::getYScale() const
{
    return settings().m_yDefaultScale * m_yAdditionalScale;
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawViewportRect(QPainter &p)
{
    int lineHeight = calculateLineHeight();
    QPointF rectPos(0, value() * lineHeight);
    QRectF rect(rectPos, QSizeF(settings().m_scrollBarWidth / getXScale(),
                                linesInViewportCount() * lineHeight));

    p.setBrush(QBrush(settings().m_viewportColor));
    p.drawRect(rect);
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::calculateLineHeight() const
{
    QFontMetrics fm(settings().m_font);
    return fm.height();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawPreview(QPainter &p)
{
    internalDocument().drawContents(&p);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::applySettingsToDocument(QTextDocument &doc) const
{
    doc.setDefaultFont(settings().m_font);
    doc.setDefaultTextOption(settings().m_textOption);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::highlightEntryInDocument(QTextDocument & doc, const QString &str, const QColor& color) const
{
    if(str.isEmpty())
    {
        return;
    }
    QTextCursor cur_cursor(&doc);
    while(true)
    {
        cur_cursor = doc.find(str, cur_cursor);
        if(!cur_cursor.isNull())
        {
            QTextCharFormat format(cur_cursor.charFormat());
            format.setBackground(color);
            cur_cursor.mergeCharFormat(format);
        }
        else
        {
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        qreal yPos = event->posF().y();
        qreal documentHeight = internalDocument().lineCount() * calculateLineHeight() * getYScale();
        int value = int(yPos * maximum() / documentHeight);
        if(value > maximum())
            value = maximum();
        setValue(value);
    }
    else if(event->button() == Qt::RightButton)
    {
        highlightEntryInDocument(internalDocument(), m_stringToHighlight, Qt::white);
        update();
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::contextMenuEvent(QContextMenuEvent *event)
{
    if(!settings().m_disableContextMenu)
    {
        QScrollBar::contextMenuEvent(event);
    }
}

void CoolScrollBar::mouseMoveEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {

    }
}
