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

#ifndef METER_H
#define METER_H

class QString;

namespace Spiceplus {

class Schematic;

class Meter
{
public:
    enum Type { Voltmeter, Ammeter };
    Meter(const QString &testPoint1, const QString &testPoint2);
    Meter(const QString &ammeter);

    Type type() const { return m_type; }
    QString testPoint1() const { return m_testPoint1; }
    QString testPoint2() const { return m_testPoint2; }
    QString ammeter() const { return m_ammeter; }

    QString name() const { return m_type == Voltmeter ? m_testPoint1 + "->" + m_testPoint2 : m_ammeter; }
    QString shortUnit() const { return m_type == Voltmeter ? "V" : "A"; }

    QString createCommand(Schematic *schematic);
    QString errorString() const { return m_errorString; }

private:
    Type m_type;
    QString m_testPoint1;
    QString m_testPoint2;
    QString m_ammeter;

    QString m_errorString;
};

} // namespace Spiceplus

#endif // METER_H

// vim: ts=4 sw=4 et
