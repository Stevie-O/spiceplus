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

#include <qwidgetstack.h>

#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <klocale.h>

#include "document.h"
#include "mainwindow.h"

using namespace Spiceplus;

Document::Document(QWidgetStack *toolWindowStack, QWidget *parent, const char *name)
    : KMdiChildView(parent, name), m_toolWindowStack(toolWindowStack), m_isBrandNew(false)
{
}

QString Document::fileName(bool withExtension)
{
    if (withExtension)
        return m_fileURL.fileName();
    else
        return m_fileURL.fileName().section('.', 0, -2);
}

bool Document::isFileNewOrCanOverwrite(const KURL &url)
{
    int code = KMessageBox::Continue;

    if (KIO::NetAccess::exists(url, true, MainWindow::self()))
        code = KMessageBox::warningContinueCancel(this,
               i18n("A file named \"%1\" already exists. Are you sure you want to overwrite it?").arg(url.fileName()),
               i18n("Overwrite File?"), i18n("&Overwrite"));

    return code == KMessageBox::Continue;
}

#include "document.moc"

// vim: ts=4 sw=4 et
