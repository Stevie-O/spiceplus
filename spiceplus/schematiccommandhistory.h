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

#ifndef SCHEMATICCOMMANDHISTORY_H
#define SCHEMATICCOMMANDHISTORY_H

#include <qobject.h>

namespace Spiceplus {

class SchematicCommand;

class SchematicCommandHistory : public QObject
{
    Q_OBJECT

public:
    SchematicCommandHistory(QObject *parent = 0, const char *name = 0);
    virtual ~SchematicCommandHistory();

    void add(SchematicCommand *cmd);
    void undo();
    void redo();

    bool isUndoAvailable();
    bool isRedoAvailable();

    bool isModified() { return m_modificationCounter != 0; }
    void resetModificationCounter();

signals:
    void modified(bool yes);
    void undoAvailable(bool yes);
    void redoAvailable(bool yes);

private:
    QValueList<SchematicCommand *> m_commands;
    // m_current points to the _next_ command (to be executed) following the last executed command (left to right).
    // If m_current == m_commands.begin(), we are at the beginning of the history, therefore Undo is not available.
    // If m_current == m_commands.end(), we are at the end of the history, therefore Redo is not available.
    QValueList<SchematicCommand *>::Iterator m_current;
    // We use the counter to track the saved position.
    int m_modificationCounter;
};

} // namespace Spiceplus

#endif // SCHEMATICCOMMANDHISTORY_H

// vim: ts=4 sw=4 et
