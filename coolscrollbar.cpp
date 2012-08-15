/*
*
* Copyright (C) 2011 EgorZhuk
*
* Authors: Egor Zhuk <egor.zhuk@gmail.com>
*
* This file is part of CoolScroll plugin for QtCreator.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "coolscrollbar.h"
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTextBlock>
#include <QtGui/QTextLayout>
#include <QtGui/QTextDocumentFragment>

#include <QtGui/QPainter>
#include <QApplication>
#include <QDebug>

#include <texteditor/basetexteditor.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditorconstants.h>

#include "coolscrollbarsettings.h"

namespace
{
    const int REFRESH_PERIOD_MSEC = 1000;
    const int MARKER_MARGIN_WIDTH = 5;
    const int MARKER_TICK_SIZE = 3;
    const int MARKER_TICK_XPOS = 1;
}

////////////////////////////////////////////////////////////////////////////
CoolScrollBar::CoolScrollBar(TextEditor::BaseTextEditorWidget* edit,
                             QSharedPointer<CoolScrollbarSettings>& settings) :
    m_parentEdit(edit),
    m_settings(settings),
    m_yAdditionalScale(1.0),
    m_highlightNextSelection(false),
    m_leftButtonPressed(false),
    m_stateDirty(false)
{
    m_parentEdit->viewport()->installEventFilter(this);

    m_internalDocument = originalDocument().clone();
    (void)m_internalDocument->documentLayout(); // make sure that there is a layout

    // TODO: deprecate
    applySettingsToDocument(internalDocument());
    connect(m_parentEdit, SIGNAL(textChanged()), SLOT(documentContentChanged()));
    connect(m_parentEdit, SIGNAL(selectionChanged()), SLOT(documentSelectionChanged()));

    m_refreshTimer.setSingleShot(true);
    connect(&m_refreshTimer, SIGNAL(timeout()), SLOT(onRefreshTimeout()));

    m_refreshTimer.start(REFRESH_PERIOD_MSEC);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if(m_stateDirty)
    {
        fullUpdateSettings();
    }

    QPainter p(this);
    p.drawPixmap(0, 0, width(), height(), m_previewPic);

    p.scale(getXScale(), getYScale());

    drawViewportRect(p);
    drawSelections(p);

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
    return QSize(settings().scrollBarWidth, 0);
}
////////////////////////////////////////////////////////////////////////////
QSize CoolScrollBar::minimumSizeHint() const
{
    return QSize(settings().scrollBarWidth, 0);
}
////////////////////////////////////////////////////////////////////////////
const QTextDocument & CoolScrollBar::originalDocument() const
{
    return *m_parentEdit->document();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::documentContentChanged()
{
    internalDocument().setPlainText(originalDocument().toPlainText());

    // Restart update timer: we want refresh to occur only after a pause,
    // not during writing.
    m_refreshTimer.start(REFRESH_PERIOD_MSEC);

// TODO: make optional
//    QTextBlock origBlock = originalDocument().firstBlock();
//    QTextBlock internBlock = internalDocument().firstBlock();
//    while(origBlock.isValid() && internBlock.isValid())
//    {
//        internBlock.layout()->setAdditionalFormats(origBlock.layout()->additionalFormats());
//        origBlock = origBlock.next();
//        internBlock = internBlock.next();
//    }
    //updatePicture();
   // update();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::documentSelectionChanged()
{
    if(m_highlightNextSelection)
    {
        QString selectedStr = m_parentEdit->textCursor().selection().toPlainText();
        if(selectedStr != m_stringToHighlight)
        {
            // clear previous highlight
            clearHighlight();
            m_stringToHighlight = selectedStr;
            // highlight new selection
            highlightSelectedWord();

            updatePicture();
            update();
        }
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::onRefreshTimeout()
{
    updatePicture();
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
    return settings().xDefaultScale;
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::getYScale() const
{
    return settings().yDefaultScale * m_yAdditionalScale;
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawViewportRect(QPainter &p)
{
    qreal lineHeight = calculateLineHeight();
    QPointF rectPos(0, value() * lineHeight);
    QRectF rect(rectPos, QSizeF(settings().scrollBarWidth / getXScale(),
                                linesInViewportCount() * lineHeight));

    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(settings().viewportColor));
    p.drawRect(rect);
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::calculateLineHeight() const
{
    QFontMetrics fm(settings().m_font);
    return qreal(fm.height());
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawPreview(QPainter& p)
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
void CoolScrollBar::highlightEntryInDocument(const QString& str)
{
    if(str.isEmpty())
    {
        return;
    }
    // TODO: optimize using QVector::reserve()
    m_selectionRects.clear();
    QTextCursor cur_cursor(&internalDocument());
    int numIter = 0;
    qreal yScale = getYScale();
    while(true)
    {
        cur_cursor = internalDocument().find(str, cur_cursor);
        if(!cur_cursor.isNull())
        {
            const QTextLayout* layout = cur_cursor.block().layout();

            QTextLine line = layout->lineForTextPosition(cur_cursor.positionInBlock());
            if (!line.isValid())
            {
                continue;
            }

            QRectF selectionRect = line.naturalTextRect();
            
            // calculate bounding rect for selected word
            int blockPos = cur_cursor.block().position();

            qreal leftPercent = qreal(cur_cursor.selectionStart() - blockPos) / line.textLength();
            qreal rightPercent = qreal(cur_cursor.selectionEnd() - blockPos) / line.textLength();

            selectionRect.setLeft(line.naturalTextWidth() * leftPercent);
            selectionRect.setRight(line.naturalTextWidth() * rightPercent);

            selectionRect.translate(layout->position());

            // apply minimum selection height for good visibility on large files
            if((selectionRect.height() * yScale) < settings().m_minSelectionHeight)
            {
                selectionRect.setHeight(settings().m_minSelectionHeight / yScale);
            }

            m_selectionRects.push_back(selectionRect);
        }
        else
        {
            break;
        }
        // prevents UI freezing
        ++numIter;
        if(numIter % 15 == 0)
        {
            qApp->processEvents();
        }
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        setValue(posToScrollValue(event->posF().y()));
        m_leftButtonPressed = true;
    }
    else if(event->button() == Qt::RightButton)
    {
        clearHighlight();
        update();
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::contextMenuEvent(QContextMenuEvent *event)
{
    if(!settings().disableContextMenu)
    {
        QScrollBar::contextMenuEvent(event);
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::mouseMoveEvent(QMouseEvent *event)
{
    if(m_leftButtonPressed)
    {
        setValue(posToScrollValue(event->posF().y()));
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
    documentHeight *= settings().yDefaultScale;

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
int CoolScrollBar::posToScrollValue(qreal pos) const
{
    qreal documentHeight = internalDocument().lineCount() * calculateLineHeight() * getYScale();
    int value = int(pos * (maximum() + linesInViewportCount()) / documentHeight);

    // set center of a viewport to position of click
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
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::highlightSelectedWord()
{
    highlightEntryInDocument(m_stringToHighlight);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::clearHighlight()
{
    m_selectionRects.clear();
    m_stringToHighlight.clear();
    update();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawSelections(QPainter &p)
{
    p.setBrush(settings().selectionHighlightColor);
    p.setPen(Qt::NoPen);

    foreach (QRectF rect, m_selectionRects)
    {
        p.drawRect(
            QRect(
                MARKER_TICK_XPOS * m_coordScalingX,
                rect.top() * m_coordScalingY,
                MARKER_TICK_SIZE,
                MARKER_TICK_SIZE));
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::fullUpdateSettings()
{
    m_stateDirty = false;
    resize(settings().scrollBarWidth, height());
    updateGeometry();
    applySettingsToDocument(internalDocument());
    updatePicture();
    update();
}
