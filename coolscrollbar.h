#ifndef COOLSCROLLAREA_H
#define COOLSCROLLAREA_H

#include <QtGui/QScrollBar>

class QPlainTextEdit;

class CoolScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    CoolScrollBar(QPlainTextEdit* edit);

protected:

    void paintEvent(QPaintEvent *event) {}

private:

    QPlainTextEdit* m_parentEdit;

};

#endif // COOLSCROLLAREA_H
