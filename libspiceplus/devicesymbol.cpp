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

#include <math.h>

#include <qdom.h>
#include <qwmatrix.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include <kurl.h>
#include <klocale.h>

#include "devicesymbol.h"
#include "settings.h"
#include "file.h"

using namespace Spiceplus;

//
// DeviceSymbolShape
//

QPointArray DeviceSymbolShape::createPointArray(QDomNode node)
{
    QPointArray points;
    int i = 0;

    for (; !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        QDomElement elem = node.toElement();
        if (elem.tagName() == "point")
            points.putPoints(i++, 1, elem.attribute("x").toInt(),
                                     elem.attribute("y").toInt());
    }

    return points;
}

//
// DeviceSymbolLine
//

QRect DeviceSymbolLine::boundingRect() const
{
    return QRect(QPoint(m_x1, m_y1), QPoint(m_x2, m_y2)).normalize();
}

void DeviceSymbolLine::draw(QPainter &p) const
{
    p.drawLine(m_x1, m_y1, m_x2, m_y2);
}

//
// DeviceSymbolRect
//

QRect DeviceSymbolRect::boundingRect() const
{
    return m_rect;
}

void DeviceSymbolRect::draw(QPainter &p) const
{
    if (m_fill)
    {
        QBrush b = p.brush();
        p.setBrush(p.pen().color());
        p.drawRect(m_rect);
        p.setBrush(b);
    }
    else
        p.drawRect(m_rect);
}

//
// DeviceSymbolEllipse
//

QRect DeviceSymbolEllipse::boundingRect() const
{
    return m_rect;
}

void DeviceSymbolEllipse::draw(QPainter &p) const
{
    if (m_fill)
    {
        QBrush b = p.brush();
        p.setBrush(p.pen().color());
        p.drawEllipse(m_rect);
        p.setBrush(b);
    }
    else
        p.drawEllipse(m_rect);
}

//
// DeviceSymbolPolygon
//

QRect DeviceSymbolPolygon::boundingRect() const
{
    return m_points.boundingRect();
}

void DeviceSymbolPolygon::draw(QPainter &p) const
{
    if (m_fill)
    {
        QBrush b = p.brush();
        p.setBrush(p.pen().color());
        p.drawPolygon(m_points, m_winding);
        p.setBrush(b);
    }
    else
        p.drawPolygon(m_points, m_winding);
}

//
// DeviceSymbolPolyline
//

QRect DeviceSymbolPolyline::boundingRect() const
{
    return m_points.boundingRect();
}

void DeviceSymbolPolyline::draw(QPainter &p) const
{
    p.drawPolyline(m_points);
}

//
// DeviceSymbolCubicBezier
//

QRect DeviceSymbolCubicBezier::boundingRect() const
{
    return m_points.boundingRect();
}

void DeviceSymbolCubicBezier::draw(QPainter &p) const
{
    for (int i = 0; m_points.count() - i > 3; i += 3)
        p.drawCubicBezier(m_points, i);
}

//
// DeviceSymbolLabel
//

DeviceSymbolLabel::DeviceSymbolLabel()
    : m_x(0), m_y(0), m_anchor(Middle), m_angle(0), m_autoAlign(false)
{
}

void DeviceSymbolLabel::setAngle(double a)
{
    a = fmod(a, 360);
    m_angle = a < 0 ? a + 360 : a;
}

QFont DeviceSymbolLabel::font() const
{
    switch (m_fontSize)
    {
    case Tiny:
        return Settings::self()->tinySymbolFont();
    case Small:
        return Settings::self()->smallSymbolFont();
    case Normal:
        return Settings::self()->normalSymbolFont();
    case Large:
        return Settings::self()->largeSymbolFont();
    case Huge:
        return Settings::self()->hugeSymbolFont();
    }

    return Settings::self()->normalSymbolFont(); // make g++ happy ;)
}

void DeviceSymbolLabel::setFont(const QString &fontSize)
{
    if (fontSize == "tiny")
        m_fontSize = Tiny;
    else if (fontSize == "small")
        m_fontSize = Small;
    else if (fontSize == "large")
        m_fontSize = Large;
    else if (fontSize == "huge")
        m_fontSize = Huge;
    else
        m_fontSize = Normal;
}

