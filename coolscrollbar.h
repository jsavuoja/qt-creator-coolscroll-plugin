#ifndef COOLSCROLLAREA_H
#define COOLSCROLLAREA_H

#include <QtGui/QScrollBar>
#include <QtGui/QPixmap>
#include <QTextCharFormat>

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
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    inline const CoolScrollbarSettings& settings() const { return *m_settings; }

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    int unfoldedLinesCount() const;
    int linesInViewportCount() const;
    qreal calculateLineHeight() const;

    qreal getXScale() const;
    qreal getYScale() const;

    // original document access
    const QTextDocument& originalDocument() const;

    // access to a copy of original document
    inline QTextDocument& internalDocument() { return *m_internalDocument; }
    inline const QTextDocument& internalDocument() const { return *m_internalDocument; }

    void applySettingsToDocument(QTextDocument& doc) const;

    bool eventFilter(QObject *obj, QEvent *e);

protected slots:

    void onDocumentContentChanged();
    void onDocumentSelectionChanged();

private:

    void drawPreview(QPainter& p);
    void drawSelections(QPainter& p);
    void drawViewportRect(QPainter& p);

    void updatePicture();

    void updateScaleFactors();

    int posToValue(qreal pos) const;

    void highlightSelectedWord();
    void clearHighlight();
    void highlightEntryInDocument(const QString& str, const QTextCharFormat& format);

    TextEditor::BaseTextEditorWidget* m_parentEdit;
    const QSharedPointer<CoolScrollbarSettings> m_settings;
    qreal m_yAdditionalScale;
    QTextDocument* m_internalDocument;

    QPixmap m_previewPic;

    QString m_stringToHighlight;
    bool m_highlightNextSelection;

    bool m_leftButtonPressed;

    QVector<QRectF> m_selectionRects;

};

#endif // COOLSCROLLAREA_H
