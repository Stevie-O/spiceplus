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

#include <qwmatrix.h>
#include <qcanvas.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstatusbar.h>

#include "mainwindow.h"
#include "schematictool.h"
#include "schematicview.h"
#include "schematiccommand.h"
#include "schematiccommandhistory.h"
#include "schematic.h"
#include "schematicwire.h"
#include "schematicjunction.h"
#include "schematicground.h"
#include "schematictestpoint.h"
#include "schematicammeter.h"
#include "settings.h"
#include "devicepropertiesdialog.h"

using namespace Spiceplus;

//
// SchematicTool
//

SchematicTool::SchematicTool(SchematicView *view)
    : m_view(view)
{
    view->setTool(this);
}

SchematicTool::~SchematicTool()
{
}

int SchematicTool::nextStep(int i)
{
    int s = Settings::self()->gridSize();
    return i - i % s + (i % s >= s / 2 ? s : 0);
}

QPoint SchematicTool::nextStep(const QPoint &p)
{
    return QPoint(nextStep(p.x()), nextStep(p.y()));
}

//
// SchematicToolDeviceConnector
//

SchematicToolDeviceConnector::SchematicToolDeviceConnector(Schematic *schematic)
    : m_schematic(schematic)
{
}

void SchematicToolDeviceConnector::placeConnectionMarks(SchematicDevice *device)
{
    const QValueVector<SchematicDevicePin *> pins = device->pins();
    for (size_t i = 0; i < pins.count(); ++i)
    {
        if (device->id() != SchematicJunction::ID && pins[i]->wireCount() > 0 || pins[i]->wireCount() > 1)
            continue;

        SchematicDevicePin *pin = m_schematic->findPinExcluding(pins[i]->worldPoint(), pins[i]);
        if (pin)
        {
            if (!canConnectItem(pin->device()))
                pin = 0;
            else if (pin->device()->id() == SchematicJunction::ID)
                pin = pins[i];
        }
        else
        {
            SchematicWire *wire;
            if (pins[i]->wireCount() > 0)
                wire = m_schematic->findWireExcluding(pins[i]->worldPoint(), pins[i]->wireEnds().first()->wire());
            else
                wire = m_schematic->findWire(pins[i]->worldPoint());

            if (wire && canConnectItem(wire))
                pin = pins[i];
        }

        if (!pin)
            continue;

        QCanvasRectangle *r = new QCanvasRectangle(pin->worldPoint().x() - Settings::self()->gridSize(),
                                                   pin->worldPoint().y() - Settings::self()->gridSize(),
                                                   Settings::self()->gridSize() * 2,
                                                   Settings::self()->gridSize() * 2,
                                                   m_schematic);
        r->setPen(Settings::self()->connectionMarkColor());
        r->show();
        m_connectionMarks.append(r);
    }
}

void SchematicToolDeviceConnector::deleteConnectionMarks()
{
    for (size_t i = 0; i < m_connectionMarks.count(); ++i)
        delete m_connectionMarks[i];
    m_connectionMarks.clear();
}