void DeviceSymbolLabel::metrics(QRect &boundingRect, QPointArray &worldPoints) const
{
    boundingRect = QFontMetrics(font()).boundingRect(m_x, m_y, 0, 0, Qt::AlignLeft | Qt::AlignTop, m_text);
    boundingRect.addCoords(0, 0, 2, 2);

    QWMatrix matrix;
    matrix.translate(boundingRect.x(), boundingRect.y());
    matrix.rotate(-m_angle);
    matrix.translate(-boundingRect.x(), -boundingRect.y());

    switch (m_anchor)
    {
    case North:
        boundingRect.moveBy(-boundingRect.width() / 2, 0);
        break;
    case South:
        boundingRect.moveBy(-boundingRect.width() / 2, -boundingRect.height());
        break;
    case West:
        boundingRect.moveBy(0, -boundingRect.height() / 2);
        break;
    case East:
        boundingRect.moveBy(-boundingRect.width(), -boundingRect.height() / 2);
        break;
    case Northwest:
        // do nothing
        break;
    case Northeast:
        boundingRect.moveBy(-boundingRect.width(), 0);
        break;
    case Southwest:
        boundingRect.moveBy(0, -boundingRect.height());
        break;
    case Southeast:
        boundingRect.moveBy(-boundingRect.width(), -boundingRect.height());
        break;
    case Middle:
        boundingRect.moveBy(-boundingRect.width() / 2, -boundingRect.height() / 2);
        break;
    }

    worldPoints = matrix.map(QPointArray(boundingRect));
}

//
// DeviceSymbol
//

DeviceSymbol::DeviceSymbol()
    : m_angle(0), m_flipped(false), m_highlighted(false)
{
}

DeviceSymbol::~DeviceSymbol()
{
    for (size_t i = 0; i < m_shapes.count(); ++i)
        delete m_shapes[i];
}

void DeviceSymbol::setAngle(double a)
{
    a = fmod(a, 360);
    m_angle = a < 0 ? a + 360 : a;
}

bool DeviceSymbol::pinPosition(const QString &pinID, int &x, int &y) const
{
    QMap<QString, QPoint>::ConstIterator it = m_pinPositions.find(pinID);
    if (it != m_pinPositions.end())
    {
        x = it.data().x();
        y = it.data().y();
        return true;
    }
    return false;
}

