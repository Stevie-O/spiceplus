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
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>

#include <klocale.h>
#include <kdialog.h>

#include "independentsource.h"
#include "parameterlineedit.h"
#include "editlistview.h"
#include "groupbox.h"

using namespace Spiceplus;
using namespace Plugin;

//
// IndependentSource
//

IndependentSource::IndependentSource(Schematic *schematic)
    : SchematicStandardDevice(schematic)
{
    addPin(new SchematicDevicePin("p", this));
    addPin(new SchematicDevicePin("n", this));
}

bool IndependentSource::initialize()
{
    return true;
}

void IndependentSource::updateLabels()
{
    setLabelText("name", name());
}

void IndependentSource::setDefaults()
{
    setName(schematic()->createUniqueDeviceName(type().upper()));
    setParameter("type", "dc");
    setParameter("dcvalue", "1");
    setParameter("acmag", "1");
    setParameter("acphase", "0");
    setParameter("distof1mag", "");
    setParameter("distof1phase", "");
    setParameter("distof2mag", "");
    setParameter("distof2phase", "");

    updateLabels();
}

QString IndependentSource::createCommand()
{
    QString cmd;
    SchematicDevicePin *pinP = findPin("p");
    SchematicDevicePin *pinN = findPin("n");

    if (!pinP || !pinN)
    {
        setErrorString(i18n("%1 %2: Pin not found").arg(description()).arg(name()));
        return cmd;
    }

    if (!pinP->node() || !pinN->node())
    {
        setErrorString(i18n("%1 %2: Pin(s) not connected").arg(description()).arg(name()));
        return cmd;
    }

    cmd = type() + name().lower() + " " + pinP->node()->name() + " " + pinN->node()->name();

    if (parameter("type") == "dc")
        cmd += " dc " + parameter("dcvalue");

    cmd += " ac " + parameter("acmag") + " " + parameter("acphase");

    if (!parameter("distof1mag").isEmpty())
        cmd += " distof1 " + parameter("distof1mag") + " " + parameter("distof1phase");

    if (!parameter("distof2mag").isEmpty())
        cmd += " distof2 " + parameter("distof2mag") + " " + parameter("distof2phase");

    if (parameter("type") == "pulse")
    {
        cmd += " pulse(" + parameter("v1") + " "
                         + parameter("v2") + " "
                         + parameter("td") + " "
                         + parameter("tr") + " "
                         + parameter("tf") + " "
                         + parameter("pw") + " "
                         + parameter("per") + ")";
    }
    else if (parameter("type") == "sin")
    {
        cmd += " sin(" + parameter("vo") + " "
                       + parameter("va") + " "
                       + parameter("freq") + " "
                       + parameter("td") + " "
                       + parameter("theta") + ")";
    }
    else if (parameter("type") == "exp")
    {
        cmd += " exp(" + parameter("v1") + " "
                       + parameter("v2") + " "
                       + parameter("td1") + " "
                       + parameter("tau1") + " "
                       + parameter("td2") + " "
                       + parameter("tau2") + ")";
    }
    else if (parameter("type") == "pwl")
    {
        const StringTable table = parameterToStringTable("points");
        if (table.count() > 0)
        {
            cmd += " pwl(";
            for (size_t row = 0; row < table.count(); ++row)
                cmd += table[row][0] + " " + table[row][1] + " ";
            cmd += ")";
        }
    }
    if (parameter("type") == "sffm")
    {
        cmd += " sffm(" + parameter("vo") + " "
                        + parameter("va") + " "
                        + parameter("fc") + " "
                        + parameter("mdi") + " "
                        + parameter("fs") + ")";
    }

    return cmd;
}

SchematicDevicePropertiesWidget *IndependentSource::createPropertiesWidget(QWidget *parent)
{
    return new IndependentSourcePropertiesWidget(this, parent);
}

//
// IndependentSourceValuesDC
//

IndependentSourceValuesDC::IndependentSourceValuesDC(IndependentSource *source, QWidget *parent)
    : IndependentSourceValues(source, parent)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
    QBoxLayout *layout = new QHBoxLayout(topLayout, KDialog::spacingHint());

    layout->addWidget(new QLabel(i18n("DC Value:"), this));
    layout->addWidget(m_dcvalue = new ParameterLineEdit("1", "0", this));
    layout->addWidget(new QLabel(m_source->unit(), this));
    topLayout->addStretch();
}

void IndependentSourceValuesDC::loadParameters()
{
    m_dcvalue->setText(m_source->parameter("dcvalue"));
}

