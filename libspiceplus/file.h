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

#ifndef FILE_H
#define FILE_H

#include <qstring.h>
#include <qfile.h>
#include <qguardedptr.h>

#include <kurl.h>
#include <ktempfile.h>

namespace Spiceplus {

class File : public QFile
{
public:
    File();
    File(const QString &name);
    File(const KURL &url);
    ~File();

    void setName(const KURL &url);
    bool open(int m);
    void close();
    bool closeWithStatus();

    static void setMainWindow(QWidget *mainWindow) { s_mainWindow = mainWindow; }
    static QWidget *mainWindow() { return s_mainWindow; }

    QString errorString() const { return m_errorString; }

private:
    void init();

    KURL m_url;
    KTempFile m_tempFile;
    static QGuardedPtr<QWidget> s_mainWindow;

    QString m_errorString;
};

} // namespace Spiceplus

#endif // FILE_H

// vim: ts=4 sw=4 et
