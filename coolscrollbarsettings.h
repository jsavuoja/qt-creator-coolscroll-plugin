#ifndef COOLSCROLLBARSETTINGS_H
#define COOLSCROLLBARSETTINGS_H

#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QPointF>
#include <QTextOption>

struct CoolScrollbarSettings
{
public:
    CoolScrollbarSettings();

    int m_scrollBarWidth;
    QFont m_font;
    QTextOption m_textOption;
    QColor m_viewportColor;
    qreal m_xDefaultScale;
    qreal m_yDefaultScale;
    QColor m_selectionHighlightColor;
    bool m_disableContextMenu;
    qreal m_minSelectionHeight;

};

#endif // COOLSCROLLBARSETTINGS_H
