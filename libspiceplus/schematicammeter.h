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

#ifndef SCHEMATICAMMETER_H
#define SCHEMATICAMMETER_H

#include "schematicstandarddevice.h"

namespace Spiceplus {

class ParameterLineEdit;

class SchematicAmmeter : public SchematicStandardDevice
{
public:
    SchematicAmmeter(Schematic *schematic);

    QString id() const { return ID; }
    QString type() const { return QString::null; }
    QString defaultSymbolPath() const { return "standard:Ammeter/General.symbol"; }

    bool initialize();
    void updateLabels();
    void setDefaults();

    ModelPolicy modelPolicy() const { return None; }

    // TODO: create command
    bool hasCommand() const { return true; }
    QString createCommand();

    bool hasPropertiesWidget() const { return true; }
    SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent);

    static const char *ID;
    static const char *Name;
};

class SchematicAmmeterPropertiesWidget : public SchematicDevicePropertiesWidget
{
    Q_OBJECT

public:
    SchematicAmmeterPropertiesWidget(SchematicAmmeter *ammeter, QWidget *parent = 0);
    ~SchematicAmmeterPropertiesWidget();

    bool apply();

private:
    SchematicAmmeter *m_ammeter;
    ParameterLineEdit *m_name;
};

} // namespace Spiceplus

#endif // SCHEMATICAMMETER_H

// vim: ts=4 sw=4 et
