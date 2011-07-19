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

CoolScrollbarSettings::CoolScrollbarSettings() :
    m_scrollBarWidth(70),
    m_viewportColor(QColor(0, 0, 255, 20)),
    m_xDefaultScale(0.9),
    m_yDefaultScale(0.7),
    m_selectionHighlightColor(Qt::red),
    m_disableContextMenu(true)
{
    m_font.setPointSizeF(1.5);
    m_font.setStyleHint(QFont::Monospace);
    m_font.setFamily("inconsolata");

    m_font.setBold(true);
   // m_font.setStretch(QFont::UltraCondensed);
    m_font.setStyleStrategy(QFont::NoAntialias);
    //m_font.setCapitalization(QFont::AllUppercase);

    m_textOption.setTabStop(2.0);
}
