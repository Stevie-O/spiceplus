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

#include "schematictestpoint.h"
#include "parameterlineedit.h"

using namespace Spiceplus;

//
// SchematicTestPoint
//

const char *SchematicTestPoint::ID = "_TestPoint";
const char *SchematicTestPoint::Name = "TP";

SchematicTestPoint::SchematicTestPoint(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(m_pin = new SchematicDevicePin("pin", this));
}

SchematicDevicePin *SchematicTestPoint::pin() const
{
    return m_pin;
}

QString SchematicTestPoint::nodeName() const
{
    QString nn;
    if (m_pin && m_pin->node())
        nn = m_pin->node()->name();
    return nn;
}

bool SchematicTestPoint::initialize()
{
    if (!loadSymbol())
        return false;

    setName(labelText("name"));

    return true;
}

void SchematicTestPoint::updateLabels()
{
    setLabelText("name", name());
}

void SchematicTestPoint::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(Name));
    updateLabels();
}

SchematicDevicePropertiesWidget *SchematicTestPoint::createPropertiesWidget(QWidget *parent)
{
    return new SchematicTestPointPropertiesWidget(this, parent);
}

//
// SchematicTestPointPropertiesWidget
//

SchematicTestPointPropertiesWidget::SchematicTestPointPropertiesWidget(SchematicTestPoint *testPoint, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_testPoint(testPoint)
{
    QGridLayout *grid = new QGridLayout(this, 1, 2, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    grid->addWidget(m_name = new ParameterLineEdit(m_testPoint->name(), this, ParameterLineEdit::String), 0, 1);

    m_name->setFocus();
}

SchematicTestPointPropertiesWidget::~SchematicTestPointPropertiesWidget()
{
}

bool SchematicTestPointPropertiesWidget::apply()
{
    QString name = m_name->text();

    if (name.lower() != m_testPoint->name().lower() && m_testPoint->schematic()->findDevice(name))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (name.isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    m_testPoint->setName(name);
    m_testPoint->updateLabels();

    return true;
}

#include "schematictestpoint.moc"

// vim: ts=4 sw=4 et
