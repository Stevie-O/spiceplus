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

#include <math.h>

#include <kdebug.h>

#include "schematiccommand.h"
#include "schematicwire.h"
#include "schematicdevice.h"
#include "schematicjunction.h"
#include "schematicground.h"
#include "settings.h"

using namespace Spiceplus;

//
// SchematicCommandGroup
//

SchematicCommandGroup::~SchematicCommandGroup()
{
    for (size_t i = 0; i < m_commands.count(); ++i)
        delete m_commands[i];
}

void SchematicCommandGroup::execute()
{
    for (size_t i = 0; i < m_commands.count(); ++i)
        m_commands[i]->execute();
}

void SchematicCommandGroup::unexecute()
{
    for (int i = m_commands.count() - 1; i >= 0; --i)
        m_commands[i]->unexecute();
}

//
// SchematicCommandMoveDevice
//

SchematicCommandMoveDevice::SchematicCommandMoveDevice(SchematicDevice *device, const QPoint &oldPosition)
    : m_device(device),
      m_dx(device->x() - oldPosition.x()),
      m_dy(device->y() - oldPosition.y())
{
}

void SchematicCommandMoveDevice::execute()
{
    m_device->moveBy(m_dx, m_dy);
}

void SchematicCommandMoveDevice::unexecute()
{
    m_device->moveBy(-m_dx, -m_dy);
}

//
// SchematicCommandDeleteDevice
//

SchematicCommandDeleteDevice::SchematicCommandDeleteDevice(SchematicDevice *device)
    : m_device(device), m_schematic(device->schematic()), m_commands(0), m_deleted(false)
{
}

SchematicCommandDeleteDevice::~SchematicCommandDeleteDevice()
{
    delete m_commands;

    if (m_deleted)
        delete m_device;
}

void SchematicCommandDeleteDevice::execute()
{
    if (m_commands)
        m_commands->execute();
    else
    {
        m_commands = new SchematicCommandGroup;

        const QValueVector<SchematicDevicePin *> pins = m_device->pins();
        for (size_t i = 0; i < pins.count(); ++i)
        {
            // delete zero-length (invisible) wire
            if (pins[i]->wireCount() == 1 && pins[i]->worldPoint() == pins[i]->wireEnds().first()->oppositePin()->worldPoint())
            {
                SchematicCommand *cmd = new SchematicCommandDeleteWire(pins[i]->wireEnds().first()->wire());
                cmd->execute();
                m_commands->add(cmd);
            }

            QValueList<SchematicWireEnd *> wireEnds = pins[i]->wireEnds();
            if (wireEnds.count() == 2 &&
                pinVariance(wireEnds.first()->oppositePin(), wireEnds.last()->oppositePin(), pins[i]) < 1)
            {
                SchematicWire *wire = new SchematicWire(wireEnds.first()->oppositePin(), wireEnds.last()->oppositePin(), m_schematic);
                wire->show();
                m_commands->add(new SchematicCommandPlaceWire(wire));

                SchematicCommand *cmd = new SchematicCommandDeleteWire(wireEnds.first()->wire());
                cmd->execute();
                m_commands->add(cmd);

                cmd = new SchematicCommandDeleteWire(wireEnds.last()->wire());
                cmd->execute();
                m_commands->add(cmd);
            }
            else if (wireEnds.count() > 0)
            {
                SchematicJunction *jun = new SchematicJunction(pins[i]->worldPoint(), m_schematic);
                jun->show();

                for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
                {
                    SchematicWire *wire = new SchematicWire((*it)->oppositePin(), jun->pin(), m_schematic);
                    wire->show();
                    m_commands->add(new SchematicCommandPlaceWire(wire));

                    SchematicCommand *cmd = new SchematicCommandDeleteWire((*it)->wire());
                    cmd->execute();
                    m_commands->add(cmd);
                }
            }
        }
    }

    m_device->setSchematic(0);
    m_deleted = true;
}

void SchematicCommandDeleteDevice::unexecute()
{
    m_device->setSchematic(m_schematic);
    m_deleted = false;

    m_commands->unexecute();
}

double SchematicCommandDeleteDevice::pinVariance(SchematicDevicePin *pin1, SchematicDevicePin *pin2, SchematicDevicePin *refPin) const
{
    QPoint p1a = refPin->worldPoint() - pin1->worldPoint();
    QPoint p1b = pin2->worldPoint() - pin1->worldPoint();

    QPoint p2a = refPin->worldPoint() - pin2->worldPoint();
    QPoint p2b = pin1->worldPoint() - pin2->worldPoint();

    double a1 = fabs(fmod((atan2(p1a.y(), p1a.x()) - atan2(p1b.y(), p1b.x())) / M_PI * 180, 360));
    double a2 = fabs(fmod((atan2(p2a.y(), p2a.x()) - atan2(p2b.y(), p2b.x())) / M_PI * 180, 360));

    return a1 > a2 ? a1 : a2;
}

