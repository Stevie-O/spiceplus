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

#include <qwmatrix.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qdom.h>

#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>

#include "schematicstandarddevice.h"
#include "settings.h"
#include "model.h"
#include "modelfile.h"

using namespace Spiceplus;

SchematicStandardDevice::SchematicStandardDevice(Schematic *schematic)
    : SchematicDevice(schematic)
{
}

SchematicStandardDevice::~SchematicStandardDevice()
{
    hide();
}

void SchematicStandardDevice::rotate(double a, bool absolute)
{
    invalidate();
    m_symbol.setAngle(absolute ? a : m_symbol.angle() + a);
    m_symbol.alignLabels();
    update();
    updateWirePositions();
}

void SchematicStandardDevice::rotate(int a, bool absolute)
{
    invalidate();
    m_symbol.setAngle(absolute ? a : static_cast<int>(m_symbol.angle()) + a);
    m_symbol.alignLabels();
    update();
    updateWirePositions();
}

void SchematicStandardDevice::flip(Qt::Orientation o)
{
    invalidate();
    m_symbol.setAngle((o == Qt::Horizontal ? 0 : 180) - m_symbol.angle());
    m_symbol.setFlipped(!m_symbol.flipped());
    m_symbol.alignLabels();
    update();
    updateWirePositions();
}

bool SchematicStandardDevice::highlighted() const
{
    return m_symbol.highlighted();
}

void SchematicStandardDevice::setHighlighted(bool yes)
{
    invalidate();
    m_symbol.setHighlighted(yes);
    update();
}

QPoint SchematicStandardDevice::toWorld(const QPoint &point) const
{
    QWMatrix matrix;
    matrix.translate(x(), y());

    if (m_symbol.flipped())
    {
        matrix.scale(-1, 1);
        matrix.rotate(m_symbol.angle());
    }
    else
        matrix.rotate(-m_symbol.angle());

    return matrix.map(point);
}

QPointArray SchematicStandardDevice::areaPoints() const
{
    QRect rect = m_symbol.boundingRect();

    int dia = Settings::self()->devicePinDiameter();
    const QValueVector<SchematicDevicePin *> pins = this->pins();
    for (size_t i = 0; i < pins.count(); ++i)
        rect |= QRect(pins[i]->x() - dia / 2, pins[i]->y() - dia / 2, dia, dia);

    rect.addCoords(-2, -2, 2, 2);

    QWMatrix matrix;
    matrix.translate(x(), y());

    if (m_symbol.flipped())
    {
        matrix.scale(-1, 1);
        matrix.rotate(m_symbol.angle());
    }
    else
        matrix.rotate(-m_symbol.angle());

    return matrix.map(QPointArray(rect));
}

void SchematicStandardDevice::drawShape(QPainter &p)
{
    QPen oldPen = p.pen();
    QBrush oldBrush = p.brush();

    if (m_symbol.highlighted())
    {
        p.setPen(Settings::self()->deviceColorHighlighted());
        p.setBrush(Settings::self()->deviceColorHighlighted());
    }
    else
    {
        p.setPen(Settings::self()->deviceColor());
        p.setBrush(Settings::self()->deviceColor());
    }

    int dia = Settings::self()->devicePinDiameter();
    const QValueVector<SchematicDevicePin *> pins = this->pins();
    for (size_t i = 0; i < pins.count(); ++i)
    {
        if (m_symbol.highlighted() || displayPinWireCount() >= 0 && pins[i]->wireCount() >= static_cast<size_t>(displayPinWireCount()))
        {
            QPoint point = pins[i]->worldPoint();
            p.drawEllipse(point.x() - dia / 2, point.y() - dia / 2, dia, dia);
        }
    }

    p.setBrush(oldBrush);
    p.setPen(oldPen);

    m_symbol.draw(p, static_cast<int>(x()), static_cast<int>(y()));
}

QString SchematicStandardDevice::parameter(const QString &name) const
{
    QMap<QString, SchematicParameter>::ConstIterator it = m_parameters.find(name);
    return it != m_parameters.end() ? it.data().toString() : QString::null;
}

