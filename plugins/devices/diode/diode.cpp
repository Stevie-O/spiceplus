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
#include <qcheckbox.h>

#include <klocale.h>
#include <kdialog.h>

#include "diode.h"
#include "modelfile.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_device_diode, DiodeFactory)

//
// DiodeSelector
//

DiodeSelector::DiodeSelector(QWidget *parent)
    : ModelSelector("d", parent)
{
}

bool DiodeSelector::isDeviceSelected() const
{
    return isModelSelected();
}

SchematicDevice *DiodeSelector::createSelectedDevice(Schematic *schematic)
{
    if (!isModelSelected())
    {
        setErrorString(i18n("No model selected"));
        return 0;
    }

    Diode *d = new Diode(schematic);

    ModelFile model;
    if (model.load(modelURL()))
    {
        d->setModelPath(modelPath());
        d->setModelName(model.name());
        if (!d->loadSymbol(model.symbolPath()))
        {
            setErrorString(d->errorString());
            delete d;
            d = 0;
        }
    }
    else
    {
        setErrorString(model.errorString());
        delete d;
        d = 0;
    }

    return d;
}

//
// DiodeFactory
//

SchematicDevice *DiodeFactory::createDevice(Schematic *schematic)
{
    return new Diode(schematic);
}

SchematicDeviceSelector *DiodeFactory::createSelector(QWidget *parent)
{
    ModelSelector *sel = new DiodeSelector(parent);
    if (!sel->setupTree())
    {
        setErrorString(sel->errorString());
        delete sel;
        return 0;
    }

    return sel;
}

//
// Diode
//

Diode::Diode(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(new SchematicDevicePin("p", this));
    addPin(new SchematicDevicePin("n", this));
}

bool Diode::initialize()
{
    return true;
}

void Diode::updateLabels()
{
    setLabelText("name", name());
    setLabelText("model", modelName());
    setShapeGroupEnabled("ic", !parameter("ic").isEmpty());
}

void Diode::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(type().upper()));
    setParameter("area", "1");
    updateLabels();
}

QString Diode::createCommand()
{
    SchematicDevicePin *pinP = findPin("p");
    SchematicDevicePin *pinN = findPin("n");

    if (!pinP || !pinN)
    {
        setErrorString(i18n("Diode %1: Pin not found").arg(name()));
        return QString::null;
    }

    if (!pinP->node() || !pinN->node())
    {
        setErrorString(i18n("Diode %1: Pin(s) not connected").arg(name()));
        return QString::null;
    }

    QString cmd = type() + name().lower() + " " + pinP->node()->name() + " " + pinN->node()->name() + " " + modelID() + " " + parameter("area");

    if (parameter("off") == "true")
        cmd += " off";

    if (!parameter("ic").isEmpty())
        cmd += " ic=" + parameter("ic");

    if (!parameter("temp").isEmpty())
        cmd += " temp=" + parameter("temp");

    return cmd;
}

SchematicDevicePropertiesWidget *Diode::createPropertiesWidget(QWidget *parent)
{
    return new DiodePropertiesWidget(this, parent);
}

//
// DiodePropertiesWidget
//

DiodePropertiesWidget::DiodePropertiesWidget(Diode *diode, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_diode(diode)
{
    QGridLayout *grid = new QGridLayout(this, 5, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    grid->addWidget(m_name = new ParameterLineEdit(m_diode->name(), this, ParameterLineEdit::String), 0, 1);

    grid->addWidget(new QLabel(i18n("Area:"), this), 1, 0);
    grid->addWidget(m_area = new ParameterLineEdit(m_diode->parameter("area"), "1", this), 1, 1);

    grid->addWidget(new QLabel(i18n("Initial condition:"), this), 2, 0);
    grid->addWidget(m_ic = new ParameterLineEdit(m_diode->parameter("ic"), this), 2, 1);
    grid->addWidget(new QLabel(i18n("Volts"), this), 2, 2);

    grid->addWidget(new QLabel(i18n("Temperature:"), this), 3, 0);
    grid->addWidget(m_temp = new ParameterLineEdit(m_diode->parameter("temp"), this), 3, 1);
    grid->addWidget(new QLabel(i18n("Degrees Centigrade"), this), 3, 2);

    m_off = new QCheckBox(i18n("Starting condition for DC analysis: OFF"), this);
    m_off->setChecked(m_diode->parameter("off") == "true");
    grid->addMultiCellWidget(m_off, 4, 4, 0, 2);

    m_name->setFocus();
}

bool DiodePropertiesWidget::apply()
{
    if (m_name->text().lower() != m_diode->name().lower() && m_diode->schematic()->findDevice(m_name->text()))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (m_name->text().isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    m_diode->setName(m_name->text());
    m_diode->setParameter("area", m_area->text());
    m_diode->setParameter("ic", m_ic->text());
    m_diode->setParameter("temp", m_temp->text());
    m_diode->setParameter("off", m_off->isChecked() ? "true" : "false");

    m_diode->updateLabels();
    return true;
}

#include "diode.moc"

// vim: ts=4 sw=4 et
