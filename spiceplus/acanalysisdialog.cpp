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

#include <klocale.h>

#include "acanalysisdialog.h"
#include "schematic.h"
#include "schematicview.h"
#include "plot.h"
#include "settings.h"
#include "spiceprocess.h"

using namespace Spiceplus;

//
// ACAnalysisBodeDialog
//

ACAnalysisBodeDialog::ACAnalysisBodeDialog(SchematicView *view, const QString &startFrequency, const QString &stopFrequency, const Meter &meter)
    : AnalysisDialog(view),
      m_startFrequency(startFrequency),
      m_stopFrequency(stopFrequency),
      m_meter(meter),
      m_magnitudeCurve(0),
      m_phaseCurve(0)
{
    setCaption(i18n("Bode Plot %1").arg(m_meter.name()));

    m_magnitudePlot = new Plot(centralWidget());
    m_magnitudePlot->setAxisOptions(QwtPlot::xBottom, QwtAutoScale::Logarithmic);
    m_magnitudePlot->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    m_magnitudePlot->setAxisTitle(QwtPlot::yLeft, "Magnitude [dB]");
    m_plotLayout->addWidget(m_magnitudePlot);
    m_phasePlot = new Plot(centralWidget());
    m_phasePlot->setAxisOptions(QwtPlot::xBottom, QwtAutoScale::Logarithmic);
    m_phasePlot->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    m_phasePlot->setAxisTitle(QwtPlot::yLeft, "Phase [deg]");
    m_plotLayout->addWidget(m_phasePlot);
}

bool ACAnalysisBodeDialog::runAnalysis()
{
    QString cmdList = m_view->schematic()->createCommandList();
    if (cmdList.isNull())
    {
        m_errorString = m_view->schematic()->errorString();
        return false;
    }

    QString meterCmd = m_meter.createCommand(m_view->schematic());
    if (meterCmd.isNull())
    {
        m_errorString = m_meter.errorString();
        return false;
    }

    cmdList += ".control\n"
               "set nobreak\n"
               "set units=degrees\n"
               ".endc\n"
               ".ac dec " + QString::number(Settings::self()->acAnalysisNumPointsPerDecade()) + " "
                          + m_startFrequency + " " + m_stopFrequency + "\n"
               ".print ac db(" + meterCmd + ") ph(" + meterCmd + ")\n"
               ".end\n";

    if (!m_spiceProcess->start(cmdList, 5))
    {
        m_errorString = m_spiceProcess->errorString();
        return false;
    }

    return true;
}


void ACAnalysisBodeDialog::plotData(const QValueVector<QMemArray<double> > &table)
{
    if (!m_magnitudeCurve)
    {
        m_magnitudeCurve = new QwtPlotCurve(m_magnitudePlot);
        m_magnitudeCurve->setPen(Qt::red);
        m_magnitudePlot->insertCurve(m_magnitudeCurve);
    }
    m_magnitudeCurve->setData(table[1], table[3]);

    if (!m_phaseCurve)
    {
        m_phaseCurve = new QwtPlotCurve(m_phasePlot);
        m_phaseCurve->setPen(Qt::red);
        m_phasePlot->insertCurve(m_phaseCurve);
    }
    m_phaseCurve->setData(table[1], table[4]);

    m_magnitudePlot->replot();
    m_phasePlot->replot();
}

//
// ACAnalysisNyquistDialog
//

ACAnalysisNyquistDialog::ACAnalysisNyquistDialog(SchematicView *view, const QString &startFrequency, const QString &stopFrequency, const Meter &meter)
    : AnalysisDialog(view),
      m_startFrequency(startFrequency),
      m_stopFrequency(stopFrequency),
      m_meter(meter),
      m_curve(0)
{
    setCaption(i18n("Nyquist Plot %1").arg(m_meter.name()));

    m_plot = new Plot(centralWidget());
    m_plot->setAxisTitle(QwtPlot::xBottom, "Real [" + m_meter.shortUnit() + "]");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Imag [" + m_meter.shortUnit() + "]");
    m_plotLayout->addWidget(m_plot);
}

bool ACAnalysisNyquistDialog::runAnalysis()
{
    QString cmdList = m_view->schematic()->createCommandList();
    if (cmdList.isNull())
    {
        m_errorString = m_view->schematic()->errorString();
        return false;
    }

    QString meterCmd = m_meter.createCommand(m_view->schematic());
    if (meterCmd.isNull())
    {
        m_errorString = m_meter.errorString();
        return false;
    }

    cmdList += ".control\n"
               "set nobreak\n"
               ".endc\n"
               ".ac dec " + QString::number(Settings::self()->acAnalysisNumPointsPerDecade()) + " "
                          + m_startFrequency + " " + m_stopFrequency + "\n"
               ".print ac real(" + meterCmd + ") imag(" + meterCmd + ")\n"
               ".end\n";

    m_spiceProcess->start(cmdList, 5);

    return true;
}


void ACAnalysisNyquistDialog::plotData(const QValueVector<QMemArray<double> > &table)
{
    if (!m_curve)
    {
        m_curve = new QwtPlotCurve(m_plot);
        m_curve->setPen(Qt::red);
        m_plot->insertCurve(m_curve);
    }
    m_curve->setData(table[3], table[4]);
    m_plot->replot();
}

//
// ACAnalysisLinearMagnitudeDialog
//

ACAnalysisLinearMagnitudeDialog::ACAnalysisLinearMagnitudeDialog(SchematicView *view, const QString &startFrequency, const QString &stopFrequency, const Meter &meter)
    : AnalysisDialog(view),
      m_startFrequency(startFrequency),
      m_stopFrequency(stopFrequency),
      m_meter(meter),
      m_curve(0)
{
    setCaption(i18n("Linear Magnitude Plot %1").arg(m_meter.name()));

    m_plot = new Plot(centralWidget());
    m_plot->setAxisOptions(QwtPlot::xBottom, QwtAutoScale::Logarithmic);
    m_plot->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Magnitude [" + m_meter.shortUnit() + "]");
    m_plotLayout->addWidget(m_plot);
}

bool ACAnalysisLinearMagnitudeDialog::runAnalysis()
{
    QString cmdList = m_view->schematic()->createCommandList();
    if (cmdList.isNull())
    {
        m_errorString = m_view->schematic()->errorString();
        return false;
    }

    QString meterCmd = m_meter.createCommand(m_view->schematic());
    if (meterCmd.isNull())
    {
        m_errorString = m_meter.errorString();
        return false;
    }

    cmdList += ".control\n"
               "set nobreak\n"
               "set units=degrees\n"
               ".endc\n"
               ".ac dec " + QString::number(Settings::self()->acAnalysisNumPointsPerDecade()) + " "
                          + m_startFrequency + " " + m_stopFrequency + "\n"
               ".print ac mag(" + meterCmd + ")\n"
               ".end\n";

    m_spiceProcess->start(cmdList, 4);

    return true;
}


void ACAnalysisLinearMagnitudeDialog::plotData(const QValueVector<QMemArray<double> > &table)
{
    if (!m_curve)
    {
        m_curve = new QwtPlotCurve(m_plot);
        m_curve->setPen(Qt::red);
        m_plot->insertCurve(m_curve);
    }
    m_curve->setData(table[1], table[3]);
    m_plot->replot();
}

#include "acanalysisdialog.moc"

// vim: ts=4 sw=4 et