void SchematicToolDeviceConnector::connectDevice(SchematicDevice *device, SchematicCommandGroup *cmdGroup)
{
    const QValueVector<SchematicDevicePin *> pins = device->pins();

    if (pins.count() == 2 && pins[0]->wireCount() > 0 && pins[1]->wireCount() > 0)
        return;

    SchematicDevicePin *connPins[] = { 0, 0 };

    for (size_t i = 0; i < pins.count(); ++i)
    {
        if (device->id() != SchematicJunction::ID && pins[i]->wireCount() > 0 || pins[i]->wireCount() > 1)
            continue;

        SchematicDevicePin *pin = m_schematic->findPinExcluding(pins[i]->worldPoint(), pins[i]);
        if (pin && canConnectItem(pin->device()))
        {
            if (device->id() == SchematicJunction::ID)
            {
                SchematicWireEnd *wireEnd = static_cast<SchematicJunction *>(device)->pin()->wireEnds().first();
                SchematicWire *w = new SchematicWire(wireEnd->oppositePin(), pin, m_schematic);
                w->show();
                cmdGroup->add(new SchematicCommandPlaceWire(w));
                
                SchematicCommand *cmd = new SchematicCommandDeleteWire(wireEnd->wire());
                cmd->execute();
                cmdGroup->add(cmd);

                connectorReplacedItem(wireEnd->wire(), w);
                connectorReplacedItem(device, pin->device());
            }
            else if (pin->device()->id() == SchematicJunction::ID)
            {
                QValueList<SchematicWireEnd *> wireEnds = pin->wireEnds();
                for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
                {
                    SchematicWire *w = new SchematicWire((*it)->oppositePin(), pins[i], m_schematic);
                    w->show();
                    cmdGroup->add(new SchematicCommandPlaceWire(w));
                    
                    SchematicCommand *cmd = new SchematicCommandDeleteWire((*it)->wire());
                    cmd->execute();
                    cmdGroup->add(cmd);
                }
            }
            else
            {
                if (pins.count() == 2)
                    connPins[i] = pin;

                SchematicWire *w = new SchematicWire(pin, pins[i], m_schematic);
                w->show();
                cmdGroup->add(new SchematicCommandPlaceWire(w));
            }
        }
        else
        {
            SchematicWire *wire;
            if (pins[i]->wireCount() > 0)
                wire = m_schematic->findWireExcluding(pins[i]->worldPoint(), pins[i]->wireEnds().first()->wire());
            else
                wire = m_schematic->findWire(pins[i]->worldPoint());
                
            if (wire && canConnectItem(wire))
            {
                SchematicWire *w = new SchematicWire(wire->end1()->pin(), pins[i], m_schematic);
                w->show();
                cmdGroup->add(new SchematicCommandPlaceWire(w));

                w = new SchematicWire(wire->end2()->pin(), pins[i], m_schematic);
                w->show();
                cmdGroup->add(new SchematicCommandPlaceWire(w));

                SchematicCommand *cmd = new SchematicCommandDeleteWire(wire);
                cmd->execute();
                cmdGroup->add(cmd);
            }
        }
    }

    // check if we created a bypass at a 2-pin device; if yes, remove it
    if (pins.count() == 2)
    {
        QValueList<SchematicWireEnd *> wireEnds = pins[0]->wireEnds();
        for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
        {
            if ((*it)->oppositePin() == pins[1])
            {
                SchematicCommand *cmd = new SchematicCommandDeleteWire((*it)->wire());
                cmd->execute();
                cmdGroup->add(cmd);
            }
        }

        if (connPins[0])
        {
            QValueList<SchematicWireEnd *> wireEnds = connPins[0]->wireEnds();
            for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
            {
                if ((*it)->oppositePin() == pins[1])
                {
                    SchematicCommand *cmd = new SchematicCommandDeleteWire((*it)->wire());
                    cmd->execute();
                    cmdGroup->add(cmd);
                }
            }
        }

        if (connPins[1])
        {
            QValueList<SchematicWireEnd *> wireEnds = connPins[1]->wireEnds();
            for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
            {
                if ((*it)->oppositePin() == pins[0])
                {
                    SchematicCommand *cmd = new SchematicCommandDeleteWire((*it)->wire());
                    cmd->execute();
                    cmdGroup->add(cmd);
                }
            }
        }
    }
}

//
// SchematicToolSelect
//

SchematicToolSelect::SchematicToolSelect(SchematicView *view)
    : SchematicTool(view), SchematicToolDeviceConnector(view->schematic()),
      m_selectionArea(0),
      m_deviceSelectionMoved(false),
      m_settingProperties(false),
      m_isDeleteAvailable(false),
      m_isSelectAllAvailable(true),
      m_isRotateAvailable(false),
      m_isFlipAvailable(false)
{
}

SchematicToolSelect::~SchematicToolSelect()
{
    reset();
}

SchematicToolSelect::DeviceState::DeviceState()
    : device(0)
{
}

SchematicToolSelect::DeviceState::DeviceState(SchematicDevice *dev, const QPoint &p)
    : device(dev), oldPosition(dev->position()), origin(p - dev->position())
{
}

void SchematicToolSelect::processLeaveEvent(QEvent *)
{
    unhighlightItems();
    m_view->schematic()->update();
}

void SchematicToolSelect::processMouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (!m_deviceSelectionMoved)
        {
            m_deviceSelection.clear();
            m_settingProperties = true;
        }
    }
    else
        processMousePressEvent(event);
}

void SchematicToolSelect::processMousePressEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (event->button() == Qt::LeftButton)
    {
        emit selectAllAvailable(m_isSelectAllAvailable = false);

        if (m_highlightedItems.count() > 0)
        {
            if (selectionContains(m_highlightedItems[0]))
            {
                for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
                {
                    (*it).item->raiseToTop();
                    deviceSelectionAdd((*it).item, p);
                }
            }
            else
            {
                if (!(event->state() & Qt::ControlButton))
                    unselectItems();

                m_highlightedItems[0]->raiseToTop();
                deviceSelectionAdd(m_highlightedItems[0], p);
            }
        }
        else
        {
            unselectItems();
            m_selectionArea = new QCanvasRectangle(p.x(), p.y(), 0, 0, m_view->schematic());
            m_selectionArea->setPen(QPen(Settings::self()->selectionMarkColor(), 0, Qt::DotLine));
        }
        m_view->schematic()->update();

        m_deviceSelectionMoved = false;
        m_settingProperties = false;
    }
}