bool IndependentSourceValuesDC::saveParameters() const
{
    m_source->setParameter("type", "dc");
    m_source->setParameter("dcvalue", m_dcvalue->text());
    return true;
}

//
// IndependentSourceValuesPulse
//

IndependentSourceValuesPulse::IndependentSourceValuesPulse(IndependentSource *source, QWidget *parent)
    : IndependentSourceValues(source, parent)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
    QGridLayout *grid = new QGridLayout(topLayout, 7, 3, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Initial Value:"), this), 0, 0);
    grid->addWidget(m_v1 = new ParameterLineEdit("0", "0", this), 0, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 0, 2);

    grid->addWidget(new QLabel(i18n("Pulsed Value:"), this), 1, 0);
    grid->addWidget(m_v2 = new ParameterLineEdit("0", "0", this), 1, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 1, 2);

    grid->addWidget(new QLabel(i18n("Delay Time:"), this), 2, 0);
    grid->addWidget(m_td = new ParameterLineEdit("0", "0", this), 2, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 2, 2);

    grid->addWidget(new QLabel(i18n("Rise Time:"), this), 3, 0);
    grid->addWidget(m_tr = new ParameterLineEdit("0", "0", this), 3, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 3, 2);

    grid->addWidget(new QLabel(i18n("Fall Time:"), this), 4, 0);
    grid->addWidget(m_tf = new ParameterLineEdit("0", "0", this), 4, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 4, 2);

    grid->addWidget(new QLabel(i18n("Pulse Width:"), this), 5, 0);
    grid->addWidget(m_pw = new ParameterLineEdit("0", "0", this), 5, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 5, 2);

    grid->addWidget(new QLabel(i18n("Period:"), this), 6, 0);
    grid->addWidget(m_per = new ParameterLineEdit("0", "0", this), 6, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 6, 2);

    topLayout->addStretch();
}

void IndependentSourceValuesPulse::loadParameters()
{
    m_v1->setText(m_source->parameter("v1"));
    m_v2->setText(m_source->parameter("v2"));
    m_td->setText(m_source->parameter("td"));
    m_tr->setText(m_source->parameter("tr"));
    m_tf->setText(m_source->parameter("tf"));
    m_pw->setText(m_source->parameter("pw"));
    m_per->setText(m_source->parameter("per"));
}

bool IndependentSourceValuesPulse::saveParameters() const
{
    m_source->setParameter("type", "pulse");
    m_source->setParameter("v1", m_v1->text());
    m_source->setParameter("v2", m_v2->text());
    m_source->setParameter("td", m_td->text());
    m_source->setParameter("tr", m_tr->text());
    m_source->setParameter("tf", m_tf->text());
    m_source->setParameter("pw", m_pw->text());
    m_source->setParameter("per", m_per->text());

    return true;
}

//
// IndependentSourceValuesSin
//

IndependentSourceValuesSin::IndependentSourceValuesSin(IndependentSource *source, QWidget *parent)
    : IndependentSourceValues(source, parent)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
    QGridLayout *grid = new QGridLayout(topLayout, 5, 3, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Offset:"), this), 0, 0);
    grid->addWidget(m_vo = new ParameterLineEdit("0", "0", this), 0, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 0, 2);

    grid->addWidget(new QLabel(i18n("Amplitude:"), this), 1, 0);
    grid->addWidget(m_va = new ParameterLineEdit("0", "0", this), 1, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 1, 2);

    grid->addWidget(new QLabel(i18n("Frequency:"), this), 2, 0);
    grid->addWidget(m_freq = new ParameterLineEdit("0", "0", this), 2, 1);
    grid->addWidget(new QLabel(i18n("Herz"), this), 2, 2);

    grid->addWidget(new QLabel(i18n("Delay:"), this), 3, 0);
    grid->addWidget(m_td = new ParameterLineEdit("0", "0", this), 3, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 3, 2);

    grid->addWidget(new QLabel(i18n("Damping Factor:"), this), 4, 0);
    grid->addWidget(m_theta = new ParameterLineEdit("0", "0", this), 4, 1);
    grid->addWidget(new QLabel(i18n("Seconds ^ -1"), this), 4, 2);

    topLayout->addStretch();
}

void IndependentSourceValuesSin::loadParameters()
{
    m_vo->setText(m_source->parameter("vo"));
    m_va->setText(m_source->parameter("va"));
    m_freq->setText(m_source->parameter("freq"));
    m_td->setText(m_source->parameter("td"));
    m_theta->setText(m_source->parameter("theta"));
}

