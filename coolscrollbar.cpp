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
#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QTextBlock>
#include <QtGui/QTextLayout>
#include <QtGui/QTextDocumentFragment>

#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

#include <texteditor/texteditor.h>
#include <texteditor/textdocumentlayout.h>
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
CoolScrollBar::CoolScrollBar(TextEditor::TextEditorWidget* edit,
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

    connect(m_parentEdit, &QPlainTextEdit::textChanged, this, &CoolScrollBar::onDocumentContentChanged);
    connect(m_parentEdit, &QPlainTextEdit::selectionChanged, this, &CoolScrollBar::onDocumentSelectionChanged);

    // Catch notifications about code folding.
    connect(
        m_parentEdit->document()->documentLayout(), 
        &QAbstractTextDocumentLayout::documentSizeChanged,
        this, &CoolScrollBar::onDocumentSizeChanged);

    m_refreshTimer.setSingleShot(true);
    connect(&m_refreshTimer, &QTimer::timeout, this, &CoolScrollBar::onRefreshTimeout);

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
        fontSettings.formatFor(TextEditor::C_TEXT).background());

    p.drawPixmap(
        MARKER_MARGIN_WIDTH,
        0,
        width() - MARKER_MARGIN_WIDTH,
        height(),
        m_previewPic);

    drawFoldingMarkers(p);
    drawViewportRect(p);
    drawSelections(p);

    p.end();
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::unfoldedLinesCount() const
{
    Q_ASSERT(m_parentEdit);
    int lineCount = 0;
    QTextBlock textBlock = originalDocument().firstBlock();
    while (textBlock.isValid())
    {
        if (!textBlock.userData() ||
            !static_cast<TextEditor::TextBlockUserData*>(textBlock.userData())->folded())
        {
            lineCount += textBlock.lineCount();
        }
        else
        {
            // Folder rows are represented with a single folded marker row.
            lineCount += 1;
        }
        textBlock = textBlock.next();
    }
    return lineCount;
}
////////////////////////////////////////////////////////////////////////////
int CoolScrollBar::linesInViewportCount() const
{
    return unfoldedLinesCount() - maximum();
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
const QTextDocument& CoolScrollBar::originalDocument() const
{
    return *m_parentEdit->document();
}
////////////////////////////////////////////////////////////////////////////
QTextDocument& CoolScrollBar::originalDocument()
{
    return *m_parentEdit->document();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::onDocumentContentChanged()
{
    // Restart update timer: we want refresh to occur only after a pause,
    // not during writing.
    restartDeferredUpdate();
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::restartDeferredUpdate()
{
    // Restart update timer: we want refresh to occur only after a pause,
    // not during writing.
    m_refreshTimer.start(REFRESH_PERIOD_MSEC);
}
////////////////////////////////////////////////////////////////////////////
void CoolScrollBar::onDocumentSelectionChanged()
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
void CoolScrollBar::onDocumentSizeChanged(const QSizeF& newSize)
{
    Q_UNUSED(newSize);
    restartDeferredUpdate();
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
void CoolScrollBar::drawFoldingMarkers(QPainter& p)
{
    QTextBlock textBlock = originalDocument().firstBlock();
    
    QBrush markerBrush(settings().foldMarkerColor);
    p.setBrush(markerBrush);
    
    QPen pen(markerBrush, 1);
    p.setPen(pen);
    
    while (textBlock.isValid())
    {
        if (textBlock.userData() &&
            static_cast<TextEditor::TextBlockUserData*>(textBlock.userData())->folded() &&
            textBlock.lineCount() > 0)   // < skip folds under other folds
        {
            // This row is a fold point.
            qreal yPos = lineCountToDocumentHeight(textBlock.firstLineNumber()) * m_squeezeFactorY;
            
            QPainterPath markerPath(
                QPointF(width() - MARKER_MARGIN_WIDTH - 6.0f, yPos));
            markerPath.lineTo(
                width() - MARKER_MARGIN_WIDTH, yPos);
            markerPath.lineTo(
                width() - MARKER_MARGIN_WIDTH, yPos + 6.0f);
            
            p.drawPath(markerPath);
        }
        textBlock = textBlock.next();
    }
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::calculateLineHeight() const
{
    QFontMetricsF fm(settings().m_font);
    return fm.height();
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::lineCountToDocumentHeight(int lineCount) const
{
    QFontMetricsF fm(settings().m_font);
    
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
    return lineCountToDocumentHeight(m_parentEdit->document()->lineCount());
}
////////////////////////////////////////////////////////////////////////////
qreal CoolScrollBar::documentHeightScreen() const
{
    return documentHeightVirtual() * m_squeezeFactorY;
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
    QTextCursor selectionCursor(&originalDocument());
    int numIter = 0;
    while (true)
    {
        selectionCursor = originalDocument().find(str, selectionCursor);
        if(!selectionCursor.isNull())
        {
            QTextLayout* layout = selectionCursor.block().layout();

            QTextLine line = layout->lineForTextPosition(selectionCursor.positionInBlock());
            if (!line.isValid())
            {
                continue;
            }

            QRectF selectionRect(
                0.0f,
                lineCountToDocumentHeight(selectionCursor.block().firstLineNumber() + line.lineNumber() + 1),
                0.0f,
                0.0f);

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
        setValue(posToScrollValue(event->localPos().y()));
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
        setValue(posToScrollValue(event->localPos().y()));
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
    int lineCount = unfoldedLinesCount();

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
        fontSettings.formatFor(TextEditor::C_TEXT).background());

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
    updatePicture();
    update();
}