void SchematicToolSelect::processMouseReleaseEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (event->button() == Qt::LeftButton)
    {
        if (m_selectionArea)
        {
            selectItems();
            unhighlightItems();

            delete m_selectionArea;
            m_selectionArea = 0;

            highlightItem(p);
            m_view->schematic()->update();
        }
        else if (m_settingProperties)
        {
            SchematicDevice *dev = highlightedDevice();
            if (dev && dev->hasPropertiesWidget())
            {
                SchematicDeviceState *oldState = dev->createState();

                DevicePropertiesDialog *dlg = new DevicePropertiesDialog(dev, m_view);
                if (dlg->exec() == KDialogBase::Ok)
                {
                    SchematicDeviceState *newState = dev->createState();
                    SchematicCommand *cmd = new SchematicCommandChangeDeviceProperties(dev, oldState, newState);
                    m_view->history()->add(cmd);
                    deleteSelectionMarks();
                    updateSelectionMarks();
                }
                else
                    delete oldState;

                delete dlg;
                m_view->schematic()->update();
            }
        }
        else if (m_deviceSelectionMoved)
        {
            unhighlightItems();
            highlightItem(p);

            updateSelectionMarks();
            deleteConnectionMarks();
            SchematicCommandGroup *cmdGroup = new SchematicCommandGroup;

            for (size_t i = 0; i < m_deviceSelection.count(); ++i)
            {
                if (m_deviceSelection[i].device->position() != m_deviceSelection[i].oldPosition)
                {
                    cmdGroup->add(new SchematicCommandMoveDevice(m_deviceSelection[i].device, m_deviceSelection[i].oldPosition));
                    connectDevice(m_deviceSelection[i].device, cmdGroup);
                }
            }

            if (cmdGroup->isEmpty())
                delete cmdGroup;
            else
                m_view->history()->add(cmdGroup);

            m_view->schematic()->update();
            m_deviceSelection.clear();
        }
        else
        {
            m_deviceSelection.clear();

            if (m_highlightedItems.count() > 0)
            {
                if (selectionContains(m_highlightedItems[0]))
                {
                    if (event->state() & Qt::ControlButton)
                        unselectItem(m_highlightedItems[0]);
                    else
                    {
                        unselectItems();
                        selectItems();
                    }
                }
                else
                    selectItems();
    
                m_view->schematic()->update();
            }
        }

        emit selectAllAvailable(m_isSelectAllAvailable = true);
    }
}

void SchematicToolSelect::processMouseMoveEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (m_selectionArea)
    {
        unhighlightItems();

        m_selectionArea->show();
        m_selectionArea->setSize(p.x() - static_cast<int>(m_selectionArea->x()) + 1, p.y() - static_cast<int>(m_selectionArea->y()) + 1);

        highlightItems(m_selectionArea->rect());
        m_view->schematic()->update();
    }
    else if (!m_deviceSelection.isEmpty())
    {
        // check if all devices stay on the schematic when moving and if they move at all
        bool move = true;
        for (size_t i = 0; i < m_deviceSelection.count(); ++i)
        {
            int x = nextStep(p.x() - m_deviceSelection[i].origin.x());
            int y = nextStep(p.y() - m_deviceSelection[i].origin.y());
            if (x == m_deviceSelection[i].device->x() && y == m_deviceSelection[i].device->y() || x < 0 || y < 0)
            {
                move = false;
                break;
            }
        }

        if (move)
        {
            deleteSelectionMarks();
            deleteConnectionMarks();
            for (size_t i = 0; i < m_deviceSelection.count(); ++i)
            {
                m_deviceSelection[i].device->move(nextStep(p.x() - m_deviceSelection[i].origin.x()), nextStep(p.y() - m_deviceSelection[i].origin.y()));
                placeConnectionMarks(m_deviceSelection[i].device);
            }
            m_deviceSelectionMoved = true;
            m_view->schematic()->update();
        }
    }
    else
    {
        unhighlightItems();
        highlightItem(p);
        m_view->schematic()->update();
    }
}

bool SchematicToolSelect::canConnectItem(SchematicItem *item) const
{
    return !selectionContains(item);
}

void SchematicToolSelect::connectorReplacedItem(SchematicItem *oldItem, SchematicItem *newItem)
{
    if (m_selection.count() > 0)
    {
        unselectItem(oldItem);
        selectItem(newItem);
    }
}

void SchematicToolSelect::reset()
{
    if (!m_deviceSelection.isEmpty())
    {
        for (size_t i = 0; i < m_deviceSelection.count(); ++i)
            m_deviceSelection[i].device->setPosition(m_deviceSelection[i].oldPosition);
        m_deviceSelection.clear();
    }

    unselectItems();
    unhighlightItems();
    deleteConnectionMarks();
    delete m_selectionArea;

    m_view->schematic()->update();
}

