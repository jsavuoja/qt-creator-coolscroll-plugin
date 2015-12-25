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


#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QtGui/QIcon>
#include <coreplugin/dialogs/ioptionspage.h>
#include "coolscrollbarsettings.h"

class SettingsDialog;

class SettingsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    SettingsPage(QSharedPointer<CoolScrollbarSettings>& settings);
    ~SettingsPage();

    QString id() const;
    QString trName() const;
    QString category() const;
    QString trCategory() const;
    QString displayName() const;
    QIcon categoryIcon() const;
    QString displayCategory() const;
    QWidget *widget() override;
    void apply() override;
    void finish() override;

private:

    QSharedPointer<CoolScrollbarSettings> m_settings;

    SettingsDialog* m_dialog;

signals:

    void settingsChanged();

};

#endif // SETTINGSPAGE_H
