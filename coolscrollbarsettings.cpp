#include "coolscrollbarsettings.h"

CoolScrollbarSettings::CoolScrollbarSettings() :
    m_scrollBarWidth(70),
    m_viewportColor(QColor(0, 0, 255, 20)),
    m_xDefaultScale(0.7),
    m_yDefaultScale(1.0),
    m_selectionHighlightColor(Qt::red),
    m_disableContextMenu(true)
{
    m_font.setPointSizeF(1.5);
    m_font.setStyleHint(QFont::Monospace);
    m_font.setFamily("inconsolata");
    m_font.setWeight(99);
    m_font.setCapitalization(QFont::AllUppercase);

    m_textOption.setTabStop(2.0);
}
