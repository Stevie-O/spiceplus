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

#ifndef SCHEMATICTOOL_H
#define SCHEMATICTOOL_H

#include <qobject.h>
#include <qvaluevector.h>
#include <qvaluelist.h>

class QEvent;
class QMouseEvent;
class QCanvasRectangle;

namespace Spiceplus {

class Schematic;
class SchematicItem;
class SchematicDevice;
class SchematicDeviceState;
class SchematicView;
class SchematicCommandGroup;
class SchematicWire;
class SchematicDeviceSelector;
class SchematicDevicePin;
class SchematicJunction;

class SchematicTool : public QObject
{
    Q_OBJECT

public:
    SchematicTool(SchematicView *view);
    virtual ~SchematicTool();

    virtual int rtti() const = 0;

    virtual void processLeaveEvent(QEvent *event) = 0;
    virtual void processMouseDoubleClickEvent(QMouseEvent *event) = 0;
    virtual void processMousePressEvent(QMouseEvent *event) = 0;
    virtual void processMouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void processMouseMoveEvent(QMouseEvent *event) = 0;

    virtual bool isDeleteAvailable() const { return false; }
    virtual bool isSelectAllAvailable() const { return false; }
    virtual bool isRotateAvailable() const { return false; }
    virtual bool isFlipAvailable() const { return false; }
    virtual void reset() = 0;

public slots:
    virtual void deleteSelected() {}
    virtual void selectAll() {}
    virtual void rotateCCW() {}
    virtual void rotateCW() {}
    virtual void flipHorizontal() {}
    virtual void flipVertical() {}

signals:
    void deleteAvailable(bool yes);
    void selectAllAvailable(bool yes);
    void rotateAvailable(bool yes);
    void flipAvailable(bool yes);

protected:
    static int nextStep(int i);
    static QPoint nextStep(const QPoint &p);

    SchematicView *m_view;
};

class SchematicToolDeviceConnector
{
public:
    SchematicToolDeviceConnector(Schematic *schematic);
    virtual ~SchematicToolDeviceConnector() {}

protected:
    void placeConnectionMarks(SchematicDevice *device);
    void deleteConnectionMarks();
    void connectDevice(SchematicDevice *device, SchematicCommandGroup *cmdGroup);

    virtual bool canConnectItem(SchematicItem * /* item */) const { return true; }
    virtual void connectorReplacedItem(SchematicItem * /* oldItem */, SchematicItem * /* newItem */) {}

private:
    Schematic *m_schematic;
    QValueVector<QCanvasRectangle *> m_connectionMarks;
};

class SchematicToolSelect : public SchematicTool, public SchematicToolDeviceConnector
{
    Q_OBJECT 

public:
    SchematicToolSelect(SchematicView *view);
    ~SchematicToolSelect();

    static const int RTTI = 1;
    int rtti() const { return RTTI; }

    void processLeaveEvent(QEvent *);
    void processMouseDoubleClickEvent(QMouseEvent *event);
    void processMousePressEvent(QMouseEvent *event);
    void processMouseReleaseEvent(QMouseEvent *event);
    void processMouseMoveEvent(QMouseEvent *event);

    bool isDeleteAvailable() const { return m_isDeleteAvailable; }
    bool isSelectAllAvailable() const { return m_isSelectAllAvailable; }
    bool isRotateAvailable() const { return m_isRotateAvailable; }
    bool isFlipAvailable() const { return m_isFlipAvailable; }
    void reset();

protected:
    bool canConnectItem(SchematicItem *item) const;
    void connectorReplacedItem(SchematicItem *oldItem, SchematicItem *newItem);

public slots:
    void deleteSelected();
    void selectAll();
    void rotateCCW();
    void rotateCW();
    void flipHorizontal();
    void flipVertical();

private:
    void rotate(int direction);
    void flip(Qt::Orientation o);

    void highlightItem(const QPoint &p);
    void highlightItems(const QRect &r);
    void unhighlightItems();
    SchematicDevice *highlightedDevice() const;

    void selectItem(SchematicItem *item);
    void selectItems();
    void unselectItem(SchematicItem *item);
    void unselectItems();
    bool selectionContains(SchematicItem *item) const;
    void deleteSelectionMarks();
    void updateSelectionMarks();
    void setEditFlags(SchematicItem *item);

    bool deviceSelectionContains(SchematicDevice *device) const;
    void deviceSelectionAdd(SchematicItem *item, const QPoint &p);

    QValueVector<SchematicItem *> m_highlightedItems;

    QCanvasRectangle *m_selectionArea;

    struct SelectionItem
    {
        SchematicItem *item;
        QCanvasRectangle *selectionMark;
    };

