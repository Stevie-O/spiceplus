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

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "schematicstandarddevice.h"
#include "modelselector.h"

class QWidget;
class QString;

namespace Spiceplus {

class ParameterLineEdit;

namespace Plugin {

class CapacitorFactory : public SchematicDeviceFactory
{
    Q_OBJECT

public:
    SchematicDevice *createDevice(Schematic *schematic);
    SchematicDeviceSelector *createSelector(QWidget *parent = 0);
};

class Capacitor : public SchematicStandardDevice
{
public:
    Capacitor(Schematic *schematic);

    QString id() const { return "Standard/Capacitor"; }
    QString type() const { return "c"; }
    QString defaultSymbolPath() const { return "standard:Capacitor/General.symbol"; }

    bool initialize();
    void updateLabels();
    void setDefaults();

    ModelPolicy modelPolicy() const { return Optional; }

    bool hasCommand() const { return true; }
    QString createCommand();

    bool hasPropertiesWidget() const { return true; }
    SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent = 0);
};

class CapacitorSelector : public ModelSelector
{
public:
    CapacitorSelector(QWidget *parent = 0);
    bool isDeviceSelected() const;
    SchematicDevice *createSelectedDevice(Schematic *schematic);
};

class CapacitorPropertiesWidget : public SchematicDevicePropertiesWidget
{
    Q_OBJECT

public:
    CapacitorPropertiesWidget(Capacitor *capacitor, QWidget *parent = 0);

    bool apply();

private:
    Capacitor *m_capacitor;

    ParameterLineEdit *m_name;
    ParameterLineEdit *m_value;
    ParameterLineEdit *m_l;
    ParameterLineEdit *m_w;
    ParameterLineEdit *m_ic;
};

} // namespace Plugin
} // namespace Spiceplus

#endif // CAPACITOR_H

// vim: ts=4 sw=4 et
