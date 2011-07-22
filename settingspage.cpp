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
#include <QtGui/QWidget>
#include "settingspage.h"
#include "settingsdialog.h"

SettingsPage::SettingsPage(QSharedPointer<CoolScrollbarSettings>& settings) :
    m_settings(settings)
{
}
////////////////////////////////////////////////////////////////////////////
SettingsPage::~SettingsPage()
{
}
////////////////////////////////////////////////////////////////////////////
QString SettingsPage::id() const
{
    return "CoolScrollSettings";
}
////////////////////////////////////////////////////////////////////////////
QString SettingsPage::trName() const
{
    return tr("CoolScroll Plugin");
}
////////////////////////////////////////////////////////////////////////////
QString SettingsPage::category() const
{
    return "CoolScroll";
}
////////////////////////////////////////////////////////////////////////////
QString SettingsPage::trCategory() const
{
    return tr("CoolScroll");
}
////////////////////////////////////////////////////////////////////////////
QString SettingsPage::displayName() const
{
    return trName();
}
////////////////////////////////////////////////////////////////////////////
QString SettingsPage::displayCategory() const
{
    return trCategory();
}
////////////////////////////////////////////////////////////////////////////
QIcon SettingsPage::categoryIcon() const
{
    // TODO: need icon
    return QIcon();
}
////////////////////////////////////////////////////////////////////////////
void SettingsPage::apply()
{
    if(m_dialog->isSettingsChanged())
    {
        m_dialog->getSettings(*m_settings);
        emit settingsChanged();
    }
}
////////////////////////////////////////////////////////////////////////////
void SettingsPage::finish()
{
    // nothing
}
////////////////////////////////////////////////////////////////////////////
QWidget * SettingsPage::createPage(QWidget *parent)
{
    m_dialog = new SettingsDialog(parent);
    m_dialog->setSettings(*m_settings);
    return m_dialog;
}
