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

#include <qstring.h>

#include <klocale.h>

#include "meter.h"
#include "schematic.h"
#include "schematictestpoint.h"
#include "schematicammeter.h"

using namespace Spiceplus;

Meter::Meter(const QString &testPoint1, const QString &testPoint2)
    : m_type(Voltmeter), m_testPoint1(testPoint1), m_testPoint2(testPoint2)
{
}

Meter::Meter(const QString &ammeter)
    : m_type(Ammeter), m_ammeter(ammeter)
{
}

QString Meter::createCommand(Schematic *schematic)
{
    QString cmd;

    if (m_type == Voltmeter)
    {
        SchematicTestPoint *tp1 = dynamic_cast<SchematicTestPoint *>(schematic->findDevice(m_testPoint1));
        if (!tp1)
        {
            m_errorString = i18n("Test Point %1 not found").arg(m_testPoint1);
            return QString::null;
        }

        QString tp1NodeName = tp1->nodeName();
        if (tp1NodeName.isNull())
        {
            m_errorString = i18n("Test Point %1 not connected").arg(m_testPoint1);
            return QString::null;
        }

        SchematicTestPoint *tp2 = dynamic_cast<SchematicTestPoint *>(schematic->findDevice(m_testPoint2));
        if (!tp2)
        {
            m_errorString = i18n("Test Point %1 not found").arg(m_testPoint2);
            return QString::null;
        }

        QString tp2NodeName = tp2->nodeName();
        if (tp2NodeName.isNull())
        {
            m_errorString = i18n("Test Point %1 not connected").arg(m_testPoint2);
            return QString::null;
        }

        if (tp1NodeName == tp2NodeName)
        {
            m_errorString = i18n("Both Test Points are connected to the same node").arg(m_testPoint2);
            return QString::null;
        }

        if (tp1NodeName == "0")
            cmd = "-v(" + tp2NodeName + ")";
        else if (tp2NodeName == "0")
            cmd = "v(" + tp1NodeName + ")";
        else
            cmd = "v(" + tp1NodeName + "," + tp2NodeName + ")";
    }
    else
    {
        SchematicAmmeter *amm = dynamic_cast<SchematicAmmeter *>(schematic->findDevice(m_ammeter));
        if (!amm)
        {
            m_errorString = i18n("Ammeter %1 not found").arg(m_ammeter);
            return QString::null;
        }

        cmd = "i(v" + m_ammeter.lower() + ")";
    }

    return cmd;
}

// vim: ts=4 sw=4 et
