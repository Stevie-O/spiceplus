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

#include "schematiccommandhistory.h"
#include "schematiccommand.h"

using namespace Spiceplus;

SchematicCommandHistory::SchematicCommandHistory(QObject *parent, const char *name)
    : QObject(parent, name), m_modificationCounter(0)
{
    m_current = m_commands.end();
}

SchematicCommandHistory::~SchematicCommandHistory()
{
    for (QValueList<SchematicCommand *>::Iterator it = m_commands.begin(); it != m_commands.end(); ++it)
        delete *it;
}

void SchematicCommandHistory::add(SchematicCommand *cmd)
{
    if (!isUndoAvailable())
        emit undoAvailable(true);

    if (isRedoAvailable())
        emit redoAvailable(false);

    while (m_current != m_commands.end())
    {
        delete *m_current;
        m_current = m_commands.erase(m_current);
    }

    m_commands.insert(m_current, cmd);
    m_current = m_commands.end();

    if (m_modificationCounter == 0)
        emit modified(true);
    else if (m_modificationCounter < 0)
        m_modificationCounter = 0x40000000;

    ++m_modificationCounter;
}

void SchematicCommandHistory::undo()
{
    if (isUndoAvailable())
    {
        if (!isRedoAvailable())
            emit redoAvailable(true);

        (*(--m_current))->unexecute();

        if (m_modificationCounter-- == 0)
            emit modified(true);
        else if (m_modificationCounter == 0)
            emit modified(false);

        if (!isUndoAvailable())
            emit undoAvailable(false);
    }
}

void SchematicCommandHistory::redo()
{
    if (isRedoAvailable())
    {
        if (!isUndoAvailable())
            emit undoAvailable(true);

        (*(m_current++))->execute();

        if (m_modificationCounter++ == 0)
            emit modified(true);
        else if (m_modificationCounter == 0)
            emit modified(false);

        if (!isRedoAvailable())
            emit redoAvailable(false);
    }
}

bool SchematicCommandHistory::isUndoAvailable()
{
    return m_current != m_commands.begin();
}

bool SchematicCommandHistory::isRedoAvailable()
{
    return m_current != m_commands.end();
}

void SchematicCommandHistory::resetModificationCounter()
{
    if (m_modificationCounter != 0)
    {
        m_modificationCounter = 0;
        emit modified(false);
    }
}

#include "schematiccommandhistory.moc"

// vim: ts=4 sw=4 et