void SchematicToolSelect::deleteSelected()
{
    SchematicCommandGroup *cmdGroup = new SchematicCommandGroup;

    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        if ((*it).item->rtti() == SchematicWire::RTTI)
        {
            SchematicCommand *cmd = new SchematicCommandDeleteWire(static_cast<SchematicWire *>((*it).item));
            cmd->execute();
            cmdGroup->add(cmd);
        }
    }

    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        if ((*it).item->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>((*it).item);
            if (dev->id() != SchematicJunction::ID)
            {
                SchematicCommand *cmd = new SchematicCommandDeleteDevice(dev);
                cmd->execute();
                cmdGroup->add(cmd);
            }
        }
    }

    unselectItems();

    if (!cmdGroup->isEmpty())
        m_view->history()->add(cmdGroup);
    else
        delete cmdGroup;

    m_view->schematic()->update();
}

void SchematicToolSelect::rotate(int direction)
{
    SchematicCommandGroup *cmdGroup = new SchematicCommandGroup;

    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        if ((*it).item->rtti() == SchematicDevice::RTTI)
        {
            SchematicCommand *cmd = new SchematicCommandRotateDevice(static_cast<SchematicDevice *>((*it).item), direction);
            cmd->execute();
            cmdGroup->add(cmd);
        }
    }

    if (!cmdGroup->isEmpty())
    {
        deleteSelectionMarks();
        updateSelectionMarks();
        m_view->schematic()->update();
        m_view->history()->add(cmdGroup);
    }
    else
        delete cmdGroup;
}

void SchematicToolSelect::selectAll()
{
    unselectItems();

    QCanvasItemList l = m_view->schematic()->allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        SchematicItem *item = dynamic_cast<SchematicItem *>(*it);
        if (item)
            selectItem(item);
    }

    m_view->schematic()->update();
}

void SchematicToolSelect::rotateCCW()
{
    rotate(SchematicCommandRotateDevice::CCW);
}

void SchematicToolSelect::rotateCW()
{
    rotate(SchematicCommandRotateDevice::CW);
}

void SchematicToolSelect::flip(Qt::Orientation o)
{
    SchematicCommandGroup *cmdGroup = new SchematicCommandGroup;

    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        if ((*it).item->rtti() == SchematicDevice::RTTI)
        {
            SchematicCommand *cmd = new SchematicCommandFlipDevice(static_cast<SchematicDevice *>((*it).item), o);
            cmd->execute();
            cmdGroup->add(cmd);
        }
    }

    if (!cmdGroup->isEmpty())
    {
        deleteSelectionMarks();
        updateSelectionMarks();
        m_view->schematic()->update();
        m_view->history()->add(cmdGroup);
    }
    else
        delete cmdGroup;
}

void SchematicToolSelect::flipHorizontal()
{
    flip(Qt::Horizontal);
}

void SchematicToolSelect::flipVertical()
{
    flip(Qt::Vertical);
}

void SchematicToolSelect::highlightItem(const QPoint &p)
{
    SchematicItem *item = m_view->schematic()->findItem(p);
    if (item)
    {
        item->setHighlighted(true);
        m_highlightedItems.append(item);
    }
}

void SchematicToolSelect::highlightItems(const QRect &r)
{
    QCanvasItemList l = m_view->schematic()->collisions(r);
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        SchematicItem *item = dynamic_cast<SchematicItem *>(*it);
        if (item)
        {
            item->setHighlighted(true);
            m_highlightedItems.append(item);
        }
    }
}

void SchematicToolSelect::unhighlightItems()
{
    for (size_t i = 0; i < m_highlightedItems.count(); ++i)
        m_highlightedItems[i]->setHighlighted(false);
    m_highlightedItems.clear();
}

SchematicDevice *SchematicToolSelect::highlightedDevice() const
{
    return m_highlightedItems.count() > 0 ? dynamic_cast<SchematicDevice *>(m_highlightedItems[0]) : 0;
}

void SchematicToolSelect::selectItem(SchematicItem *item)
{
    int b = Settings::self()->gridSize() / 2;
    m_isDeleteAvailable = m_isRotateAvailable = m_isFlipAvailable = false;

    QRect r = item->boundingRect();
    r.addCoords(-b, -b, b, b);

    SelectionItem si;
    si.item = item;
    si.selectionMark = new QCanvasRectangle(r, m_view->schematic());
    si.selectionMark->setPen(QPen(Settings::self()->selectionMarkColor(), 0, Qt::DotLine));
    si.selectionMark->show();
    m_selection.append(si);

    setEditFlags(si.item);

    emit deleteAvailable(m_isDeleteAvailable);
    emit rotateAvailable(m_isRotateAvailable);
    emit flipAvailable(m_isFlipAvailable);
}

void SchematicToolSelect::selectItems()
{
    int b = Settings::self()->gridSize() / 2;
    m_isDeleteAvailable = m_isRotateAvailable = m_isFlipAvailable = false;

    for (size_t i = 0; i < m_highlightedItems.count(); ++i)
    {
        QRect r = m_highlightedItems[i]->boundingRect();
        r.addCoords(-b, -b, b, b);

        SelectionItem si;
        si.item = m_highlightedItems[i];
        si.selectionMark = new QCanvasRectangle(r, m_view->schematic());
        si.selectionMark->setPen(QPen(Settings::self()->selectionMarkColor(), 0, Qt::DotLine));
        si.selectionMark->show();
        m_selection.append(si);

        setEditFlags(si.item);
    }

    emit deleteAvailable(m_isDeleteAvailable);
    emit rotateAvailable(m_isRotateAvailable);
    emit flipAvailable(m_isFlipAvailable);
}

