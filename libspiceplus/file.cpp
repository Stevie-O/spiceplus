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

#include <kio/netaccess.h>
#include <klocale.h>

#include "file.h"

using namespace Spiceplus;

QGuardedPtr<QWidget> File::s_mainWindow;

File::File()
{
    init();
}

File::File(const QString &name)
    : QFile(name)
{
    init();
}

File::File(const KURL &url)
    : m_url(url)
{
    init();
}

File::~File()
{
    close();
}

void File::init()
{
    m_tempFile.setAutoDelete(true);
}

void File::setName(const KURL &url)
{
    setName("");
    m_url = url;
}

bool File::open(int m)
{
    if (name().isEmpty() && !m_url.isEmpty())
    {
        QString tempFile = m_tempFile.name();
        if (m & IO_ReadOnly && !KIO::NetAccess::download(m_url, tempFile, s_mainWindow) && !(m & IO_WriteOnly))
        {
            m_errorString = KIO::NetAccess::lastErrorString();
            return false;
        }
        QFile::setName(tempFile);
    }

    if (!QFile::open(m))
    {
        m_errorString = i18n("Cannot open file");
        return false;
    }

    return true;
}

void File::close()
{
    closeWithStatus();
}

bool File::closeWithStatus()
{
    int m = mode();
    QFile::close();

    if (m & IO_WriteOnly && !KIO::NetAccess::upload(m_tempFile.name(), m_url, s_mainWindow))
    {
        m_errorString = KIO::NetAccess::lastErrorString();
        return false;
    }

    return true;
}

// vim: ts=4 sw=4 et
