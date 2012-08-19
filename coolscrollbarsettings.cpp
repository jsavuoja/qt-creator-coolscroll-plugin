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
#include "coolscrollbarsettings.h"

namespace
{
    const QString l_nWidth("scrollbar_width");
    const QString l_nFontSize("preferred_font_size");
    const QString l_nViewportColor("viewport_color");
    const QString l_nSelectionColor("selection_color");
    const QString l_nFoldMarkerColor("fold_marker_color");
    const QString l_nInvertViewportColoring("invert_viewport_coloring");
    const QString l_nContextMenu("disable_context_menu");
}

CoolScrollbarSettings::CoolScrollbarSettings() :
    scrollBarWidth(70),
    preferredFontSize(2.0),
    viewportColor(QColor(0, 0, 255, 20)),
    selectionHighlightColor(Qt::red),
    invertViewportColoring(false),
    disableContextMenu(true),
    m_minSelectionHeight(1.5)
{
    m_font.setPointSizeF(preferredFontSize);
    m_font.setStyleHint(QFont::Monospace);
    m_font.setFamily("monospace");
    m_font.setBold(true);
    m_font.setStyleStrategy(QFont::NoAntialias);

    m_textOption.setTabStop(2.0);
}

void CoolScrollbarSettings::save(QSettings *settings)
{
    settings->setValue(l_nWidth, scrollBarWidth);
    settings->setValue(l_nFontSize, preferredFontSize);
    settings->setValue(l_nViewportColor, viewportColor);
    settings->setValue(l_nSelectionColor, selectionHighlightColor);
    settings->setValue(l_nFoldMarkerColor, foldMarkerColor);
    settings->setValue(l_nInvertViewportColoring, invertViewportColoring);
    settings->setValue(l_nContextMenu, disableContextMenu);
}

void CoolScrollbarSettings::read(const QSettings *settings)
{
    scrollBarWidth = settings->value(l_nWidth, scrollBarWidth).toInt();
    preferredFontSize = settings->value(l_nFontSize, preferredFontSize).toDouble();
    viewportColor = settings->value(l_nViewportColor, QVariant(viewportColor)).
                                    value<QColor>();
    selectionHighlightColor = settings->value(l_nSelectionColor,
                                              QVariant(selectionHighlightColor)).
                                              value<QColor>();
    foldMarkerColor = settings->value(l_nFoldMarkerColor, 
                                      QVariant(foldMarkerColor)).value<QColor>();
    invertViewportColoring = settings->value(l_nInvertViewportColoring, invertViewportColoring).toBool();
    disableContextMenu = settings->value(l_nContextMenu, disableContextMenu).toBool();
    
    m_font.setPointSizeF(preferredFontSize);
}
