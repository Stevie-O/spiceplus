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

#include "resistor.h"
#include "modelfile.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_device_resistor, ResistorFactory)

//
// ResistorSelector
//

ResistorSelector::ResistorSelector(QWidget *parent)
    : ModelSelector("r", parent)
{
}

bool ResistorSelector::isDeviceSelected() const
{
    return true;
}

SchematicDevice *ResistorSelector::createSelectedDevice(Schematic *schematic)
{
    Resistor *r = new Resistor(schematic);

    if (isModelSelected())
    {
        ModelFile model;
        if (model.load(modelURL()))
        {
            r->setModelPath(modelPath());
            r->setModelName(model.name());
            if (!r->loadSymbol(model.symbolPath()))
            {
                setErrorString(r->errorString());
                delete r;
                r = 0;
            }
        }
        else
        {
            setErrorString(model.errorString());
            delete r;
            r = 0;
        }
    }
    else if (!r->loadSymbol())
    {
        setErrorString(r->errorString());
        delete r;
        r = 0;
    }

    return r;
}

//
// ResistorFactory
//

SchematicDevice *ResistorFactory::createDevice(Schematic *schematic)
{
    return new Resistor(schematic);
}

SchematicDeviceSelector *ResistorFactory::createSelector(QWidget *parent)
{
    ModelSelector *sel = new ResistorSelector(parent);
    if (!sel->setupTree())
    {
        setErrorString(sel->errorString());
        delete sel;
        return 0;
    }

    return sel;
}

//
// Resistor
//

Resistor::Resistor(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(new SchematicDevicePin("a", this));
    addPin(new SchematicDevicePin("b", this));
}

bool Resistor::initialize()
{
    setName(labelText("name"));
    setParameter("value", labelText("value"));
    return true;
}

void Resistor::updateLabels()
{
    setLabelText("name", name());

    if (hasModel())
        setLabelText("value", modelName());
    else
        setLabelText("value", parameter("value"));
}

void Resistor::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(type().upper()));

    if (hasModel())
        setParameter("l", "10u");
    else
        setParameter("value", "1k");

    updateLabels();
}

QString Resistor::createCommand()
{
    SchematicDevicePin *pinA = findPin("a");
    SchematicDevicePin *pinB = findPin("b");

    if (!pinA || !pinB)
    {
        setErrorString(i18n("Resistor %1: Pin not found").arg(name()));
        return QString::null;
    }

    if (!pinA->node() || !pinB->node())
    {
        setErrorString(i18n("Resistor %1: Pin(s) not connected").arg(name()));
        return QString::null;
    }

    QString cmd = type() + name().lower() + " " + pinA->node()->name() + " " + pinB->node()->name() + " ";

    if (hasModel())
    {
        cmd += modelID();

        if (parameter("l").isEmpty())
        {
            setErrorString(i18n("Resistor %1: No length specified").arg(name()));
            return QString::null;
        }
        cmd += " l=" + parameter("l");

        if (!parameter("w").isEmpty())
            cmd += " w=" + parameter("w");
    }
    else
        cmd += parameter("value");

    if (!parameter("temp").isEmpty())
        cmd += " temp=" + parameter("temp");

    return cmd;
}

SchematicDevicePropertiesWidget *Resistor::createPropertiesWidget(QWidget *parent)
{
    return new ResistorPropertiesWidget(this, parent);
}

//
// ResistorPropertiesWidget
//

ResistorPropertiesWidget::ResistorPropertiesWidget(Resistor *resistor, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_resistor(resistor)
{
    QGridLayout *grid;
    if (m_resistor->hasModel())
        grid = new QGridLayout(this, 4, 3, 0, KDialog::spacingHint());
    else
        grid = new QGridLayout(this, 3, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    grid->addWidget(m_name = new ParameterLineEdit(m_resistor->name(), this, ParameterLineEdit::String), 0, 1);

    if (m_resistor->hasModel())
    {
        grid->addWidget(new QLabel(i18n("Length:"), this), 1, 0);
        grid->addWidget(m_l = new ParameterLineEdit(m_resistor->parameter("l"), this), 1, 1);
        grid->addWidget(new QLabel(i18n("Meters"), this), 1, 2);

        grid->addWidget(new QLabel(i18n("Width:"), this), 2, 0);
        grid->addWidget(m_w = new ParameterLineEdit(m_resistor->parameter("w"), this), 2, 1);
        grid->addWidget(new QLabel(i18n("Meters"), this), 2, 2);

        m_value = 0;
    }
    else
    {
        grid->addWidget(new QLabel(i18n("Resistance:"), this), 1, 0);
        grid->addWidget(m_value = new ParameterLineEdit(m_resistor->parameter("value"), this), 1, 1);
        grid->addWidget(new QLabel(i18n("Ohms"), this), 1, 2);

        m_l = m_w = 0;
    }

    grid->addWidget(new QLabel(i18n("Temperature:"), this), m_resistor->hasModel() ? 3 : 2, 0);
    grid->addWidget(m_temp = new ParameterLineEdit(m_resistor->parameter("temp"), this), m_resistor->hasModel() ? 3 : 2, 1);
    grid->addWidget(new QLabel(i18n("Degrees Centigrade"), this), m_resistor->hasModel() ? 3 : 2, 2);

    m_name->setFocus();
}

bool ResistorPropertiesWidget::apply()
{
    if (m_name->text().lower() != m_resistor->name().lower() && m_resistor->schematic()->findDevice(m_name->text()))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (m_name->text().isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    if (m_resistor->hasModel() && m_l->text().isEmpty())
    {
        setErrorString(i18n("Length must be specified"));
        return false;
    }

    if (!m_resistor->hasModel() && m_value->text().isEmpty())
    {
        setErrorString(i18n("No resistance specified"));
        return false;
    }

    m_resistor->setName(m_name->text());

    if (m_resistor->hasModel())
    {
        m_resistor->setParameter("l", m_l->text());
        m_resistor->setParameter("w", m_w->text());
    }
    else
        m_resistor->setParameter("value", m_value->text());

    m_resistor->setParameter("temp", m_temp->text());

    m_resistor->updateLabels();
    return true;
}

#include "resistor.moc"

// vim: ts=4 sw=4 et