void SchematicToolSelect::unselectItem(SchematicItem *item)
{
    m_isDeleteAvailable = m_isRotateAvailable = m_isFlipAvailable = false;

    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        if ((*it).item == item)
        {
            delete (*it).selectionMark;
            if ((it = m_selection.remove(it)) == m_selection.end())
                break;    
        }

        setEditFlags((*it).item);
    }

    emit deleteAvailable(m_isDeleteAvailable);
    emit rotateAvailable(m_isRotateAvailable);
    emit flipAvailable(m_isFlipAvailable);
}

void SchematicToolSelect::unselectItems()
{
    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
        delete (*it).selectionMark;
    m_selection.clear();

    emit deleteAvailable(m_isDeleteAvailable = false);
    emit rotateAvailable(m_isRotateAvailable = false);
    emit flipAvailable(m_isFlipAvailable = false);
}

bool SchematicToolSelect::selectionContains(SchematicItem *item) const
{
    for (QValueList<SelectionItem>::ConstIterator it = m_selection.begin(); it != m_selection.end(); ++it)
        if ((*it).item == item)
            return true;
    return false;
}

void SchematicToolSelect::deleteSelectionMarks()
{
    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        delete (*it).selectionMark;
        (*it).selectionMark = 0;
    }

    emit deleteAvailable(m_isDeleteAvailable = false);
    emit rotateAvailable(m_isRotateAvailable = false);
    emit flipAvailable(m_isFlipAvailable = false);
}

void SchematicToolSelect::updateSelectionMarks()
{
    int b = Settings::self()->gridSize() / 2;
    m_isDeleteAvailable = m_isRotateAvailable = m_isFlipAvailable = false;

    for (QValueList<SelectionItem>::Iterator it = m_selection.begin(); it != m_selection.end(); ++it)
    {
        QRect r = (*it).item->boundingRect();
        r.addCoords(-b, -b, b, b);

        delete (*it).selectionMark;
        (*it).selectionMark = new QCanvasRectangle(r, m_view->schematic());
        (*it).selectionMark->setPen(QPen(Settings::self()->selectionMarkColor(), 0, Qt::DotLine));
        (*it).selectionMark->show();

        setEditFlags((*it).item);
    }

    emit deleteAvailable(m_isDeleteAvailable);
    emit rotateAvailable(m_isRotateAvailable);
    emit flipAvailable(m_isFlipAvailable);
}

void SchematicToolSelect::setEditFlags(SchematicItem *item)
{
    if (item->rtti() == SchematicDevice::RTTI && static_cast<SchematicDevice *>(item)->id() != SchematicJunction::ID ||
        item->rtti() == SchematicWire::RTTI)
    {
        m_isDeleteAvailable = true;
        if (item->rtti() != SchematicWire::RTTI)
            m_isRotateAvailable = m_isFlipAvailable = true;
    }
}

bool SchematicToolSelect::deviceSelectionContains(SchematicDevice *device) const
{
    for (size_t i = 0; i < m_deviceSelection.count(); ++i)
        if (m_deviceSelection[i].device == device)
            return true;
    return false;
}

void SchematicToolSelect::deviceSelectionAdd(SchematicItem *item, const QPoint &p)
{
    if (item->rtti() == SchematicDevice::RTTI)
    {
        SchematicDevice *dev = static_cast<SchematicDevice *>(item);
        if (!deviceSelectionContains(dev))
            m_deviceSelection.append(DeviceState(dev, p));
    }
    else if (item->rtti() == SchematicWire::RTTI)
    {
        SchematicWire *wire = static_cast<SchematicWire *>(item);
        SchematicDevice *dev;

        dev = wire->end1()->pin()->device();
        if (dev->id() == SchematicJunction::ID && !deviceSelectionContains(dev))
        {
            dev->raiseToTop();
            m_deviceSelection.append(DeviceState(dev, p));
        }

        dev = wire->end2()->pin()->device();
        if (dev->id() == SchematicJunction::ID && !deviceSelectionContains(dev))
        {
            dev->raiseToTop();
            m_deviceSelection.append(DeviceState(dev, p));
        }
    }
}

//
// SchematicToolPlaceWire
//

SchematicToolPlaceWire::SchematicToolPlaceWire(SchematicView *view)
    : SchematicTool(view), m_wire(0), m_wireMoved(false), m_wireCommands(0), m_connectionMark(0)
{
}

SchematicToolPlaceWire::~SchematicToolPlaceWire()
{
    reset();
}

