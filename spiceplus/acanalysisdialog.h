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

#ifndef ACANALYSISDIALOG_H
#define ACANALYSISDIALOG_H

#include "analysisdialog.h"
#include "meter.h"

class QwtPlotCurve;

namespace Spiceplus {

class Plot;

class ACAnalysisBodeDialog : public AnalysisDialog
{
    Q_OBJECT

public:
    ACAnalysisBodeDialog(SchematicView *view, const QString &startFrequency, const QString &stopFrequency, const Meter &meter);

    bool runAnalysis();

protected slots:
    void plotData(const QValueVector<QMemArray<double> > &table);

private:
    QString m_startFrequency;
    QString m_stopFrequency;
    Meter m_meter;

    Plot *m_magnitudePlot;
    QwtPlotCurve *m_magnitudeCurve;
    Plot *m_phasePlot;
    QwtPlotCurve *m_phaseCurve;
};

class ACAnalysisNyquistDialog : public AnalysisDialog
{
    Q_OBJECT

public:
    ACAnalysisNyquistDialog(SchematicView *view, const QString &startFrequency, const QString &stopFrequency, const Meter &meter);

    bool runAnalysis();

protected slots:
    void plotData(const QValueVector<QMemArray<double> > &table);

private:
    QString m_startFrequency;
    QString m_stopFrequency;
    Meter m_meter;

    Plot *m_plot;
    QwtPlotCurve *m_curve;
};

class ACAnalysisLinearMagnitudeDialog : public AnalysisDialog
{
    Q_OBJECT

public:
    ACAnalysisLinearMagnitudeDialog(SchematicView *view, const QString &startFrequency, const QString &stopFrequency, const Meter &meter);

    bool runAnalysis();

protected slots:
    void plotData(const QValueVector<QMemArray<double> > &table);

private:
    QString m_startFrequency;
    QString m_stopFrequency;
    Meter m_meter;

    Plot *m_plot;
    QwtPlotCurve *m_curve;
};

} // namespace Spiceplus

#endif // ACANALYSISDIALOG_H

// vim: ts=4 sw=4 et
