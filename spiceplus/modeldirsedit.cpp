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
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

#include <klocale.h>
#include <kdialog.h>
#include <klistview.h>
#include <kurlrequester.h>
#include <kmessagebox.h>

#include "modeldirsedit.h"
#include "modeldirs.h"
#include "file.h"
#include "parameterlineedit.h"

using namespace Spiceplus;

//
// ModelDirsEditInputDialog
//

ModelDirsEditInputDialog::ModelDirsEditInputDialog(const QString &caption, QWidget *parent)
    : KDialogBase(parent, 0, true, caption, Ok | Cancel)
{
    init();
}

ModelDirsEditInputDialog::ModelDirsEditInputDialog(const QString &caption, const QString &id, const QString &name, const QString &path, bool containsTypeDirs, QWidget *parent)
    : KDialogBase(parent, 0, true, caption, Ok | Cancel)
{
    init();
    m_id->setText(id);
    m_name->setText(name);
    m_path->setURL(path);
    m_containsTypeDirs->setChecked(containsTypeDirs);
}

void ModelDirsEditInputDialog::init()
{
    QWidget *w = new QWidget(this);
    setMainWidget(w);

    QGridLayout *grid = new QGridLayout(w, 4, 2, KDialog::spacingHint());
    grid->addWidget(new QLabel(i18n("ID:"), w), 0, 0);
    grid->addWidget(m_id = new ParameterLineEdit(w, ParameterLineEdit::String), 0, 1);
    grid->addWidget(new QLabel(i18n("Name:"), w), 1, 0);
    grid->addWidget(m_name = new QLineEdit(w), 1, 1);
    grid->addWidget(new QLabel(i18n("Path:"), w), 2, 0);
    m_path = new KURLRequester(w);
    m_path->setMode(KFile::Directory);
    m_path->setMinimumWidth(400);
    grid->addWidget(m_path, 2, 1);
    grid->addWidget(m_containsTypeDirs = new QCheckBox(i18n("Contains type directories"), w), 3, 1);
}

QString ModelDirsEditInputDialog::id() const
{
    return m_id->text();
}

QString ModelDirsEditInputDialog::name() const
{
    return m_name->text();
}

QString ModelDirsEditInputDialog::path() const
{
    return m_path->url();
}

bool ModelDirsEditInputDialog::containsTypeDirs() const
{
    return m_containsTypeDirs->isChecked();
}

void ModelDirsEditInputDialog::slotOk()
{
    if (m_id->text().isEmpty())
    {
        KMessageBox::sorry(this, i18n("No ID specified"));
        return;
    }

    if (m_name->text().isEmpty())
    {
        KMessageBox::sorry(this, i18n("No name specified"));
        return;
    }

    if (m_path->url().isEmpty())
    {
        KMessageBox::sorry(this, i18n("No path specified"));
        return;
    }

    if (m_id->text() == "standard" || m_id->text() == "project")
    {
        KMessageBox::sorry(this, i18n("The ID '%1' is reserved. Please choose another one.").arg(m_id->text()));
        return;
    }

    done(Accepted);
}

//
// ModelDirsEdit
//

const char *ModelDirsEdit::Yes = I18N_NOOP("Yes");
const char *ModelDirsEdit::No = I18N_NOOP("No");

ModelDirsEdit::ModelDirsEdit(QWidget *parent)
    : QWidget(parent)
{
    QBoxLayout *hbox = new QHBoxLayout(this, 0, KDialog::spacingHint());
    m_dirs = new KListView(this);
    m_dirs->setMaximumWidth(500);
    m_dirs->setAllColumnsShowFocus(true);
    m_dirs->addColumn(i18n("ID"));
    m_dirs->addColumn(i18n("Name"));
    m_dirs->addColumn(i18n("Path"));
    m_dirs->addColumn(i18n("Contains type directories"));
    hbox->addWidget(m_dirs);

    connect(m_dirs, SIGNAL(selectionChanged()), SLOT(updateButtons()));

    QBoxLayout *vbox = new QVBoxLayout(hbox, KDialog::spacingHint());
    QPushButton *button = new QPushButton(i18n("&Add..."), this);
    connect(button, SIGNAL(clicked()), SLOT(addDir()));
    vbox->addWidget(button);
    m_modifyButton = new QPushButton(i18n("M&odify..."), this);
    connect(m_modifyButton, SIGNAL(clicked()), SLOT(modifyDir()));
    vbox->addWidget(m_modifyButton);
    m_removeButton = new QPushButton(i18n("&Remove"), this);
    connect(m_removeButton, SIGNAL(clicked()), SLOT(removeDir()));
    vbox->addWidget(m_removeButton);
    vbox->addStretch();

    if (!m_dirs->selectedItem())
    {
        m_modifyButton->setEnabled(false);
        m_removeButton->setEnabled(false);
    }
}

