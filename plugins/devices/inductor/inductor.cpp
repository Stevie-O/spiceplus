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
#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdialog.h>

#include "inductor.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_device_inductor, InductorFactory)

//
// InductorSelector
//

InductorSelector::InductorSelector(QWidget *parent)
    : SchematicDeviceSelector(parent)
{
}

bool InductorSelector::isDeviceSelected() const
{
    return true;
}

SchematicDevice *InductorSelector::createSelectedDevice(Schematic *schematic)
{
    Inductor *i = new Inductor(schematic);

    if (!i->loadSymbol())
    {
        setErrorString(i->errorString());
        delete i;
        i = 0;
    }

    return i;
}

//
// InductorFactory
//

SchematicDevice *InductorFactory::createDevice(Schematic *schematic)
{
    return new Inductor(schematic);
}

SchematicDeviceSelector *InductorFactory::createSelector(QWidget *parent)
{
    return new InductorSelector(parent);
}

//
// Inductor
//

Inductor::Inductor(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(new SchematicDevicePin("p", this));
    addPin(new SchematicDevicePin("n", this));
}

bool Inductor::initialize()
{
    return true;
}

void Inductor::updateLabels()
{
    setLabelText("name", name());
    setLabelText("value", parameter("value"));
    setShapeGroupEnabled("ic", !parameter("ic").isEmpty());
}

void Inductor::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(type().upper()));
    setParameter("value", "1u");

    updateLabels();
}

QString Inductor::createCommand()
{
    QString cmd;
    SchematicDevicePin *pinP = findPin("p");
    SchematicDevicePin *pinN = findPin("n");

    if (!pinP || !pinN)
    {
        setErrorString(i18n("Inductor %1: Pin not found").arg(name()));
        return cmd;
    }

    if (!pinP->node() || !pinN->node())
    {
        setErrorString(i18n("Inductor %1: Pin(s) not connected").arg(name()));
        return cmd;
    }

    cmd = type() + name().lower() + " " + pinP->node()->name() + " " + pinN->node()->name() + " " + parameter("value");

    if (!parameter("ic").isEmpty())
        cmd += " ic=" + parameter("ic");

    return cmd;
}

SchematicDevicePropertiesWidget *Inductor::createPropertiesWidget(QWidget *parent)
{
    return new InductorPropertiesWidget(this, parent);
}

//
// InductorPropertiesWidget
//

InductorPropertiesWidget::InductorPropertiesWidget(Inductor *inductor, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_inductor(inductor)
{
    QGridLayout *grid = new QGridLayout(this, 3, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    grid->addWidget(m_name = new ParameterLineEdit(m_inductor->name(), this, ParameterLineEdit::String), 0, 1);

    grid->addWidget(new QLabel(i18n("Inductance:"), this), 1, 0);
    grid->addWidget(m_value = new ParameterLineEdit(m_inductor->parameter("value"), this), 1, 1);
    grid->addWidget(new QLabel(i18n("Henries"), this), 1, 2);

    grid->addWidget(new QLabel(i18n("Initial condition:"), this), 2, 0);
    grid->addWidget(m_ic = new ParameterLineEdit(m_inductor->parameter("ic"), this), 2, 1);
    grid->addWidget(new QLabel(i18n("Amps"), this), 2, 2);

    m_name->setFocus();
}

bool InductorPropertiesWidget::apply()
{
    QString name = m_name->text();
    QString value = m_value->text();
    QString ic = m_ic->text();

    if (name.lower() != m_inductor->name().lower() && m_inductor->schematic()->findDevice(name))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (name.isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    if (value.isEmpty())
    {
        setErrorString(i18n("No inductance specified"));
        return false;
    }

    m_inductor->setName(name);
    m_inductor->setParameter("value", value);
    m_inductor->setParameter("ic", ic);

    m_inductor->updateLabels();

    return true;
}

#include "inductor.moc"

// vim: ts=4 sw=4 et