void SchematicToolPlaceWire::processLeaveEvent(QEvent *)
{
    deleteConnectionMark();
    m_view->schematic()->update();
}

void SchematicToolPlaceWire::processMouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_wire)
    {
        MainWindow::self()->statusBar()->changeItem("", 0);

        m_wireCommands->unexecute();
        m_wireCommands = 0;
        m_view->schematic()->update();
        m_wireMoved = false;
        m_wire = 0;
    }
}

void SchematicToolPlaceWire::processMousePressEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (event->button() == Qt::LeftButton)
    {
        SchematicDevicePin *wirePin = setupWirePin(p);
        if (wirePin)
        {
            if (m_wire)
            {
                m_view->history()->add(m_wireCommands);
                m_wireCommands = new SchematicCommandGroup;
            }

            SchematicJunction *jun2 = new SchematicJunction(nextStep(p), m_view->schematic());
            jun2->show();

            m_wire = new SchematicWire(wirePin, jun2->pin(), m_view->schematic());
            m_wire->setHighlighted(true);
            m_wire->show();
            m_view->schematic()->update();

            m_wireCommands->add(new SchematicCommandPlaceWire(m_wire));
        }
    }
}

void SchematicToolPlaceWire::processMouseReleaseEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (event->button() == Qt::LeftButton && m_wireMoved && setupWirePin(p))
    {
        m_view->history()->add(m_wireCommands);
        m_wireCommands = 0;
        m_view->schematic()->update();
        m_wireMoved = false;
        m_wire = 0;
    }
    else if (event->button() == Qt::LeftButton && m_wire)
        MainWindow::self()->statusBar()->changeItem(i18n("Double click to terminate wire"), 0);
}

void SchematicToolPlaceWire::processMouseMoveEvent(QMouseEvent *event)
{
    QPoint p = nextStep(m_view->inverseWorldMatrix().map(event->pos()));

    deleteConnectionMark();
    if (m_wire && (m_wire->end2()->pin()->device()->x() != p.x() || m_wire->end2()->pin()->device()->y() != p.y()))
    {
        m_wire->end2()->pin()->device()->move(p.x(), p.y());
        m_wireMoved = true;
    }
    placeConnectionMark(p);
    m_view->schematic()->update();
}

void SchematicToolPlaceWire::reset()
{
    MainWindow::self()->statusBar()->changeItem("", 0);

    if (m_wire)
    {
        m_wireCommands->unexecute();
        m_wireCommands = 0;
        m_wireMoved = false;
        m_wire = 0;
    }
    deleteConnectionMark();
    m_view->schematic()->update();
}

void SchematicToolPlaceWire::placeConnectionMark(const QPoint &p)
{
    SchematicDevicePin *pin;
    if (!m_wire)
        pin = m_view->schematic()->findPin(p);
    else
    {
        pin = m_view->schematic()->findPinExcluding(p, m_wire->end2()->pin());
        if (pin == m_wire->end1()->pin())
            return;
    }

    SchematicWire *wire = 0;
    if (!pin)
    {
        if (!m_wire)
            wire = m_view->schematic()->findWire(p);
        else
            wire = m_view->schematic()->findWireExcluding(p, m_wire);
    }

    if (!pin && !wire)
        return;

    bool isRedundant = false;

    if (m_wire)
    {
        SchematicNode *node = m_wire->end1()->pin()->node();
        if (pin && pin->node() == node || wire && wire->end1()->pin()->node() == node || pin && pin->device()->id() == SchematicGround::ID && node->name() == "0")
            isRedundant = true;
    }

    QPoint connPoint;

    if (pin)
        connPoint = pin->worldPoint();
    else
        connPoint = p;

    m_connectionMark = new QCanvasRectangle(connPoint.x() - Settings::self()->gridSize(),
                                            connPoint.y() - Settings::self()->gridSize(),
                                            Settings::self()->gridSize() * 2,
                                            Settings::self()->gridSize() * 2,
                                            m_view->schematic());
    m_connectionMark->setPen(isRedundant ? Settings::self()->connectionMarkColorRedundant() : Settings::self()->connectionMarkColor());
    m_connectionMark->show();
}

void SchematicToolPlaceWire::deleteConnectionMark()
{
    delete m_connectionMark;
    m_connectionMark = 0;
}

