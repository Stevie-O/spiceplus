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

#include <qsplitter.h>
#include <qstyle.h>
#include <qheader.h>
#include <qpainter.h>

#include <klocale.h>
#include <kfiletreeview.h>
#include <kiconloader.h>

#include "symboldialog.h"
#include "settings.h"
#include "project.h"

using namespace Spiceplus;

//
// SymbolDialogPreview
//

SymbolDialogPreview::SymbolDialogPreview(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(style().pixelMetric(QStyle::PM_DefaultFrameWidth, this));
    setPaletteBackgroundColor(Settings::self()->schematicBackgroundColor());
    setMinimumWidth(200);
}

void SymbolDialogPreview::loadSymbol(const KURL &url)
{
    m_symbol.load(url);
    repaint();
}

void SymbolDialogPreview::unloadSymbol()
{
    m_symbol.unload();
    repaint();
}

void SymbolDialogPreview::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    m_symbol.draw(p, width() / 2, height() / 2);
    QFrame::paintEvent(event);
}

//
// SymbolDialogTreeBranch
//

SymbolDialogTreeBranch::SymbolDialogTreeBranch(const QString &symbolDirID,
                                               KFileTreeView *view,
                                               const KURL &startURL,
                                               const QString &name,
                                               const QPixmap &pix)
    : KFileTreeBranch(view, startURL, name, pix), m_symbolDirID(symbolDirID)
{
    setNameFilter("*.symbol");
    setShowExtensions(false);
}

KFileTreeViewItem *SymbolDialogTreeBranch::createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem)
{
    KFileTreeViewItem *item = new KFileTreeViewItem(parent, fileItem, this);
    QString name;

    item->setText(1, (item->isDir() ? '0' : '1') + fileItem->text());

    if (!item->isDir())
        item->setPixmap(0, KGlobal::iconLoader()->loadIcon("spiceplus_device_symbol", KIcon::Small));

    return item;
}

//
// SymbolDialog
//

SymbolDialog::SymbolDialog(QWidget *parent)
    : KDialogBase(parent, 0, true, i18n("Select symbol"), Ok | Cancel)
{
    QWidget *splitter = new QSplitter(this);
    setMainWidget(splitter);

    m_tree = new KFileTreeView(splitter);
    m_tree->setMinimumSize(300, 400);
    m_tree->setRootIsDecorated(true);
    m_tree->header()->hide();
    m_tree->addColumn("");
    m_tree->setSorting(1);
    m_preview = new SymbolDialogPreview(splitter);

    KURL startURL = Settings::self()->standardSymbolDir();
    startURL.adjustPath(1);
    SymbolDialogTreeBranch *branch = new SymbolDialogTreeBranch("standard",
                                                                m_tree,
                                                                startURL,
                                                                i18n("Standard Symbols"),
                                                                QPixmap());
    m_tree->addBranch(branch);
    branch->root()->setText(1, "0");

    if (Project::self()->isOpen())
    {
        SymbolDialogTreeBranch *branch = new SymbolDialogTreeBranch("project",
                                                                    m_tree,
                                                                    Project::self()->dir(),
                                                                    i18n("Project Symbols"),
                                                                    QPixmap());
        m_tree->addBranch(branch);
        branch->root()->setText(1, "1");
    }

    QMap<QString, SymbolDirs::Dir> symbolDirs = Settings::self()->userSymbolDirs().dirs();
    for (QMap<QString, SymbolDirs::Dir>::Iterator it = symbolDirs.begin(); it != symbolDirs.end(); ++it)
    {
        KURL startURL = it.data().path;
        startURL.adjustPath(1);
        SymbolDialogTreeBranch *branch = new SymbolDialogTreeBranch(it.key(),
                                                                    m_tree,
                                                                    startURL,
                                                                    it.data().name,
                                                                    QPixmap());
        m_tree->addBranch(branch);
        branch->root()->setText(1, "2" + it.data().name);
    }

    connect(m_tree, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
}

QString SymbolDialog::symbolPath() const
{
    KFileTreeViewItem *item = m_tree->currentKFileTreeViewItem();
    if (item && !item->isDir())
    {
        SymbolDialogTreeBranch *branch = static_cast<SymbolDialogTreeBranch *>(item->branch());
        return branch->symbolDirID() + ":" + KURL::relativeURL(branch->rootUrl(), item->url());
    }

    return QString::null;
}

void SymbolDialog::slotSelectionChanged()
{
    KFileTreeViewItem *item = m_tree->currentKFileTreeViewItem();
    if (item && !item->isDir())
        m_preview->loadSymbol(item->url());
    else
        m_preview->unloadSymbol();
}

#include "symboldialog.moc"

// vim: ts=4 sw=4 et
