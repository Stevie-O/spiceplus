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

#include <qdom.h>

#include <kapplication.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kdebug.h>

#include "projectmanager.h"
#include "project.h"
#include "projectdialognew.h"
#include "mainwindow.h"
#include "settings.h"
#include "file.h"
#include "document.h"

using namespace Spiceplus;

ProjectManager *ProjectManager::s_self = 0;

ProjectManager::ProjectManager()
    : QObject(kapp)
{
}

ProjectManager::~ProjectManager()
{
    s_self = 0;
}

ProjectManager *ProjectManager::self()
{
    if (!s_self)
        s_self = new ProjectManager;
    return s_self;
}

void ProjectManager::saveProject()
{
    File file(Project::self()->projectFile());

    if (!file.open(IO_WriteOnly))
    {
        KMessageBox::error(MainWindow::self(), file.errorString());
        return;
    }

    QDomDocument domdoc("spiceplus");
    QDomElement root = domdoc.createElement("spiceplus");
    domdoc.appendChild(root);

    QDomElement nameElem = domdoc.createElement("project-name");
    nameElem.appendChild(domdoc.createTextNode(Project::self()->name()));
    root.appendChild(nameElem);

    QDomElement sessionElem = domdoc.createElement("session");
    QDomElement docsElem = domdoc.createElement("documents");

    KMdiIterator<KMdiChildView *> *it = MainWindow::self()->createIterator();
    for (; !it->isDone(); it->next())
    {
        Document *doc = static_cast<Document *>(it->currentItem());
        if (doc->isBrandNew())
            continue;

        QDomElement docElem = domdoc.createElement("document");

        if (MainWindow::self()->activeWindow() == doc)
            docElem.setAttribute("activated", "true");

        QDomElement urlElem = domdoc.createElement("file");
        urlElem.appendChild(domdoc.createTextNode(KURL::relativeURL(Project::self()->dir(), doc->fileURL())));
        docElem.appendChild(urlElem);

        docsElem.appendChild(docElem);
    }
    MainWindow::self()->deleteIterator(it);

    sessionElem.appendChild(docsElem);
    root.appendChild(sessionElem);

    QTextStream stream(&file);
    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << domdoc.toString();

    if (!file.closeWithStatus())
        KMessageBox::error(MainWindow::self(), file.errorString());
}

void ProjectManager::newProject()
{
    ProjectDialogNew *dlg = new ProjectDialogNew(MainWindow::self());
    if (dlg->exec() == QDialog::Accepted && (!Project::self()->isOpen() || closeProject()))
    {
        if (!KIO::NetAccess::mkdir(dlg->projectDir(), MainWindow::self()))
            KMessageBox::error(MainWindow::self(), KIO::NetAccess::lastErrorString());
        else
        {
            KURL projectFile = dlg->projectDir();
            projectFile.addPath(dlg->projectName() + ".spiceplus");
            Project::self()->open(dlg->projectName(), dlg->projectDir(), projectFile);
            KFileDialog::setStartDir(dlg->projectDir());
        }
    }
    delete dlg;
}

void ProjectManager::openProject()
{
    KURL url = KFileDialog::getOpenURL(":project",
                                       QString("*.spiceplus|%1").arg(i18n("Project File")),
                                       MainWindow::self(), i18n("Open Project"));

    if (url.isEmpty())
        return;

    if (Project::self()->isOpen() && !closeProject())
        return;

    File file(url);

    if (!file.open(IO_ReadOnly))
    {
        KMessageBox::error(MainWindow::self(), file.errorString());
        return;
    }

    QDomDocument domdoc;

    if (!domdoc.setContent(&file))
    {
        KMessageBox::error(MainWindow::self(), i18n("Invalid document structure"));
        return;
    }

    QDomElement root = domdoc.documentElement();

    if (root.tagName() != "spiceplus")
    {
        KMessageBox::error(MainWindow::self(), i18n("Invalid Project File"));
        return;
    }

    QDomNode node = root.firstChild();
    if (!node.isElement() || node.nodeName() != "project-name" || node.firstChild().isNull() || !node.firstChild().isText())
    {
        KMessageBox::error(MainWindow::self(), i18n("Invalid Project File"));
        return;
    }

    Project::self()->open(node.firstChild().nodeValue(), url.directory(false), url);
    KFileDialog::setStartDir(Project::self()->dir());

    if ((node = node.nextSibling()).isNull() || !node.isElement() || node.nodeName() != "session")
        return;

    if ((node = node.firstChild()).isNull() || !node.isElement() || node.nodeName() != "documents")
        return;

    Document *activeDoc = 0;
    for (node = node.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement() || node.nodeName() != "document")
            continue;

        QDomNode fileNode = node.firstChild();

        if (!fileNode.isElement() || fileNode.nodeName() != "file" || fileNode.firstChild().isNull() || !fileNode.firstChild().isText())
            continue;

        Document *doc = MainWindow::self()->openDocument(KURL(Project::self()->dir(), fileNode.firstChild().nodeValue()));
        if (node.toElement().attribute("activated") == "true")
            activeDoc = doc;
    }

    if (activeDoc)
        MainWindow::self()->activateDocument(activeDoc);
}

bool ProjectManager::closeProject()
{
    if (MainWindow::self()->saveAllDocuments())
    {
        saveProject();
        MainWindow::self()->closeAllWindows();
        KFileDialog::setStartDir(QString::null);
        Project::self()->close();
        return true;
    }

    return false;
}

#include "projectmanager.moc"

// vim: ts=4 sw=4 et
