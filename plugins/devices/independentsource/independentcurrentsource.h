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

#ifndef INDEPENDENTCURRENTSOURCE_H
#define INDEPENDENTCURRENTSOURCE_H

#include "independentsource.h"

class QWidget;
class QString;

namespace Spiceplus {
namespace Plugin {

class IndependentCurrentSourceFactory : public SchematicDeviceFactory
{
    Q_OBJECT

public:
    SchematicDevice *createDevice(Schematic *schematic);
    SchematicDeviceSelector *createSelector(QWidget *parent = 0);
};

class IndependentCurrentSource : public IndependentSource
{
public:
    IndependentCurrentSource(Schematic *schematic) : IndependentSource(schematic) {}

    QString id() const { return "Standard/IndependentCurrentSource"; }
    QString type() const { return "i"; }
    QString defaultSymbolPath() const { return "standard:IndependentCurrentSource/General.symbol"; }

    QString description() const { return "Current Source"; }
    QString unit() const { return "Amps"; }
};

class IndependentCurrentSourceSelector : public SchematicDeviceSelector
{
public:
    IndependentCurrentSourceSelector(QWidget *parent = 0);
    bool isDeviceSelected() const;
    SchematicDevice *createSelectedDevice(Schematic *schematic);
};

} // namespace Plugin
} // namespace Spiceplus

#endif // INDEPENDENTCURRENTSOURCE_H

// vim: ts=4 sw=4 et
