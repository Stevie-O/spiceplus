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

#ifndef SCHEMATICVIEW_H
#define SCHEMATICVIEW_H

#include <qcanvas.h>
#include <qvaluevector.h>

namespace Spiceplus {

class Schematic;
class SchematicTool;
class SchematicCommandHistory;

class SchematicView : public QCanvasView
{
    Q_OBJECT

public:
    SchematicView(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    SchematicView(Schematic *schematic, QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    ~SchematicView();

    Schematic *schematic() const;
    void setSchematic(Schematic *schematic);
    SchematicCommandHistory *history() const { return m_history; }

    SchematicTool *tool() const { return m_tool; }
    void setTool(SchematicTool *tool);

    void undo();
    void redo();
    void zoomIn();
    void zoomOut();
    void zoom(int zoomFactorIndex);
    int zoomFactorIndex() const { return m_zoomFactorIndex; }

    bool isZoomInAvailable() const;
    bool isZoomOutAvailable() const;

    static const double *zoomFactors() { return s_zoomFactors; }

public slots:
    void resetTool();

signals:
    void zoomInAvailable(bool yes);
    void zoomOutAvailable(bool yes);
    void zoomChanged(int zoomFactorIndex);

protected:
    void leaveEvent(QEvent *event);
    void contentsMouseDoubleClickEvent(QMouseEvent *event);
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseReleaseEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent *event);

private:
    void init();

    static const double s_zoomFactors[];
    int m_zoomFactorIndex;

    SchematicTool *m_tool;
    SchematicCommandHistory *m_history;
};

} // namespace Spiceplus

#endif // SCHEMATICVIEW_H

// vim: ts=4 sw=4 et
