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

#ifndef MODELVIEWSELECTOR_H
#define MODELVIEWSELECTOR_H

#include <qvaluelist.h>

#include <kdialogbase.h>
#include <kfileitem.h>

#include "pluginmanager.h"

class QComboBox;
class KDirLister;

namespace Spiceplus {

class ModelViewSelector : public KDialogBase
{
    Q_OBJECT

public:
    ModelViewSelector(QWidget *parent = 0);
    ~ModelViewSelector();

    QString deviceType() const;

private slots:
    void addItem(const KFileItemList &items);
    void insertItems();

private:
    class Item;

    QComboBox *m_comboBox;
    KDirLister *m_dirLister;
    QValueList<Item> m_items;
};

} // namespace Spiceplus

#endif // MODELVIEWSELECTOR_H

// vim: ts=4 sw=4 et
