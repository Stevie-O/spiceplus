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
#include <qpushbutton.h>

#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kseparator.h>

#include "analysisdialog.h"
#include "schematicview.h"
#include "spiceprocess.h"

using namespace Spiceplus;

AnalysisDialog::AnalysisDialog(SchematicView *view, QBoxLayout::Direction plotLayoutDirection)
    : m_view(view)
{
    resize(560, 420);

    connect(m_view, SIGNAL(destroyed()), SLOT(close()));

    m_spiceProcess = new SpiceProcess(this);
    connect(m_spiceProcess, SIGNAL(analysisFailed(const QString &, const QString &)), SLOT(displayErrorMessage(const QString &, const QString &)));
    connect(m_spiceProcess, SIGNAL(analysisFinished(const QValueVector<QMemArray<double> > &)), SLOT(plotData(const QValueVector<QMemArray<double> > &)));

    QWidget *w = new QWidget(this);
    setCentralWidget(w);
    QBoxLayout *vbox = new QVBoxLayout(w, KDialog::marginHint(), KDialog::spacingHint());

    m_plotLayout = new QBoxLayout(vbox, plotLayoutDirection, KDialog::spacingHint());

    vbox->addWidget(new KSeparator(KSeparator::HLine, w));

    QBoxLayout *hbox = new QHBoxLayout(vbox, KDialog::spacingHint());
    hbox->addStretch();

    QPushButton *updateButton = new QPushButton(i18n("Update"), w);
    connect(updateButton, SIGNAL(clicked()), SLOT(updatePlot()));
    hbox->addWidget(updateButton);
}

void AnalysisDialog::displayErrorMessage(const QString &errorString, const QString &errorDetails)
{
    if (errorDetails.isNull())
        KMessageBox::error(this, errorString);
    else
        KMessageBox::detailedError(this, errorString, errorDetails);
}

void AnalysisDialog::updatePlot()
{
    if (m_spiceProcess->isRunning())
        return;

    m_view->resetTool();

    if (!runAnalysis())
        KMessageBox::error(this, m_errorString);
}

#include "analysisdialog.moc"

// vim: ts=4 sw=4 et