bool DeviceSymbol::load(const KURL &url)
{
    QString errorString = i18n("Cannot load symbol file %1").arg(url.prettyURL());

    File file(url);

    if (!file.open(IO_ReadOnly))
    {
        m_errorString = errorString;
        return false;
    }

    QDomDocument doc;

    if (!doc.setContent(&file))
    {
        m_errorString = errorString;
        return false;
    }

    QDomElement root = doc.documentElement();

    if (root.tagName() != "symbol")
    {
        m_errorString = errorString;
        return false;
    }

    unload();

    for (QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
            continue;

        QDomElement elem = node.toElement();

        if (elem.tagName() == "shapes")
        {
            for (QDomNode shapeNode = node.firstChild(); !shapeNode.isNull(); shapeNode = shapeNode.nextSibling())
            {
                if (!shapeNode.isElement())
                    continue;

                QDomElement shapeElem = shapeNode.toElement();
                if (shapeElem.tagName() == "line")
                {
                    DeviceSymbolLine *line = new DeviceSymbolLine(shapeElem.attribute("x1").toInt(),
                                                                  shapeElem.attribute("y1").toInt(),
                                                                  shapeElem.attribute("x2").toInt(),
                                                                  shapeElem.attribute("y2").toInt());
                    line->setEnabled(shapeElem.attribute("enabled") != "false");
                    line->setGroup(shapeElem.attribute("group"));
                    m_shapes.append(line);
                }
                else if (shapeElem.tagName() == "rect")
                {
                    DeviceSymbolRect *rect = new DeviceSymbolRect(shapeElem.attribute("x").toInt(),
                                                                  shapeElem.attribute("y").toInt(),
                                                                  shapeElem.attribute("width").toInt(),
                                                                  shapeElem.attribute("height").toInt(),
                                                                  shapeElem.attribute("fill") == "true");
                    rect->setEnabled(shapeElem.attribute("enabled") != "false");
                    rect->setGroup(shapeElem.attribute("group"));
                    m_shapes.append(rect);
                }
                else if (shapeElem.tagName() == "ellipse")
                {
                    DeviceSymbolEllipse *ellipse = new DeviceSymbolEllipse(shapeElem.attribute("x").toInt(),
                                                                           shapeElem.attribute("y").toInt(),
                                                                           shapeElem.attribute("width").toInt(),
                                                                           shapeElem.attribute("height").toInt(),
                                                                           shapeElem.attribute("fill") == "true");
                    ellipse->setEnabled(shapeElem.attribute("enabled") != "false");
                    ellipse->setGroup(shapeElem.attribute("group"));
                    m_shapes.append(ellipse);
                }
                else if (shapeElem.tagName() == "polygon")
                {
                    QPointArray points = DeviceSymbolShape::createPointArray(shapeNode.firstChild());
                    DeviceSymbolPolygon *polygon = new DeviceSymbolPolygon(points,
                                                                           shapeElem.attribute("winding") == "true",
                                                                           shapeElem.attribute("fill") == "true");
                    polygon->setEnabled(shapeElem.attribute("enabled") != "false");
                    polygon->setGroup(shapeElem.attribute("group"));
                    m_shapes.append(polygon);
                }
                else if (shapeElem.tagName() == "polyline")
                {
                    QPointArray points = DeviceSymbolShape::createPointArray(shapeNode.firstChild());
                    DeviceSymbolPolyline *polyline = new DeviceSymbolPolyline(points);
                    polyline->setEnabled(shapeElem.attribute("enabled") != "false");
                    polyline->setGroup(shapeElem.attribute("group"));
                    m_shapes.append(polyline);
                }
                else if (shapeElem.tagName() == "cubic-bezier")
                {
                    QPointArray points = DeviceSymbolShape::createPointArray(shapeNode.firstChild());
                    DeviceSymbolCubicBezier *bezier = new DeviceSymbolCubicBezier(points);
                    bezier->setEnabled(shapeElem.attribute("enabled") != "false");
                    bezier->setGroup(shapeElem.attribute("group"));
                    m_shapes.append(bezier);
                }
            }
        }
        else if (elem.tagName() == "labels")
        {
            for (QDomNode labelNode = node.firstChild(); !labelNode.isNull(); labelNode = labelNode.nextSibling())
            {
                if (!labelNode.isElement())
                    continue;

                QDomElement labelElem = labelNode.toElement();
                if (labelElem.tagName() == "label")
                {
                    DeviceSymbolLabel label;

                    label.setID(labelElem.attribute("id"));
                    label.setX(labelElem.attribute("x").toInt());
                    label.setY(labelElem.attribute("y").toInt());

                    QString anchor = labelElem.attribute("anchor");
                    if (anchor == "north")
                        label.setAnchor(DeviceSymbolLabel::North);
                    else if (anchor == "south")
                        label.setAnchor(DeviceSymbolLabel::South);
                    else if (anchor == "west")
                        label.setAnchor(DeviceSymbolLabel::West);
                    else if (anchor == "east")
                        label.setAnchor(DeviceSymbolLabel::East);
                    else if (anchor == "northwest")
                        label.setAnchor(DeviceSymbolLabel::Northwest);
                    else if (anchor == "northeast")
                        label.setAnchor(DeviceSymbolLabel::Northeast);
                    else if (anchor == "southwest")
                        label.setAnchor(DeviceSymbolLabel::Southwest);
                    else if (anchor == "southeast")
                        label.setAnchor(DeviceSymbolLabel::Southeast);
                    else
                        label.setAnchor(DeviceSymbolLabel::Middle);
                    label.setStartAnchor(label.anchor());

                    label.setAngle(labelElem.attribute("angle").toDouble());
                    label.setStartAngle(label.angle());
                    label.setFont(labelElem.attribute("size"));
                    label.setAutoAlign(labelElem.attribute("auto-align") == "true");

                    QDomNode textNode = labelNode.firstChild();
                    if (textNode.isText())
                        label.setText(textNode.nodeValue());

                    m_labels.append(label);
                }
            }
        }
        else if (elem.tagName() == "pins")
        {
            for (QDomNode pinNode = node.firstChild(); !pinNode.isNull(); pinNode = pinNode.nextSibling())
            {
                if (!pinNode.isElement())
                    continue;

                QDomElement pinElem = pinNode.toElement();
                if (pinElem.tagName() == "pin")
                    m_pinPositions[pinElem.attribute("id")] = QPoint(pinElem.attribute("x").toInt(), pinElem.attribute("y").toInt());
            }
        }
    }

    return true;
}

void DeviceSymbol::unload()
{
    m_shapes.clear();
    m_labels.clear();
    m_pinPositions.clear();
}

