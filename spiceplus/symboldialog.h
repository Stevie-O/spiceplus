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

#ifndef SYMBOLDIALOG_H
#define SYMBOLDIALOG_H

#include <kdialogbase.h>
#include <kfiletreebranch.h>

#include "devicesymbol.h"

class KFileTreeView;

namespace Spiceplus {

class SymbolDialogPreview : public QFrame
{
public:
    SymbolDialogPreview(QWidget *parent);

    void loadSymbol(const KURL &url);
    void unloadSymbol();

protected:
    void paintEvent(QPaintEvent *event);

private:
    DeviceSymbol m_symbol;
};

class SymbolDialogTreeBranch : public KFileTreeBranch
{
public:
    SymbolDialogTreeBranch(const QString &symbolDirID,
                           KFileTreeView *view,
                           const KURL &startURL,
                           const QString &name,
                           const QPixmap &pix);

    QString symbolDirID() const { return m_symbolDirID; }

protected:
    KFileTreeViewItem *createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem);

private:
    QString m_symbolDirID;
};

class SymbolDialog : public KDialogBase
{
    Q_OBJECT

public:
    SymbolDialog(QWidget *parent = 0);
    QString symbolPath() const;

private slots:
    void slotSelectionChanged();

private:
    KFileTreeView *m_tree;
    SymbolDialogPreview *m_preview;
};

} // namespace Spiceplus

#endif // SYMBOLDIALOG_H

// vim: ts=4 sw=4 et
