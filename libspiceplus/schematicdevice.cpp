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

#include "schematicdevice.h"
#include "schematicwire.h"
#include "settings.h"

using namespace Spiceplus;

//
// SchematicDevicePin
//

SchematicDevicePin::SchematicDevicePin(SchematicDevice *device)
    : m_x(0), m_y(0), m_device(device), m_node(0)
{
}

SchematicDevicePin::SchematicDevicePin(const QString &id, SchematicDevice *device)
    : m_id(id), m_x(0), m_y(0), m_device(device), m_node(0)
{
}

SchematicDevicePin::~SchematicDevicePin()
{
}

bool SchematicDevicePin::isWireConnected(SchematicWire *wire) const
{
    return m_wireEnds.contains(wire->end1()) || m_wireEnds.contains(wire->end2());
}

QPoint SchematicDevicePin::worldPoint() const
{
    return m_device->toWorld(QPoint(m_x, m_y));
}

void SchematicDevicePin::setNode(SchematicNode *node)
{
    if (m_node)
        m_node->removePin(this);

    if (node)
        node->addPin(this);

    m_node = node;
}

//
// SchematicDevicePropertiesWidget
//

SchematicDevicePropertiesWidget::SchematicDevicePropertiesWidget(QWidget *parent)
    : QWidget(parent)
{
}

SchematicDevicePropertiesWidget::~SchematicDevicePropertiesWidget()
{
}

//
// SchematicDevice
//

SchematicDevice::SchematicDevice(Schematic *schematic)
    : SchematicItem(schematic), m_displayPinWireCount(2)
{
    raiseToTop();
}

SchematicDevice::~SchematicDevice()
{
    hide();

    for (size_t i = 0; i < m_pins.count(); ++i)
        delete m_pins[i];
}

void SchematicDevice::raiseToTop()
{
    setZ(Schematic::nextZIndex() + 1e12);
}

SchematicDevicePin *SchematicDevice::findPin(const QString &id) const
{
    for (size_t i = 0; i < m_pins.count(); ++i)
        if (m_pins[i]->id() == id)
            return m_pins[i];

    return 0;
}

SchematicDevicePin *SchematicDevice::findPin(const QPoint &point) const
{
    int dia = Settings::self()->devicePinDiameter();
    int ssz = Settings::self()->gridSize();

    for (size_t i = 0; i < m_pins.count(); ++i)
    {
        QRect pinRect(m_pins[i]->worldPoint().x() - dia / 2, m_pins[i]->worldPoint().y() - dia / 2, dia, dia);
        if (pinRect.intersects(QRect(point.x() - ssz / 2, point.y() - ssz / 2, ssz, ssz)))
            return m_pins[i];
    }

    return 0;
}

bool SchematicDevice::allPinsConnected() const
{
    for (size_t i = 0; i < m_pins.count(); ++i)
        if (m_pins[i]->wireCount() == 0)
            return false;
    return true;
}

void SchematicDevice::updateWirePositions() const
{
    for (size_t i = 0; i < m_pins.count(); ++i)
    {
        QValueList<SchematicWireEnd *> wireEnds = m_pins[i]->wireEnds();
        for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
            (*it)->wire()->updatePosition();
    }
}

void SchematicDevice::restoreState(const SchematicDeviceState *state)
{
    if (x() != state->m_x || y() != state->m_y)
        move(state->m_x, state->m_y);

    m_name = state->m_name;
}

void SchematicDevice::moveBy(double dx, double dy)
{
    QCanvasPolygonalItem::moveBy(dx, dy);
    updateWirePositions();
}

#include "schematicdevice.moc"

// vim: ts=4 sw=4 et
