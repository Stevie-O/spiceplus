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

#include <qwt/qwt_plot_canvas.h>

#include "plot.h"

using namespace Spiceplus;

Plot::Plot(QWidget *parent)
    : QwtPlot(parent)
{
    init();
}

Plot::Plot(const QString &title, QWidget *parent)
    : QwtPlot(title, parent)
{
    init();
}

void Plot::init()
{
    setCanvasBackground(Qt::white);
    canvas()->setFrameStyle(QFrame::Plain | QFrame::Box);
    canvas()->setLineWidth(1);
    canvas()->setMidLineWidth(0);

    enableGridXMin();
    setGridMajPen(QPen(Qt::lightGray, 0, DotLine));
    setGridMinPen(QPen(Qt::lightGray, 0 , DotLine));
}

#include "plot.moc"

// vim: ts=4 sw=4 et
