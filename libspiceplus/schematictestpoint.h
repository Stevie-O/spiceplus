/*
 * SPICE+
 * Copyright (C) 2004 Andreas Unger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SCHEMATICTESTPOINT_H
#define SCHEMATICTESTPOINT_H

#include "schematicstandarddevice.h"

namespace Spiceplus {

class ParameterLineEdit;

class SchematicTestPoint : public SchematicStandardDevice
{
public:
    SchematicTestPoint(Schematic *schematic);

    QString id() const { return ID; }
    QString type() const { return QString::null; }
    QString defaultSymbolPath() const { return "standard:TestPoint/General.symbol"; }

    SchematicDevicePin *pin() const;
    QString nodeName() const;

    bool initialize();
    void updateLabels();
    void setDefaults();

    ModelPolicy modelPolicy() const { return None; }

    bool hasCommand() const { return false; }
    QString createCommand() { return QString::null; }

    bool hasPropertiesWidget() const { return true; }
    SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent);

    static const char *ID;
    static const char *Name;

private:
    SchematicDevicePin *m_pin;
};

class SchematicTestPointPropertiesWidget : public SchematicDevicePropertiesWidget
{
    Q_OBJECT

public:
    SchematicTestPointPropertiesWidget(SchematicTestPoint *testPoint, QWidget *parent = 0);
    ~SchematicTestPointPropertiesWidget();

    bool apply();

private:
    SchematicTestPoint *m_testPoint;
    ParameterLineEdit *m_name;
};

} // namespace Spiceplus

#endif // SCHEMATICTESTPOINT_H

// vim: ts=4 sw=4 et
