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

#include <qscrollview.h>
#include <qlayout.h>
#include <qwidgetstack.h>
#include <qpushbutton.h>

#include <kiconloader.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <klineedit.h>

#include "modeldocument.h"
#include "groupbox.h"
#include "symboldialog.h"

using namespace Spiceplus;

ModelDocument::ModelDocument(ModelViewFactory *factory, QWidgetStack *toolWindowStack, QWidget *parent, const char *name)
    : Document(toolWindowStack, parent, name), m_isModified(false)
{
    setIcon(SmallIcon("misc_doc"));

    QBoxLayout *topLayout = new QVBoxLayout(this);
    QScrollView *sv = new QScrollView(this);
    sv->setResizePolicy(QScrollView::AutoOneFit);
    QWidget *w = new QWidget(sv->viewport());
    sv->addChild(w);
    topLayout->addWidget(sv);

    QBoxLayout *layout = new QVBoxLayout(w, KDialog::marginHint(), KDialog::spacingHint());
    m_view = factory->createModelView(m_model, w);
    layout->addWidget(m_view);

    GroupBox *miscBox = new GroupBox(0, Qt::Vertical, i18n("Misc Options"), w);
    QGridLayout *grid = new QGridLayout(miscBox->layout(), 2, 3, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Alias:"), miscBox), 0, 0);
    m_alias = new KLineEdit(miscBox);
    connect(m_alias, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));
    grid->addWidget(m_alias, 0, 1);

    grid->addWidget(new QLabel(i18n("Symbol path:"), miscBox), 1, 0);
    m_symbolPath = new KLineEdit(miscBox);
    m_symbolPath->setReadOnly(true);
    connect(m_symbolPath, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));
    grid->addWidget(m_symbolPath, 1, 1);
    QPushButton *selectButton = new QPushButton(i18n("Select..."), miscBox);
    connect(selectButton, SIGNAL(clicked()), SLOT(openSymbolDialog()));
    grid->addWidget(selectButton, 1, 2);

    layout->addWidget(miscBox);
    layout->addStretch();

    connect(m_view, SIGNAL(modified(bool)), SLOT(setIsModified(bool)));

    m_toolWindow = new QWidget(m_toolWindowStack);
    m_toolWindowStack->addWidget(m_toolWindow);
}

ModelDocument::~ModelDocument()
{
    m_toolWindowStack->removeWidget(m_toolWindow);
    delete m_toolWindow;
}

void ModelDocument::openNew(const KURL &url)
{
    m_fileURL = url;
    setMDICaption(fileName());
}

void ModelDocument::open(const ModelFile &model, const KURL &url)
{
    m_model = model;
    m_view->blockSignals(true);
    m_view->update();
    m_view->blockSignals(false);

    m_alias->blockSignals(true);
    m_alias->setText(m_model.alias());
    m_alias->blockSignals(false);

    m_symbolPath->blockSignals(true);
    m_symbolPath->setText(m_model.symbolPath());
    m_symbolPath->blockSignals(false);

    m_fileURL = url;
    setMDICaption(fileName());
}

bool ModelDocument::save()
{
    if (isBrandNew())
        return saveAs();

    if (m_view->apply())
    {
        m_model.setAlias(m_alias->text().stripWhiteSpace());
        m_model.setSymbolPath(m_symbolPath->text());

        if (m_model.save(fileURL()))
            setIsModified(false);
        else
        {
            KMessageBox::error(this, m_model.errorString());
            return false;
        }
    }
    else
    {
        KMessageBox::error(this, m_view->errorString());
        return false;
    }

    return true;
}

bool ModelDocument::saveAs()
{
    KURL url = KFileDialog::getSaveURL(isBrandNew() ? ":document" : fileURL().url(),
                                       "*.model|" + i18n("Model File"), this, i18n("Save Model"));

    if (url.isEmpty() || !isFileNewOrCanOverwrite(url))
        return false;

    if (m_view->apply())
    {
        m_model.setAlias(m_alias->text().stripWhiteSpace());
        m_model.setSymbolPath(m_symbolPath->text());

        if (m_model.save(url))
        {
            m_fileURL = url;
            setMDICaption(fileName());
            m_isBrandNew = false;
            setIsModified(false);
        }
        else
        {
            KMessageBox::error(this, m_model.errorString());
            return false;
        }
    }
    else
    {
        KMessageBox::error(this, m_view->errorString());
        return false;
    }

    return true;
}

bool ModelDocument::isModified() const
{
    return m_isModified;
}

void ModelDocument::updateState()
{
    emit modified(m_isModified);

    emit undoAvailable(false);
    emit redoAvailable(false);
    emit deleteAvailable(false);
    emit selectAllAvailable(false);
    emit rotateAvailable(false);
    emit flipAvailable(false);

    emit zoomInAvailable(false);
    emit zoomOutAvailable(false);
    emit zoomAvailable(false);

    emit analysisEnabled(false);

    emit toolsChecked(false);
    emit toolsEnabled(false);

    m_toolWindowStack->raiseWidget(m_toolWindow);
}

void ModelDocument::openSymbolDialog()
{
    SymbolDialog *dlg = new SymbolDialog(this);
    if (dlg->exec() == QDialog::Accepted)
        m_symbolPath->setText(dlg->symbolPath());
    delete dlg;
}

void ModelDocument::setIsModified(bool isModified)
{
    emit modified(m_isModified = isModified);
}

#include "modeldocument.moc"

// vim: ts=4 sw=4 et
