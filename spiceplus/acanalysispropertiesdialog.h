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

#ifndef ACANALYSISPROPERTIESDIALOG_H
#define ACANALYSISPROPERTIESDIALOG_H

#include <kdialogbase.h>

class QWidget;
class QRadioButton;

namespace Spiceplus {

class SchematicView;
class ParameterLineEdit;
class MeterSelector;

class ACAnalysisPropertiesDialog : public KDialogBase
{
    Q_OBJECT

public:
    ACAnalysisPropertiesDialog(SchematicView *view, QWidget *parent = 0);

private slots:
    void slotUser1();

private:
    SchematicView *m_view;

    QRadioButton *m_bodeButton;
    QRadioButton *m_nyquistButton;
    QRadioButton *m_linearMagnitudeButton;

    ParameterLineEdit *m_startFrequency;
    ParameterLineEdit *m_stopFrequency;

    MeterSelector *m_meterSelector;
};

} // namespace Spiceplus

#endif // ACANALYSISPROPERTIESDIALOG_H

// vim: ts=4 sw=4 et
