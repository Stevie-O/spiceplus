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

#include <qfile.h>

#include <kapplication.h>
#include <klocale.h>

#include "pluginmanager.h"
#include "schematicdevice.h"
#include "model.h"
#include "settings.h"

using namespace Spiceplus;

PluginManager *PluginManager::s_self = 0;

PluginManager::PluginManager()
    : QObject(kapp)
{
}

PluginManager::~PluginManager()
{
    s_self = 0;
}

PluginManager *PluginManager::self()
{
    if (!s_self)
        s_self = new PluginManager;
    return s_self;
}

SchematicDeviceFactory *PluginManager::deviceFactory(const QString &deviceID)
{
    if (m_deviceFactories.contains(deviceID))
        return m_deviceFactories[deviceID];

    QString path = Settings::self()->deviceDir() + "/" + deviceID + ".device";
    KConfig *config = new KConfig(path, true);
    QString libname = config->readEntry("Library");
    delete config;

    if (libname.isEmpty())
    {
        m_errorString = i18n("Reading library name from %1 failed").arg(path);
        return 0;
    }

    KLibrary *lib = KLibLoader::self()->library(libname);
    if (!lib)
    {
        m_errorString = i18n("%1: Loading plugin failed").arg(deviceID);
        return 0;
    }

    if (!lib->factory()->inherits("Spiceplus::SchematicDeviceFactory"))
    {
        lib->unload();
        m_errorString = i18n("%1: Device plugin has wrong type").arg(deviceID);
        return 0;
    }

    return m_deviceFactories[deviceID] = static_cast<SchematicDeviceFactory *>(lib->factory());
}

ModelViewFactory *PluginManager::modelViewFactory(const QString &deviceType)
{
    if (m_modelViewFactories.contains(deviceType))
        return m_modelViewFactories[deviceType];

    QString path = Settings::self()->modelViewDir() + "/" + deviceType + ".modelview";
    KConfig *config = new KConfig(path, true);
    QString libname = config->readEntry("Library");
    delete config;

    if (libname.isEmpty())
    {
        m_errorString = i18n("Reading library name from %1 failed").arg(path);
        return 0;
    }

    KLibrary *lib = KLibLoader::self()->library(libname);
    if (!lib)
    {
        m_errorString = i18n("Loading Model View plugin failed");
        return 0;
    }

    if (!lib->factory()->inherits("Spiceplus::ModelViewFactory"))
    {
        lib->unload();
        m_errorString = i18n("Model View plugin has wrong type");
        return 0;
    }

    return m_modelViewFactories[deviceType] = static_cast<ModelViewFactory *>(lib->factory());
}

#include "pluginmanager.moc"

// vim: ts=4 sw=4 et
