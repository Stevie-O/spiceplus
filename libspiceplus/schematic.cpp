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

#include <config.h>

#include <qdom.h>
#include <qtextstream.h>
#include <qbitmap.h>
#include <qimage.h>

#include <klocale.h>

#include "schematic.h"
#include "schematicwire.h"
#include "schematicjunction.h"
#include "schematicground.h"
#include "schematictestpoint.h"
#include "schematicammeter.h"
#include "file.h"
#include "settings.h"
#include "pluginmanager.h"
#include "model.h"

using namespace Spiceplus;

//
// Schematic
//

double Schematic::s_nextZIndex = 0;

Schematic::Schematic(QObject *parent)
    : QCanvas(parent)
{
    loadSettings();
    m_nodes.append(new SchematicNode("0"));

    connect(Settings::self(), SIGNAL(settingsChanged()), SLOT(updateAll()));
}

Schematic::~Schematic()
{
    for (QValueList<SchematicNode *>::Iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
        delete *it;
}

void Schematic::loadSettings()
{
    if (Settings::self()->isGridVisible())
    {
        QImage image(Settings::self()->gridSize(), Settings::self()->gridSize(), 32);
        image.fill(Settings::self()->schematicBackgroundColor().rgb());
        image.setPixel(0, 0, Settings::self()->gridColor().rgb());
        setBackgroundPixmap(image);
    }
    else
    {
        setBackgroundPixmap(QPixmap());
        setBackgroundColor(Settings::self()->schematicBackgroundColor());
    }
}

void Schematic::updateAll()
{
    loadSettings();
    setAllChanged();
    update();
}

bool Schematic::load(const KURL &url)
{
    File file(url);

    if (!file.open(IO_ReadOnly))
    {
        m_errorString = file.errorString();
        return false;
    }

    QDomDocument doc;

    if (!doc.setContent(&file))
    {
        m_errorString = i18n("Invalid document structure");
        return false;
    }

    QDomElement root = doc.documentElement();

    if (root.tagName() != "schematic")
    {
        m_errorString = i18n("Invalid Schematic File");
        return false;
    }

    for (QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        QDomElement elem = node.toElement();
        if (elem.tagName() == "devices")
        {
            for (QDomNode devNode = node.firstChild(); !devNode.isNull(); devNode = devNode.nextSibling())
            {
                if (!devNode.isElement())
                    continue;

                QDomElement devElem = devNode.toElement();
                if (devElem.tagName() == "device")
                {
                    SchematicDevice *dev;

                    QString id = devElem.attribute("id");
                    if (id == SchematicJunction::ID)
                        dev = new SchematicJunction(this);
                    else if (id == SchematicGround::ID)
                        dev = new SchematicGround(this);
                    else if (id == SchematicTestPoint::ID)
                        dev = new SchematicTestPoint(this);
                    else if (id == SchematicAmmeter::ID)
                        dev = new SchematicAmmeter(this);
                    else
                    {
                        SchematicDeviceFactory *factory = PluginManager::self()->deviceFactory(id);
                        if (!factory)
                        {
                            m_errorString = PluginManager::self()->errorString();
                            return false;
                        }

                        dev = factory->createDevice(this);
                        if (!dev)
                        {
                            m_errorString = i18n("Could not create device %1").arg(id);
                            return false;
                        }
                    }

                    dev->setName(devElem.attribute("name"));

                    if (!dev->loadData(devElem))
                    {
                      m_errorString = dev->errorString();
                      return false;
                    }

                    dev->show();
                }
            }
        }
        else if (elem.tagName() == "wires")
        {
            for (QDomNode wireNode = node.firstChild(); !wireNode.isNull(); wireNode = wireNode.nextSibling())
            {
                if (!wireNode.isElement())
                    continue;

                QDomElement wireElem = wireNode.toElement();
                if (wireElem.tagName() == "wire")
                {
                    SchematicWire *wire = new SchematicWire(this);

                    if (!wire->loadData(wireElem))
                    {
                        m_errorString = i18n("Cannot connect wire");
                        return false;
                    }

                    wire->show();
                }
            }
        }
        else if (elem.tagName() == "nodes")
        {
            for (QDomNode nodeNode = node.firstChild(); !nodeNode.isNull(); nodeNode = nodeNode.nextSibling())
            {
                if (!nodeNode.isElement())
                    continue;

                QDomElement nodeElem = nodeNode.toElement();
                if (nodeElem.tagName() == "node")
                {
                    SchematicNode *node;
                    if (nodeElem.attribute("name") == "0")
                        node = findNode("0");
                    else
                    {
                        node = new SchematicNode(nodeElem.attribute("name"));
                        m_nodes.append(node);
                    }

                    QDomNode pinsNode = nodeNode.firstChild();
                    if (!pinsNode.isElement())
                        pinsNode = pinsNode.nextSibling();

                    if (pinsNode.isElement() && pinsNode.nodeName() == "pins")
                    {
                        for (QDomNode pinNode = pinsNode.firstChild(); !pinNode.isNull(); pinNode = pinNode.nextSibling())
                        {
                            if (!pinNode.isElement())
                                continue;

                            QDomElement pinElem = pinNode.toElement();
                            if (pinElem.tagName() == "pin")
                            {
                                SchematicDevice *dev = findDevice(pinElem.attribute("device-name"));

                                if (dev)
                                {
                                    SchematicDevicePin *pin = dev->findPin(pinElem.attribute("id"));
                                    if (pin)
                                        pin->setNode(node);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return true;
}

bool Schematic::save(const KURL &url)
{
    File file(url);

    if (!file.open(IO_WriteOnly))
    {
        m_errorString = file.errorString();
        return false;
    }

    QDomDocument doc("schematic");
    QDomElement root = doc.createElement("schematic");
    doc.appendChild(root);

    QCanvasItemList l = allItems();

    QDomElement devsElem = doc.createElement("devices");
    root.appendChild(devsElem);

    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if (dev->isVisible())
            {
                QDomElement devElem = doc.createElement("device");
                devElem.setAttribute("name", dev->name());
                devElem.setAttribute("id", dev->id());
                dev->saveData(devElem);
                devsElem.appendChild(devElem);
            }
        }
    }

    QDomElement wiresElem = doc.createElement("wires");
    root.appendChild(wiresElem);

    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->rtti() == SchematicWire::RTTI)
        {
            SchematicWire *wire = static_cast<SchematicWire *>(*it);

            QDomElement wireElem = doc.createElement("wire");
            wire->saveData(wireElem);
            wiresElem.appendChild(wireElem);
        }
    }

    QDomElement nodesElem = doc.createElement("nodes");
    root.appendChild(nodesElem);

    for (QValueList<SchematicNode *>::Iterator itn = m_nodes.begin(); itn != m_nodes.end(); ++itn)
    {
        QDomElement nodeElem = doc.createElement("node");
        nodeElem.setAttribute("name", (*itn)->name());

        QDomElement pinsElem = doc.createElement("pins");
        nodeElem.appendChild(pinsElem);

        QValueList<SchematicDevicePin *> pins = (*itn)->pins();
        for (QValueList<SchematicDevicePin *>::Iterator itp = pins.begin(); itp != pins.end(); ++itp)
        {
            QDomElement pinElem = doc.createElement("pin");
            pinElem.setAttribute("id", (*itp)->id());
            pinElem.setAttribute("device-name", (*itp)->device()->name());
            pinsElem.appendChild(pinElem);
        }

        nodesElem.appendChild(nodeElem);
    }

    QTextStream stream(&file);
    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << doc.toString();

    if (!file.closeWithStatus())
    {
        m_errorString = file.errorString();
        return false;
    }

    return true;
}

SchematicItem *Schematic::findItem(const QPoint &point) const
{
    QCanvasItemList l = collisionsSnapped(point);
    if (l.count() > 0)
        return dynamic_cast<SchematicItem *>(l.first());
    return 0;
}

SchematicDevice *Schematic::findDevice(const QString &name)
{
    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if (dev->name().lower() == name.lower())
                return dev;
        }
    }

    return 0;
}

SchematicDevice *Schematic::findDevice(const QPoint &point) const
{
    QCanvasItemList l = collisionsSnapped(point);
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
        if ((*it)->rtti() == SchematicDevice::RTTI)
            return static_cast<SchematicDevice *>(*it);

    return 0;
}

SchematicDevice *Schematic::findDevice(const QPoint &point1, const QPoint &point2) const
{
    SchematicDevice *dev = findDevice(point1);
    return dev == findDevice(point2) ? dev : 0;
}

QStringList Schematic::deviceNamesByType(const QString &type)
{
    QStringList names;

    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->isVisible() && (*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if (dev->type() == type)
                names << dev->name();
        }
    }

    names.sort();
    return names;
}

SchematicDevicePin *Schematic::findPin(const QPoint &point) const
{
    SchematicDevice *dev = findDevice(point);
    return dev ? dev->findPin(point) : 0;
}

SchematicDevicePin *Schematic::findPinExcluding(const QPoint &point, SchematicDevicePin *excludedPin1, SchematicDevicePin *excludedPin2) const
{
    QCanvasItemList l = collisionsSnapped(point);
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevicePin *pin = static_cast<SchematicDevice *>(*it)->findPin(point);
            if (pin && pin != excludedPin1 && pin != excludedPin2)
                return pin;
        }
    }

    return 0;
}