StringTable SchematicStandardDevice::parameterToStringTable(const QString &name) const
{
    QMap<QString, SchematicParameter>::ConstIterator it = m_parameters.find(name);
    return it != m_parameters.end() ? it.data().toStringTable() : StringTable();
}

void SchematicStandardDevice::setParameter(const QString &name, const SchematicParameter &param)
{
    m_parameters[name] = param;
}

void SchematicStandardDevice::removeParameter(const QString &name)
{
    m_parameters.remove(name);
}

void SchematicStandardDevice::clearParameters()
{
    m_parameters.clear();
}

bool SchematicStandardDevice::hasModel() const
{
    return !m_modelPath.isEmpty();
}

Model SchematicStandardDevice::createModel()
{
    if (m_modelPath.isEmpty())
    {
        setErrorString(i18n("Missing model for device %1").arg(name()));
        return Model();
    }

    ModelFile model;
    if (!model.load(m_modelPath))
    {
        setErrorString(model.errorString());
        return Model();
    }

    return model;
}

void SchematicStandardDevice::setLabelText(const QString &id, const QString &text)
{
    invalidate();
    m_symbol.setLabelText(id, text);
    update();
}

bool SchematicStandardDevice::loadData(const QDomElement &elem)
{
    setX(elem.attribute("x").toDouble());
    setY(elem.attribute("y").toDouble());

    m_symbol.setAngle(elem.attribute("angle").toDouble());
    m_symbol.setFlipped(elem.attribute("flipped") == "true");

    for (QDomNode node = elem.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        QDomElement elem = node.toElement();
        if (elem.tagName() == "parameters")
        {
            for (QDomNode paramNode = node.firstChild(); !paramNode.isNull(); paramNode = paramNode.nextSibling())
            {
                if (!paramNode.isElement())
                    continue;

                QDomElement paramElem = paramNode.toElement();
                if (paramElem.tagName() == "parameter")
                {
                    QDomNode valueNode = paramElem.firstChild();
                    if (!valueNode.isNull())
                    {
                        if (valueNode.isText())
                            m_parameters[paramElem.attribute("name")] = valueNode.nodeValue();
                        else if (valueNode.isElement() && valueNode.nodeName() == "table")
                        {
                            StringTable table;

                            for (QDomNode rowNode = valueNode.firstChild(); !rowNode.isNull(); rowNode = rowNode.nextSibling())
                            {
                                if (!rowNode.isElement() || rowNode.nodeName() != "row")
                                    continue;

                                StringTableRow row;
                                for (QDomNode dataNode = rowNode.firstChild(); !dataNode.isNull(); dataNode = dataNode.nextSibling())
                                {
                                    if (!dataNode.isElement() || dataNode.nodeName() != "data")
                                        continue;

                                    if (dataNode.firstChild().isText())
                                        row.append(dataNode.firstChild().nodeValue());
                                }
                                table.append(row);
                            }

                            m_parameters[paramElem.attribute("name")] = table;
                        }
                    }
                }
            }
        }
        else if (elem.tagName() == "model" && modelPolicy() != None)
        {
            for (QDomNode modelNode = node.firstChild(); !modelNode.isNull(); modelNode = modelNode.nextSibling())
            {
                if (!modelNode.isElement())
                    continue;

                QDomElement modelElem = modelNode.toElement();
                if (modelElem.tagName() == "path")
                {
                    QDomNode textNode = modelElem.firstChild();
                    if (!textNode.isNull() && textNode.isText())
                        m_modelPath = textNode.nodeValue();
                }
            }
        }
    }

    QString symbolPath;

    if (modelPolicy() != None)
    {
        if (!m_modelPath.isEmpty())
        {
            ModelFile model;
            if (!model.load(m_modelPath))
            {
                setErrorString(model.errorString());
                return false;
            }
            symbolPath = model.symbolPath();
            setModelPath(m_modelPath);
            setModelName(model.name());
        }
        else if (modelPolicy() == Required)
        {
            setErrorString(i18n("No model specified for device %1").arg(id()));
            return false;
        }
    }

    if (!loadSymbol(symbolPath))
        return false;

    m_symbol.alignLabels();
    updateLabels();

    return true;
}

