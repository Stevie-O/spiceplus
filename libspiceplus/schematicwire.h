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

#ifndef SCHEMATICWIRE_H
#define SCHEMATICWIRE_H

#include "schematic.h"

class QDomElement;

namespace Spiceplus {

class SchematicWire;
class SchematicDevicePin;

class SchematicWireEnd
{
public:
    SchematicWireEnd(SchematicWire *wire);

    SchematicWire *wire() const { return m_wire; }

    SchematicDevicePin *pin() const { return m_pin; }
    SchematicDevicePin *oppositePin() const;
    void connect(SchematicDevicePin *pin);
    void disconnect();

private:
    SchematicWire *m_wire;
    SchematicDevicePin *m_pin;
};

class SchematicWireBase : public SchematicItem
{
public:
    SchematicWireBase(Schematic *schematic);
    ~SchematicWireBase();

    void setPoints(int x1, int y1, int x2, int y2);

    QPoint startPoint() const { return QPoint(x1, y1); }
    QPoint endPoint() const { return QPoint(x2, y2); }

    void setPen(QPen p);
    void moveBy(double dx, double dy);

protected:
    void drawShape(QPainter &p);
    QPointArray areaPoints() const;

private:
    int x1, y1, x2, y2;
};

class SchematicWire : public SchematicWireBase
{
public:
    SchematicWire(Schematic *schematic);
    SchematicWire(SchematicDevicePin *pin1, SchematicDevicePin *pin2, Schematic *schematic);
    ~SchematicWire();

    SchematicWireEnd *end1() const { return m_end1; }
    SchematicWireEnd *end2() const { return m_end2; }

    void updatePosition();
    void raiseToTop();

    static const int RTTI = 1000;
    int rtti() const { return RTTI; }

    bool loadData(const QDomElement &elem);
    void saveData(QDomElement &elem) const;

    bool highlighted() const;
    void setHighlighted(bool yes);

private:
    SchematicWireEnd *m_end1;
    SchematicWireEnd *m_end2;
    bool m_highlighted;
};

} // namespace Spiceplus

#endif // SCHEMATICWIRE_H

// vim: ts=4 sw=4 et
