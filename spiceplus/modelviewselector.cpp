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
#include <qcombobox.h>
#include <qregexp.h>

#include <klocale.h>
#include <kdirlister.h>
#include <kmessagebox.h>

#include "modelviewselector.h"
#include "settings.h"

using namespace Spiceplus;

//
// ModelViewSelector::Item
//

class ModelViewSelector::Item
{
public:
    Item() {}
    Item(const QString &deviceType, const QString &name, int position) : m_deviceType(deviceType), m_name(name), m_position(position) {}
    bool operator<(const Item &i) const { return m_position < i.m_position || m_position == i.m_position && m_name < i.m_name; }

    QString m_deviceType;
    QString m_name;
    int m_position;
};

//
// ModelViewSelector
//

ModelViewSelector::ModelViewSelector(QWidget *parent)
    : KDialogBase(parent, 0, true, i18n("Model Device Type"), Ok | Cancel)
{
    QWidget *w = new QWidget(this);
    setMainWidget(w);

    QBoxLayout *topLayout = new QVBoxLayout(w, 0, spacingHint());
    QBoxLayout *hl = new QHBoxLayout(topLayout);

    hl->addWidget(new QLabel(i18n("Select the device type of the model:"), w));
    hl->addWidget(m_comboBox = new QComboBox(w));
    topLayout->addStretch();

    m_dirLister = new KDirLister;
    m_dirLister->setAutoUpdate(false);
    m_dirLister->setNameFilter("*.modelview");
    connect(m_dirLister, SIGNAL(newItems(const KFileItemList &)), SLOT(addItem(const KFileItemList &)));
    connect(m_dirLister, SIGNAL(completed()), SLOT(insertItems()));
    m_dirLister->openURL(Settings::self()->modelViewDir(), true);
}

ModelViewSelector::~ModelViewSelector()
{
    delete m_dirLister;
}

QString ModelViewSelector::deviceType() const
{
    if (m_items.count() == 0)
        return QString::null;

    return m_items[m_comboBox->currentItem()].m_deviceType;
}

void ModelViewSelector::addItem(const KFileItemList &items)
{
    for (KFileItemList::Iterator it = items.begin(); it != items.end(); ++it)
    {
        if ((*it)->isDir())
            continue;

        KConfig *config = new KConfig((*it)->url().path(), true);
        m_items << Item(QString((*it)->text()).remove(QRegExp("\\.modelview$")) , config->readEntry("Name"), config->readNumEntry("Position", -1));
        delete config;
    }
}

void ModelViewSelector::insertItems()
{
    if (m_items.count() == 0)
    {
        show();
        KMessageBox::error(this, i18n("No Model View plugins found"));
    }
    else
    {
        qHeapSort(m_items);
        for (QValueList<Item>::Iterator it = m_items.begin(); it != m_items.end(); ++it)
            m_comboBox->insertItem((*it).m_name);
    }
}

#include "modelviewselector.moc"

// vim: ts=4 sw=4 et
