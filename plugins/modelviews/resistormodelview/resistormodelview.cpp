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

#include "resistormodelview.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_modelview_resistor, ResistorModelViewFactory)

//
// ResistorModelViewFactory
//

ModelView *ResistorModelViewFactory::createModelView(Model &model, QWidget *parent)
{
    return new ResistorModelView(model, parent);
}

//
// ResistorModelView
//

ResistorModelView::ResistorModelView(Model &model, QWidget *parent)
    : ModelView(parent), m_model(model)
{
    QGridLayout *grid = new QGridLayout(this, 6, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("First order temperature coefficient (TC1):"), this), 0, 0);
    grid->addWidget(m_tc1 = new ParameterLineEdit(this), 0, 1);
    grid->addWidget(new QLabel(i18n("Z * Degrees Centigrade ^ -1"), this), 0, 2);
    connect(m_tc1, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Second order temperature coefficient (TC2):"), this), 1, 0);
    grid->addWidget(m_tc2 = new ParameterLineEdit(this), 1, 1);
    grid->addWidget(new QLabel(i18n("Z * Degrees Centigrade ^ -2"), this), 1, 2);
    connect(m_tc2, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Sheet resistance (RSH):"), this), 2, 0);
    grid->addWidget(m_rsh = new ParameterLineEdit(this), 2, 1);
    grid->addWidget(new QLabel(i18n("Ohm / []"), this), 2, 2);
    connect(m_rsh, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Default width (DEFW):"), this), 3, 0);
    grid->addWidget(m_defw = new ParameterLineEdit(this), 3, 1);
    grid->addWidget(new QLabel(i18n("Meters"), this), 3, 2);
    connect(m_defw, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Narrowing due to side etching (NARROW):"), this), 4, 0);
    grid->addWidget(m_narrow = new ParameterLineEdit(this), 4, 1);
    grid->addWidget(new QLabel(i18n("Meters"), this), 4, 2);
    connect(m_narrow, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Parameter measurement temperature (TNOM):"), this), 5, 0);
    grid->addWidget(m_tnom = new ParameterLineEdit(this), 5, 1);
    grid->addWidget(new QLabel(i18n("Degrees Centigrade"), this), 5, 2);
    connect(m_tnom, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));
}

void ResistorModelView::update()
{
    m_tc1->setText(m_model.parameter("tc1"));
    m_tc2->setText(m_model.parameter("tc2"));
    m_rsh->setText(m_model.parameter("rsh"));
    m_defw->setText(m_model.parameter("defw"));
    m_narrow->setText(m_model.parameter("narrow"));
    m_tnom->setText(m_model.parameter("tnom"));
}

bool ResistorModelView::apply()
{
    m_model.setType("r");
    m_model.setDeviceType("r");
    m_model.setParameter("tc1", m_tc1->text());
    m_model.setParameter("tc2", m_tc2->text());
    m_model.setParameter("rsh", m_rsh->text());
    m_model.setParameter("defw", m_defw->text());
    m_model.setParameter("narrow", m_narrow->text());
    m_model.setParameter("tnom", m_tnom->text());
    return true;
}

void ResistorModelView::slotTextChanged(const QString &)
{
    emit modified(true);
}

#include "resistormodelview.moc"

// vim: ts=4 sw=4 et