//
// SchematicCommandRotateDevice
//

SchematicCommandRotateDevice::SchematicCommandRotateDevice(SchematicDevice *device, int direction)
    : m_device(device), m_direction(direction), m_angle(Settings::self()->deviceRotationAngle())
{
}

SchematicCommandRotateDevice::~SchematicCommandRotateDevice()
{
}

void SchematicCommandRotateDevice::execute()
{
    m_device->rotate(m_direction * m_angle);
}

void SchematicCommandRotateDevice::unexecute()
{
    m_device->rotate(-m_direction * m_angle);
}

//
// SchematicCommandFlipDevice
//

SchematicCommandFlipDevice::SchematicCommandFlipDevice(SchematicDevice *device, Qt::Orientation o)
    : m_device(device), m_orientation(o)
{
}

SchematicCommandFlipDevice::~SchematicCommandFlipDevice()
{
}

void SchematicCommandFlipDevice::execute()
{
    m_device->flip(m_orientation);
}

void SchematicCommandFlipDevice::unexecute()
{
    m_device->flip(m_orientation);
}

//
// SchematicCommandDeleteWire
//

SchematicCommandDeleteWire::SchematicCommandDeleteWire(SchematicWire *wire)
    : m_wire(wire),
      m_schematic(wire->schematic()),
      m_pin1(wire->end1()->pin()),
      m_pin2(wire->end2()->pin()),
      m_junction1(0),
      m_junction2(0),
      m_addedNode(0),
      m_removedNode(0),
      m_groundNode(0),
      m_deleted(false)
{
}

SchematicCommandDeleteWire::~SchematicCommandDeleteWire()
{
    if (m_deleted)
    {
        delete m_removedNode;
        delete m_junction1;
        delete m_junction2;
        delete m_wire;
    }
    else
        delete m_addedNode;
}

void SchematicCommandDeleteWire::execute()
{
    if (m_pin1->device()->id() == SchematicJunction::ID && m_pin1->wireCount() < 2)
    {
        m_junction1 = m_pin1->device();
        m_junction1->setSchematic(0);
    }

    if (m_pin2->device()->id() == SchematicJunction::ID && m_pin2->wireCount() < 2)
    {
        m_junction2 = m_pin2->device();
        m_junction2->setSchematic(0);
    }

    m_wire->end1()->disconnect();
    m_wire->end2()->disconnect();
    m_wire->setSchematic(0);

    if (m_pin1->node()->name() == "0" || m_pin2->node()->name() == "0")
    {
        m_groundNode = m_schematic->findNode("0");

        // check if pins are connected to grounds
        bool connected1 = false, connected2 = false;

        QValueList<SchematicDevicePin *> pins = m_schematic->findNode("0")->pins();
        for (QValueList<SchematicDevicePin *>::Iterator it = pins.begin(); it != pins.end(); ++it)
        {
            if ((*it)->device()->id() != SchematicGround::ID)
                continue;

            if (m_schematic->isNodeEqual(m_pin1, *it))
                connected1 = true;
            if (m_schematic->isNodeEqual(m_pin2, *it))
                connected2 = true;
            if (connected1 && connected2)
                break;
        }

        if (m_pin1->wireCount() == 0)
            m_pin1->setNode(0);
        else if (!connected1)
        {
            if (!m_addedNode)
                m_addedNode = new SchematicNode(m_schematic->createUniqueNodeName());
            m_schematic->addNode(m_addedNode);
            m_schematic->setNodePins(m_pin1, m_addedNode);
        }

        if (m_pin2->wireCount() == 0)
            m_pin2->setNode(0);
        else if (!connected2)
        {
            if (!m_addedNode)
                m_addedNode = new SchematicNode(m_schematic->createUniqueNodeName());
            m_schematic->addNode(m_addedNode);
            m_schematic->setNodePins(m_pin2, m_addedNode);
        }

        if (!connected1 && !connected2)
            kdDebug() << k_funcinfo << "Something is wrong here" << endl;
    }
    else if (m_pin1->wireCount() == 0 && m_pin2->wireCount() == 0)
    {
        m_removedNode = m_pin1->node();
        m_schematic->removeNode(m_removedNode);
        m_pin1->setNode(0);
        m_pin2->setNode(0);
    }
    else if (m_pin1->wireCount() == 0)
    {
        m_pin1->setNode(0);
    }
    else if (m_pin2->wireCount() == 0)
    {
        m_pin2->setNode(0);
    }
    else if (!m_schematic->isNodeEqual(m_pin1, m_pin2))
    {
        if (!m_addedNode)
            m_addedNode = new SchematicNode(m_schematic->createUniqueNodeName());
        m_schematic->addNode(m_addedNode);
        m_schematic->setNodePins(m_pin2, m_addedNode);
    }

    m_deleted = true;
}