SchematicWire *Schematic::findWire(const QPoint &point) const
{
    QCanvasItemList l = collisionsSnapped(point);
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
        if ((*it)->rtti() == SchematicWire::RTTI)
            return static_cast<SchematicWire *>(*it);

    return 0;
}

SchematicWire *Schematic::findWire(const QPoint &point1, const QPoint &point2) const
{
    SchematicWire *wire = findWire(point1);
    return wire == findWire(point2) ? wire : 0;
}

SchematicWire *Schematic::findWireExcluding(const QPoint &point, SchematicWire *excludedWire) const
{
    QCanvasItemList l = collisionsSnapped(point);
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->rtti() == SchematicWire::RTTI)
        {
            SchematicWire *wire = static_cast<SchematicWire *>(*it);
            if (wire != excludedWire)
                return wire;
        }
    }

    return 0;
}

bool Schematic::canRunAnalysis()
{
    bool hasSource = false, hasAmmeter = false, hasGround = false;
    int numTestPoints = 0;

    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->isVisible() && (*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if ((dev->type() == "v" || dev->type() == "i") && dev->allPinsConnected())
                hasSource = true;
            else if (dev->id() == SchematicGround::ID && dev->allPinsConnected())
                hasGround = true;
            else if (dev->id() == SchematicTestPoint::ID && dev->allPinsConnected())
                ++numTestPoints;
            else if (dev->id() == SchematicAmmeter::ID && dev->allPinsConnected())
                hasAmmeter = true;

            if (hasSource && hasGround && (numTestPoints >= 2 || hasAmmeter))
                break;
        }
    }

    if (!hasSource)
    {
        m_errorString = i18n("You must connect an independent Source to run analysis.");
        return false;
    }

    if (!hasGround)
    {
        m_errorString = i18n("You must connect a Ground to run analysis.");
        return false;
    }

    if (numTestPoints < 2 && !hasAmmeter)
    {
        m_errorString = i18n("You must connect at least 2 Test Points or 1 Ammeter to run analysis.");
        return false;
    }

    return true;
}

