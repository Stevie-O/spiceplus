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

#ifndef MODELDIRS_H
#define MODELDIRS_H

#include <qstring.h>
#include <qmap.h>

class KURL;

namespace Spiceplus {

class ModelDirs
{
    friend class Settings;

public:
    struct Dir
    {
        Dir() : containsTypeDirs(false) {}
        bool operator==(const Dir &dir) const { return name == dir.name && path == dir.path && containsTypeDirs == dir.containsTypeDirs; }

        QString name;
        QString path;
        bool containsTypeDirs;
    };

    bool load(const KURL &url);
    bool save(const KURL &url);

    Dir dir(const QString &id) const { return m_dirs.contains(id) ? m_dirs[id] : Dir(); }
    void setDir(const QString &id, const QString &name, const QString &path, bool containsTypeDirs);
    QMap<QString, Dir> dirs() const { return m_dirs; }

    void clear() { m_dirs.clear(); }

    bool operator==(const ModelDirs &dirs) const;
    bool operator!=(const ModelDirs &dirs) const;

    QString errorString() const { return m_errorString; }

private:
    QMap<QString, Dir> m_dirs;
    QString m_errorString;
};

} // namespace Spiceplus

#endif // MODELDIRS_H

// vim: ts=4 sw=4 et
