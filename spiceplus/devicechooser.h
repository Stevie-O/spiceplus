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

#ifndef DEVICECHOOSER_H
#define DEVICECHOOSER_H

#include <kfiletreeview.h>

namespace Spiceplus {

class DeviceChooserBranch : public KFileTreeBranch
{
public:
    DeviceChooserBranch(KFileTreeView *view, const KURL &url, const QString &name, const QPixmap &pix);

protected:
     KFileTreeViewItem *createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem);
};

class DeviceChooser : public KFileTreeView
{
    Q_OBJECT

public:
    DeviceChooser(QWidget *parent);
    ~DeviceChooser();

public slots:
    void resetTree();

signals:
    void selectionChanged(const QString &deviceID);

protected slots:
    QPixmap itemIcon(KFileTreeViewItem *item, int gap = 0) const;

private slots:
    void addDirectories(const KFileItemList &items);
    void slotSelectionChanged();

private:
    KDirLister *m_dirLister;
};

} // namespace Spiceplus

#endif // DEVICECHOOSER_H

// vim: ts=4 sw=4 et
