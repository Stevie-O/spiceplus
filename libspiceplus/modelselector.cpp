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

#include <qlayout.h>
#include <qheader.h>
#include <qdom.h>
#include <qregexp.h>

#include <kfiletreeview.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include "modelselector.h"
#include "modelfile.h"
#include "settings.h"
#include "file.h"
#include "project.h"

using namespace Spiceplus;

//
// ModelSelectorTreeBranch
//

ModelSelectorTreeBranch::ModelSelectorTreeBranch(const KURL &modelDir,
                                                 const QString &modelDirID,
                                                 const QString &deviceType,
                                                 KFileTreeView *view,
                                                 const KURL &startURL,
                                                 const QString &name,
                                                 const QPixmap &pix)
    : KFileTreeBranch(view, startURL, name, pix), m_modelDir(modelDir), m_modelDirID(modelDirID), m_deviceType(deviceType)
{
    setNameFilter("*.model");
}

bool ModelSelectorTreeBranch::matchesFilter(const KFileItem *item) const
{
    ModelFile model;

    if (item->isDir() || model.load(item->url()) && model.deviceType() == m_deviceType)
        return true;

    return false;
}

KFileTreeViewItem *ModelSelectorTreeBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    KFileTreeViewItem *item = new KFileTreeViewItem(parent, fileItem, this);
    QString name;

    if (item->isDir())
    {
        KConfig *config = new KConfig(fileItem->url().path() + "/_directory", true);
        name = config->readEntry("Name", fileItem->text());
        delete config;
    }
    else
    {
        ModelFile model;
        model.load(fileItem->url());
        name = model.name();
    }

    item->setText(0, name);
    item->setText(1, (item->isDir() ? '0' : '1') + name);

    if (!item->isDir())
        item->setPixmap(0, KGlobal::iconLoader()->loadIcon("spiceplus_device_model", KIcon::Small));

    return item;
}

//
// ModelSelector
//

ModelSelector::ModelSelector(const QString &deviceType, QWidget *parent)
    : SchematicDeviceSelector(parent), m_deviceType(deviceType)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());

    m_tree = new KFileTreeView(this);
    m_tree->header()->hide();
    m_tree->addColumn("");
    m_tree->setSorting(1);
    connect(m_tree, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));

    topLayout->addWidget(m_tree);

    connect(Settings::self(), SIGNAL(settingsChanged()), SLOT(setupTree()));
    connect(Project::self(), SIGNAL(opened()), SLOT(setupTree()));
    connect(Project::self(), SIGNAL(closed()), SLOT(setupTree()));
}

bool ModelSelector::setupTree()
{
    m_tree->clear();
    KURL modelDir, startURL;

    modelDir = Settings::self()->standardModelDir();
    modelDir.adjustPath(1);
    startURL = modelDir;
    startURL.addPath(m_deviceType);

    ModelSelectorTreeBranch *branch = new ModelSelectorTreeBranch(modelDir,
                                                                  "standard",
                                                                  m_deviceType,
                                                                  m_tree,
                                                                  startURL,
                                                                  i18n("Standard Models"),
                                                                  QPixmap());
    m_tree->addBranch(branch);
    branch->root()->setText(1, "0");
    branch->root()->setOpen(true);

    if (Project::self()->isOpen())
    {
        ModelSelectorTreeBranch *branch = new ModelSelectorTreeBranch(Project::self()->dir(),
                                                                      "project",
                                                                      m_deviceType,
                                                                      m_tree,
                                                                      Project::self()->dir(),
                                                                      i18n("Project Models"),
                                                                      QPixmap());
        m_tree->addBranch(branch);
        branch->root()->setText(1, "1");
        branch->root()->setOpen(true);
    }

    QMap<QString, ModelDirs::Dir> modelDirs = Settings::self()->userModelDirs().dirs();
    for (QMap<QString, ModelDirs::Dir>::Iterator it = modelDirs.begin(); it != modelDirs.end(); ++it)
    {
        modelDir = it.data().path;
        modelDir.adjustPath(1);
        startURL = modelDir;
        if (it.data().containsTypeDirs)
            startURL.addPath(m_deviceType);
        
        ModelSelectorTreeBranch *branch = new ModelSelectorTreeBranch(modelDir,
                                                                      it.key(),
                                                                      m_deviceType,
                                                                      m_tree,
                                                                      startURL,
                                                                      it.data().name,
                                                                      QPixmap());
        m_tree->addBranch(branch);
        branch->root()->setText(1, "2" + it.data().name);
        branch->root()->setOpen(true);
    }

    return true;
}

bool ModelSelector::isModelSelected() const
{
    KFileTreeViewItem *item = m_tree->currentKFileTreeViewItem();
    return item && !item->isDir();
}

KURL ModelSelector::modelURL() const
{
    return m_tree->currentURL();
}

QString ModelSelector::modelPath() const
{
    KFileTreeViewItem *item = m_tree->currentKFileTreeViewItem();
    if (item)
    {
        ModelSelectorTreeBranch *branch = static_cast<ModelSelectorTreeBranch *>(item->branch());
        return branch->modelDirID() + ":" + KURL::relativeURL(branch->modelDir(), item->url());
    }

    return QString::null;
}

void ModelSelector::slotSelectionChanged()
{
    if (!m_tree->currentKFileTreeViewItem())
        m_tree->clearFocus();

    emit deviceChanged();
}

#include "modelselector.moc"

// vim: ts=4 sw=4 et
