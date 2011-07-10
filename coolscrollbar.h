#ifndef COOLSCROLLAREA_H
#define COOLSCROLLAREA_H

#include <QtGui/QScrollBar>

namespace TextEditor
{
    class BaseTextEditorWidget;
}

class CoolScrollbarSettings;

class CoolScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    CoolScrollBar(TextEditor::BaseTextEditorWidget* edit,
                  QSharedPointer<CoolScrollbarSettings>& settings);

    inline int scrollBarWidth() const { return m_scrollBarWidth; }
    inline void setScrollBarWidth(int w) { m_scrollBarWidth = w; }

    inline const CoolScrollbarSettings& settings() const { return *m_settings; }

protected:

    void paintEvent(QPaintEvent *event);

    QSize sizeHint() const { return QSize(scrollBarWidth(), 0); }
    QSize minimumSizeHint() const { return QSize(scrollBarWidth(), 0); }

    int unfoldedLinesCount() const;
    int linesInViewportCount() const;

private:

    TextEditor::BaseTextEditorWidget* m_parentEdit;

    int m_scrollBarWidth;

    const QSharedPointer<CoolScrollbarSettings> m_settings;

};

#endif // COOLSCROLLAREA_H
