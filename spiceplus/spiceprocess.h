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

#ifndef SPICEPROCESS_H
#define SPICEPROCESS_H

#include <kprocess.h>

template<class T> class QValueVector;
template<class T> class QMemArray;

namespace Spiceplus {

class SpiceProcess : public KProcess
{
    Q_OBJECT

public:
    SpiceProcess(QObject *parent = 0);

    bool start(const QString &commandList, int numColumns);
    QString errorString() const { return m_errorString; }

signals:
    void analysisFailed(const QString &errorString, const QString &errorDetails = QString::null);
    void analysisFinished(const QValueVector<QMemArray<double> > &table);

private slots:
    void closeStdin(KProcess *proc);
    void processStdout(KProcess *proc, char *buffer, int buflen);
    void processStderr(KProcess *proc, char *buffer, int buflen);
    void finishAnalysis(KProcess *proc);

private:
    QString m_commandList;
    int m_numColumns;
    QString m_stdout;
    QString m_stderr;
    QString m_errorString;
};

} // namespace Spiceplus

#endif // SPICEPROCESS_H

// vim: ts=4 sw=4 et
