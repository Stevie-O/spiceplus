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

#ifndef MODELFILE_H
#define MODELFILE_H

#include "model.h"

namespace Spiceplus {

class ModelFile : public Model
{
public:
    QString name() const { return !m_alias.isEmpty() ? m_alias : m_name; }

    QString alias() const { return m_alias; }
    void setAlias(const QString &alias) { m_alias = alias; }

    QString symbolPath() const { return m_symbolPath; }
    void setSymbolPath(const QString &path) { m_symbolPath = path; }

    bool load(const QString &modelPath);
    bool load(const KURL &url);
    bool save(const KURL &url);

    QString errorString() const { return m_errorString; }

private:
    QString m_name;
    QString m_alias;
    QString m_symbolPath;
    QString m_errorString;
};

} // namespace Spiceplus

#endif // MODELFILE_H

// vim: ts=4 sw=4 et
