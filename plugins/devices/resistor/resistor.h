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

#ifndef RESISTOR_H
#define RESISTOR_H

#include "schematicstandarddevice.h"
#include "modelselector.h"

class QWidget;
class QString;

namespace Spiceplus {

class ParameterLineEdit;

namespace Plugin {

class ResistorFactory : public SchematicDeviceFactory
{
    Q_OBJECT

public:
    SchematicDevice *createDevice(Schematic *schematic);
    SchematicDeviceSelector *createSelector(QWidget *parent = 0);
};

class Resistor : public SchematicStandardDevice
{
public:
    Resistor(Schematic *schematic);

    QString id() const { return "Standard/Resistor"; }
    QString type() const { return "r"; }
    QString defaultSymbolPath() const { return "standard:Resistor/General.symbol"; }

    bool initialize();
    void updateLabels();
    void setDefaults();

    ModelPolicy modelPolicy() const { return Optional; }

    bool hasCommand() const { return true; }
    QString createCommand();

    bool hasPropertiesWidget() const { return true; }
    SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent = 0);
};

class ResistorSelector : public ModelSelector
{
public:
    ResistorSelector(QWidget *parent = 0);
    bool isDeviceSelected() const;
    SchematicDevice *createSelectedDevice(Schematic *schematic);
};

class ResistorPropertiesWidget : public SchematicDevicePropertiesWidget
{
    Q_OBJECT

public:
    ResistorPropertiesWidget(Resistor *resistor, QWidget *parent = 0);

    bool apply();

private:
    Resistor *m_resistor;

    ParameterLineEdit *m_name;
    ParameterLineEdit *m_value;
    ParameterLineEdit *m_l;
    ParameterLineEdit *m_w;
    ParameterLineEdit *m_temp;
};

} // namespace Plugin
} // namespace Spiceplus

#endif // RESISTOR_H

// vim: ts=4 sw=4 et
