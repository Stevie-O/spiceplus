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

#include "signalmultiplexer.h"
#include "document.h"

using namespace Spiceplus;

SignalMultiplexer::SignalMultiplexer(QObject *parent, const char *name)
    : QObject(parent, name)
{
}

void SignalMultiplexer::connect(QObject *sender, const char *signal, const char *slot)
{
    Connection conn;
    conn.sender = sender;
    conn.signal = signal;
    conn.slot = slot;

    m_connections << conn;
    connect(conn);
}

bool SignalMultiplexer::disconnect(QObject *sender, const char *signal, const char *slot)
{
    for (QValueList<Connection>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it)
    {
	    Connection conn = *it;
	    if (static_cast<QObject *>(conn.sender) == sender && qstrcmp(conn.signal, signal) == 0 && qstrcmp(conn.slot, slot) == 0)
        {
	        disconnect(conn);
	        m_connections.remove(it);
	        return true;
	    }
    }
    return false;
}

void SignalMultiplexer::connect(const char *signal, QObject *receiver, const char *slot)
{
    Connection conn;
    conn.receiver = receiver;
    conn.signal = signal;
    conn.slot = slot;

    m_connections << conn;
    connect(conn);
}

bool SignalMultiplexer::disconnect(const char *signal, QObject *receiver, const char *slot)
{
    for (QValueList<Connection>::Iterator it = m_connections.begin(); it != m_connections.end(); ++it)
    {
	    Connection conn = *it;
	    if (static_cast<QObject *>(conn.receiver) == receiver && qstrcmp(conn.signal, signal) == 0 && qstrcmp(conn.slot, slot) == 0)
        {
	        disconnect(conn);
	        m_connections.remove(it);
	        return true;
	    }
    }
    return false;
}

void SignalMultiplexer::setCurrentObject(QObject *newObject)
{
    if (newObject == m_object)
	    return;

    QValueList<Connection>::ConstIterator it;
    for (it = m_connections.begin(); it != m_connections.end(); ++it)
	    disconnect(*it);

    m_object = newObject;

    for (it = m_connections.begin(); it != m_connections.end(); ++it)
	    connect(*it);

    Document *doc = dynamic_cast<Document *>(newObject);
    if (doc)
	    doc->updateState();
}

void SignalMultiplexer::connect(const Connection &conn)
{
    if (!m_object)
	    return;

    if (!conn.sender && !conn.receiver)
	    return;

    if (conn.sender)
	    QObject::connect(static_cast<QObject *>(conn.sender), conn.signal, static_cast<QObject *>(m_object), conn.slot);
    else
	    QObject::connect(static_cast<QObject *>(m_object), conn.signal, static_cast<QObject *>(conn.receiver), conn.slot);
}

void SignalMultiplexer::disconnect(const Connection &conn)
{
    if (!m_object)
	    return;

    if (!conn.sender && !conn.receiver)
	    return;

    if (conn.sender)
	    QObject::disconnect(static_cast<QObject *>(conn.sender), conn.signal, static_cast<QObject *>(m_object), conn.slot);
    else
	    QObject::disconnect(static_cast<QObject *>(m_object), conn.signal, static_cast<QObject *>(conn.receiver), conn.slot);
}

#include "signalmultiplexer.moc"

// vim: ts=4 sw=4 et
