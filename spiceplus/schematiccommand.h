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

#ifndef SCHEMATICCOMMAND_H
#define SCHEMATICCOMMAND_H

#include <qvaluevector.h>
#include <qnamespace.h>

class QPoint;

namespace Spiceplus {

class Schematic;
class SchematicNode;
class SchematicWire;
class SchematicDevice;
class SchematicDeviceState;
class SchematicDevicePin;

class SchematicCommand
{
public:
    virtual ~SchematicCommand() {}

    virtual void execute() = 0;
    virtual void unexecute() = 0;
};

class SchematicCommandGroup: public SchematicCommand
{
public:
    virtual ~SchematicCommandGroup();

    virtual void execute();
    virtual void unexecute();

    void add(SchematicCommand *cmd) { m_commands.append(cmd); }
    bool isEmpty() const { return m_commands.isEmpty(); }

private:
    QValueVector<SchematicCommand *> m_commands;
};

class SchematicCommandMoveDevice : public SchematicCommand
{
public:
    SchematicCommandMoveDevice(SchematicDevice *device, const QPoint &oldPosition);

    virtual void execute();
    virtual void unexecute();

private:
    SchematicDevice *m_device;
    double m_dx;
    double m_dy;
};

class SchematicCommandDeleteDevice : public SchematicCommand
{
public:
    SchematicCommandDeleteDevice(SchematicDevice *device);
    ~SchematicCommandDeleteDevice();

    virtual void execute();
    virtual void unexecute();

private:
    double pinVariance(SchematicDevicePin *pin1, SchematicDevicePin *pin2, SchematicDevicePin *refPin) const;

    SchematicDevice *m_device;
    Schematic *m_schematic;
    SchematicCommandGroup *m_commands;
    bool m_deleted;
};

class SchematicCommandRotateDevice : public SchematicCommand
{
public:
    static const int CCW = 1;
    static const int CW = -1;

    SchematicCommandRotateDevice(SchematicDevice *device, int direction);
    virtual ~SchematicCommandRotateDevice();

    virtual void execute();
    virtual void unexecute();

private:
    SchematicDevice *m_device;
    int m_direction;
    int m_angle;
};

class SchematicCommandFlipDevice : public SchematicCommand
{
public:
    SchematicCommandFlipDevice(SchematicDevice *device, Qt::Orientation o);
    virtual ~SchematicCommandFlipDevice();

    virtual void execute();
    virtual void unexecute();

private:
    SchematicDevice *m_device;
    Qt::Orientation m_orientation;
};

class SchematicCommandDeleteWire : public SchematicCommand
{
public:
    SchematicCommandDeleteWire(SchematicWire *wire);
    virtual ~SchematicCommandDeleteWire();

    virtual void execute();
    virtual void unexecute();

private:
    SchematicWire *m_wire;
    Schematic *m_schematic;
    SchematicDevicePin *m_pin1;
    SchematicDevicePin *m_pin2;
    SchematicDevice *m_junction1;
    SchematicDevice *m_junction2;
    SchematicNode *m_addedNode;
    SchematicNode *m_removedNode;
    SchematicNode *m_groundNode;
    bool m_deleted;
};

class SchematicCommandPlaceWire : public SchematicCommand
{
public:
    SchematicCommandPlaceWire(SchematicWire *wire);
    virtual ~SchematicCommandPlaceWire();

    virtual void execute();
    virtual void unexecute();

private:
    SchematicWire *m_wire;
    Schematic *m_schematic;
    SchematicDevicePin *m_pin1;
    SchematicDevicePin *m_pin2;
    SchematicDevice *m_junction1;
    SchematicDevice *m_junction2;
    SchematicNode *m_node1;
    SchematicNode *m_node2;
    SchematicNode *m_addedNode;
    SchematicNode *m_removedNode;
    bool m_connected;
};

class SchematicCommandPlaceDevice : public SchematicCommand
{
public:
    SchematicCommandPlaceDevice(SchematicDevice *device);
    virtual ~SchematicCommandPlaceDevice();

    virtual void execute();
    virtual void unexecute();

private:
    SchematicDevice *m_device;
    Schematic *m_schematic;
    bool m_added;
};

class SchematicCommandChangeDeviceProperties : public SchematicCommand
{
public:
    SchematicCommandChangeDeviceProperties(SchematicDevice *device, SchematicDeviceState *oldState, SchematicDeviceState *newState);
    virtual ~SchematicCommandChangeDeviceProperties();

    virtual void execute();
    virtual void unexecute();

private:
    SchematicDevice *m_device;
    SchematicDeviceState *m_oldState;
    SchematicDeviceState *m_newState;
};

} // namespace Spiceplus

#endif // SCHEMATICCOMMAND_H

// vim: ts=4 sw=4 et
