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

#ifndef DCANALYSISDIALOG_H
#define DCANALYSISDIALOG_H

#include <qvaluevector.h>

#include "analysisdialog.h"
#include "meter.h"

class QwtPlotCurve;

namespace Spiceplus {

class Plot;

class DCAnalysisDialog : public AnalysisDialog
{
    Q_OBJECT

public:
    DCAnalysisDialog(SchematicView *view,
                     const Meter &meter,
                     const QString &sourceName,
                     const QString &startingValue,
                     const QString &finalValue,
                     const QString &incrementingValue,
                     const QString &sourceName2 = QString::null,
                     const QString &startingValue2 = QString::null,
                     const QString &finalValue2 = QString::null,
                     const QString &incrementingValue2 = QString::null);

    bool runAnalysis();

protected slots:
    void plotData(const QValueVector<QMemArray<double> > &table);

private:
    QString m_sourceName;
    QString m_startingValue;
    QString m_finalValue;
    QString m_incrementingValue;
    QString m_sourceName2;
    QString m_startingValue2;
    QString m_finalValue2;
    QString m_incrementingValue2;
    Meter m_meter;

    Plot *m_plot;
    QValueVector<QwtPlotCurve *> m_curves;
};

} // namespace Spiceplus

#endif // DCANALYSISDIALOG_H

// vim: ts=4 sw=4 et
