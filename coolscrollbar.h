#ifndef COOLSCROLLAREA_H
#define COOLSCROLLAREA_H

#include <QtGui/QScrollBar>

namespace TextEditor
{
    class BaseTextEditorWidget;
}

class CoolScrollBarSettings;

class CoolScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    CoolScrollBar(TextEditor::BaseTextEditorWidget* edit);

    inline int scrollBarWidth() const { return m_scrollBarWidth; }
    inline void setScrollBarWidth(int w) { m_scrollBarWidth = w; }

    static void setSettings(CoolScrollBarSettings* s) { m_settings = s; }

protected:

    void paintEvent(QPaintEvent *event);

    QSize sizeHint() const { return QSize(scrollBarWidth(), 0); }
    QSize minimumSizeHint() const { return QSize(scrollBarWidth(), 0); }

    int unfoldedLinesCount() const;
    int linesInViewportCount() const;

private:

    TextEditor::BaseTextEditorWidget* m_parentEdit;

    int m_scrollBarWidth;

    static CoolScrollBarSettings* m_settings;

};

#endif // COOLSCROLLAREA_H
