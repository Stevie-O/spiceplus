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

#include "dcanalysisdialog.h"
#include "schematicdevice.h"
#include "schematicview.h"
#include "plot.h"
#include "settings.h"
#include "spiceprocess.h"

using namespace Spiceplus;

DCAnalysisDialog::DCAnalysisDialog(SchematicView *view,
                                   const Meter &meter,
                                   const QString &sourceName,
                                   const QString &startingValue,
                                   const QString &finalValue,
                                   const QString &incrementingValue,
                                   const QString &sourceName2,
                                   const QString &startingValue2,
                                   const QString &finalValue2,
                                   const QString &incrementingValue2)
    : AnalysisDialog(view),
      m_sourceName(sourceName),
      m_startingValue(startingValue),
      m_finalValue(finalValue),
      m_incrementingValue(incrementingValue),
      m_sourceName2(sourceName2),
      m_startingValue2(startingValue2),
      m_finalValue2(finalValue2),
      m_incrementingValue2(incrementingValue2),
      m_meter(meter)
{
    SchematicDevice *dev = m_view->schematic()->findDevice(m_sourceName);
    QString unit;
    if (dev->type() == "v")
        unit = " [V]";
    else if (dev->type() = "i")
        unit = " [A]";

    setCaption(i18n("DC Plot %1,%2").arg(m_sourceName).arg(m_meter.name()));
    m_plot = new Plot(centralWidget());
    m_plot->setAxisTitle(QwtPlot::xBottom, "Source value" + unit);
    m_plot->setAxisTitle(QwtPlot::yLeft, "Measured value [" + m_meter.shortUnit() + "]");
    m_plotLayout->addWidget(m_plot);
}

bool DCAnalysisDialog::runAnalysis()
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
               ".endc\n";

    SchematicDevice *src = m_view->schematic()->findDevice(m_sourceName);
    if (!src)
    {
        m_errorString = i18n("Source %1 not found").arg(m_sourceName);
        return false;
    }
    cmdList += ".dc " + src->type() + src->name().lower() + " " + m_startingValue + " " + m_finalValue + " " + m_incrementingValue;

    if (!m_sourceName2.isNull())
    {
        SchematicDevice *src2 = m_view->schematic()->findDevice(m_sourceName2);
        if (!src2)
        {
            m_errorString = i18n("Source %1 not found").arg(m_sourceName2);
            return false;
        }
        cmdList += " " + src2->type() + src2->name().lower() + " " + m_startingValue2 + " " + m_finalValue2 + " " + m_incrementingValue2;
    }

    cmdList += "\n"
               ".print dc " + meterCmd + "\n"
               ".end\n";

    if (!m_spiceProcess->start(cmdList, 3))
    {
        m_errorString = m_spiceProcess->errorString();
        return false;
    }

    return true;
}

void DCAnalysisDialog::plotData(const QValueVector<QMemArray<double> > &table)
{
    m_plot->removeCurves();

    if (table[1].count() == 0)
        return;

    int firstRow = 0;
    double firstValue = table[1][0];
    for (size_t row = 1;; ++row)
    {
        if (row == table[1].count() || table[1][row] == firstValue)
        {
            QwtPlotCurve *curve = new QwtPlotCurve(m_plot);
            curve->setPen(Qt::red);
            curve->setData(table[1].data() + firstRow, table[2].data() + firstRow, row - firstRow);
            m_plot->insertCurve(curve);
            m_curves.append(curve);

            if (row < table[1].count())
                firstRow = row;
            else
                break;
        }
    }

    m_plot->replot();
}

#include "dcanalysisdialog.moc"

// vim: ts=4 sw=4 et
