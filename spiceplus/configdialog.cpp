/*
 * SPICE+
 * Copyright (C) 2004 Andreas Unger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <qtabwidget.h>

#include <kdialog.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kfontrequester.h>
#include <kurlrequester.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include "configdialog.h"
#include "groupbox.h"
#include "settings.h"
#include "symboldirsedit.h"
#include "modeldirsedit.h"

using namespace Spiceplus;

//
// ConfigDialog
//

ConfigDialog::ConfigDialog(QWidget *parent, const char *name)
    : KConfigDialog(parent, name, Settings::self())
{
    addPage(new ConfigDialogPageSchematicEditor, i18n("Schematic Editor"), "misc_doc", i18n("Customize the schematic editor"));
    addPage(new ConfigDialogPageAnalysis, i18n("Analysis"), "misc_doc", i18n("Analysis settings"));
    addPage(m_pathsPage = new ConfigDialogPagePaths, i18n("Paths"), "misc_doc", i18n("Configure paths"));

    connect(m_pathsPage, SIGNAL(widgetModified()), SIGNAL(widgetModified()));
    connect(m_pathsPage, SIGNAL(widgetModified()), SLOT(updateButtons()));
    connect(m_pathsPage, SIGNAL(settingsChanged()), SIGNAL(settingsChanged()));
    connect(m_pathsPage, SIGNAL(settingsChanged()), SLOT(settingsChangedSlot()));
    connect(this, SIGNAL(settingsChanged()), Settings::self(), SLOT(emitSettingsChanged()));
}

void ConfigDialog::updateSettings()
{
    KConfigDialog::updateSettings();
    m_pathsPage->updateSettings();
}

void ConfigDialog::updateWidgets()
{
    KConfigDialog::updateWidgets();
    m_pathsPage->updateWidgets();
}

bool ConfigDialog::hasChanged()
{
    return KConfigDialog::hasChanged() || m_pathsPage->hasChanged();
}

//
// ConfigDialogPageSchematicEditor
//

ConfigDialogPageSchematicEditor::ConfigDialogPageSchematicEditor()
{
    QBoxLayout *vbox = new QVBoxLayout(this, 0, KDialog::spacingHint());

    QGridLayout *grid = new QGridLayout(vbox, 5, 3, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Background color:"), this), 0, 0);
    grid->addWidget(new KColorButton(this, "kcfg_SchematicBackgroundColor"), 0, 1);
    grid->addWidget(new QLabel(i18n("Grid size:"), this), 1, 0);
    QSpinBox *spinBox = new QSpinBox(10, 20, 10, this, "kcfg_GridSize");
    spinBox->setValidator(new QRegExpValidator(QRegExp("10|20"), spinBox));
    grid->addWidget(spinBox, 1, 1);
    grid->addWidget(new QLabel(i18n("pixels"), this), 1, 2);

    QCheckBox *checkBox = new QCheckBox(i18n("Visible grid"), this, "kcfg_IsGridVisible");
    grid->addWidget(checkBox, 2, 0);

    QBoxLayout *hbox = new QHBoxLayout(KDialog::spacingHint());
    hbox->addSpacing(20);
    QLabel *label = new QLabel(i18n("Grid color:"), this);
    label->setEnabled(checkBox->isChecked());
    label->connect(checkBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    hbox->addWidget(label);
    grid->addLayout(hbox, 3, 0);

    KColorButton *colorButton = new KColorButton(this, "kcfg_GridColor");
    colorButton->setEnabled(checkBox->isChecked());
    colorButton->connect(checkBox, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    grid->addWidget(colorButton, 3, 1);

    grid->addWidget(new QLabel(i18n("Device rotation angle:"), this), 4, 0);
    spinBox = new QSpinBox(45, 90, 45, this, "kcfg_DeviceRotationAngle");
    spinBox->setValidator(new QRegExpValidator(QRegExp("45|90"), spinBox));
    grid->addWidget(spinBox, 4, 1);
    grid->addWidget(new QLabel(i18n("degrees"), this), 4, 2);

    QTabWidget *tab = new QTabWidget(this);
    
    QWidget *colorTab = new QWidget(tab);
    QBoxLayout *colorLayout = new QVBoxLayout(colorTab, KDialog::marginHint(), KDialog::spacingHint());
    grid = new QGridLayout(colorLayout, 4, 4, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Device color, normal:"), colorTab), 0, 0);
    grid->addWidget(new KColorButton(colorTab, "kcfg_DeviceColor"), 0, 1);
    grid->addWidget(new QLabel(i18n(", highlighted:"), colorTab), 0, 2);
    grid->addWidget(new KColorButton(colorTab, "kcfg_DeviceColorHighlighted"), 0, 3);
    grid->addWidget(new QLabel(i18n("Wire color, normal:"), colorTab), 1, 0);
    grid->addWidget(new KColorButton(colorTab, "kcfg_WireColor"), 1, 1);
    grid->addWidget(new QLabel(i18n(", highlighted:"), colorTab), 1, 2);
    grid->addWidget(new KColorButton(colorTab, "kcfg_WireColorHighlighted"), 1, 3);
    grid->addWidget(new QLabel(i18n("Selection mark color:"), colorTab), 2, 0);
    grid->addWidget(new KColorButton(colorTab, "kcfg_SelectionMarkColor"), 2, 1);
    grid->addWidget(new QLabel(i18n("Connection mark color, normal:"), colorTab), 3, 0);
    grid->addWidget(new KColorButton(colorTab, "kcfg_ConnectionMarkColor"), 3, 1);
    grid->addWidget(new QLabel(i18n(", redundant:"), colorTab), 3, 2);
    grid->addWidget(new KColorButton(colorTab, "kcfg_ConnectionMarkColorRedundant"), 3, 3);
    colorLayout->addStretch();
    tab->addTab(colorTab, i18n("Symbol &Colors"));

    QWidget *fontTab = new QWidget(tab);
    QBoxLayout *fontLayout = new QVBoxLayout(fontTab, KDialog::marginHint(), KDialog::spacingHint());
    grid = new QGridLayout(fontLayout, 5, 2, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Tiny font:"), fontTab), 0, 0);
    grid->addWidget(new KFontRequester(fontTab, "kcfg_TinySymbolFont"), 0, 1);
    grid->addWidget(new QLabel(i18n("Small font:"), fontTab), 1, 0);
    grid->addWidget(new KFontRequester(fontTab, "kcfg_SmallSymbolFont"), 1, 1);
    grid->addWidget(new QLabel(i18n("Normal font:"), fontTab), 2, 0);
    grid->addWidget(new KFontRequester(fontTab, "kcfg_NormalSymbolFont"), 2, 1);
    grid->addWidget(new QLabel(i18n("Large font:"), fontTab), 3, 0);
    grid->addWidget(new KFontRequester(fontTab, "kcfg_LargeSymbolFont"), 3, 1);
    grid->addWidget(new QLabel(i18n("Huge font:"), fontTab), 4, 0);
    grid->addWidget(new KFontRequester(fontTab, "kcfg_HugeSymbolFont"), 4, 1);
    fontLayout->addStretch();
    tab->addTab(fontTab, i18n("Symbol &Fonts"));

    vbox->addWidget(tab);
    vbox->addStretch();
}

//
// ConfigDialogPageAnalysis
//

ConfigDialogPageAnalysis::ConfigDialogPageAnalysis()
{
    QBoxLayout *vbox = new QVBoxLayout(this, 0, KDialog::spacingHint());

    GroupBox *group = new GroupBox(0, Qt::Vertical, i18n("AC Analysis"), this);
    QGridLayout *grid = new QGridLayout(group->layout(), 1, 2, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Number of points per decade:"), group), 0, 0);
    grid->addWidget(new QSpinBox(1, 999999, 1, group, "kcfg_ACAnalysisNumPointsPerDecade"), 0, 1);
    vbox->addWidget(group);

    vbox->addStretch();
}

//
// ConfigDialogPagePaths
//

ConfigDialogPagePaths::ConfigDialogPagePaths()
{
    QBoxLayout *vbox = new QVBoxLayout(this, 0, KDialog::spacingHint());

    QGridLayout *grid = new QGridLayout(vbox, 3, 2, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Path to SPICE executable:"), this), 0, 0);
    grid->addWidget(new KURLRequester(this, "kcfg_SpiceExecutablePath"), 0, 1);
    grid->addWidget(new QLabel(i18n("Device directory:"), this), 1, 0);
    KURLRequester *req = new KURLRequester(this, "kcfg_DeviceDir");
    req->setMode(KFile::Directory);
    grid->addWidget(req, 1, 1);
    grid->addWidget(new QLabel(i18n("Model View directory:"), this), 2, 0);
    req = new KURLRequester(this, "kcfg_ModelViewDir");
    req->setMode(KFile::Directory);
    grid->addWidget(req, 2, 1);

    QTabWidget *tab = new QTabWidget(this);
    
    QWidget *symbolTab = new QWidget(tab);
    grid = new QGridLayout(symbolTab, 3, 2, KDialog::marginHint(), KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Symbol directory:"), symbolTab), 0, 0);
    req = new KURLRequester(symbolTab, "kcfg_StandardSymbolDir");
    req->setMode(KFile::Directory);
    grid->addWidget(req, 0, 1);
    grid->addMultiCellWidget(new QLabel(i18n("User defined symbol directories:"), symbolTab), 1, 1, 0, 1);
    m_symbolDirsEdit = new SymbolDirsEdit(symbolTab);
    connect(m_symbolDirsEdit, SIGNAL(modified()), SIGNAL(widgetModified()));
    grid->addMultiCellWidget(m_symbolDirsEdit, 2, 2, 0, 1);
    tab->addTab(symbolTab, i18n("&Symbols"));

    QWidget *modelTab = new QWidget(tab);
    grid = new QGridLayout(modelTab, 3, 2, KDialog::marginHint(), KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Model directory:"), modelTab), 0, 0);
    req = new KURLRequester(modelTab, "kcfg_StandardModelDir");
    req->setMode(KFile::Directory);
    grid->addWidget(req, 0, 1);
    grid->addMultiCellWidget(new QLabel(i18n("User defined model directories:"), modelTab), 1, 1, 0, 1);
    m_modelDirsEdit = new ModelDirsEdit(modelTab);
    connect(m_modelDirsEdit, SIGNAL(modified()), SIGNAL(widgetModified()));
    grid->addMultiCellWidget(m_modelDirsEdit, 2, 2, 0, 1);
    tab->addTab(modelTab, i18n("&Models"));

    vbox->addWidget(tab);
    vbox->addStretch();
}

void ConfigDialogPagePaths::updateSettings()
{
    Settings::self()->setUserSymbolDirs(m_symbolDirsEdit->dirs());
    Settings::self()->setUserModelDirs(m_modelDirsEdit->dirs());
    emit settingsChanged();
}

void ConfigDialogPagePaths::updateWidgets()
{
    m_symbolDirsEdit->setDirs(Settings::self()->userSymbolDirs());
    m_modelDirsEdit->setDirs(Settings::self()->userModelDirs());
}

bool ConfigDialogPagePaths::hasChanged()
{
    return Settings::self()->userSymbolDirs() != m_symbolDirsEdit->dirs() || Settings::self()->userModelDirs() != m_modelDirsEdit->dirs();
}

#include "configdialog.moc"

// vim: ts=4 sw=4 et
