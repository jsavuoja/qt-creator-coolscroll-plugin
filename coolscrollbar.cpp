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
    m_highlightNextSelection(false),
    m_leftButtonPressed(false),
    m_stateDirty(false),
    m_redrawOnResize(true),
    m_squeezeFactorX(1.0),
    m_squeezeFactorY(1.0)
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

    TextEditor::FontSettings fontSettings =
        TextEditor::TextEditorSettings::instance()->fontSettings();
    p.fillRect(
        rect(),
        fontSettings.formatFor(QLatin1String(TextEditor::Constants::C_TEXT)).background());

    p.drawPixmap(
        MARKER_MARGIN_WIDTH,
        0,
        width() - MARKER_MARGIN_WIDTH,
        height(),
        m_previewPic);

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
void CoolScrollBar::drawViewportRect(QPainter &p)
{
    if (settings().invertViewportColoring)
    {
        drawViewportRect(
            p, 
            0.0f, 
            lineCountToDocumentHeight(value() - 1) * m_squeezeFactorY);
            
        qreal bottomPartStartY = 
            lineCountToDocumentHeight(value() - 1 + linesInViewportCount()) * m_squeezeFactorY;
            
        drawViewportRect(
            p, 
            bottomPartStartY,
            height() - bottomPartStartY);
    }
    else
    {
        drawViewportRect(
            p, 
            lineCountToDocumentHeight(value()) * m_squeezeFactorY, 
            lineCountToDocumentHeight(linesInViewportCount()) * m_squeezeFactorY);
    }
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::drawViewportRect(QPainter& p, qreal startY, qreal sizeY)
{
    QPointF viewportTopLeft(0, startY);
    
    QRectF rect(
        viewportTopLeft,
        QSizeF(
            settings().scrollBarWidth,
            sizeY));

    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(settings().viewportColor));
    p.drawRect(rect);
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::calculateLineHeight() const
{
    QFontMetricsF fm(m_internalDocument->defaultFont());
    return fm.height();
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::lineCountToDocumentHeight(int lineCount) const
{
    QFontMetricsF fm(m_internalDocument->defaultFont());
    
    qreal documentHeight = fm.height() * lineCount;
    qreal lineLeading = fm.leading();
    if (lineCount > 1 && lineLeading > 0.0)
    {
        // Add line spacing.
        documentHeight += (lineCount - 1) * lineLeading;
    }
    return documentHeight;
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::documentHeightVirtual() const
{
    return lineCountToDocumentHeight(m_internalDocument->lineCount());
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::documentHeightScreen() const
{
    return documentHeightVirtual() * m_squeezeFactorY;
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
            if((selectionRect.height() * m_squeezeFactorY) < settings().m_minSelectionHeight)
            {
                selectionRect.setHeight(settings().m_minSelectionHeight / m_squeezeFactorY);
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
    if (!TextEditor::TextEditorSettings::instance())
    {
        return;
    }

    TextEditor::FontSettings fontSettings =
        TextEditor::TextEditorSettings::instance()->fontSettings();
    QFontMetricsF fm(fontSettings.font());
    int lineCount = m_internalDocument->lineCount();

    qreal parentFullHeight = fm.height() * lineCount;
    qreal lineLeading = fm.leading();
    if (lineCount > 1 && lineLeading > 0.0)
    {
        // Add line spacing.
        parentFullHeight += (lineCount - 1) * lineLeading;
    }

    // Ensure that the height is not less than the current scrollbar viewport
    // height.
    //parentFullHeight = std::max<qreal>(parentFullHeight, height());

    // Exclude left-hand side extra area and scroll bar areas from the
    // preview pic.
    int extraAreaWidth = m_parentEdit->extraAreaWidth();
    int scrollBarWidth = width();

    QSize parentFullSize(
        m_parentEdit->width(),
        parentFullHeight);

    QSize oldSize(m_parentEdit->size());
    int scrollValue = value();

    int pixmapHeight = parentFullSize.height();

    qreal referenceDocumentHeight = documentHeightVirtual();
    if (referenceDocumentHeight < height())
    {
        pixmapHeight += pixmapHeight * (height() / referenceDocumentHeight - 1.0);
        m_squeezeFactorY = 1.0f;
    }
    else
    {
        m_squeezeFactorY = qreal(height()) / referenceDocumentHeight;
    }

    // Disallow resize events to recursively call the current method.
    m_redrawOnResize = false;

    m_parentEdit->resize(parentFullSize);

    QPixmap pixmap(
        parentFullSize.width() - extraAreaWidth - scrollBarWidth,
        pixmapHeight); // parentFullSize.height());

    pixmap.fill(
        fontSettings.formatFor(QLatin1String(TextEditor::Constants::C_TEXT)).background());

    m_parentEdit->render(
        &pixmap,
        QPoint(),
        QRegion(
            extraAreaWidth,
            0,
            parentFullSize.width() - extraAreaWidth - scrollBarWidth,
            parentFullSize.height())
    );

    m_parentEdit->resize(oldSize);
    setValue(scrollValue);

    m_redrawOnResize = true;

    m_squeezeFactorX = qreal(width() - MARKER_MARGIN_WIDTH) / qreal(pixmap.width());

    m_previewPic = pixmap.scaled(
        width() - MARKER_MARGIN_WIDTH,
        height(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::resizeEvent(QResizeEvent *)
{
    if (m_redrawOnResize)
    {
        updatePicture();
    }
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::posToScrollValue(qreal pos) const
{
    qreal documentHeight = documentHeightScreen();
    int scrollTick = int(pos * (maximum() + linesInViewportCount()) / documentHeight);

    // set center of a viewport to position of click
    scrollTick -= linesInViewportCount() / 2;
    if(scrollTick > maximum())
    {
        scrollTick = maximum();
    }
    else if (scrollTick < minimum())
    {
        scrollTick = minimum();
    }
    return scrollTick;
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
                MARKER_TICK_XPOS * m_squeezeFactorX,
                rect.top() * m_squeezeFactorY,
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
