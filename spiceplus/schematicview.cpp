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

#include "schematicview.h"
#include "schematic.h"
#include "schematiccommandhistory.h"
#include "schematictool.h"
#include "settings.h"

using namespace Spiceplus;

const double SchematicView::s_zoomFactors[] = { 400, 200, 100, 50, 25, -1 };

SchematicView::SchematicView(QWidget *parent, const char *name, WFlags f)
    : QCanvasView(parent, name, f)
{
    init();
}

SchematicView::SchematicView(Schematic *schematic, QWidget *parent, const char *name, WFlags f)
    : QCanvasView(schematic, parent, name, f)
{
    init();
}

SchematicView::~SchematicView()
{
    delete m_tool;
}

void SchematicView::init()
{
    viewport()->setMouseTracking(true);

    m_zoomFactorIndex = 2;
    m_tool = 0;
    m_history = new SchematicCommandHistory(this);

    connect(Settings::self(), SIGNAL(settingsChanged()), SLOT(resetTool()));
}

Schematic *SchematicView::schematic() const
{
    return dynamic_cast<Schematic *>(canvas());
}

void SchematicView::setSchematic(Schematic *schematic)
{
    setCanvas(schematic);
}

void SchematicView::setTool(SchematicTool *tool)
{
    delete m_tool;
    m_tool = tool;
}

void SchematicView::resetTool()
{
    m_tool->reset();
}

void SchematicView::undo()
{
    resetTool();

    m_history->undo();
    schematic()->update();
}

void SchematicView::redo()
{
    resetTool();

    m_history->redo();
    schematic()->update();
}

void SchematicView::zoomIn()
{
    if (!isZoomInAvailable())
        return;

    if (!isZoomOutAvailable())
        emit zoomOutAvailable(true);

    --m_zoomFactorIndex;

    QWMatrix wm;
    wm.scale(s_zoomFactors[m_zoomFactorIndex] / 100.0, s_zoomFactors[m_zoomFactorIndex] / 100.0);
    setWorldMatrix(wm);

    if (!isZoomInAvailable())
        emit zoomInAvailable(false);

    emit zoomChanged(m_zoomFactorIndex);
}

void SchematicView::zoomOut()
{
    if (!isZoomOutAvailable())
        return;

    if (!isZoomInAvailable())
        emit zoomInAvailable(true);

    ++m_zoomFactorIndex;

    QWMatrix wm;
    wm.scale(s_zoomFactors[m_zoomFactorIndex] / 100.0, s_zoomFactors[m_zoomFactorIndex] / 100.0);
    setWorldMatrix(wm);

    if (!isZoomOutAvailable())
        emit zoomOutAvailable(false);

    emit zoomChanged(m_zoomFactorIndex);
}

void SchematicView::zoom(int zoomFactorIndex)
{
    int maxIndex;
    for (maxIndex = 0; s_zoomFactors[maxIndex] != -1; ++maxIndex);

    if (zoomFactorIndex < maxIndex)
    {
        QWMatrix wm;
        wm.scale(s_zoomFactors[zoomFactorIndex] / 100.0, s_zoomFactors[zoomFactorIndex] / 100.0);
        setWorldMatrix(wm);

        m_zoomFactorIndex = zoomFactorIndex;

        emit zoomInAvailable(isZoomInAvailable());
        emit zoomOutAvailable(isZoomOutAvailable());
    }
}

bool SchematicView::isZoomInAvailable() const
{
    return m_zoomFactorIndex != 0;
}

bool SchematicView::isZoomOutAvailable() const
{
    return s_zoomFactors[m_zoomFactorIndex + 1] != -1;
}

void SchematicView::leaveEvent(QEvent *event)
{
    m_tool->processLeaveEvent(event);
}

void SchematicView::contentsMouseDoubleClickEvent(QMouseEvent *event)
{
    m_tool->processMouseDoubleClickEvent(event);
}

void SchematicView::contentsMousePressEvent(QMouseEvent *event)
{
    m_tool->processMousePressEvent(event);
}

void SchematicView::contentsMouseReleaseEvent(QMouseEvent *event)
{
    m_tool->processMouseReleaseEvent(event);
}

void SchematicView::contentsMouseMoveEvent(QMouseEvent *event)
{
    m_tool->processMouseMoveEvent(event);
}

#include "schematicview.moc"

// vim: ts=4 sw=4 et