void SchematicCommandDeleteWire::unexecute()
{
    if (m_junction1)
    {
        m_junction1->setSchematic(m_schematic);
        m_junction1 = 0;
    }

    if (m_junction2)
    {
        m_junction2->setSchematic(m_schematic);
        m_junction2 = 0;
    }

    m_wire->end1()->connect(m_pin1);
    m_wire->end2()->connect(m_pin2);
    m_wire->setSchematic(m_schematic);

    if (m_groundNode)
    {
        if (!m_pin1->node())
            m_pin1->setNode(m_groundNode);
        else if (m_pin1->node() != m_groundNode)
        {
            m_schematic->removeNode(m_addedNode);
            m_schematic->setNodePins(m_pin1, m_groundNode);
        }

        if (!m_pin2->node())
            m_pin2->setNode(m_groundNode);
        else if (m_pin2->node() != m_groundNode)
        {
            m_schematic->removeNode(m_addedNode);
            m_schematic->setNodePins(m_pin2, m_groundNode);
        }
    }
    else if (!m_pin1->node() && !m_pin2->node())
    {
        m_schematic->addNode(m_removedNode);
        m_pin1->setNode(m_removedNode);
        m_pin2->setNode(m_removedNode);
    }
    else if (!m_pin1->node())
    {
        m_pin1->setNode(m_pin2->node());
    }
    else if (!m_pin2->node())
    {
        m_pin2->setNode(m_pin1->node());
    }
    else if (m_pin1->node() != m_pin2->node())
    {
        m_schematic->removeNode(m_addedNode);
        m_schematic->setNodePins(m_pin2, m_pin1->node());
    }

    m_deleted = false;
}

//
// SchematicCommandPlaceWire
//

SchematicCommandPlaceWire::SchematicCommandPlaceWire(SchematicWire *wire)
    : m_wire(wire),
      m_schematic(wire->schematic()),
      m_pin1(wire->end1()->pin()),
      m_pin2(wire->end2()->pin()),
      m_junction1(0),
      m_junction2(0),
      m_node1(wire->end1()->pin()->node()),
      m_node2(wire->end2()->pin()->node()),
      m_addedNode(0),
      m_removedNode(0),
      m_connected(true)
{
    SchematicNode *gndNode = m_schematic->findNode("0");

    if (m_node1 == gndNode || m_pin1->device()->id() == SchematicGround::ID ||
        m_node2 == gndNode || m_pin2->device()->id() == SchematicGround::ID)
    {
        if (m_node1 && m_node1 != gndNode)
        {
            m_removedNode = m_node1;
            m_schematic->removeNode(m_removedNode);
            m_schematic->setNodePins(m_pin1, gndNode);
        }
        else if (m_node2 && m_node2 != gndNode)
        {
            m_removedNode = m_node2;
            m_schematic->removeNode(m_removedNode);
            m_schematic->setNodePins(m_pin2, gndNode);
        }
        else
        {
            if (!m_node1)
                m_pin1->setNode(gndNode);
            if (!m_node2)
                m_pin2->setNode(gndNode);
        }
    }
    else if (!m_node1 && !m_node2)
    {
        SchematicNode *node = new SchematicNode(m_schematic->createUniqueNodeName());
        m_schematic->addNode(node);
        m_pin1->setNode(node);
        m_pin2->setNode(node);
        m_addedNode = node;
    }
    else if (!m_node1)
    {
        m_pin1->setNode(m_node2);
    }
    else if (!m_node2)
    {
        m_pin2->setNode(m_node1);
    }
    else if (m_node1 != m_node2)
    {
        m_schematic->removeNode(m_node2); 
        m_schematic->setNodePins(m_pin2, m_node1);
        m_removedNode = m_node2;
    }
}

SchematicCommandPlaceWire::~SchematicCommandPlaceWire()
{
    if (!m_connected)
    {
        delete m_addedNode;
        delete m_junction1;
        delete m_junction2;
        delete m_wire;
    }
    else
        delete m_removedNode;
}

