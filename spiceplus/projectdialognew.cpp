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

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>

#include <klocale.h>
#include <kdialog.h>
#include <kurlrequester.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>

#include "projectdialognew.h"
#include "settings.h"
#include "mainwindow.h"

using namespace Spiceplus;

ProjectDialogNew::ProjectDialogNew(QWidget *parent)
    : KDialogBase(parent, 0, true, i18n("New Project"), Ok | Cancel)
{
    QWidget *w = new QWidget(this);
    setMainWidget(w);

    QBoxLayout *vbox = new QVBoxLayout(w, 0, KDialog::spacingHint());

    QGridLayout *grid = new QGridLayout(vbox, 3, 2, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("Project name:"), w), 0, 0);
    m_projectName = new QLineEdit(w);
    m_projectName->setValidator(new QRegExpValidator(QRegExp("(\\w| )*"), m_projectName));
    connect(m_projectName, SIGNAL(textChanged(const QString &)), SLOT(updateProjectDir(const QString &)));
    grid->addWidget(m_projectName, 0, 1);

    grid->addWidget(new QLabel(i18n("Location:"), w), 1, 0);
    m_location = new KURLRequester(Settings::self()->projectsDir(), w);
    m_location->setMode(KFile::Directory);
    m_location->setMinimumWidth(400);
    connect(m_location, SIGNAL(textChanged(const QString &)), SLOT(updateProjectDir(const QString &)));
    grid->addWidget(m_location, 1, 1);

    grid->addWidget(new QLabel(i18n("Project directory:"), w), 2, 0);
    grid->addWidget(m_projectDir = new QLabel(w), 2, 1);

    updateProjectDir();

    vbox->addStretch();
}

QString ProjectDialogNew::projectName() const
{
    return m_projectName->text().stripWhiteSpace();
}

KURL ProjectDialogNew::projectDir() const
{
    KURL url;
    url.setDirectory(m_projectDir->text());
    return url;
}

void ProjectDialogNew::slotOk()
{
    if (m_projectName->text().stripWhiteSpace().isEmpty())
    {
        KMessageBox::sorry(this, i18n("No project name specified"));
        return;
    }

    if (!KIO::NetAccess::exists(m_location->url(), true, MainWindow::self()))
    {
        KMessageBox::sorry(this, i18n("Directory %1 does not exist").arg(m_location->url()));
        return;
    }

    if (KIO::NetAccess::exists(m_projectDir->text(), true, MainWindow::self()))
    {
        KMessageBox::sorry(this, i18n("Directory or file %1 already exists").arg(m_projectDir->text()));
        return;
    }

    Settings::self()->setProjectsDir(m_location->url());
    done(Accepted);
}

void ProjectDialogNew::updateProjectDir(const QString &)
{
    KURL url = m_location->url();
    url.addPath(m_projectName->text().stripWhiteSpace());
    url.cleanPath();
    m_projectDir->setText(url.isLocalFile() ? url.path() : url.prettyURL());
}

#include "projectdialognew.moc"

// vim: ts=4 sw=4 et
