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

#ifndef SCHEMATICSYMBOL_H
#define SCHEMATICSYMBOL_H

#include <qvaluevector.h>
#include <qpointarray.h>
#include <qvariant.h>
#include <qrect.h>
#include <qfont.h>

class QDomNode;
class KURL;

namespace Spiceplus {

class DeviceSymbolShape
{
public:
    DeviceSymbolShape() : m_enabled(true) {}
    virtual ~DeviceSymbolShape() {}

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    QString group() const { return m_group; }
    void setGroup(const QString &group) { m_group = group; }

    virtual QRect boundingRect() const = 0;
    virtual void draw(QPainter &p) const = 0;

    static QPointArray createPointArray(QDomNode node);

private:
    bool m_enabled;
    QString m_group;
};

class DeviceSymbolLine : public DeviceSymbolShape
{
public:
    DeviceSymbolLine(int x1, int y1, int x2, int y2) : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2) {}

    QRect boundingRect() const;
    void draw(QPainter &p) const;

private:
    int m_x1;
    int m_y1;
    int m_x2;
    int m_y2;
};

class DeviceSymbolRect : public DeviceSymbolShape
{
public:
    DeviceSymbolRect(int x, int y, int width, int height, bool fill) : m_rect(x, y, width, height), m_fill(fill) {}

    QRect boundingRect() const;
    void draw(QPainter &p) const;

private:
    QRect m_rect;
    bool m_fill;
};

class DeviceSymbolEllipse : public DeviceSymbolShape
{
public:
    DeviceSymbolEllipse(int x, int y, int width, int height, bool fill) : m_rect(x, y, width, height), m_fill(fill) {}

    QRect boundingRect() const;
    void draw(QPainter &p) const;

private:
    QRect m_rect;
    bool m_fill;
};

class DeviceSymbolPolygon : public DeviceSymbolShape
{
public:
    DeviceSymbolPolygon(const QPointArray &points, bool winding, bool fill) : m_points(points), m_winding(winding), m_fill(fill) {}

    QRect boundingRect() const;
    void draw(QPainter &p) const;

private:
    QPointArray m_points;
    bool m_winding;
    bool m_fill;
};

class DeviceSymbolPolyline : public DeviceSymbolShape
{
public:
    DeviceSymbolPolyline(const QPointArray &points) : m_points(points) {}

    QRect boundingRect() const;
    void draw(QPainter &p) const;

private:
    QPointArray m_points;
};

class DeviceSymbolCubicBezier : public DeviceSymbolShape
{
public:
    DeviceSymbolCubicBezier(const QPointArray &points) : m_points(points) {}

    QRect boundingRect() const;
    void draw(QPainter &p) const;

private:
    QPointArray m_points;
};

class DeviceSymbolLabel
{
public:
    enum TextAnchor
    {
        North,
        South,
        West,
        East,
        Northwest,
        Northeast,
        Southwest,
        Southeast,
        Middle
    };

    DeviceSymbolLabel();

    QString id() const { return m_id; }
    void setID(const QString &id) { m_id = id; }

    int x() const { return m_x; }
    void setX(int x) { m_x = x; }
    int y() const { return m_y; }
    void setY(int y) { m_y = y; }

    TextAnchor startAnchor() const { return m_startAnchor; }
    void setStartAnchor(TextAnchor a) { m_startAnchor = a; }
    TextAnchor anchor() const { return m_anchor; }
    void setAnchor(TextAnchor a) { m_anchor = a; }

    double startAngle() const { return m_startAngle; }
    void setStartAngle(double a) { m_startAngle = a; }
    double angle() const { return m_angle; }
    void setAngle(double a);

    QFont font() const;
    void setFont(const QString &fontSize);

    QString text() const { return m_text; }
    void setText(const QString &text) { m_text = text; }

    bool autoAlign() const { return m_autoAlign; }
    void setAutoAlign(bool yes) { m_autoAlign = yes; }

    void metrics(QRect &boundingRect, QPointArray &worldPoints) const;

private:
    enum FontSize
    {
        Tiny,
        Small,
        Normal,
        Large,
        Huge
    };

    QString m_id;
    int m_x;
    int m_y;

    TextAnchor m_startAnchor;
    TextAnchor m_anchor;

    double m_startAngle;
    double m_angle;

    FontSize m_fontSize;
    QString m_text;
    bool m_autoAlign;
};

class DeviceSymbol
{
public:
    DeviceSymbol();
    ~DeviceSymbol();

    double angle() const { return m_angle; }
    void setAngle(double a);

    bool flipped() const { return m_flipped; }
    void setFlipped(bool flipped) { m_flipped = flipped; }

    bool highlighted() const { return m_highlighted; }
    void setHighlighted(bool yes) { m_highlighted = yes; }

    bool pinPosition(const QString &pinID, int &x, int &y) const;

    bool load(const KURL &url);
    void unload();

    void alignLabels();
    QString labelText(const QString &id) const;
    void setLabelText(const QString &id, const QString &text);

    QRect boundingRect() const;
    void draw(QPainter &p, int x, int y) const;
    void setShapeGroupEnabled(const QString &group, bool enabled);

    QString errorString() const { return m_errorString; }

private:
    double m_angle;
    bool m_flipped;
    bool m_highlighted;

    QValueVector<DeviceSymbolShape *> m_shapes;
    QValueVector<DeviceSymbolLabel> m_labels;
    QMap<QString, QPoint> m_pinPositions;

    QString m_errorString;
};

} // namespace Spiceplus

#endif // SCHEMATICSYMBOL_H

// vim: ts=4 sw=4 et
