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

    // TODO: optimize
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
    delete m_internalDocument;
    m_internalDocument = originalDocument().clone();
    applySettingsToDocument(internalDocument());
    update();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::onDocumentSelectionChanged()
{
    if(m_highlightNextSelection)
    {
        m_stringToHighlight = m_parentEdit->textCursor().selection().toPlainText();
        qDebug() << m_stringToHighlight;
    }
}
////////////////////////////////////////////////////////////////////////////
bool CoolScrollBar::eventFilter(QObject *o, QEvent *e)
{
    if(o == m_parentEdit->viewport())
    {
        m_highlightNextSelection = (e->type() == QEvent::MouseButtonDblClick);
        qDebug() << m_highlightNextSelection << e->type();
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
void CoolScrollBar::drawPreview2(QPainter &p)
{
    QPointF pos(0, 0);

    p.setFont(settings().m_font);

    // text drawing
    QTextOption opt;
    opt.setTabStop(8.0);
    p.setPen(QPen(Qt::black));

    QTextBlock block = originalDocument().firstBlock();
    int i =0;
    while(block.isValid())
    {
        if(block.isVisible())
        {
            QRectF rect;
//            rect.setTopLeft(pos);
//            rect.setWidth(2 * settings().m_scrollBarWidth);
//            rect.setHeight(lineHeight * block.lineCount());
//            p.drawText(rect, block.text(), opt);
//            pos.setY(pos.y() + lineHeight * block.lineCount());

            p.setBrush(Qt::red);
            QTextBlock::Iterator it = block.begin();
            while (it != block.end())
            {
                p.drawText(pos, it.fragment().text());
                pos.setX(pos.x() + it.fragment().length());
                ++it;
            }
            pos.setY(pos.y() + calculateLineHeight() * block.lineCount());
            pos.setX(0);
        }
        block = block.next();
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawViewportRect(QPainter &p)
{
    int lineHeight = calculateLineHeight();
    QPointF rectPos(0, value() * lineHeight);
    QRectF rect(rectPos, QSizeF(settings().m_scrollBarWidth,  linesInViewportCount() * lineHeight));

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
    QTextCursor find_cursor = m_internalDocument->find("include");
    QTextCharFormat format(find_cursor.charFormat());
    format.setBackground(QBrush(Qt::red));
    find_cursor.mergeCharFormat(format);

    internalDocument().drawContents(&p);
}
////////////////////////////////////////////////////////////////////////////
QTextDocument & CoolScrollBar::internalDocument()
{
    return *m_internalDocument;
}
////////////////////////////////////////////////////////////////////////////
const QTextDocument & CoolScrollBar::internalDocument() const
{
    return *m_internalDocument;
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::applySettingsToDocument(QTextDocument &doc) const
{
    doc.setDefaultFont(settings().m_font);
    doc.setDefaultTextOption(settings().m_textOption);
}
