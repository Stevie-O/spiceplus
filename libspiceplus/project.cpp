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

#include <kapplication.h>

#include "project.h"

using namespace Spiceplus;

Project *Project::s_self = 0;

Project::Project()
    : QObject(kapp)
{
}

Project::~Project()
{
    s_self = 0;
}

Project *Project::self()
{
    if (!s_self)
        s_self = new Project;
    return s_self;
}

void Project::open(const QString &name, const KURL &dir, const KURL &projectFile)
{
    m_name = name;
    m_dir = dir;
    m_dir.adjustPath(1);
    m_projectFile = projectFile;
    emit opened(true);
    emit opened();
}

void Project::close()
{
    m_name = QString::null;
    m_dir = KURL();
    m_projectFile = KURL();
    emit opened(false);
    emit closed();
}

#include "project.moc"

// vim: ts=4 sw=4 et
