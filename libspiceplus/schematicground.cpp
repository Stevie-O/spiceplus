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

#include "schematicground.h"

using namespace Spiceplus;

const char *SchematicGround::ID = "_Ground";
const char *SchematicGround::Name = "_GND";

SchematicGround::SchematicGround(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(m_pin = new SchematicDevicePin("pin", this));
}

SchematicDevicePin *SchematicGround::pin() const
{
    return m_pin;
}

bool SchematicGround::initialize()
{
    if (!loadSymbol())
        return false;

    return true;
}

void SchematicGround::updateLabels()
{
    // do nothing
}

void SchematicGround::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(Name));
}

// vim: ts=4 sw=4 et
