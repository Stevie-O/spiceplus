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

#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdialog.h>

#include "schematicammeter.h"
#include "parameterlineedit.h"

using namespace Spiceplus;

//
// SchematicAmmeter
//

const char *SchematicAmmeter::ID = "_Ammeter";
const char *SchematicAmmeter::Name = "AM";

SchematicAmmeter::SchematicAmmeter(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(new SchematicDevicePin("p", this));
    addPin(new SchematicDevicePin("n", this));
}

bool SchematicAmmeter::initialize()
{
    if (!loadSymbol())
        return false;

    setName(labelText("name"));

    return true;
}

void SchematicAmmeter::updateLabels()
{
    setLabelText("name", name());
}

void SchematicAmmeter::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(Name));
    updateLabels();
}

QString SchematicAmmeter::createCommand()
{
    SchematicDevicePin *pinP = findPin("p");
    SchematicDevicePin *pinN = findPin("n");

    if (!pinP || !pinN)
    {
        setErrorString(i18n("Ammeter %1: Pin not found").arg(name()));
        return QString::null;
    }

    if (!pinP->node() || !pinN->node())
    {
        setErrorString(i18n("Ammeter %1: Pin not connected").arg(name()));
        return QString::null;
    }

    return "v" + name().lower() + " " + pinP->node()->name() + " " + pinN->node()->name() + " 0";
}

SchematicDevicePropertiesWidget *SchematicAmmeter::createPropertiesWidget(QWidget *parent)
{
    return new SchematicAmmeterPropertiesWidget(this, parent);
}

//
// SchematicAmmeterPropertiesWidget
//

SchematicAmmeterPropertiesWidget::SchematicAmmeterPropertiesWidget(SchematicAmmeter *ammeter, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_ammeter(ammeter)
{
    QGridLayout *grid = new QGridLayout(this, 1, 2, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    grid->addWidget(m_name = new ParameterLineEdit(m_ammeter->name(), this, ParameterLineEdit::String), 0, 1);

    m_name->setFocus();
}

SchematicAmmeterPropertiesWidget::~SchematicAmmeterPropertiesWidget()
{
}

bool SchematicAmmeterPropertiesWidget::apply()
{
    QString name = m_name->text();

    if (name.lower() != m_ammeter->name().lower() && m_ammeter->schematic()->findDevice(name))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (name.isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    m_ammeter->setName(name);
    m_ammeter->updateLabels();

    return true;
}

#include "schematicammeter.moc"

// vim: ts=4 sw=4 et
