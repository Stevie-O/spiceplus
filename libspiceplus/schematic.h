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

#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <qstring.h>
#include <qmap.h>
#include <qcanvas.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

#include "types.h"

class QStringList;
class KURL;

namespace Spiceplus {

class Model;
class SchematicItem;
class SchematicDevice;
class SchematicDevicePin;
class SchematicWire;
class SchematicNode;

class Schematic : public QCanvas
{
    Q_OBJECT

public:
    Schematic(QObject *parent = 0);
    virtual ~Schematic();
    void loadSettings();

    bool load(const KURL &url);
    bool save(const KURL &url);

    SchematicItem *findItem(const QPoint &point) const;

    SchematicDevice *findDevice(const QString &name);
    SchematicDevice *findDevice(const QPoint &point) const;
    SchematicDevice *findDevice(const QPoint &point1, const QPoint &point2) const;
    QStringList deviceNamesByType(const QString &type);

    SchematicDevicePin *findPin(const QPoint &point) const;
    SchematicDevicePin *findPinExcluding(const QPoint &point, SchematicDevicePin *excludedPin1, SchematicDevicePin *excludedPin2 = 0) const;

    SchematicWire *findWire(const QPoint &point) const;
    SchematicWire *findWire(const QPoint &point1, const QPoint &point2) const;
    SchematicWire *findWireExcluding(const QPoint &point, SchematicWire *excludedWire) const;

    bool canRunAnalysis();
    QStringList testPointNames();
    QStringList ammeterNames();

    QString createUniqueDeviceName(const QString &prefix);

    void setNodePins(SchematicDevicePin *startPin, SchematicNode *node) const;
    void addNode(SchematicNode *node) { m_nodes.append(node); }
    void removeNode(SchematicNode *node) { m_nodes.remove(node); }
    bool isNodeEqual(SchematicDevicePin *pin1, SchematicDevicePin *pin2) const;
    SchematicNode *findNode(const QString &name) const;
    QString createUniqueNodeName() const;

    bool createModelList(QMap<QString, Model> &modelList);
    QString createCommandList();

    QString errorString() const { return m_errorString; }

    static double nextZIndex() { return s_nextZIndex++; }

private slots:
    void updateAll();

private:
    QCanvasItemList collisionsSnapped(const QPoint &p) const;

    QValueList<SchematicNode *> m_nodes;
    QString m_errorString;

    static double s_nextZIndex;
};

class SchematicItem : public QCanvasPolygonalItem
{
public:
    SchematicItem(Schematic *schematic) : QCanvasPolygonalItem(schematic) {}
    ~SchematicItem() { hide(); }

    Schematic *schematic() const { return static_cast<Schematic *>(canvas()); }
    void setSchematic(Schematic *schematic) { setCanvas(schematic); }

    virtual bool highlighted() const = 0;
    virtual void setHighlighted(bool yes) = 0;

    virtual void raiseToTop() = 0;
};

class SchematicNode
{
public:
    SchematicNode() {}
    SchematicNode(const QString &name) : m_name(name) {}

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    void addPin(SchematicDevicePin *pin) { m_pins.append(pin); }
    void removePin(SchematicDevicePin *pin) { m_pins.remove(pin); }
    QValueList<SchematicDevicePin *> pins() const { return m_pins; }

private:
    QString m_name;
    QValueList<SchematicDevicePin *> m_pins;
};

class SchematicParameter
{
public:
    enum Type { TypeInvalid, TypeString, TypeStringTable };

    SchematicParameter() : m_type(TypeInvalid) {}
    SchematicParameter(const char *str) : m_type(TypeString), m_string(str) {}
    SchematicParameter(const QString &str) : m_type(TypeString), m_string(str) {}
    SchematicParameter(const StringTable &tab) : m_type(TypeStringTable), m_stringTable(tab) {}

    Type type() const { return m_type; }

    QString toString() const { return m_string; }
    StringTable toStringTable() const { return m_stringTable; }

private:
    Type m_type;
    QString m_string;
    StringTable m_stringTable;
};

} // namespace Spiceplus

#endif // SCHEMATIC_H

// vim: ts=4 sw=4 et
