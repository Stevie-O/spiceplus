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

#ifndef SCHEMATICDEVICE_H
#define SCHEMATICDEVICE_H

#include <klibloader.h>

#include "schematic.h"

class QDomElement;

namespace Spiceplus {

class SchematicDevice;
class SchematicWire;
class SchematicWireEnd;
class SchematicNode;

class SchematicDevicePin
{
public:
    SchematicDevicePin(SchematicDevice *device);
    SchematicDevicePin(const QString &id, SchematicDevice *device);
    ~SchematicDevicePin();

    QString id() const { return m_id; }
    void setID(const QString &id) { m_id = id; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    void setX(int x) { m_x = x; }
    void setY(int y) { m_y = y; }
    QPoint worldPoint() const;
    size_t wireCount() const { return m_wireEnds.size(); }
    void addWireEnd(SchematicWireEnd *end) { m_wireEnds.append(end); }
    void removeWireEnd(SchematicWireEnd *end) { m_wireEnds.remove(end); }
    bool isWireConnected(SchematicWire *wire) const;

    QValueList<SchematicWireEnd *> wireEnds() const { return m_wireEnds; }
    SchematicDevice *device() const { return m_device; }
    SchematicNode *node() const { return m_node; }
    void setNode(SchematicNode *node);

private:
    QString m_id;
    int m_x;
    int m_y;
    QValueList<SchematicWireEnd *> m_wireEnds;
    SchematicDevice *m_device;
    SchematicNode *m_node;
};

class SchematicDevicePropertiesWidget : public QWidget
{
    Q_OBJECT

public:
    SchematicDevicePropertiesWidget(QWidget *parent = 0);
    virtual ~SchematicDevicePropertiesWidget();

    virtual bool apply() = 0;
    QString errorString() const { return m_errorString; }

protected:
    void setErrorString(const QString &str) { m_errorString = str; }

private:
    QString m_errorString;
};

class SchematicDeviceState;

class SchematicDevice : public SchematicItem
{
    friend class SchematicDeviceState;

public:
    enum ModelPolicy
    {
        None,
        Optional,
        Required
    };

    SchematicDevice(Schematic *schematic);
    virtual ~SchematicDevice();

    static const int RTTI = 1001;
    virtual int rtti() const { return RTTI; }

    virtual QString id() const = 0;
    virtual QString type() const = 0;

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    inline QPoint position() const;
    inline void setPosition(const QPoint &point);
    void raiseToTop();

    virtual void rotate(double a, bool absolute = false) = 0;
    virtual void rotate(int a, bool absolute = false) = 0;
    virtual void flip(Qt::Orientation o) = 0;

    virtual bool initialize() = 0;

    SchematicDevicePin *findPin(const QString &id) const;
    SchematicDevicePin *findPin(const QPoint &point) const;
    QValueVector<SchematicDevicePin *> pins() const { return m_pins; }
    bool allPinsConnected() const;

    virtual ModelPolicy modelPolicy() const = 0;
    virtual bool hasModel() const = 0;
    virtual Model createModel() = 0;

    QString modelID() const { return m_modelID; }
    void setModelID(const QString &id) { m_modelID = id; }

    virtual bool loadData(const QDomElement &elem) = 0;
    virtual void saveData(QDomElement &elem) const = 0;
    virtual void setDefaults() = 0;
    virtual bool hasCommand() const = 0;
    virtual QString createCommand() = 0;

    virtual SchematicDeviceState *createState() const = 0;
    virtual void restoreState(const SchematicDeviceState *state);

    virtual bool hasPropertiesWidget() const = 0;
    virtual SchematicDevicePropertiesWidget *createPropertiesWidget(QWidget *parent = 0) = 0;

    virtual QPoint toWorld(const QPoint &point) const = 0;
    virtual void moveBy(double dx, double dy);

    QString errorString() const { return m_errorString; }

protected:
    int displayPinWireCount() const { return m_displayPinWireCount; }
    void setDisplayPinWireCount(int c) { m_displayPinWireCount = c; }

    void addPin(SchematicDevicePin *pin) { m_pins.append(pin); }
    void updateWirePositions() const;

    void setErrorString(const QString &str) { m_errorString = str; }

private:
    QString m_name;
    QString m_modelID;

    QValueVector<SchematicDevicePin *> m_pins;
    int m_displayPinWireCount;

    QString m_errorString;
};

class SchematicDeviceState
{
    friend class SchematicDevice;

public:
    virtual ~SchematicDeviceState() {}

protected:
    SchematicDeviceState(const SchematicDevice *dev) : m_x(dev->x()), m_y(dev->y()), m_name(dev->m_name) {}

    double x() const { return m_x; }
    double y() const { return m_y; }
    QString name() const { return m_name; }

private:
    double m_x;
    double m_y;
    QString m_name;
};

inline QPoint SchematicDevice::position() const
{
    return QPoint(static_cast<int>(x()), static_cast<int>(y()));
}

inline void SchematicDevice::setPosition(const QPoint &point)
{
    setX(point.x());
    setY(point.y());
}

class SchematicDeviceSelector : public QWidget
{
    Q_OBJECT

public:
    SchematicDeviceSelector(QWidget *parent = 0) : QWidget(parent) {}
    virtual bool isDeviceSelected() const = 0;
    virtual SchematicDevice *createSelectedDevice(Schematic *schematic) = 0;

    QString errorString() const { return m_errorString; }

signals:
    void deviceChanged();

protected:
    void setErrorString(const QString &str) { m_errorString = str; }

private:
    QString m_errorString;
};

class SchematicDeviceFactory : public KLibFactory
{
    Q_OBJECT

public:
    virtual SchematicDevice *createDevice(Schematic *schematic) = 0;
    virtual SchematicDeviceSelector *createSelector(QWidget *parent = 0) = 0;

    QString errorString() const { return m_errorString; }

protected:
    void setErrorString(const QString &str) { m_errorString = str; }
    QObject *createObject(QObject *, const char *, const char *, const QStringList &) { return 0; }

private:
    QString m_errorString;
};

} // namespace Spiceplus

#endif // SCHEMATICDEVICE_H

// vim: ts=4 sw=4 et
