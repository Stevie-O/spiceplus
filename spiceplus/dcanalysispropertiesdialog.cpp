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
#include <qcombobox.h>
#include <qcheckbox.h>

#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>

#include "dcanalysispropertiesdialog.h"
#include "schematic.h"
#include "schematicview.h"
#include "parameterlineedit.h"
#include "meterselector.h"
#include "dcanalysisdialog.h"
#include "groupbox.h"

using namespace Spiceplus;

DCAnalysisPropertiesDialog::DCAnalysisPropertiesDialog(SchematicView *view, QWidget *parent)
    : KDialogBase(parent, 0, true, i18n("DC Analysis"), User1 | Close, User1, false, KGuiItem(i18n("&Plot"), "ok")), m_view(view)
{
    QStringList sources = m_view->schematic()->deviceNamesByType("v") + m_view->schematic()->deviceNamesByType("i");

    QWidget *w = new QWidget(this);
    setMainWidget(w);
    QBoxLayout *topLayout = new QVBoxLayout(w, 0, KDialog::spacingHint());

    GroupBox *srcBox = new GroupBox(0, Qt::Vertical, i18n("Source Settings"), w);
    srcBox->layout()->setMargin(KDialog::marginHint());
    srcBox->layout()->setSpacing(KDialog::spacingHint());

    QGridLayout *srcGrid = new QGridLayout(srcBox->layout(), 9, 3, KDialog::spacingHint());
    srcGrid->addWidget(new QLabel(i18n("Source:"), srcBox), 0, 0);
    m_sourceName = new QComboBox(srcBox);
    m_sourceName->insertStringList(sources);
    srcGrid->addWidget(m_sourceName, 0, 1);

    srcGrid->addWidget(new QLabel(i18n("Starting Value:"), srcBox), 1, 0);
    srcGrid->addWidget(m_startingValue = new ParameterLineEdit("0", "0", srcBox), 1, 1);
    srcGrid->addWidget(new QLabel(i18n("Volts / Amps"), srcBox), 1, 2);

    srcGrid->addWidget(new QLabel(i18n("Final Value:"), srcBox), 2, 0);
    srcGrid->addWidget(m_finalValue = new ParameterLineEdit("10", "0", srcBox), 2, 1);
    srcGrid->addWidget(new QLabel(i18n("Volts / Amps"), srcBox), 2, 2);

    srcGrid->addWidget(new QLabel(i18n("Incrementing Value:"), srcBox), 3, 0);
    srcGrid->addWidget(m_incrementingValue = new ParameterLineEdit("0.1", "0", srcBox), 3, 1);
    srcGrid->addWidget(new QLabel(i18n("Volts / Amps"), srcBox), 3, 2);

    srcGrid->addMultiCellWidget(m_useSource2 = new QCheckBox(i18n("Use second source:"), srcBox), 4, 4, 0, 2);
    if (sources.count() < 2)
        m_useSource2->setEnabled(false);

    QLabel *label;

    label = new QLabel(i18n("Source:"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 5, 0);
    m_sourceName2 = new QComboBox(srcBox);
    m_sourceName2->setEnabled(false);
    m_sourceName2->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    if (sources.count() > 1)
    {
        m_sourceName2->insertStringList(sources);
        m_sourceName2->setCurrentItem(1);
    }
    srcGrid->addWidget(m_sourceName2, 5, 1);

    label = new QLabel(i18n("Starting Value:"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 6, 0);
    srcGrid->addWidget(m_startingValue2 = new ParameterLineEdit("0", "0", srcBox), 6, 1);
    m_startingValue2->setEnabled(false);
    m_startingValue2->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    label = new QLabel(i18n("Volts / Amps"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 6, 2);

    label = new QLabel(i18n("Final Value:"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 7, 0);
    srcGrid->addWidget(m_finalValue2 = new ParameterLineEdit("10", "0", srcBox), 7, 1);
    m_finalValue2->setEnabled(false);
    m_finalValue2->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    label = new QLabel(i18n("Volts / Amps"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 7, 2);

    label = new QLabel(i18n("Incrementing Value:"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 8, 0);
    srcGrid->addWidget(m_incrementingValue2 = new ParameterLineEdit("1", "0", srcBox), 8, 1);
    m_incrementingValue2->setEnabled(false);
    m_incrementingValue2->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    label = new QLabel(i18n("Volts / Amps"), srcBox);
    label->setEnabled(false);
    label->connect(m_useSource2, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    srcGrid->addWidget(label, 8, 2);

    topLayout->addWidget(srcBox);

    GroupBox *outputBox = new GroupBox(0, Qt::Vertical, i18n("Output Variable"), w);
    outputBox->layout()->setMargin(KDialog::marginHint());
    outputBox->layout()->setSpacing(KDialog::spacingHint());
    QGridLayout *outputGrid = new QGridLayout(outputBox->layout(), 2, 4, KDialog::spacingHint());
    m_meterSelector = new MeterSelector(m_view->schematic(), outputBox, outputGrid, 0, 0);
    topLayout->addWidget(outputBox);
}

void DCAnalysisPropertiesDialog::slotUser1()
{
    Meter meter = m_meterSelector->meter();

    if (meter.type() == Meter::Voltmeter && meter.testPoint1() == meter.testPoint2())
    {
        KMessageBox::sorry(this, i18n("Test Points must differ"));
        return;
    }

    DCAnalysisDialog *dlg;
    if (m_useSource2->isChecked())
        dlg = new DCAnalysisDialog(m_view,
                                   meter,
                                   m_sourceName->currentText(),
                                   m_startingValue->text(),
                                   m_finalValue->text(),
                                   m_incrementingValue->text(),
                                   m_sourceName2->currentText(),
                                   m_startingValue2->text(),
                                   m_finalValue2->text(),
                                   m_incrementingValue2->text());
    else
        dlg = new DCAnalysisDialog(m_view,
                                   meter,
                                   m_sourceName->currentText(),
                                   m_startingValue->text(),
                                   m_finalValue->text(),
                                   m_incrementingValue->text());

    if (!dlg->runAnalysis())
    {
        KMessageBox::error(this, dlg->errorString());
        delete dlg;
        dlg = 0;
    }
    else
    {
        dlg->show();
        done(User1);
    }
}

#include "dcanalysispropertiesdialog.moc"

// vim: ts=4 sw=4 et