SchematicDevicePin *SchematicToolPlaceWire::setupWirePin(const QPoint &p)
{
    SchematicDevicePin *wirePin = 0, *pin;

    if (!m_wire)
    {
        m_wireCommands = new SchematicCommandGroup;
        pin = m_view->schematic()->findPin(p);
    }
    else
        pin = m_view->schematic()->findPinExcluding(p, m_wire->end2()->pin());

    if (pin)
    {
        if (!m_wire)
            wirePin = pin;
        else if (pin != m_wire->end1()->pin())
        {
            m_wire->setHighlighted(false);
            wirePin = pin;

            SchematicWire *w = new SchematicWire(m_wire->end1()->pin(), wirePin, m_view->schematic());
            w->show();
            m_wireCommands->add(new SchematicCommandPlaceWire(w));

            SchematicCommand *cmd = new SchematicCommandDeleteWire(m_wire);
            cmd->execute();
            m_wireCommands->add(cmd);

            m_wire = w;
        }
    }
    else
    {
        if (!m_wire)
        {
            SchematicJunction *jun1 = new SchematicJunction(nextStep(p), m_view->schematic());
            jun1->show();
            wirePin = jun1->pin();
        }
        else
        {
            m_wire->setHighlighted(false);
            wirePin = m_wire->end2()->pin();
        }

        SchematicWire *wire = m_view->schematic()->findWireExcluding(p, m_wire);
        if (wire)
        {
            SchematicWire *w = new SchematicWire(wire->end1()->pin(), wirePin, m_view->schematic());
            w->show();
            m_wireCommands->add(new SchematicCommandPlaceWire(w));

            w = new SchematicWire(wire->end2()->pin(), wirePin, m_view->schematic());
            w->show();
            m_wireCommands->add(new SchematicCommandPlaceWire(w));

            SchematicCommand *cmd = new SchematicCommandDeleteWire(wire);
            cmd->execute();
            m_wireCommands->add(cmd);
        }
    }

    return wirePin;
}

//
// SchematicToolSplitWire
//

SchematicToolSplitWire::SchematicToolSplitWire(SchematicView *view)
    : SchematicTool(view), m_highlightedWire(0), m_junction(0)
{
}

SchematicToolSplitWire::~SchematicToolSplitWire()
{
    reset();
}

void SchematicToolSplitWire::processLeaveEvent(QEvent *)
{
    unhighlight();
    m_junction = 0;
    m_view->schematic()->update();
}

void SchematicToolSplitWire::processMousePressEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (m_highlightedWire)
    {
        m_junction = new SchematicJunction(nextStep(p), m_view->schematic());
        m_junction->show();
        m_junction->setHighlighted(true);

        SchematicCommandGroup *cmdGroup = new SchematicCommandGroup;

        SchematicWire *w = new SchematicWire(m_highlightedWire->end1()->pin(), m_junction->pin(), m_view->schematic());
        w->show();
        cmdGroup->add(new SchematicCommandPlaceWire(w));

        w = new SchematicWire(m_highlightedWire->end2()->pin(), m_junction->pin(), m_view->schematic());
        w->show();
        cmdGroup->add(new SchematicCommandPlaceWire(w));

        SchematicCommand *cmd = new SchematicCommandDeleteWire(m_highlightedWire);
        cmd->execute();
        cmdGroup->add(cmd);

        m_view->history()->add(cmdGroup);
        m_view->schematic()->update();
    }
}

void SchematicToolSplitWire::processMouseReleaseEvent(QMouseEvent *)
{
    if (m_junction)
    {
        m_junction->setHighlighted(false);
        m_junction = 0;
        m_view->schematic()->update();
    }
}

void SchematicToolSplitWire::processMouseMoveEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (m_junction)
        m_junction->move(nextStep(p.x()), nextStep(p.y()));
    else
    {
        unhighlight();
        highlight(p);
    }
    m_view->schematic()->update();
}

void SchematicToolSplitWire::reset()
{
    unhighlight();
    m_view->schematic()->update();
}

void SchematicToolSplitWire::highlight(const QPoint &p)
{
    SchematicWire *wire = m_view->schematic()->findWire(p);
    if (wire)
    {
        wire->setHighlighted(true);
        m_highlightedWire = wire;
    }
}

void SchematicToolSplitWire::unhighlight()
{
    if (m_highlightedWire)
    {
        m_highlightedWire->setHighlighted(false);
        m_highlightedWire = 0;
    }
}

//
// SchematicToolPlaceDeviceBase
//

SchematicToolPlaceDeviceBase::SchematicToolPlaceDeviceBase(SchematicView *view)
    : SchematicTool(view), SchematicToolDeviceConnector(view->schematic()),
      m_device(0),
      m_deviceState(0),
      m_isRotateAvailable(false),
      m_isFlipAvailable(false)
{
}

SchematicToolPlaceDeviceBase::~SchematicToolPlaceDeviceBase()
{
    if (m_device)
    {
        deleteConnectionMarks();
        delete m_device;
        m_view->schematic()->update();

        MainWindow::self()->statusBar()->changeItem("", 0);
        emit rotateAvailable(false);
        emit flipAvailable(false);
    }
    delete m_deviceState;
}

void SchematicToolPlaceDeviceBase::processMouseDoubleClickEvent(QMouseEvent *event)
{
    processMousePressEvent(event);
}