bool IndependentSourceValuesSin::saveParameters() const
{
    m_source->setParameter("type", "sin");
    m_source->setParameter("vo", m_vo->text());
    m_source->setParameter("va", m_va->text());
    m_source->setParameter("freq", m_freq->text());
    m_source->setParameter("td", m_td->text());
    m_source->setParameter("theta", m_theta->text());

    return true;
}

//
// IndependentSourceValuesExp
//

IndependentSourceValuesExp::IndependentSourceValuesExp(IndependentSource *source, QWidget *parent)
    : IndependentSourceValues(source, parent)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
    QGridLayout *grid = new QGridLayout(topLayout, 6, 3, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Initial Value:"), this), 0, 0);
    grid->addWidget(m_v1 = new ParameterLineEdit("0", "0", this), 0, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 0, 2);

    grid->addWidget(new QLabel(i18n("Pulsed Value:"), this), 1, 0);
    grid->addWidget(m_v2 = new ParameterLineEdit("0", "0", this), 1, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 1, 2);

    grid->addWidget(new QLabel(i18n("Rise Delay Time:"), this), 2, 0);
    grid->addWidget(m_td1 = new ParameterLineEdit("0", "0", this), 2, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 2, 2);

    grid->addWidget(new QLabel(i18n("Rise Time Constant:"), this), 3, 0);
    grid->addWidget(m_tau1 = new ParameterLineEdit("0", "0", this), 3, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 3, 2);

    grid->addWidget(new QLabel(i18n("Fall Delay Time:"), this), 4, 0);
    grid->addWidget(m_td2 = new ParameterLineEdit("0", "0", this), 4, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 4, 2);

    grid->addWidget(new QLabel(i18n("Fall Time Constant:"), this), 5, 0);
    grid->addWidget(m_tau2 = new ParameterLineEdit("0", "0", this), 5, 1);
    grid->addWidget(new QLabel(i18n("Seconds"), this), 5, 2);

    topLayout->addStretch();
}

void IndependentSourceValuesExp::loadParameters()
{
    m_v1->setText(m_source->parameter("v1"));
    m_v2->setText(m_source->parameter("v2"));
    m_td1->setText(m_source->parameter("td1"));
    m_tau1->setText(m_source->parameter("tau1"));
    m_td2->setText(m_source->parameter("td2"));
    m_tau2->setText(m_source->parameter("tau2"));
}

bool IndependentSourceValuesExp::saveParameters() const
{
    m_source->setParameter("type", "exp");
    m_source->setParameter("v1", m_v1->text());
    m_source->setParameter("v2", m_v2->text());
    m_source->setParameter("td1", m_td1->text());
    m_source->setParameter("tau1", m_tau1->text());
    m_source->setParameter("td2", m_td2->text());
    m_source->setParameter("tau2", m_tau2->text());

    return true;
}

//
// IndependentSourceValuesPWL
//

IndependentSourceValuesPWL::IndependentSourceValuesPWL(IndependentSource *source, QWidget *parent)
    : IndependentSourceValues(source, parent)
{
    QBoxLayout *topLayout = new QHBoxLayout(this, 0, KDialog::spacingHint());

    m_points = new EditListView(this);
    m_points->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, true);
    m_points->addColumn(i18n("Time"));
    m_points->addColumn(i18n("Value"));
    m_points->setColumnAlignment(0, Qt::AlignRight);
    m_points->setColumnAlignment(1, Qt::AlignRight);
    topLayout->addWidget(m_points);

    QBoxLayout *naviLayout = new QVBoxLayout(topLayout, KDialog::spacingHint());
    naviLayout->addStretch();

    QGridLayout *inputGrid = new QGridLayout(naviLayout, 2, 3, KDialog::spacingHint());
    inputGrid->addWidget(new QLabel(i18n("Time:"), this), 0, 0);
    inputGrid->addWidget(m_time = new ParameterLineEdit("0", "0", this), 0, 1);
    inputGrid->addWidget(new QLabel(i18n("Seconds"), this), 0, 2);
    inputGrid->addWidget(new QLabel(i18n("Value:"), this), 1, 0);
    inputGrid->addWidget(m_value = new ParameterLineEdit("0", "0", this), 1, 1);
    inputGrid->addWidget(new QLabel(m_source->unit(), this), 1, 2);

    EditListViewControlPanel *panel = new EditListViewControlPanel(m_points, this);
    connect(panel, SIGNAL(insertClicked()), SLOT(insertPoint()));
    naviLayout->addWidget(panel);
}

