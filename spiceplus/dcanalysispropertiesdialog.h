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

#ifndef DCANALYSISPROPERTIESDIALOG_H
#define DCANALYSISPROPERTIESDIALOG_H

#include <kdialogbase.h>

class QWidget;
class QComboBox;
class QCheckBox;

namespace Spiceplus {

class SchematicView;
class ParameterLineEdit;
class MeterSelector;

class DCAnalysisPropertiesDialog : public KDialogBase
{
    Q_OBJECT

public:
    DCAnalysisPropertiesDialog(SchematicView *view, QWidget *parent = 0);

private slots:
    void slotUser1();

private:
    SchematicView *m_view;

    QComboBox *m_sourceName;
    ParameterLineEdit *m_startingValue;
    ParameterLineEdit *m_finalValue;
    ParameterLineEdit *m_incrementingValue;
    QCheckBox *m_useSource2;
    QComboBox *m_sourceName2;
    ParameterLineEdit *m_startingValue2;
    ParameterLineEdit *m_finalValue2;
    ParameterLineEdit *m_incrementingValue2;

    MeterSelector *m_meterSelector;
};

} // namespace Spiceplus

#endif // DCANALYSISPROPERTIESDIALOG_H

// vim: ts=4 sw=4 et
