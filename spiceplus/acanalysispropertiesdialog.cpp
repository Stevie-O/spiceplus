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
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>

#include "acanalysispropertiesdialog.h"
#include "schematicview.h"
#include "parameterlineedit.h"
#include "meterselector.h"
#include "acanalysisdialog.h"
#include "groupbox.h"

using namespace Spiceplus;

ACAnalysisPropertiesDialog::ACAnalysisPropertiesDialog(SchematicView *view, QWidget *parent)
    : KDialogBase(parent, 0, true, i18n("AC Analysis"), User1 | Close, User1, false, KGuiItem(i18n("&Plot"), "ok")), m_view(view)
{
    QWidget *w = new QWidget(this);
    setMainWidget(w);
    QBoxLayout *topLayout = new QVBoxLayout(w, 0, KDialog::spacingHint());

    QButtonGroup *typeGroup = new QButtonGroup(1, Qt::Horizontal, i18n("Plot Type"), w);
    typeGroup->layout()->setMargin(KDialog::marginHint());
    typeGroup->layout()->setSpacing(KDialog::spacingHint());
    m_bodeButton = new QRadioButton(i18n("Bode"), typeGroup);
    m_bodeButton->setChecked(true);
    m_nyquistButton = new QRadioButton(i18n("Nyquist"), typeGroup);
    m_linearMagnitudeButton = new QRadioButton(i18n("Linear Magnitude"), typeGroup);
    topLayout->addWidget(typeGroup);

    GroupBox *freqBox = new GroupBox(3, Qt::Horizontal, i18n("Frequency"), w);
    freqBox->layout()->setMargin(KDialog::marginHint());
    freqBox->layout()->setSpacing(KDialog::spacingHint());
    new QLabel(i18n("Start:"), freqBox);
    m_startFrequency = new ParameterLineEdit("10", "10", freqBox);
    new QLabel(i18n("Herz"), freqBox);
    new QLabel(i18n("Stop:"), freqBox);
    m_stopFrequency = new ParameterLineEdit("100meg", "100meg", freqBox);
    new QLabel(i18n("Herz"), freqBox);
    topLayout->addWidget(freqBox);

    GroupBox *outputBox = new GroupBox(0, Qt::Vertical, i18n("Output Variable"), w);
    outputBox->layout()->setMargin(KDialog::marginHint());
    outputBox->layout()->setSpacing(KDialog::spacingHint());
    QGridLayout *outputGrid = new QGridLayout(outputBox->layout(), 2, 4, KDialog::spacingHint());
    m_meterSelector = new MeterSelector(m_view->schematic(), outputBox, outputGrid, 0, 0);
    topLayout->addWidget(outputBox);
}

void ACAnalysisPropertiesDialog::slotUser1()
{
    Meter meter = m_meterSelector->meter();

    if (meter.type() == Meter::Voltmeter && meter.testPoint1() == meter.testPoint2())
    {
        KMessageBox::sorry(this, i18n("Test Points must differ"));
        return;
    }

    AnalysisDialog *dlg;

    if (m_bodeButton->isChecked())
        dlg = new ACAnalysisBodeDialog(m_view, m_startFrequency->text(), m_stopFrequency->text(), meter);
    else if (m_nyquistButton->isChecked())
        dlg = new ACAnalysisNyquistDialog(m_view, m_startFrequency->text(), m_stopFrequency->text(), meter);
    else
        dlg = new ACAnalysisLinearMagnitudeDialog(m_view, m_startFrequency->text(), m_stopFrequency->text(), meter);

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

#include "acanalysispropertiesdialog.moc"

// vim: ts=4 sw=4 et
