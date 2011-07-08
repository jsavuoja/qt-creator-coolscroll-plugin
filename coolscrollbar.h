#ifndef COOLSCROLLAREA_H
#define COOLSCROLLAREA_H

#include <QtGui/QScrollBar>
#include <texteditor/basetexteditor.h>

class CoolScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    CoolScrollBar(TextEditor::BaseTextEditorWidget* edit);

    inline int scrollBarWidth() const { return m_scrollBarWidth; }
    inline void setScrollBarWidth(int w) { m_scrollBarWidth = w; }

protected:

    void paintEvent(QPaintEvent *event);

    QSize sizeHint() const { return QSize(scrollBarWidth(), 0); }
    QSize minimumSizeHint() const { return QSize(scrollBarWidth(), 0); }

    int unfoldedLinesCount() const;
    int linesInViewportCount() const;

    int calculateSliderLenght() const;

private:

    TextEditor::BaseTextEditorWidget* m_parentEdit;

    int m_scrollBarWidth;

};

#endif // COOLSCROLLAREA_H
