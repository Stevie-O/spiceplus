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

#include <qvaluevector.h>
#include <qmemarray.h>
#include <qregexp.h>
#include <qfile.h>

#include <klocale.h>

#include "spiceprocess.h"
#include "settings.h"

using namespace Spiceplus;

SpiceProcess::SpiceProcess(QObject *parent)
    : KProcess(parent), m_numColumns(0)
{
    *this << Settings::self()->spiceExecutablePath() << "-b";
    connect(this, SIGNAL(wroteStdin(KProcess *)), SLOT(closeStdin(KProcess *)));
    connect(this, SIGNAL(receivedStdout(KProcess *, char *, int)), SLOT(processStdout(KProcess *, char *, int)));
    connect(this, SIGNAL(receivedStderr(KProcess *, char *, int)), SLOT(processStderr(KProcess *, char *, int)));
    connect(this, SIGNAL(processExited(KProcess *)), SLOT(finishAnalysis(KProcess *)));
}

bool SpiceProcess::start(const QString &commandList, int numColumns)
{
    m_commandList = commandList;
    m_numColumns = numColumns;
    m_stdout = "";
    m_stderr = "";

    if (!QFile::exists(args()[0]))
    {
        m_errorString = i18n("SPICE executable %1 not found").arg(args()[0]);
        return false;
    }

    if (!KProcess::start(NotifyOnExit, static_cast<Communication>(KProcess::Stdin | KProcess::Stderr | KProcess::Stdout)))
    {
        m_errorString = i18n("Could not start SPICE");
        return false;
    }

    if (!writeStdin(m_commandList.latin1(), m_commandList.length()))
    {
        m_errorString = i18n("Communication with SPICE failed");
        return false;
    }

    return true;
}

void SpiceProcess::closeStdin(KProcess *)
{
    KProcess::closeStdin();
}

void SpiceProcess::processStdout(KProcess *, char *buffer, int buflen)
{
    m_stdout += QString::fromLatin1(buffer, buflen);
}

void SpiceProcess::processStderr(KProcess *, char *buffer, int buflen)
{
    m_stderr += QString::fromLatin1(buffer, buflen);
}

void SpiceProcess::finishAnalysis(KProcess *)
{
    QStringList errors = QStringList::split('\n', m_stderr, true);

    for (QStringList::Iterator it = errors.begin(); it != errors.end(); ++it)
    {
        if ((*it).contains(QRegExp("^Error:")))
        {
            emit analysisFailed(i18n("Analysis failed\nThere is probably a mistake in the schematic."), m_stderr);
            return;
        }
    }

    QStringList data = QStringList::split('\n', m_stdout, true);
    QStringList::Iterator dataIt = data.begin();

    for (; dataIt != data.end(); ++dataIt)
        if ((*dataIt).contains(QRegExp("^--------")))
            break;

    if (dataIt == data.end() || ++dataIt == data.end() || !(*dataIt).contains(QRegExp("^Index")) || ++dataIt == data.end() || !(*dataIt).contains(QRegExp("^--------")))
    {
        emit analysisFailed(i18n("Analysis failed: Invalid data"));
        return;
    }
 
    if (++dataIt == data.end())
    {
        emit analysisFailed(i18n("Analysis failed: No data"));
        return;
    }

    QValueVector<QMemArray<double> > table(m_numColumns);

    for (int col = 0; col < m_numColumns; ++col)
        table[col].detach();

    for (int row = 0; dataIt != data.end(); ++row, ++dataIt)
    {
        if ((*dataIt).isEmpty())
            break;

        QStringList values = QStringList::split(QRegExp("\\s|,"), *dataIt);
        bool ok = true;

        if (static_cast<int>(values.count()) == m_numColumns)
        {
            int col = 0;
            for (QStringList::Iterator valueIt = values.begin(); valueIt != values.end(); ++col, ++valueIt)
            {
                double value = (*valueIt).toDouble(&ok);
                if (!ok)
                    break;

                table[col].resize(row + 1, QGArray::SpeedOptim);
                table[col][row] = value;
            }
        }

        if (static_cast<int>(values.count()) != m_numColumns || !ok)
        {
            emit analysisFailed(i18n("Analysis failed: Invalid values"));
            return;
        }
    }

    emit analysisFinished(table);
}

#include "spiceprocess.moc"

// vim: ts=4 sw=4 et
