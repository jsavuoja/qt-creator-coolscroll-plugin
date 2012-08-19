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

#include <QtGui/QColorDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog),
    m_settingsChanged(false)
{
    ui->setupUi(this);

    ui->widthSpinBox->setRange(5, 400);
    ui->fontSizeSpinBox->setRange(1.0, 5.0);
    ui->fontSizeSpinBox->setSingleStep(0.5);

    connect(ui->viewportColorButton, SIGNAL(clicked()),
                                     SLOT(colorSettingsButtonClicked()));
    connect(ui->selectionColorButton, SIGNAL(clicked()),
                                      SLOT(colorSettingsButtonClicked()));
    connect(ui->foldMarkerColorButton, SIGNAL(clicked()),
                                       SLOT(colorSettingsButtonClicked()));

    connect(ui->widthSpinBox, SIGNAL(valueChanged(int)), SLOT(settingsChanged()));
    connect(ui->fontSizeSpinBox, SIGNAL(valueChanged(double)), SLOT(settingsChanged()));
    connect(ui->invertViewportColoring, SIGNAL(stateChanged(int)), SLOT(settingsChanged()));
    connect(ui->contextMenuCheckBox, SIGNAL(stateChanged(int)), SLOT(settingsChanged()));
}
////////////////////////////////////////////////////////////////////////////
SettingsDialog::~SettingsDialog()
{
    delete ui;
}
////////////////////////////////////////////////////////////////////////////
void SettingsDialog::setSettings(const CoolScrollbarSettings &settings)
{
    ui->widthSpinBox->setValue(settings.scrollBarWidth);
    ui->fontSizeSpinBox->setValue(settings.preferredFontSize);
    setButtonColor(ui->viewportColorButton, settings.viewportColor);
    setButtonColor(ui->selectionColorButton, settings.selectionHighlightColor);
    setButtonColor(ui->foldMarkerColorButton, settings.foldMarkerColor);
    ui->invertViewportColoring->setChecked(settings.invertViewportColoring);
    ui->contextMenuCheckBox->setChecked(!settings.disableContextMenu);
}
////////////////////////////////////////////////////////////////////////////
void SettingsDialog::colorSettingsButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    Q_ASSERT(button != 0);
    QColorDialog dialog;
    dialog.setOption(QColorDialog::ShowAlphaChannel, true);
    dialog.setCurrentColor(getButtonColor(button));
    if(dialog.exec() == QDialog::Accepted)
    {
        if(getButtonColor(button) != dialog.selectedColor())
        {
            setButtonColor(button, dialog.selectedColor());
            settingsChanged();
        }
    }
}
////////////////////////////////////////////////////////////////////////////
void SettingsDialog::setButtonColor(QPushButton *button, const QColor &color)
{
    QPalette p;
    p.setBrush(QPalette::Button, color);
    button->setPalette(p);
}
////////////////////////////////////////////////////////////////////////////
QColor SettingsDialog::getButtonColor(QPushButton *button)
{
    return button->palette().button().color();
}
////////////////////////////////////////////////////////////////////////////
void SettingsDialog::getSettings(CoolScrollbarSettings &settings) const
{
    settings.scrollBarWidth = ui->widthSpinBox->value();
    settings.preferredFontSize = ui->fontSizeSpinBox->value();
    settings.viewportColor = getButtonColor(ui->viewportColorButton);
    settings.selectionHighlightColor = getButtonColor(ui->selectionColorButton);
    settings.foldMarkerColor = getButtonColor(ui->foldMarkerColorButton);
    settings.invertViewportColoring = ui->invertViewportColoring->isChecked();
    settings.disableContextMenu = !ui->contextMenuCheckBox->isChecked();
}
////////////////////////////////////////////////////////////////////////////
void SettingsDialog::settingsChanged()
{
    m_settingsChanged = true;
}