void IndependentSourceValuesPWL::loadParameters()
{
    const StringTable table = m_source->parameterToStringTable("points");

    QListViewItem *item = 0;
    for (size_t row = 0; row < table.count(); ++row)
    {
        if (item)
            item = new QListViewItem(m_points, item, table[row][0], table[row][1]);
        else
            item = new QListViewItem(m_points, table[row][0], table[row][1]);
    }
}

bool IndependentSourceValuesPWL::saveParameters() const
{
    m_source->setParameter("type", "pwl");

    StringTable table;
    for (QListViewItem *item = m_points->firstChild(); item; item = item->nextSibling())
    {
        StringTableRow row;
        row.append(item->text(0));
        row.append(item->text(1));
        table.append(row);
    }
    m_source->setParameter("points", table);

    return true;
}

void IndependentSourceValuesPWL::insertPoint()
{
    m_points->arrangeNewItem(new QListViewItem(m_points, m_time->text(), m_value->text()));
}

//
// IndependentSourceValuesSFFM
//

IndependentSourceValuesSFFM::IndependentSourceValuesSFFM(IndependentSource *source, QWidget *parent)
    : IndependentSourceValues(source, parent)
{
    QBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
    QGridLayout *grid = new QGridLayout(topLayout, 5, 3, KDialog::spacingHint());

    grid->addWidget(new QLabel(i18n("Offset:"), this), 0, 0);
    grid->addWidget(m_vo = new ParameterLineEdit("0", "0", this), 0, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 0, 2);

    grid->addWidget(new QLabel(i18n("Amplitude:"), this), 1, 0);
    grid->addWidget(m_va = new ParameterLineEdit("0", "0", this), 1, 1);
    grid->addWidget(new QLabel(m_source->unit(), this), 1, 2);

    grid->addWidget(new QLabel(i18n("Carrier Frequency:"), this), 2, 0);
    grid->addWidget(m_fc = new ParameterLineEdit("0", "0", this), 2, 1);
    grid->addWidget(new QLabel(i18n("Herz"), this), 2, 2);

    grid->addWidget(new QLabel(i18n("Modulation Index:"), this), 3, 0);
    grid->addWidget(m_mdi = new ParameterLineEdit("0", "0", this), 3, 1);

    grid->addWidget(new QLabel(i18n("Signal Frequency:"), this), 4, 0);
    grid->addWidget(m_fs = new ParameterLineEdit("0", "0", this), 4, 1);
    grid->addWidget(new QLabel(i18n("Herz"), this), 4, 2);

    topLayout->addStretch();
}

void IndependentSourceValuesSFFM::loadParameters()
{
    m_vo->setText(m_source->parameter("vo"));
    m_va->setText(m_source->parameter("va"));
    m_fc->setText(m_source->parameter("fc"));
    m_mdi->setText(m_source->parameter("mdi"));
    m_fs->setText(m_source->parameter("fs"));
}

bool IndependentSourceValuesSFFM::saveParameters() const
{
    m_source->setParameter("type", "sffm");
    m_source->setParameter("vo", m_vo->text());
    m_source->setParameter("va", m_va->text());
    m_source->setParameter("fc", m_fc->text());
    m_source->setParameter("mdi", m_mdi->text());
    m_source->setParameter("fs", m_fs->text());

    return true;
}

//
// IndependentSourcePropertiesWidget
//

