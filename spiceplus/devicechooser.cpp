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

#include <qheader.h>
#include <qregexp.h>

#include <klocale.h>
#include <kiconloader.h>

#include "devicechooser.h"
#include "settings.h"

using namespace Spiceplus;

//
// DeviceChooserBranch
//

DeviceChooserBranch::DeviceChooserBranch(KFileTreeView *view, const KURL &url, const QString &name, const QPixmap &pix)
    : KFileTreeBranch(view, url, name, pix)
{
    setAutoUpdate(false);
    setNameFilter("*.device");
}

KFileTreeViewItem *DeviceChooserBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    KFileTreeViewItem *item = new KFileTreeViewItem(parent, fileItem, this);
    KConfig *config = new KConfig(item->isDir() ? fileItem->url().path() + "/_directory" : fileItem->url().path(), true);

    QString name = config->readEntry("Name", item->isDir() ? fileItem->text() : QString(fileItem->text()).remove(QRegExp("\\.device$")));
    item->setText(0, name);
    item->setText(1, (item->isDir() ? '0' : '1') + config->readEntry("Position", "9999").rightJustify(4, '0') + name);

    QString icon;
    if (!item->isDir() && !(icon = config->readEntry("Icon")).isEmpty())
        item->setPixmap(0, KGlobal::iconLoader()->loadIcon(icon, KIcon::Panel));
    else
        item->setPixmap(0, QPixmap());

    delete config;

    return item;
}

//
// DeviceChooser
//

DeviceChooser::DeviceChooser(QWidget *parent)
    : KFileTreeView(parent)
{
    setHScrollBarMode(AlwaysOff);
    header()->hide();
    addColumn("");
    setSorting(1);
    setAlternateBackground(QColor());
    connect(this, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));

    m_dirLister = new KDirLister;
    m_dirLister->setAutoUpdate(false);
    m_dirLister->setDirOnlyMode(true);
    connect(m_dirLister, SIGNAL(newItems(const KFileItemList &)), SLOT(addDirectories(const KFileItemList &)));
    m_dirLister->openURL(Settings::self()->deviceDir(), true);

    connect(Settings::self(), SIGNAL(settingsChanged()), SLOT(resetTree()));
}

DeviceChooser::~DeviceChooser()
{
    delete m_dirLister;
}

void DeviceChooser::resetTree()
{
    m_dirLister->stop();
    selectAll(false);
    clear();
    m_dirLister->openURL(Settings::self()->deviceDir(), true);
}

QPixmap DeviceChooser::itemIcon(KFileTreeViewItem *, int) const
{
    return QPixmap();
}

void DeviceChooser::addDirectories(const KFileItemList &items)
{
    for (KFileItemList::Iterator it = items.begin(); it != items.end(); ++it)
    {
        DeviceChooserBranch *branch = new DeviceChooserBranch(this, (*it)->url(), (*it)->text(), QPixmap());
        KConfig *config = new KConfig(branch->root()->url().path() + "/_directory", true);

        QString name = config->readEntry("Name", (*it)->text());
        branch->root()->setText(0, name);
        branch->root()->setText(1, '0' + config->readEntry("Position", "9999").rightJustify(4, '0') + name);

        delete config;

        addBranch(branch);
        branch->root()->setOpen(true);
    }
}

void DeviceChooser::slotSelectionChanged()
{
    KFileTreeViewItem *item = currentKFileTreeViewItem();
    if (!item || item->isDir())
        emit selectionChanged(QString::null);
    else
        emit selectionChanged(KURL::relativeURL(item->branch()->rootUrl(), item->url()).remove(QRegExp("\\.device$")));
}

#include "devicechooser.moc"

// vim: ts=4 sw=4 et
