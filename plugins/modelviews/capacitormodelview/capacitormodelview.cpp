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

#include <qstring.h>
#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdialog.h>

#include "capacitormodelview.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_modelview_capacitor, CapacitorModelViewFactory)

//
// CapacitorModelViewFactory
//

ModelView *CapacitorModelViewFactory::createModelView(Model &model, QWidget *parent)
{
    return new CapacitorModelView(model, parent);
}

//
// CapacitorModelView
//

CapacitorModelView::CapacitorModelView(Model &model, QWidget *parent)
    : ModelView(parent), m_model(model)
{
    QGridLayout *grid = new QGridLayout(this, 4, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Junction bottom capacitance (CJ):"), this), 0, 0);
    grid->addWidget(m_cj = new ParameterLineEdit(this), 0, 1);
    grid->addWidget(new QLabel(i18n("Farads * (Meters ^ -2)"), this), 0, 2);
    connect(m_cj, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Junction sidewall capacitance (CJSW):"), this), 1, 0);
    grid->addWidget(m_cjsw = new ParameterLineEdit(this), 1, 1);
    grid->addWidget(new QLabel(i18n("Farads * (Meters ^ -1)"), this), 1, 2);
    connect(m_cjsw, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Default device width (DEFW):"), this), 2, 0);
    grid->addWidget(m_defw = new ParameterLineEdit(this), 2, 1);
    grid->addWidget(new QLabel(i18n("Meters"), this), 2, 2);
    connect(m_defw, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Narrowing due to side etching (NARROW):"), this), 3, 0);
    grid->addWidget(m_narrow = new ParameterLineEdit(this), 3, 1);
    grid->addWidget(new QLabel(i18n("Meters"), this), 3, 2);
    connect(m_narrow, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));
}

void CapacitorModelView::update()
{
    m_cj->setText(m_model.parameter("cj"));
    m_cjsw->setText(m_model.parameter("cjsw"));
    m_defw->setText(m_model.parameter("defw"));
    m_narrow->setText(m_model.parameter("narrow"));
}

bool CapacitorModelView::apply()
{
    m_model.setType("c");
    m_model.setDeviceType("c");
    m_model.setParameter("cj", m_cj->text());
    m_model.setParameter("cjsw", m_cjsw->text());
    m_model.setParameter("defw", m_defw->text());
    m_model.setParameter("narrow", m_narrow->text());
    return true;
}

void CapacitorModelView::slotTextChanged(const QString &)
{
    emit modified(true);
}

#include "capacitormodelview.moc"

// vim: ts=4 sw=4 et
