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

#include <kmessagebox.h>
#include <klocale.h>

#include "devicepropertiesdialog.h"
#include "schematicdevice.h"
#include "mainwindow.h"

using namespace Spiceplus;

DevicePropertiesDialog::DevicePropertiesDialog(SchematicDevice *device, QWidget *parent)
    : KDialogBase(parent, 0, true, i18n("Device Properties"), Ok | Cancel)
{
    QWidget *w = new QWidget(this);
    setMainWidget(w);

    QBoxLayout *layout = new QVBoxLayout(w);
    m_propertiesWidget = device->createPropertiesWidget(w);
    layout->addWidget(m_propertiesWidget);
    layout->addStretch();
}

DevicePropertiesDialog::~DevicePropertiesDialog()
{
}

void DevicePropertiesDialog::slotOk()
{
    if (m_propertiesWidget->apply())
        done(Ok);
    else
        KMessageBox::error(this, m_propertiesWidget->errorString());
}

#include "devicepropertiesdialog.moc"

// vim: ts=4 sw=4 et