bool ModelDirsEdit::isEmpty() const
{
    return m_dirs->childCount() == 0;
}

ModelDirs ModelDirsEdit::dirs() const
{
    ModelDirs dirs;

    for (QListViewItem *item = m_dirs->firstChild(); item; item = item->nextSibling())
        dirs.setDir(item->text(0), item->text(1), item->text(2), item->text(3) == Yes);

    return dirs;
}

void ModelDirsEdit::setDirs(const ModelDirs &modelDirs)
{
    m_dirs->clear();

    QMap<QString, ModelDirs::Dir> dirs = modelDirs.dirs();
    for (QMap<QString, ModelDirs::Dir>::Iterator it = dirs.begin(); it != dirs.end(); ++it)
        new KListViewItem(m_dirs, it.key(), it.data().name, it.data().path, it.data().containsTypeDirs ? Yes : No);

    if (m_dirs->firstChild())
        m_dirs->firstChild()->setSelected(true);

    updateButtons();
}

void ModelDirsEdit::clear()
{
    return m_dirs->clear();
}

void ModelDirsEdit::addDir()
{
    ModelDirsEditInputDialog *dlg = new ModelDirsEditInputDialog(i18n("Add model directory"), this);
    if (dlg->exec() == QDialog::Accepted)
    {
        if (m_dirs->findItem(dlg->id(), 0))
            KMessageBox::sorry(this, i18n("ID already exists"));
        else
        {
            new KListViewItem(m_dirs, dlg->id(), dlg->name(), dlg->path(), dlg->containsTypeDirs() ? Yes : No);
            emit modified();
        }
    }
    delete dlg;
}

void ModelDirsEdit::modifyDir()
{
    if (!m_dirs->selectedItem())
        return;

    ModelDirsEditInputDialog *dlg = new ModelDirsEditInputDialog(i18n("Modify model directory"),
                                                                 m_dirs->selectedItem()->text(0),
                                                                 m_dirs->selectedItem()->text(1),
                                                                 m_dirs->selectedItem()->text(2),
                                                                 m_dirs->selectedItem()->text(3) == Yes,
                                                                 this);
    if (dlg->exec() == QDialog::Accepted)
    {
        QListViewItem *item = m_dirs->findItem(dlg->id(), 0);
        if (item && item != m_dirs->selectedItem())
            KMessageBox::sorry(this, i18n("ID already exists"));
        else
        {
            m_dirs->selectedItem()->setText(0, dlg->id());
            m_dirs->selectedItem()->setText(1, dlg->name());
            m_dirs->selectedItem()->setText(2, dlg->path());
            m_dirs->selectedItem()->setText(3, dlg->containsTypeDirs() ? Yes : No);
            emit modified();
        }
    }
    delete dlg;
}

void ModelDirsEdit::removeDir()
{
    if (m_dirs->selectedItem())
    {
        delete m_dirs->selectedItem();
        emit modified();
    }
}

void ModelDirsEdit::updateButtons()
{
    if (m_dirs->selectedItem())
    {
        m_modifyButton->setEnabled(true);
        m_removeButton->setEnabled(true);
    }
    else
    {
        m_modifyButton->setEnabled(false);
        m_removeButton->setEnabled(false);
    }
}

#include "modeldirsedit.moc"

// vim: ts=4 sw=4 et
