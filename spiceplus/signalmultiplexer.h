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

#ifndef SIGNALMULTIPLEXER_H
#define SIGNALMULTIPLEXER_H

#include <qobject.h>
#include <qguardedptr.h>

namespace Spiceplus {

class SignalMultiplexer : public QObject
{
    Q_OBJECT

public:
    SignalMultiplexer(QObject *parent = 0, const char *name = 0);

    void connect(QObject *sender, const char *signal, const char *slot);
    bool disconnect(QObject *sender, const char *signal, const char *slot);
    void connect(const char *signal, QObject *receiver, const char *slot);
    bool disconnect(const char *signal, QObject *receiver, const char *slot);

    QObject *currentObject() const { return m_object; }

public slots:
    void setCurrentObject(QObject *newObject);

private:
    struct Connection
    {
        QGuardedPtr<QObject> sender;
        QGuardedPtr<QObject> receiver;
        const char *signal;
        const char *slot;
    };

    void connect(const Connection &conn);
    void disconnect(const Connection &conn);

    QGuardedPtr<QObject> m_object;
    QValueList<Connection> m_connections;
};

} // namespace Spiceplus

#endif // SIGNALMULTIPLEXER_H

// vim: ts=4 sw=4 et
