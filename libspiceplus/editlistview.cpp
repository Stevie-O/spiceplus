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
#include <qpushbutton.h>

#include <klocale.h>
#include <kdialog.h>

#include "editlistview.h"

using namespace Spiceplus;

EditListView::EditListView(QWidget *parent)
    : KListView(parent)
{
    setSorting(-1);
    setAllColumnsShowFocus(true);
    connect(this, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
}

void EditListView::arrangeNewItem(QListViewItem *item)
{
    if (selectedItem() && selectedItem()->itemAbove())
        item->moveItem(selectedItem()->itemAbove());
    else if (!selectedItem() && lastItem())
        item->moveItem(lastItem());
}

void EditListView::deleteSelected()
{
    if (selectedItem())
    {
        delete selectedItem();

        if (currentItem())
            setSelected(currentItem(), true);
    }
}

void EditListView::deleteAll()
{
    clearSelection();
    clear();
}

void EditListView::slotSelectionChanged()
{
    emit itemSelected(isItemSelected());
}

//
// EditListViewControlPanel
//

EditListViewControlPanel::EditListViewControlPanel(EditListView *view, QWidget *parent)
    : QWidget(parent)
{
    QBoxLayout *layout = new QHBoxLayout(this, 0, KDialog::spacingHint());

    QPushButton *insertButton = new QPushButton(i18n("&Insert"), this);
    connect(insertButton, SIGNAL(clicked()), SIGNAL(insertClicked()));
    layout->addWidget(insertButton);

    QPushButton *deleteButton = new QPushButton(i18n("&Delete"), this);
    deleteButton->setEnabled(view->isItemSelected());
    deleteButton->connect(view, SIGNAL(itemSelected(bool)), SLOT(setEnabled(bool)));
    connect(deleteButton, SIGNAL(clicked()), view, SLOT(deleteSelected()));
    layout->addWidget(deleteButton);

    QPushButton *deleteAllButton = new QPushButton(i18n("Delete &All"), this);
    connect(deleteAllButton, SIGNAL(clicked()), view, SLOT(deleteAll()));
    layout->addWidget(deleteAllButton);
}

#include "editlistview.moc"

// vim: ts=4 sw=4 et
