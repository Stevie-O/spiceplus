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

#include <qpainter.h>
#include <qdom.h>

#include <kdebug.h>

#include "schematicwire.h"
#include "schematicdevice.h"
#include "settings.h"

using namespace Spiceplus;

//
// SchematicWireEnd
//

SchematicWireEnd::SchematicWireEnd(SchematicWire *wire)
    : m_wire(wire), m_pin(0)
{
}

SchematicDevicePin *SchematicWireEnd::oppositePin() const
{
    return m_wire->end1()->pin() != m_pin ? m_wire->end1()->pin() : m_wire->end2()->pin();
}

void SchematicWireEnd::connect(SchematicDevicePin *pin)
{
    m_pin = pin;
    m_pin->addWireEnd(this);
}

void SchematicWireEnd::disconnect()
{
    m_pin->removeWireEnd(this);
    m_pin = 0;
}

//
// SchematicWireBase
//
// This class has been copied from the QCanvasLine source code.
//

SchematicWireBase::SchematicWireBase(Schematic *schematic)
    : SchematicItem(schematic)
{
    x1 = y1 = x2 = y2 = 0;
}

SchematicWireBase::~SchematicWireBase()
{
    hide();
}

void SchematicWireBase::setPen(QPen p)
{
    QCanvasPolygonalItem::setPen(p);
}

void SchematicWireBase::setPoints(int xa, int ya, int xb, int yb)
{
    invalidate();
    if (x1 != xa || x2 != xb || y1 != ya || y2 != yb)
    {
        x1 = xa;
        y1 = ya;
        x2 = xb;
        y2 = yb;
    }
    update();
}

void SchematicWireBase::drawShape(QPainter &p)
{
    p.drawLine((int)(x()+x1), (int)(y()+y1), (int)(x()+x2), (int)(y()+y2));
}

QPointArray SchematicWireBase::areaPoints() const
{
    QPointArray p(4);
    int xi = int(x());
    int yi = int(y());
    int pw = pen().width();
    int dx = QABS(x1-x2);
    int dy = QABS(y1-y2);
    pw = pw*4/3+2; // approx pw*sqrt(2)
    int px = x1<x2 ? -pw : pw ;
    int py = y1<y2 ? -pw : pw ;
    if ( dx && dy && (dx > dy ? (dx*2/dy <= 2) : (dy*2/dx <= 2)) ) {
        // steep
        if ( px == py ) {
            p[0] = QPoint(x1+xi   ,y1+yi+py);
            p[1] = QPoint(x2+xi-px,y2+yi   );
            p[2] = QPoint(x2+xi   ,y2+yi-py);
            p[3] = QPoint(x1+xi+px,y1+yi   );
        } else {
            p[0] = QPoint(x1+xi+px,y1+yi   );
            p[1] = QPoint(x2+xi   ,y2+yi-py);
            p[2] = QPoint(x2+xi-px,y2+yi   );
            p[3] = QPoint(x1+xi   ,y1+yi+py);
        }
    } else if ( dx > dy ) {
        // horizontal
        p[0] = QPoint(x1+xi+px,y1+yi+py);
        p[1] = QPoint(x2+xi-px,y2+yi+py);
        p[2] = QPoint(x2+xi-px,y2+yi-py);
        p[3] = QPoint(x1+xi+px,y1+yi-py);
    } else {
        // vertical
        p[0] = QPoint(x1+xi+px,y1+yi+py);
        p[1] = QPoint(x2+xi+px,y2+yi-py);
        p[2] = QPoint(x2+xi-px,y2+yi-py);
        p[3] = QPoint(x1+xi-px,y1+yi+py);
    }
    return p;
}

void SchematicWireBase::moveBy(double dx, double dy)
{
    QCanvasPolygonalItem::moveBy(dx, dy);
}

//
// SchematicWire
//

SchematicWire::SchematicWire(Schematic *schematic)
    : SchematicWireBase(schematic), m_end1(new SchematicWireEnd(this)), m_end2(new SchematicWireEnd(this)), m_highlighted(false)
{
    raiseToTop();
    setPen(Settings::self()->wireColor());
}

SchematicWire::SchematicWire(SchematicDevicePin *pin1, SchematicDevicePin *pin2, Schematic *schematic)
    : SchematicWireBase(schematic), m_end1(new SchematicWireEnd(this)), m_end2(new SchematicWireEnd(this)), m_highlighted(false)
{
    raiseToTop();
    setPen(Settings::self()->wireColor());
    m_end1->connect(pin1);
    m_end2->connect(pin2);
    updatePosition();
}

SchematicWire::~SchematicWire()
{
    delete m_end1;
    delete m_end2;
}

void SchematicWire::updatePosition()
{
    if (!m_end1->pin() || !m_end2->pin())
    {
        kdError() << k_funcinfo << "Invalid wire" << endl;
        return;
    }

    QPoint p1 = m_end1->pin()->worldPoint();
    QPoint p2 = m_end2->pin()->worldPoint();

    setPoints(p1.x(), p1.y(), p2.x(), p2.y());
}

void SchematicWire::raiseToTop()
{
    setZ(Schematic::nextZIndex());
}

bool SchematicWire::loadData(const QDomElement &elem)
{
    SchematicDevice *dev1 = schematic()->findDevice(elem.attribute("device-name1"));
    if (!dev1)
        return false;

    SchematicDevice *dev2 = schematic()->findDevice(elem.attribute("device-name2"));
    if (!dev2)
        return false;

    SchematicDevicePin *pin1 = dev1->findPin(elem.attribute("device-pin-id1"));
    if (!pin1)
        return false;

    SchematicDevicePin *pin2 = dev2->findPin(elem.attribute("device-pin-id2"));
    if (!pin2)
        return false;

    m_end1->connect(pin1);
    m_end2->connect(pin2);

    updatePosition();

    return true;
}

void SchematicWire::saveData(QDomElement &elem) const
{
    if (!m_end1->pin() || !m_end2->pin())
    {
        kdError() << k_funcinfo << "Invalid wire" << endl;
        return;
    }

    elem.setAttribute("device-name1", m_end1->pin()->device()->name());
    elem.setAttribute("device-name2", m_end2->pin()->device()->name());
    elem.setAttribute("device-pin-id1", m_end1->pin()->id());
    elem.setAttribute("device-pin-id2", m_end2->pin()->id());
}

bool SchematicWire::highlighted() const
{
    return m_highlighted;
}

void SchematicWire::setHighlighted(bool yes)
{
    if (yes)
        setPen(Settings::self()->wireColorHighlighted());
    else
        setPen(Settings::self()->wireColor());

    m_highlighted = yes;
}

// vim: ts=4 sw=4 et
