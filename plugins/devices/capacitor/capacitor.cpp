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

#include "capacitor.h"
#include "modelfile.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_device_capacitor, CapacitorFactory)

//
// CapacitorSelector
//

CapacitorSelector::CapacitorSelector(QWidget *parent)
    : ModelSelector("c", parent)
{
}

bool CapacitorSelector::isDeviceSelected() const
{
    return true;
}

SchematicDevice *CapacitorSelector::createSelectedDevice(Schematic *schematic)
{
    Capacitor *c = new Capacitor(schematic);

    if (isModelSelected())
    {
        ModelFile model;
        if (model.load(modelURL()))
        {
            c->setModelPath(modelPath());
            c->setModelName(model.name());
            if (!c->loadSymbol(model.symbolPath()))
            {
                setErrorString(c->errorString());
                delete c;
                c = 0;
            }
        }
        else
        {
            setErrorString(model.errorString());
            delete c;
            c = 0;
        }
    }
    else if (!c->loadSymbol())
    {
        setErrorString(c->errorString());
        delete c;
        c = 0;
    }

    return c;
}

//
// CapacitorFactory
//

SchematicDevice *CapacitorFactory::createDevice(Schematic *schematic)
{
    return new Capacitor(schematic);
}

SchematicDeviceSelector *CapacitorFactory::createSelector(QWidget *parent)
{
    ModelSelector *sel = new CapacitorSelector(parent);
    if (!sel->setupTree())
    {
        setErrorString(sel->errorString());
        delete sel;
        return 0;
    }

    return sel;
}

//
// Capacitor
//

Capacitor::Capacitor(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(new SchematicDevicePin("p", this));
    addPin(new SchematicDevicePin("n", this));
}

bool Capacitor::initialize()
{
    return true;
}

void Capacitor::updateLabels()
{
    setLabelText("name", name());

    if (hasModel())
        setLabelText("value", modelName());
    else
        setLabelText("value", parameter("value"));

    setShapeGroupEnabled("ic", !parameter("ic").isEmpty());
}

void Capacitor::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(type().upper()));

    if (hasModel())
        setParameter("l", "10u");
    else
        setParameter("value", "1n");

    updateLabels();
}

QString Capacitor::createCommand()
{
    SchematicDevicePin *pinP = findPin("p");
    SchematicDevicePin *pinN = findPin("n");

    if (!pinP || !pinN)
    {
        setErrorString(i18n("Capacitor %1: Pin not found").arg(name()));
        return QString::null;
    }

    if (!pinP->node() || !pinN->node())
    {
        setErrorString(i18n("Capacitor %1: Pin(s) not connected").arg(name()));
        return QString::null;
    }

    QString cmd = type() + name().lower() + " " + pinP->node()->name() + " " + pinN->node()->name() + " ";

    if (hasModel())
    {
        cmd += modelID();

        if (parameter("l").isEmpty())
        {
            setErrorString(i18n("Capacitor %1: No length specified").arg(name()));
            return QString::null;
        }
        cmd += " l=" + parameter("l");

        if (!parameter("w").isEmpty())
            cmd += " w=" + parameter("w");
    }
    else
        cmd += parameter("value");

    if (!parameter("ic").isEmpty())
        cmd += " ic=" + parameter("ic");

    return cmd;
}

SchematicDevicePropertiesWidget *Capacitor::createPropertiesWidget(QWidget *parent)
{
    return new CapacitorPropertiesWidget(this, parent);
}

//
// CapacitorPropertiesWidget
//

CapacitorPropertiesWidget::CapacitorPropertiesWidget(Capacitor *capacitor, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_capacitor(capacitor)
{
    QGridLayout *grid;
    if (m_capacitor->hasModel())
        grid = new QGridLayout(this, 4, 3, 0, KDialog::spacingHint());
    else
        grid = new QGridLayout(this, 3, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    grid->addWidget(m_name = new ParameterLineEdit(m_capacitor->name(), this, ParameterLineEdit::String), 0, 1);

    if (m_capacitor->hasModel())
    {
        grid->addWidget(new QLabel(i18n("Length:"), this), 1, 0);
        grid->addWidget(m_l = new ParameterLineEdit(m_capacitor->parameter("l"), this), 1, 1);
        grid->addWidget(new QLabel(i18n("Meters"), this), 1, 2);

        grid->addWidget(new QLabel(i18n("Width:"), this), 2, 0);
        grid->addWidget(m_w = new ParameterLineEdit(m_capacitor->parameter("w"), this), 2, 1);
        grid->addWidget(new QLabel(i18n("Meters"), this), 2, 2);

        m_value = 0;
    }
    else
    {
        grid->addWidget(new QLabel(i18n("Capacitance:"), this), 1, 0);
        grid->addWidget(m_value = new ParameterLineEdit(m_capacitor->parameter("value"), this), 1, 1);
        grid->addWidget(new QLabel(i18n("Farads"), this), 1, 2);

        m_l = m_w = 0;
    }

    grid->addWidget(new QLabel(i18n("Initial condition:"), this), m_capacitor->hasModel() ? 3 : 2, 0);
    grid->addWidget(m_ic = new ParameterLineEdit(m_capacitor->parameter("ic"), this), m_capacitor->hasModel() ? 3 : 2, 1);
    grid->addWidget(new QLabel(i18n("Volts"), this), m_capacitor->hasModel() ? 3 : 2, 2);

    m_name->setFocus();
}

bool CapacitorPropertiesWidget::apply()
{
    if (m_name->text().lower() != m_capacitor->name().lower() && m_capacitor->schematic()->findDevice(m_name->text()))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (m_name->text().isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    if (m_capacitor->hasModel() && m_l->text().isEmpty())
    {
        setErrorString(i18n("Length must be specified"));
        return false;
    }

    if (!m_capacitor->hasModel() && m_value->text().isEmpty())
    {
        setErrorString(i18n("No capacitance specified"));
        return false;
    }

    m_capacitor->setName(m_name->text());

    if (m_capacitor->hasModel())
    {
        m_capacitor->setParameter("l", m_l->text());
        m_capacitor->setParameter("w", m_w->text());
    }
    else
        m_capacitor->setParameter("value", m_value->text());

    m_capacitor->setParameter("ic", m_ic->text());

    m_capacitor->updateLabels();
    return true;
}

#include "capacitor.moc"

// vim: ts=4 sw=4 et
