#ifndef COOLSCROLLAREA_H
#define COOLSCROLLAREA_H

#include <QtGui/QScrollBar>
#include <QtGui/QPicture>

namespace TextEditor
{
    class BaseTextEditorWidget;
}

class CoolScrollbarSettings;
class QTextDocument;

class CoolScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    CoolScrollBar(TextEditor::BaseTextEditorWidget* edit,
                  QSharedPointer<CoolScrollbarSettings>& settings);

protected:

    void paintEvent(QPaintEvent *event);

    inline const CoolScrollbarSettings& settings() const { return *m_settings; }

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    int unfoldedLinesCount() const;
    int linesInViewportCount() const;
    int calculateLineHeight() const;

    qreal getXScale() const;
    qreal getYScale() const;

    // original document access
    const QTextDocument& originalDocument() const;

    // access to a copy of original document
    QTextDocument& internalDocument();
    const QTextDocument& internalDocument() const;

    void applySettingsToDocument(QTextDocument& doc) const;

    bool eventFilter(QObject *o, QEvent *e);

protected slots:

    void onDocumentContentChanged();
    void onDocumentSelectionChanged();

private:

    void drawPreview(QPainter& p);
    void drawPreview2(QPainter &p);
    void drawViewportRect(QPainter& p);

    TextEditor::BaseTextEditorWidget* m_parentEdit;
    const QSharedPointer<CoolScrollbarSettings> m_settings;
    qreal m_yAdditionalScale;
    QTextDocument* m_internalDocument;

    QString m_stringToHighlight;
    bool m_highlightNextSelection;

};

#endif // COOLSCROLLAREA_H
