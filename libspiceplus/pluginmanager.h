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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <qobject.h>
#include <qmap.h>

namespace Spiceplus {

class SchematicDeviceFactory;
class ModelViewFactory;

class PluginManager : public QObject
{
    Q_OBJECT

private:
    PluginManager();

public:
    ~PluginManager();

    static PluginManager *self();

    SchematicDeviceFactory *deviceFactory(const QString &deviceID);
    ModelViewFactory *modelViewFactory(const QString &deviceType);

    QString errorString() const { return m_errorString; }

private:
    static PluginManager *s_self;
    QMap<QString, SchematicDeviceFactory *> m_deviceFactories;
    QMap<QString, ModelViewFactory *> m_modelViewFactories;
    QString m_errorString;
};

} // namespace Spiceplus

#endif // PLUGINMANAGER_H

// vim: ts=4 sw=4 et