void SchematicToolPlaceDeviceBase::processMousePressEvent(QMouseEvent *event)
{
    if (!m_device || !m_device->isVisible())
        return;

    if (event->button() == Qt::LeftButton)
    {
        delete m_deviceState;
        m_deviceState = m_device->createState();

        deleteConnectionMarks();
        m_device->setHighlighted(false);

        SchematicCommandGroup *cmdGroup = new SchematicCommandGroup;

        SchematicCommand *cmd = new SchematicCommandPlaceDevice(m_device);
        cmdGroup->add(cmd);

        connectDevice(m_device, cmdGroup);
        m_view->history()->add(cmdGroup);

        m_view->schematic()->update();
        selectNewDevice();

        MainWindow::self()->statusBar()->changeItem("", 0);
        emit rotateAvailable(m_isRotateAvailable = false);
        emit flipAvailable(m_isFlipAvailable = false);
    }
}

void SchematicToolPlaceDeviceBase::processMouseMoveEvent(QMouseEvent *event)
{
    QPoint p = m_view->inverseWorldMatrix().map(event->pos());

    if (m_device)
    {
        deleteConnectionMarks();

        m_device->move(nextStep(p.x()), nextStep(p.y()));
        if (!m_device->isVisible())
        {
            m_device->show();

            MainWindow::self()->statusBar()->changeItem(i18n("Press r or R to rotate device and f or F to flip device"), 0);
            emit rotateAvailable(m_isRotateAvailable = true);
            emit flipAvailable(m_isFlipAvailable = true);
        }

        placeConnectionMarks(m_device);
        m_view->schematic()->update();
    }
}

void SchematicToolPlaceDeviceBase::reset()
{
    if (m_device)
    {
        deleteConnectionMarks();
        m_device->hide();
        m_view->schematic()->update();

        MainWindow::self()->statusBar()->changeItem("", 0);
        emit rotateAvailable(m_isRotateAvailable = false);
        emit flipAvailable(m_isFlipAvailable = false);
    }
}

void SchematicToolPlaceDeviceBase::rotateCCW()
{
    deleteConnectionMarks();
    m_device->rotate(Settings::self()->deviceRotationAngle());
    placeConnectionMarks(m_device);
    m_view->schematic()->update();
}

void SchematicToolPlaceDeviceBase::rotateCW()
{
    deleteConnectionMarks();
    m_device->rotate(-Settings::self()->deviceRotationAngle());
    placeConnectionMarks(m_device);
    m_view->schematic()->update();
}

void SchematicToolPlaceDeviceBase::flipHorizontal()
{
    deleteConnectionMarks();
    m_device->flip(Qt::Horizontal);
    placeConnectionMarks(m_device);
    m_view->schematic()->update();
}

void SchematicToolPlaceDeviceBase::flipVertical()
{
    deleteConnectionMarks();
    m_device->flip(Qt::Vertical);
    placeConnectionMarks(m_device);
    m_view->schematic()->update();
}

void SchematicToolPlaceDeviceBase::selectNewDevice()
{
    if ((m_device = createDevice())) // double-parenthesis to make g++ happy ;)
    {
        if (m_device->initialize())
        {
            if (m_deviceState)
                m_device->restoreState(m_deviceState);

            m_device->setDefaults();
            m_device->setHighlighted(true);
        }
        else
        {
            KMessageBox::error(m_view, m_device->errorString());
            delete m_device;
            m_device = 0;
        }
    }
}

//
// SchematicToolPlaceDevice
//

SchematicToolPlaceDevice::SchematicToolPlaceDevice(SchematicView *view, SchematicDeviceSelector *selector)
    : SchematicToolPlaceDeviceBase(view), m_selector(selector)
{
    selectNewDevice();
}

SchematicDevice *SchematicToolPlaceDevice::createDevice()
{
    SchematicDevice *dev = m_selector->createSelectedDevice(m_view->schematic());

    if (!dev)
        KMessageBox::error(m_view, m_selector->errorString());

    return dev;
}

//
// SchematicToolPlaceGround
//

SchematicToolPlaceGround::SchematicToolPlaceGround(SchematicView *view)
    : SchematicToolPlaceDeviceBase(view)
{
    selectNewDevice();
}

SchematicDevice *SchematicToolPlaceGround::createDevice()
{
    return new SchematicGround(m_view->schematic());
}

//
// SchematicToolPlaceTestPoint
//

SchematicToolPlaceTestPoint::SchematicToolPlaceTestPoint(SchematicView *view)
    : SchematicToolPlaceDeviceBase(view)
{
    selectNewDevice();
}

SchematicDevice *SchematicToolPlaceTestPoint::createDevice()
{
    return new SchematicTestPoint(m_view->schematic());
}

//
// SchematicToolPlaceAmmeter
//

SchematicToolPlaceAmmeter::SchematicToolPlaceAmmeter(SchematicView *view)
    : SchematicToolPlaceDeviceBase(view)
{
    selectNewDevice();
}

SchematicDevice *SchematicToolPlaceAmmeter::createDevice()
{
    return new SchematicAmmeter(m_view->schematic());
}

#include "schematictool.moc"

// vim: ts=4 sw=4 et
