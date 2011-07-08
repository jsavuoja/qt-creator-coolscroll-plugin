#include "coolscrollbar.h"
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTextBlock>
#include <QtGui/QTextLayout>

#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QDebug>

CoolScrollBar::CoolScrollBar(TextEditor::BaseTextEditorWidget* edit)
    : m_parentEdit(edit)
    , m_scrollBarWidth(70)
{
}

void CoolScrollBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);

    p.fillRect(rect(), Qt::white);
    QTextDocument* doc = m_parentEdit->document()->clone();

    QFont font = m_parentEdit->font();
    QFont oldFont = font;

    // fix tabs
    QTextOption opt = doc->defaultTextOption();
    QTextOption oldOpt = opt;
    opt.setTabStop(8.0);
    doc->setDefaultTextOption(opt);

    font.setPointSizeF(1.5);
    font.setFamily("consolas");
    font.setWeight(99);
    font.setCapitalization(QFont::AllUppercase);
    //font.setPixelSize(2);

    doc->setDefaultFont(font);

    //doc->deleteLater();

    QTextBlock block = doc->firstBlock();
    QPointF pos(0, 0);
    while(block.isValid())
    {
        if(block.isVisible())
        {
            block.layout()->draw(&p, pos);
            pos.setY(pos.y() + doc->documentLayout()->blockBoundingRect(block).height());
        }
        block = block.next();
    }

    doc->setDefaultFont(oldFont);
    doc->setDefaultTextOption(oldOpt);


    QFontMetrics fm(font);
    int textHeight = fm.height() * doc->lineCount();
    int h = fm.lineSpacing() + fm.height() + 1;

    QPointF rectPos(0, value() * h);
    QRectF rect(rectPos, QSizeF(scrollBarWidth(),  linesInViewportCount() * h));

    p.setBrush(QBrush(QColor(0, 0, 255, 20)));
    p.drawRect(rect);
    p.end();

    qDebug() << h;

   // doc->deleteLater();
}

int CoolScrollBar::unfoldedLinesCount() const
{
    Q_ASSERT(m_parentEdit);
    int res = 0;
    QTextBlock b = m_parentEdit->document()->firstBlock();
    while(b != m_parentEdit->document()->lastBlock())
    {
        if(b.isVisible())
        {
            res += b.lineCount();
        }
        b = b.next();
    }
    return res;
}

int CoolScrollBar::linesInViewportCount() const
{
    return (2 * m_parentEdit->document()->lineCount() - unfoldedLinesCount() - maximum());
}

int CoolScrollBar::calculateSliderLenght() const
{
    uint valueRange = maximum() - minimum();
    return (pageStep() * height()) / (valueRange + pageStep());
}
