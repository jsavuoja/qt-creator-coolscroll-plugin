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
