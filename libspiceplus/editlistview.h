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

#ifndef EDITLISTVIEW_H
#define EDITLISTVIEW_H

#include <klistview.h>
#include <qwidget.h>

class QPushButton;

namespace Spiceplus {

class EditListView : public KListView
{
    Q_OBJECT

public:
    EditListView(QWidget *parent = 0);

    void arrangeNewItem(QListViewItem *item);
    bool isItemSelected() const { return selectedItem(); }
    // TODO: make items editable

public slots:
    void deleteSelected();
    void deleteAll();

signals:
    void itemSelected(bool yes);

private slots:
    void slotSelectionChanged();
};

class EditListViewControlPanel : public QWidget
{
    Q_OBJECT

public:
    EditListViewControlPanel(EditListView *view, QWidget *parent);

signals:
    void insertClicked();
};

} // namespace Spiceplus

#endif // EDITLISTVIEW_H

// vim: ts=4 sw=4 et
