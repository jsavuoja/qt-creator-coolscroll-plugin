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
    m_highlightNextSelection(false),
    m_leftButtonPressed(false)
{
    m_parentEdit->viewport()->installEventFilter(this);

    m_internalDocument = originalDocument().clone();
    applySettingsToDocument(internalDocument());
    connect(m_parentEdit, SIGNAL(textChanged()), SLOT(onDocumentContentChanged()));
    connect(m_parentEdit, SIGNAL(selectionChanged()), SLOT(onDocumentSelectionChanged()));

    updatePicture();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    p.drawPixmap(0, 0, width(), height(), m_previewPic);
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
    QTextBlock origBlock = originalDocument().firstBlock();
    QTextBlock internBlock = internalDocument().firstBlock();
    int globalLen = 0;

    while(origBlock.isValid() && internBlock.isValid())
    {
        QList<QTextLayout::FormatRange> formats = origBlock.layout()->additionalFormats();
        QList<QTextLayout::FormatRange>::iterator it = formats.begin();
        qDebug() << "next layout";
        while(it != formats.end())
        {
           // (*it).format.setBackground((*it).format.foreground().color());
            qDebug() << "format : " << (*it).start << (*it).length;
            globalLen += (*it).length;
            it++;
        }

        internBlock.layout()->setAdditionalFormats(formats);
        origBlock = origBlock.next();
        internBlock = internBlock.next();
    }
    qDebug() << "goablalen = " << globalLen << internalDocument().toPlainText().size();
        highlightEntryInDocument(internalDocument(), m_stringToHighlight,
                                 settings().m_selectionHighlightColor);
    updatePicture();
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

        updatePicture();
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
    qreal lineHeight = calculateLineHeight();
    lineHeight *= getYScale();
    QPointF rectPos(0, value() * lineHeight);
    QRectF rect(rectPos, QSizeF(settings().m_scrollBarWidth,
                                linesInViewportCount() * lineHeight));

    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(settings().m_viewportColor));
    p.drawRect(rect);
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::calculateLineHeight() const
{
    QFontMetrics fm(settings().m_font);
    return qreal(fm.height());
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
        setValue(posToValue(event->posF().y()));
        m_leftButtonPressed = true;
    }
    else if(event->button() == Qt::RightButton)
    {
        highlightEntryInDocument(internalDocument(), m_stringToHighlight, Qt::white);
        updatePicture();
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
    if(m_leftButtonPressed)
    {
        setValue(posToValue(event->posF().y()));
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::updatePicture()
{
    updateScaleFactors();
    m_previewPic = QPixmap(width() / getXScale(), height() / getYScale());
    m_previewPic.fill(Qt::white);
    QPainter pic(&m_previewPic);
    drawPreview(pic);
    pic.end();

    // scale pixmap with bilinear filtering
    m_previewPic = m_previewPic.scaled(width(), height(), Qt::IgnoreAspectRatio,
                                       Qt::SmoothTransformation);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::updateScaleFactors()
{
    int lineHeight = calculateLineHeight();

    qreal documentHeight = qreal(lineHeight * m_internalDocument->lineCount());
    documentHeight *= settings().m_yDefaultScale;

    if(documentHeight > size().height())
    {
        m_yAdditionalScale = size().height() / documentHeight;
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::resizeEvent(QResizeEvent *)
{
    updatePicture();
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::posToValue(qreal pos) const
{
    qreal documentHeight = internalDocument().lineCount() * calculateLineHeight() * getYScale();
    int value = int(pos * (maximum() + linesInViewportCount()) / documentHeight);

    // set center of viewport to position of click
    value -= linesInViewportCount() / 2;
    if(value > maximum())
    {
        value = maximum();
    }
    else if (value < minimum())
    {
        value = minimum();
    }
    return value;
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_leftButtonPressed = false;
    }
}
