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

#ifndef SCHEMATICSTANDARDDEVICE_H
#define SCHEMATICSTANDARDDEVICE_H

#include "schematicdevice.h"
#include "devicesymbol.h"

namespace Spiceplus {

class SchematicStandardDeviceState;

class SchematicStandardDevice : public SchematicDevice
{
    friend class SchematicStandardDeviceState;

public:
    SchematicStandardDevice(Schematic *schematic);
    virtual ~SchematicStandardDevice();

    void rotate(double a, bool absolute = false);
    void rotate(int a, bool absolute = false);
    void flip(Qt::Orientation o);

    bool highlighted() const;
    void setHighlighted(bool yes);

    virtual QString parameter(const QString &name) const;
    virtual StringTable parameterToStringTable(const QString &name) const;
    virtual void setParameter(const QString &name, const SchematicParameter &param);
    virtual void removeParameter(const QString &name);
    virtual void clearParameters();

    virtual QString defaultSymbolPath() const = 0;
    bool loadSymbol(const QString &symbolPath = QString::null);
    bool loadSymbol(const KURL &url);
    void setShapeGroupEnabled(const QString &group, bool enabled);
    virtual void updateLabels() = 0;

    virtual ModelPolicy modelPolicy() const = 0;
    virtual bool hasModel() const;
    virtual Model createModel();
    QString modelPath() const { return m_modelPath; }
    void setModelPath(const QString &path) { m_modelPath = path; }
    QString modelName() const { return m_modelName; }
    void setModelName(const QString &name) { m_modelName = name; }

    virtual bool loadData(const QDomElement &elem);
    virtual void saveData(QDomElement &elem) const;

    virtual SchematicDeviceState *createState() const;
    virtual void restoreState(const SchematicDeviceState *state);

    virtual QPoint toWorld(const QPoint &point) const;
    virtual QPointArray areaPoints() const;

protected:
    QString labelText(const QString &id) const { return m_symbol.labelText(id); }
    void setLabelText(const QString &id, const QString &text);
    void drawShape(QPainter &p);

private:
    DeviceSymbol m_symbol;
    QMap<QString, SchematicParameter> m_parameters;
    QString m_modelPath;
    QString m_modelName;
};

class SchematicStandardDeviceState : public SchematicDeviceState
{
    friend class SchematicStandardDevice;

protected:
    SchematicStandardDeviceState(const SchematicStandardDevice *dev) : SchematicDeviceState(dev),
                                                                       m_angle(dev->m_symbol.angle()),
                                                                       m_flipped(dev->m_symbol.flipped()),
                                                                       m_parameters(dev->m_parameters),
                                                                       m_modelPath(dev->m_modelPath) {}

    double angle() const { return m_angle; }
    bool flipped() const { return m_flipped; }
    QMap<QString, SchematicParameter> parameters() const { return m_parameters; }
    QString modelPath() const { return m_modelPath; }

private:
    double m_angle;
    bool m_flipped;
    QMap<QString, SchematicParameter> m_parameters;
    QString m_modelPath;
};

} // namespace Spiceplus

#endif // SCHEMATICSTANDARDDEVICE_H

// vim: ts=4 sw=4 et