void SchematicCommandPlaceWire::execute()
{
    if (m_junction1)
    {
        m_junction1->setSchematic(m_schematic);
        m_junction1 = 0;
    }

    if (m_junction2)
    {
        m_junction2->setSchematic(m_schematic);
        m_junction2 = 0;
    }

    m_wire->end1()->connect(m_pin1);
    m_wire->end2()->connect(m_pin2);
    m_wire->setSchematic(m_schematic);

    SchematicNode *gndNode = m_schematic->findNode("0");

    if (m_node1 == gndNode || m_pin1->device()->id() == SchematicGround::ID ||
        m_node2 == gndNode || m_pin2->device()->id() == SchematicGround::ID)
    {
        if (m_node1 && m_node1 != gndNode)
        {
            m_schematic->removeNode(m_removedNode);
            m_schematic->setNodePins(m_pin1, gndNode);
        }
        else if (m_node2 && m_node2 != gndNode)
        {
            m_schematic->removeNode(m_removedNode);
            m_schematic->setNodePins(m_pin2, gndNode);
        }
        else
        {
            if (!m_node1)
                m_pin1->setNode(gndNode);
            if (!m_node2)
                m_pin2->setNode(gndNode);
        }
    }
    else if (!m_node1 && !m_node2)
    {
        m_schematic->addNode(m_addedNode);
        m_pin1->setNode(m_addedNode);
        m_pin2->setNode(m_addedNode);
    }
    else if (!m_node1)
    {
        m_pin1->setNode(m_node2);
    }
    else if (!m_node2)
    {
        m_pin2->setNode(m_node1);
    }
    else if (m_node1 != m_node2)
    {
        m_schematic->removeNode(m_removedNode); 
        m_schematic->setNodePins(m_pin2, m_node1);
    }

    m_connected = true;
}

void SchematicCommandPlaceWire::unexecute()
{
    if (m_pin1->device()->id() == SchematicJunction::ID && m_pin1->wireCount() < 2)
    {
        m_junction1 = m_pin1->device();
        m_junction1->setSchematic(0);
    }

    if (m_pin2->device()->id() == SchematicJunction::ID && m_pin2->wireCount() < 2)
    {
        m_junction2 = m_pin2->device();
        m_junction2->setSchematic(0);
    }

    m_wire->end1()->disconnect();
    m_wire->end2()->disconnect();
    m_wire->setSchematic(0);

    SchematicNode *gndNode = m_schematic->findNode("0");

    if (m_node1 == gndNode || m_pin1->device()->id() == SchematicGround::ID ||
        m_node2 == gndNode || m_pin2->device()->id() == SchematicGround::ID)
    {
        if (m_node1 && m_node1 != gndNode)
        {
            m_schematic->addNode(m_removedNode);
            m_schematic->setNodePins(m_pin1, m_removedNode);
        }
        else if (m_node2 && m_node2 != gndNode)
        {
            m_schematic->addNode(m_removedNode);
            m_schematic->setNodePins(m_pin2, m_removedNode);
        }

        if (!m_node1)
            m_pin1->setNode(0);
        if (!m_node2)
            m_pin2->setNode(0);
    }
    else if (!m_node1 && !m_node2)
    {
        m_schematic->removeNode(m_addedNode);
        m_pin1->setNode(0);
        m_pin2->setNode(0);
    }
    else if (!m_node1)
    {
        m_pin1->setNode(0);
    }
    else if (!m_node2)
    {
        m_pin2->setNode(0);
    }
    else if (m_node1 != m_node2)
    {
        m_schematic->addNode(m_removedNode);
        m_schematic->setNodePins(m_pin2, m_removedNode);
    }

    m_connected = false;
}

//
// SchematicCommandPlaceDevice
//

SchematicCommandPlaceDevice::SchematicCommandPlaceDevice(SchematicDevice *device)
    : m_device(device), m_schematic(device->schematic()), m_added(true)
{
}

SchematicCommandPlaceDevice::~SchematicCommandPlaceDevice()
{
    if (!m_added)
        delete m_device;
}

void SchematicCommandPlaceDevice::execute()
{
    m_device->setSchematic(m_schematic);
    m_added = true;
}

void SchematicCommandPlaceDevice::unexecute()
{
    m_device->setSchematic(0);
    m_added = false;
}

//
// SchematicCommandChangeDeviceProperties
//

SchematicCommandChangeDeviceProperties::SchematicCommandChangeDeviceProperties(SchematicDevice *device,
                                                                               SchematicDeviceState *oldState,
                                                                               SchematicDeviceState *newState)
    : m_device(device), m_oldState(oldState), m_newState(newState)
{
}

SchematicCommandChangeDeviceProperties::~SchematicCommandChangeDeviceProperties()
{
    delete m_oldState;
    delete m_newState;
}

void SchematicCommandChangeDeviceProperties::execute()
{
    m_device->restoreState(m_newState);
}

void SchematicCommandChangeDeviceProperties::unexecute()
{
    m_device->restoreState(m_oldState);
}

// vim: ts=4 sw=4 et
