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
#include <qsplitter.h>
#include <qlabel.h>
#include <qwidgetstack.h>

#include <klocale.h>
#include <klistview.h>
#include <kmessagebox.h>

#include "devicewindow.h"
#include "devicechooser.h"
#include "pluginmanager.h"
#include "schematicdevice.h"

using namespace Spiceplus;

DeviceWindow::DeviceWindow(QWidget *parent)
    : QWidget(parent), m_deviceSelector(0)
{
    QBoxLayout *topLayout = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    m_deviceChooser = new DeviceChooser(splitter);
    m_deviceSelectorStack = new QWidgetStack(splitter);

    QLabel *blank = new QLabel(i18n("No device selected"), m_deviceSelectorStack);
    blank->setAlignment(Qt::AlignCenter);
    m_deviceSelectorStack->addWidget(blank, 0);
    m_deviceSelectorStack->raiseWidget(blank);

    splitter->setResizeMode(m_deviceChooser, QSplitter::KeepSize);

    QValueList<int> sizes;
    sizes << 250 << 250;
    splitter->setSizes(sizes);

    topLayout->addWidget(splitter);

    connect(m_deviceChooser, SIGNAL(selectionChanged(const QString &)), SLOT(setupSelector(const QString &)));
}

DeviceWindow::~DeviceWindow()
{
}

void DeviceWindow::unselectDevice()
{
    if (m_deviceChooser->selectedItem())
    {
        m_deviceChooser->blockSignals(true);
        m_deviceChooser->setSelected(m_deviceChooser->selectedItem(), false);
        m_deviceChooser->blockSignals(false);
        m_deviceSelectorStack->raiseWidget(0);
        m_deviceSelector = 0;
    }
}

void DeviceWindow::setupSelector(const QString &deviceID)
{
    m_deviceChooser->triggerUpdate();

    if (deviceID.isNull())
    {
        m_deviceSelectorStack->raiseWidget(0);
        m_deviceSelector = 0;
    }
    else
    {
        SchematicDeviceFactory *factory = PluginManager::self()->deviceFactory(deviceID);
        if (factory)
        {
            if (m_deviceSelectorCache.contains(deviceID))
            {
                m_deviceSelector = m_deviceSelectorCache[deviceID];
                m_deviceSelectorStack->raiseWidget(m_deviceSelector);
            }
            else
            {
                m_deviceSelector = factory->createSelector(m_deviceSelectorStack);
                if (m_deviceSelector)
                {
                    connect(m_deviceSelector, SIGNAL(deviceChanged()), SIGNAL(deviceChanged()));
                    m_deviceSelectorCache[deviceID] = m_deviceSelector;
                    m_deviceSelectorStack->addWidget(m_deviceSelector);
                    m_deviceSelectorStack->raiseWidget(m_deviceSelector);
                }
                else
                {
                    m_deviceSelectorStack->raiseWidget(0);
                    KMessageBox::error(this, factory->errorString());
                }
            }
        }
        else
        {
            m_deviceSelectorStack->raiseWidget(0);
            m_deviceSelector = 0;
            KMessageBox::error(this, PluginManager::self()->errorString());
        }
    }

    emit deviceChanged();
}

#include "devicewindow.moc"

// vim: ts=4 sw=4 et
