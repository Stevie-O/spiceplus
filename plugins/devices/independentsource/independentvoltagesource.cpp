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

#include <qstring.h>
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kdialog.h>

#include "independentvoltagesource.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_device_independentvoltagesource, IndependentVoltageSourceFactory)

//
// IndependentVoltageSourceSelector
//

IndependentVoltageSourceSelector::IndependentVoltageSourceSelector(QWidget *parent)
    : SchematicDeviceSelector(parent)
{
}

bool IndependentVoltageSourceSelector::isDeviceSelected() const
{
    return true;
}

SchematicDevice *IndependentVoltageSourceSelector::createSelectedDevice(Schematic *schematic)
{
    IndependentVoltageSource *s = new IndependentVoltageSource(schematic);

    if (!s->loadSymbol())
    {
        setErrorString(s->errorString());
        delete s;
        s = 0;
    }

    return s;
}

//
// IndependentVoltageSourceFactory
//

SchematicDevice *IndependentVoltageSourceFactory::createDevice(Schematic *schematic)
{
    return new IndependentVoltageSource(schematic);
}

SchematicDeviceSelector *IndependentVoltageSourceFactory::createSelector(QWidget *parent)
{
    return new IndependentVoltageSourceSelector(parent);
}

#include "independentvoltagesource.moc"

// vim: ts=4 sw=4 et
