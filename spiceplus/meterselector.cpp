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

#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlabel.h>

#include <klocale.h>

#include "meterselector.h"
#include "meter.h"
#include "schematic.h"

using namespace Spiceplus;

MeterSelector::MeterSelector(Schematic *schematic, QWidget *parent, QGridLayout *layout, int startRow, int startCol)
    : QObject(parent)
{
    QButtonGroup *group = new QButtonGroup(parent);
    group->hide();

    QStringList testPoints = schematic->testPointNames();
    QStringList ammeters = schematic->ammeterNames();

    m_voltmeterButton = new QRadioButton(i18n("Voltmeter:"), parent);
    layout->addWidget(m_voltmeterButton, startRow, startCol);
    group->insert(m_voltmeterButton);

    m_testPoint1 = new QComboBox(parent);
    m_testPoint1->connect(m_voltmeterButton, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    layout->addWidget(m_testPoint1, startRow, startCol + 1);

    QLabel *arrow = new QLabel("->", parent);
    arrow->setAlignment(Qt::AlignCenter);
    arrow->connect(m_voltmeterButton, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    layout->addWidget(arrow, startRow, startCol + 2);

    m_testPoint2 = new QComboBox(parent);
    m_testPoint2->connect(m_voltmeterButton, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    layout->addWidget(m_testPoint2, startRow, startCol + 3);

    m_ammeterButton = new QRadioButton(i18n("Ammeter:"), parent);
    layout->addWidget(m_ammeterButton, startRow + 1, startCol);
    group->insert(m_ammeterButton);

    m_ammeter = new QComboBox(parent);
    m_ammeter->connect(m_ammeterButton, SIGNAL(toggled(bool)), SLOT(setEnabled(bool)));
    layout->addWidget(m_ammeter, startRow + 1, startCol + 1);

    if (testPoints.count() < 2)
    {
        m_voltmeterButton->setEnabled(false);
        m_testPoint1->setEnabled(false);
        arrow->setEnabled(false);
        m_testPoint2->setEnabled(false);
    }
    else
    {
        m_voltmeterButton->setChecked(true);
        m_testPoint1->insertStringList(testPoints);
        m_testPoint2->insertStringList(testPoints);
        m_testPoint2->setCurrentItem(1);
    }

    if (ammeters.count() < 1)
    {
        m_ammeterButton->setEnabled(false);
        m_ammeter->setEnabled(false);
    }
    else
    {
        if (testPoints.count() < 2)
            m_ammeterButton->setChecked(true);
        else
            m_ammeter->setEnabled(false);

        m_ammeter->insertStringList(ammeters);
    }
}

Meter MeterSelector::meter() const
{
    if (m_voltmeterButton->isChecked())
        return Meter(m_testPoint1->currentText(), m_testPoint2->currentText());
    else
        return Meter(m_ammeter->currentText());
}

#include "meterselector.moc"

// vim: ts=4 sw=4 et