    QValueList<SelectionItem> m_selection;

    struct DeviceState
    {
        DeviceState();
        DeviceState(SchematicDevice *dev, const QPoint &p);

        SchematicDevice *device;
        QPoint oldPosition;
        QPoint origin;
    };

    QValueVector<DeviceState> m_deviceSelection;
    bool m_deviceSelectionMoved;
    bool m_settingProperties;

    bool m_isDeleteAvailable;
    bool m_isSelectAllAvailable;
    bool m_isRotateAvailable;
    bool m_isFlipAvailable;
};

class SchematicToolPlaceWire : public SchematicTool
{
public:
    SchematicToolPlaceWire(SchematicView *view);
    ~SchematicToolPlaceWire();

    static const int RTTI = 2;
    int rtti() const { return RTTI; }

    void processLeaveEvent(QEvent *event);
    void processMouseDoubleClickEvent(QMouseEvent *event);
    void processMousePressEvent(QMouseEvent *event);
    void processMouseReleaseEvent(QMouseEvent *event);
    void processMouseMoveEvent(QMouseEvent *event);

    void reset();

private:
    void placeConnectionMark(const QPoint &p);
    void deleteConnectionMark();
    SchematicDevicePin *setupWirePin(const QPoint &p);

    SchematicWire *m_wire;
    bool m_wireMoved;
    SchematicCommandGroup *m_wireCommands;
    QCanvasRectangle *m_connectionMark;
};

class SchematicToolSplitWire : public SchematicTool
{
public:
    SchematicToolSplitWire(SchematicView *view);
    ~SchematicToolSplitWire();

    static const int RTTI = 3;
    int rtti() const { return RTTI; }

    void processLeaveEvent(QEvent *event);
    void processMouseDoubleClickEvent(QMouseEvent *) {}
    void processMousePressEvent(QMouseEvent *event);
    void processMouseReleaseEvent(QMouseEvent *);
    void processMouseMoveEvent(QMouseEvent *event);

    void reset();

private:
    void highlight(const QPoint &p);
    void unhighlight();

    SchematicWire *m_highlightedWire;
    SchematicJunction *m_junction;
};

class SchematicToolPlaceDeviceBase : public SchematicTool, public SchematicToolDeviceConnector
{
    Q_OBJECT

public:
    SchematicToolPlaceDeviceBase(SchematicView *view);
    ~SchematicToolPlaceDeviceBase();

    void processLeaveEvent(QEvent *) {}
    void processMouseDoubleClickEvent(QMouseEvent *event);
    void processMousePressEvent(QMouseEvent *event);
    void processMouseReleaseEvent(QMouseEvent *) {}
    void processMouseMoveEvent(QMouseEvent *event);

    bool isRotateAvailable() const { return m_isRotateAvailable; }
    bool isFlipAvailable() const { return m_isFlipAvailable; }
    void reset();

public slots:
    void rotateCCW();
    void rotateCW();
    void flipHorizontal();
    void flipVertical();

protected:
    void selectNewDevice();
    virtual SchematicDevice *createDevice() = 0;

private:
    SchematicDevice *m_device;
    SchematicDeviceState *m_deviceState;

    bool m_isRotateAvailable;
    bool m_isFlipAvailable;
};

class SchematicToolPlaceDevice : public SchematicToolPlaceDeviceBase
{
public:
    SchematicToolPlaceDevice(SchematicView *view, SchematicDeviceSelector *selector);

    static const int RTTI = 4;
    int rtti() const { return RTTI; }

protected:
    SchematicDevice *createDevice();

private:
    SchematicDeviceSelector *m_selector;
};

class SchematicToolPlaceGround : public SchematicToolPlaceDeviceBase
{
public:
    SchematicToolPlaceGround(SchematicView *view);

    static const int RTTI = 5;
    int rtti() const { return RTTI; }

protected:
    SchematicDevice *createDevice();
};

class SchematicToolPlaceTestPoint : public SchematicToolPlaceDeviceBase
{
public:
    SchematicToolPlaceTestPoint(SchematicView *view);

    static const int RTTI = 6;
    int rtti() const { return RTTI; }

protected:
    SchematicDevice *createDevice();
};

class SchematicToolPlaceAmmeter : public SchematicToolPlaceDeviceBase
{
public:
    SchematicToolPlaceAmmeter(SchematicView *view);

    static const int RTTI = 7;
    int rtti() const { return RTTI; }

protected:
    SchematicDevice *createDevice();
};

} // namespace Spiceplus

#endif // SCHEMATICTOOL_H

// vim: ts=4 sw=4 et
