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

#ifndef PROJECT_H
#define PROJECT_H

#include <qobject.h>
#include <qstring.h>

#include <kurl.h>

namespace Spiceplus {

class Project : public QObject
{
    Q_OBJECT

private:
    Project();

public:
    ~Project();

    static Project *self();

    void open(const QString &name, const KURL &dir, const KURL &projectFile);
    void close();

    bool isOpen() const { return !m_name.isNull(); }
    QString name() const { return m_name; }
    KURL dir() const { return m_dir; }
    KURL projectFile() const { return m_projectFile; }

signals:
    void opened(bool yes);
    void opened();
    void closed();

private:
    static Project *s_self;

    QString m_name;
    KURL m_dir;
    KURL m_projectFile;
};

} // namespace Spiceplus

#endif // PROJECT_H

// vim: ts=4 sw=4 et
