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

#ifndef ANALYSISDIALOG_H
#define ANALYSISDIALOG_H

#include <kmainwindow.h>

template<class T> class QValueVector;
template<class T> class QMemArray;
class QString;
class QBoxLayout;

namespace Spiceplus {

class SchematicView;
class SpiceProcess;

class AnalysisDialog : public KMainWindow
{
    Q_OBJECT

public:
    AnalysisDialog(SchematicView *view, QBoxLayout::Direction plotLayoutDirection = QBoxLayout::TopToBottom);

    virtual bool runAnalysis() = 0;
    QString errorString() const { return m_errorString; }

protected slots:
    virtual void plotData(const QValueVector<QMemArray<double> > &table) = 0;

protected:
    SchematicView *m_view;
    QBoxLayout *m_plotLayout;
    SpiceProcess *m_spiceProcess;
    QString m_errorString;

private slots:
    void displayErrorMessage(const QString &errorString, const QString &errorDetails);
    void updatePlot();
};

} // namespace Spiceplus

#endif // ANALYSISDIALOG_H

// vim: ts=4 sw=4 et