QStringList Schematic::testPointNames()
{
    QStringList names;

    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->isVisible() && (*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if (dev->id() == SchematicTestPoint::ID)
                names << dev->name();
        }
    }

    names.sort();
    return names;
}

QStringList Schematic::ammeterNames()
{
    QStringList names;

    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->isVisible() && (*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if (dev->id() == SchematicAmmeter::ID)
                names << dev->name();
        }
    }

    names.sort();
    return names;
}

QString Schematic::createUniqueDeviceName(const QString &prefix)
{
    QString name = prefix + "%1";
    int num;
    for (num = 1; findDevice(name.arg(num)); ++num);

    return name.arg(num);
}

void Schematic::setNodePins(SchematicDevicePin *pin, SchematicNode *node) const
{
    if (pin->node() != node)
    {
        pin->setNode(node); 

        QValueList<SchematicWireEnd *> wireEnds = pin->wireEnds();
        for (QValueList<SchematicWireEnd *>::Iterator it = wireEnds.begin(); it != wireEnds.end(); ++it)
            setNodePins((*it)->oppositePin(), node);
    }
}

bool Schematic::isNodeEqual(SchematicDevicePin *pin1, SchematicDevicePin *pin2) const
{
    if (!pin1->node() || !pin2->node())
        return false;

    bool eq;
    SchematicNode *node = pin1->node();

    setNodePins(pin1, 0);
    eq = !pin2->node();
    setNodePins(pin1, node);

    return eq;
}

