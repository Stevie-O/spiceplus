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

#include "diodemodelview.h"
#include "parameterlineedit.h"

using namespace Spiceplus;
using namespace Plugin;

K_EXPORT_COMPONENT_FACTORY(spiceplus_modelview_diode, DiodeModelViewFactory)

//
// DiodeModelViewFactory
//

ModelView *DiodeModelViewFactory::createModelView(Model &model, QWidget *parent)
{
    return new DiodeModelView(model, parent);
}

//
// DiodeModelView
//

DiodeModelView::DiodeModelView(Model &model, QWidget *parent)
    : ModelView(parent), m_model(model)
{
    QGridLayout *grid = new QGridLayout(this, 15, 3, 0, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Saturation current (IS):"), this), 0, 0);
    grid->addWidget(m_is = new ParameterLineEdit(this), 0, 1);
    grid->addWidget(new QLabel(i18n("Amps"), this), 0, 2);
    connect(m_is, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Ohmic resistance (RS):"), this), 1, 0);
    grid->addWidget(m_rs = new ParameterLineEdit(this), 1, 1);
    grid->addWidget(new QLabel(i18n("Z"), this), 1, 2);
    connect(m_rs, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Emission coefficient (N):"), this), 2, 0);
    grid->addWidget(m_n = new ParameterLineEdit(this), 2, 1);
    connect(m_n, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Transit-time (TT):"), this), 3, 0);
    grid->addWidget(m_tt = new ParameterLineEdit(this), 3, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 3, 2);
    connect(m_tt, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Zero-bias junction capacitance (CJO):"), this), 4, 0);
    grid->addWidget(m_cjo = new ParameterLineEdit(this), 4, 1);
    grid->addWidget(new QLabel(i18n("Farads"), this), 4, 2);
    connect(m_cjo, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Junction potential (VJ):"), this), 5, 0);
    grid->addWidget(m_vj = new ParameterLineEdit(this), 5, 1);
    grid->addWidget(new QLabel(i18n("Volts"), this), 5, 2);
    connect(m_vj, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Grading coefficient (M):"), this), 6, 0);
    grid->addWidget(m_m = new ParameterLineEdit(this), 6, 1);
    connect(m_m, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Activation energy (EG):"), this), 7, 0);
    grid->addWidget(m_eg = new ParameterLineEdit(this), 7, 1);
    grid->addWidget(new QLabel(i18n("eV"), this), 7, 2);
    connect(m_eg, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Saturation-current temperature exponent (XTI):"), this), 8, 0);
    grid->addWidget(m_xti = new ParameterLineEdit(this), 8, 1);
    connect(m_xti, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Flicker noise coefficient (KF):"), this), 9, 0);
    grid->addWidget(m_kf = new ParameterLineEdit(this), 9, 1);
    connect(m_kf, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Flicker noise exponent (AF):"), this), 10, 0);
    grid->addWidget(m_af = new ParameterLineEdit(this), 10, 1);
    connect(m_af, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Coefficient for forward-bias depletion capacitance formula (FC):"), this), 11, 0);
    grid->addWidget(m_fc = new ParameterLineEdit(this), 11, 1);
    connect(m_fc, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Reverse breakdown voltage (BV):"), this), 12, 0);
    grid->addWidget(m_bv = new ParameterLineEdit(this), 12, 1);
    grid->addWidget(new QLabel(i18n("Volts"), this), 12, 2);
    connect(m_bv, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Current at breakdown voltage (IBV):"), this), 13, 0);
    grid->addWidget(m_ibv = new ParameterLineEdit(this), 13, 1);
    grid->addWidget(new QLabel(i18n("Amps"), this), 13, 2);
    connect(m_ibv, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));

    grid->addWidget(new QLabel(i18n("Parameter measurement temperature (TNOM):"), this), 14, 0);
    grid->addWidget(m_tnom = new ParameterLineEdit(this), 14, 1);
    grid->addWidget(new QLabel(i18n("Degrees Centigrade"), this), 14, 2);
    connect(m_tnom, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));
}

void DiodeModelView::update()
{
    m_is->setText(m_model.parameter("is"));
    m_rs->setText(m_model.parameter("rs"));
    m_n->setText(m_model.parameter("n"));
    m_tt->setText(m_model.parameter("tt"));
    m_cjo->setText(m_model.parameter("cjo"));
    m_vj->setText(m_model.parameter("vj"));
    m_m->setText(m_model.parameter("m"));
    m_eg->setText(m_model.parameter("eg"));
    m_xti->setText(m_model.parameter("xti"));
    m_kf->setText(m_model.parameter("kf"));
    m_af->setText(m_model.parameter("af"));
    m_fc->setText(m_model.parameter("fc"));
    m_bv->setText(m_model.parameter("bv"));
    m_ibv->setText(m_model.parameter("ibv"));
    m_tnom->setText(m_model.parameter("tnom"));
}

bool DiodeModelView::apply()
{
    m_model.setType("d");
    m_model.setDeviceType("d");
    m_model.setParameter("is", m_is->text());
    m_model.setParameter("rs", m_rs->text());
    m_model.setParameter("n", m_n->text());
    m_model.setParameter("tt", m_tt->text());
    m_model.setParameter("cjo", m_cjo->text());
    m_model.setParameter("vj", m_vj->text());
    m_model.setParameter("m", m_m->text());
    m_model.setParameter("eg", m_eg->text());
    m_model.setParameter("xti", m_xti->text());
    m_model.setParameter("kf", m_kf->text());
    m_model.setParameter("af", m_af->text());
    m_model.setParameter("fc", m_fc->text());
    m_model.setParameter("bv", m_bv->text());
    m_model.setParameter("ibv", m_ibv->text());
    m_model.setParameter("tnom", m_tnom->text());
    return true;
}

void DiodeModelView::slotTextChanged(const QString &)
{
    emit modified(true);
}

#include "diodemodelview.moc"

// vim: ts=4 sw=4 et