void DeviceSymbol::alignLabels()
{
    for (size_t i = 0; i < m_labels.size(); ++i)
    {
        if (!m_labels[i].autoAlign())
            continue;

        double anchorAngle;

        switch (m_labels[i].startAnchor())
        {
        case DeviceSymbolLabel::East:
            anchorAngle = 0;
            break;
        case DeviceSymbolLabel::Northeast:
            anchorAngle = 45;
            break;
        case DeviceSymbolLabel::North:
            anchorAngle = 90;
            break;
        case DeviceSymbolLabel::Northwest:
            anchorAngle = 135;
            break;
        case DeviceSymbolLabel::West:
            anchorAngle = 180;
            break;
        case DeviceSymbolLabel::Southwest:
            anchorAngle = 225;
            break;
        case DeviceSymbolLabel::South:
            anchorAngle = 270;
            break;
        case DeviceSymbolLabel::Southeast:
            anchorAngle = 315;
            break;
        default:
            anchorAngle = -1;
            break;
        }

        if (anchorAngle != -1)
        {
            if ((anchorAngle = fmod(anchorAngle + (m_flipped ? -m_angle : m_angle), 360)) < 0)
                anchorAngle += 360;

            if (anchorAngle < 22.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::East);
            else if (anchorAngle < 67.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::Northeast);
            else if (anchorAngle < 112.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::North);
            else if (anchorAngle < 157.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::Northwest);
            else if (anchorAngle < 202.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::West);
            else if (anchorAngle < 247.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::Southwest);
            else if (anchorAngle < 292.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::South);
            else if (anchorAngle < 337.5)
                m_labels[i].setAnchor(DeviceSymbolLabel::Southeast);
            else
                m_labels[i].setAnchor(DeviceSymbolLabel::East);
        }

        m_labels[i].setAngle(m_labels[i].startAngle() + (m_flipped ? m_angle : -m_angle));
    }
}

QString DeviceSymbol::labelText(const QString &id) const
{
    for (size_t i = 0; i < m_labels.count(); ++i)
    {
        if (m_labels[i].id() == id)
            return m_labels[i].text();
    }
    return QString::null;
}

void DeviceSymbol::setLabelText(const QString &id, const QString &text)
{
    for (size_t i = 0; i < m_labels.count(); ++i)
    {
        if (m_labels[i].id() == id)
        {
            m_labels[i].setText(text);
            break;
        }
    }
}

QRect DeviceSymbol::boundingRect() const
{
    QRect rect;

    for (size_t i = 0; i < m_shapes.count(); ++i)
        rect |= m_shapes[i]->boundingRect();

    for (size_t i = 0; i < m_labels.count(); ++i)
    {
        QRect boundingRect;
        QPointArray worldPoints;
        m_labels[i].metrics(boundingRect, worldPoints);
        rect |= worldPoints.boundingRect();
    }

    return rect;
}

void DeviceSymbol::draw(QPainter &p, int x, int y) const
{
    p.save();
    p.translate(x, y);

    if (m_flipped)
    {
        p.scale(-1, 1);
        p.rotate(m_angle);
    }
    else
        p.rotate(-m_angle);

    if (m_highlighted)
        p.setPen(Settings::self()->deviceColorHighlighted());
    else
        p.setPen(Settings::self()->deviceColor());

    for (size_t i = 0; i < m_shapes.size(); ++i)
        if (m_shapes[i]->isEnabled())
            m_shapes[i]->draw(p);

    for (size_t i = 0; i < m_labels.size(); ++i)
    {
        p.save();
        p.setFont(m_labels[i].font());

        QRect boundingRect;
        QPointArray worldPoints;
        m_labels[i].metrics(boundingRect, worldPoints);

        if (m_flipped)
        {
            p.scale(-1, 1);
            p.translate(-worldPoints.point(1).x(), worldPoints.point(1).y());
            p.rotate(m_labels[i].angle());
        }
        else
        {
            p.translate(worldPoints.point(0).x(), worldPoints.point(0).y());
            p.rotate(-m_labels[i].angle());
        }

        double totalAngle = m_flipped ? fmod(m_angle - m_labels[i].angle(), 360) : fmod(m_angle + m_labels[i].angle(), 360);
        if (totalAngle < 0)
            totalAngle += 360;

        if (totalAngle > 90 && totalAngle < 270)
        {
            p.translate(boundingRect.width() - 1, boundingRect.height() - 1);
            p.rotate(180);
        }

        p.drawText(0, 0, boundingRect.width(), boundingRect.height(), Qt::AlignHCenter | Qt::AlignVCenter, m_labels[i].text());
        p.restore();
    }

    p.restore();
}

void DeviceSymbol::setShapeGroupEnabled(const QString &group, bool enabled)
{
    for (size_t i = 0; i < m_shapes.count(); ++i)
        if (m_shapes[i]->group() == group)
            m_shapes[i]->setEnabled(enabled);
}

// vim: ts=4 sw=4 et