void SchematicStandardDevice::saveData(QDomElement &elem) const
{
    elem.setAttribute("x", x());
    elem.setAttribute("y", y());
    elem.setAttribute("angle", m_symbol.angle());
    elem.setAttribute("flipped", m_symbol.flipped() ? "true" : "false");

    QDomDocument doc = elem.ownerDocument();
    QDomElement paramsElem = doc.createElement("parameters");
    elem.appendChild(paramsElem);

    for (QMap<QString, SchematicParameter>::ConstIterator it = m_parameters.begin(); it != m_parameters.end(); ++it)
    {
        QDomElement paramElem = doc.createElement("parameter");
        paramElem.setAttribute("name", it.key());

        if (it.data().type() == SchematicParameter::TypeString)
            paramElem.appendChild(doc.createTextNode(it.data().toString()));
        else if (it.data().type() == SchematicParameter::TypeStringTable)
        {
            const StringTable table = it.data().toStringTable();

            QDomElement tableElem = doc.createElement("table");
            for (size_t row = 0; row < table.count(); ++row)
            {
                QDomElement rowElem = doc.createElement("row");
                for (size_t col = 0; col < table[row].count(); ++col)
                {
                    QDomElement dataElem = doc.createElement("data");
                    dataElem.appendChild(doc.createTextNode(table[row][col]));
                    rowElem.appendChild(dataElem);
                }
                tableElem.appendChild(rowElem);
            }
            paramElem.appendChild(tableElem);
        }
        else
            kdError() << k_funcinfo << "Invalid Parameter Type" << endl;

        paramsElem.appendChild(paramElem);
    }

    if (modelPolicy() != None)
    {
        QDomElement pathElem = doc.createElement("path");
        pathElem.appendChild(doc.createTextNode(m_modelPath));

        QDomElement modelElem = doc.createElement("model");
        modelElem.appendChild(pathElem);
        elem.appendChild(modelElem);
    }
}

SchematicDeviceState *SchematicStandardDevice::createState() const
{
    return new SchematicStandardDeviceState(this);
}

void SchematicStandardDevice::restoreState(const SchematicDeviceState *state)
{
    const SchematicStandardDeviceState *s = static_cast<const SchematicStandardDeviceState *>(state);

    SchematicDevice::restoreState(state);

    if (m_symbol.angle() != s->m_angle || m_symbol.flipped() != s->m_flipped)
    {
        invalidate();
        m_symbol.setAngle(s->m_angle);
        m_symbol.setFlipped(s->m_flipped);
        m_symbol.alignLabels();
        update();
        updateWirePositions();
    }

    m_parameters = s->m_parameters;
    m_modelPath = s->m_modelPath;
    updateLabels();
}

bool SchematicStandardDevice::loadSymbol(const QString &symbolPath)
{
    QString path = symbolPath.isEmpty() ? defaultSymbolPath() : symbolPath;
    KURL url = Settings::self()->symbolPathToURL(path);
    if (url.isEmpty())
    {
        setErrorString(i18n("Unknown symbol path: %1").arg(path));
        return false;
    }

    return loadSymbol(url);
}

bool SchematicStandardDevice::loadSymbol(const KURL &url)
{
    if (!m_symbol.load(url))
    {
        setErrorString(m_symbol.errorString());
        return false;
    }

    const QValueVector<SchematicDevicePin *> pins = this->pins();
    for (size_t i = 0; i < pins.count(); ++i)
    {
        int x, y;
        if (!m_symbol.pinPosition(pins[i]->id(), x, y))
        {
            setErrorString(i18n("Device %1: Pin position not found in symbol file").arg(name()));
            return false;
        }
        pins[i]->setX(x);
        pins[i]->setY(y);
    }

    return true;
}

void SchematicStandardDevice::setShapeGroupEnabled(const QString &group, bool enabled)
{
    invalidate();
    m_symbol.setShapeGroupEnabled(group, enabled);
    update();
}

// vim: ts=4 sw=4 et