SchematicNode *Schematic::findNode(const QString &name) const
{
    for (QValueList<SchematicNode *>::ConstIterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
        if ((*it)->name() == name)
            return *it;

    return 0;
}

QString Schematic::createUniqueNodeName() const
{
    QString name;
    for (int num = 1; findNode(name.setNum(num)); ++num);
    return name;
}

bool Schematic::createModelList(QMap<QString, Model> &modelList)
{
    QMap<QString, Model> ml;

    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->rtti() != SchematicDevice::RTTI)
            continue;

        SchematicDevice *dev = static_cast<SchematicDevice *>(*it);

        if (!dev->hasModel())
            continue;

        Model model = dev->createModel();
        if (model.isNull())
        {
            m_errorString = dev->errorString();
            return false;
        }

        QMap<QString, Model>::Iterator it;

        for (it = ml.begin(); it != ml.end(); ++it)
            if (it.data() == model)
                break;

        if (it != ml.end())
            dev->setModelID(it.key());
        else
        {
            QString id = "mod%1";
            for (int i = 1;; ++i)
            {
                if (!ml.contains(id.arg(i)))
                {
                    dev->setModelID(id.arg(i));
                    ml[id.arg(i)] = model;
                    break;
                }
            }
        }
    }

    modelList = ml;
    return true;
}

QString Schematic::createCommandList()
{
    QString cmdList = "Generated by SPICE+ " VERSION "\n";

    QMap<QString, Model> ml;
    if (!createModelList(ml))
        return QString::null;

    QCanvasItemList l = allItems();
    for (QCanvasItemList::Iterator it = l.begin(); it != l.end(); ++it)
    {
        if ((*it)->isVisible() && (*it)->rtti() == SchematicDevice::RTTI)
        {
            SchematicDevice *dev = static_cast<SchematicDevice *>(*it);
            if (dev->hasCommand())
            {
                QString cmd = dev->createCommand();
                if (cmd.isNull())
                {
                    m_errorString = dev->errorString();
                    return QString::null;
                }
                cmdList += cmd + '\n';
            }
        }
    }

    for (QMap<QString, Model>::Iterator it = ml.begin(); it != ml.end(); ++it)
        cmdList += it.data().createCommand(it.key()) + '\n';

    return cmdList;
}

QCanvasItemList Schematic::collisionsSnapped(const QPoint &p) const
{
    int s = Settings::self()->gridSize();
    return collisions(QRect(p.x() - s / 2, p.y() - s / 2, s, s));
}

#include "schematic.moc"

// vim: ts=4 sw=4 et