IndependentSourcePropertiesWidget::IndependentSourcePropertiesWidget(IndependentSource *source, QWidget *parent)
    : SchematicDevicePropertiesWidget(parent), m_source(source)
{
    QBoxLayout *layout = new QVBoxLayout(this, 0, KDialog::spacingHint());

    QGridLayout *generalGrid = new QGridLayout(layout, 2, 2, KDialog::spacingHint());
    generalGrid->addWidget(new QLabel(i18n("Name:"), this), 0, 0);
    generalGrid->addWidget(m_name = new ParameterLineEdit(m_source->name(), this, ParameterLineEdit::String), 0, 1);
    generalGrid->addWidget(new QLabel(i18n("Type:"), this), 1, 0);
    generalGrid->addWidget(m_type = new QComboBox(this), 1, 1);

    m_type->insertItem(i18n("DC"), DC);
    m_type->insertItem(i18n("Pulse"), Pulse);
    m_type->insertItem(i18n("Sinusoidal"), Sin);
    m_type->insertItem(i18n("Exponential"), Exp);
    m_type->insertItem(i18n("Piece-Wise Linear"), PWL);
    m_type->insertItem(i18n("Single-Frequency FM"), SFFM);

    GroupBox *allBox = new GroupBox(1, Qt::Horizontal, i18n("All Analyses"), this);
    allBox->layout()->setMargin(KDialog::marginHint());
    allBox->layout()->setSpacing(KDialog::spacingHint());
    m_stack = new QWidgetStack(allBox);
    m_stack->connect(m_type, SIGNAL(activated(int)), SLOT(raiseWidget(int)));
    m_stack->addWidget(new IndependentSourceValuesDC(m_source, m_stack), DC);
    m_stack->addWidget(new IndependentSourceValuesPulse(m_source, m_stack), Pulse);
    m_stack->addWidget(new IndependentSourceValuesSin(m_source, m_stack), Sin);
    m_stack->addWidget(new IndependentSourceValuesExp(m_source, m_stack), Exp);
    m_stack->addWidget(new IndependentSourceValuesPWL(m_source, m_stack), PWL);
    m_stack->addWidget(new IndependentSourceValuesSFFM(m_source, m_stack), SFFM);
    layout->addWidget(allBox);

    if (m_source->parameter("type") == "dc")
        m_type->setCurrentItem(DC);
    else if (m_source->parameter("type") == "pulse")
        m_type->setCurrentItem(Pulse);
    else if (m_source->parameter("type") == "sin")
        m_type->setCurrentItem(Sin);
    else if (m_source->parameter("type") == "exp")
        m_type->setCurrentItem(Exp);
    else if (m_source->parameter("type") == "pwl")
        m_type->setCurrentItem(PWL);
    else if (m_source->parameter("type") == "sffm")
        m_type->setCurrentItem(SFFM);

    m_stack->raiseWidget(m_type->currentItem());
    static_cast<IndependentSourceValues *>(m_stack->visibleWidget())->loadParameters();

    GroupBox *acBox = new GroupBox(5, Qt::Horizontal, i18n("AC Analysis"), this);
    acBox->layout()->setMargin(KDialog::marginHint());
    acBox->layout()->setSpacing(KDialog::spacingHint());
    new QLabel(i18n("AC Value:"), acBox);
    m_acmag = new ParameterLineEdit(m_source->parameter("acmag"), "0", acBox);
    new QLabel(m_source->unit(), acBox);
    m_acphase = new ParameterLineEdit(m_source->parameter("acphase"), "0", acBox);
    new QLabel(i18n("Degrees"), acBox);
    layout->addWidget(acBox);

    GroupBox *distoBox = new GroupBox(5, Qt::Horizontal, i18n("Distortion Analysis"), this);
    distoBox->layout()->setMargin(KDialog::marginHint());
    distoBox->layout()->setSpacing(KDialog::spacingHint());
    new QLabel(i18n("1. Frequency Value:"), distoBox);
    m_distof1mag = new ParameterLineEdit(m_source->parameter("distof1mag"), distoBox);
    new QLabel(m_source->unit(), distoBox);
    m_distof1phase = new ParameterLineEdit(m_source->parameter("distof1phase"), distoBox);
    new QLabel(i18n("Degrees"), distoBox);
    new QLabel(i18n("2. Frequency Value:"), distoBox);
    m_distof2mag = new ParameterLineEdit(m_source->parameter("distof2mag"), distoBox);
    new QLabel(m_source->unit(), distoBox);
    m_distof2phase = new ParameterLineEdit(m_source->parameter("distof2phase"), distoBox);
    new QLabel(i18n("Degrees"), distoBox);
    layout->addWidget(distoBox);

    m_name->setFocus();
}

bool IndependentSourcePropertiesWidget::apply()
{
    m_source->clearParameters();
    QString name = m_name->text();

    if (name.lower() != m_source->name().lower() && m_source->schematic()->findDevice(name))
    {
        setErrorString(i18n("Device name already exists"));
        return false;
    }

    if (name.isEmpty())
    {
        setErrorString(i18n("No name specified"));
        return false;
    }

    IndependentSourceValues *values = static_cast<IndependentSourceValues *>(m_stack->visibleWidget());
    if (!values->saveParameters())
    {
        setErrorString(values->errorString());
        return false;
    }

    m_source->setName(name);
    m_source->setParameter("acmag", m_acmag->text());
    m_source->setParameter("acphase", m_acphase->text());
    m_source->setParameter("distof1mag", m_distof1mag->text());
    m_source->setParameter("distof1phase", m_distof1phase->text());
    m_source->setParameter("distof2mag", m_distof2mag->text());
    m_source->setParameter("distof2phase", m_distof2phase->text());

    m_source->updateLabels();

    return true;
}

#include "independentsource.moc"

// vim: ts=4 sw=4 et
