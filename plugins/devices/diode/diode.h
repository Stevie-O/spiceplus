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

#ifndef DIODE_H
#define DIODE_H

#include "schematicstandarddevice.h"
#include "modelselector.h"

class QWidget;
class QString;
class QCheckBox;

namespace Spiceplus {

class ParameterLineEdit;

namespace Plugin {

class DiodeFactory : public SchematicDeviceFactory
{
    Q_OBJECT

public:
    SchematicDevice *createDevice(Schematic *schematic);
    SchematicDeviceSelector *createSelector(QWidget *parent = 0);
};

class Diode : public SchematicStandardDevice
{
public:
    Diode(Schematic *schematic);

    QString id() const { return "Standard/Diode"; }
    QString type() const { return "d"; }
    QString defaultSymbolPath() const { return QString::null; }

    bool initialize();
    void updateLabels();
    void setDefaults();

    ModelPolicy modelPolicy() const { return Required; }

    bool hasCommand() const { return true; }
    QString createCommand();

    bool hasPropertiesWidget() const { return true; }
    SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent = 0);
};

class DiodeSelector : public ModelSelector
{
public:
    DiodeSelector(QWidget *parent = 0);
    bool isDeviceSelected() const;
    SchematicDevice *createSelectedDevice(Schematic *schematic);
};

class DiodePropertiesWidget : public SchematicDevicePropertiesWidget
{
    Q_OBJECT

public:
    DiodePropertiesWidget(Diode *diode, QWidget *parent = 0);

    bool apply();

private:
    Diode *m_diode;

    ParameterLineEdit *m_name;
    ParameterLineEdit *m_area;
    ParameterLineEdit *m_ic;
    ParameterLineEdit *m_temp;
    QCheckBox *m_off;
};

} // namespace Plugin
} // namespace Spiceplus

#endif // DIODE_H

// vim: ts=4 sw=4 et
