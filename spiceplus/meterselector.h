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

#ifndef METERSELECTOR_H
#define METERSELECTOR_H

#include <qobject.h>

class QWidget;
class QGridLayout;
class QRadioButton;
class QComboBox;

namespace Spiceplus {

class Schematic;
class Meter;

class MeterSelector : public QObject
{
    Q_OBJECT

public:
    MeterSelector(Schematic *schematic, QWidget *parent, QGridLayout *layout, int startRow, int startCol);
    Meter meter() const;

private:
    QRadioButton *m_voltmeterButton;
    QComboBox *m_testPoint1;
    QComboBox *m_testPoint2;

    QRadioButton *m_ammeterButton;
    QComboBox *m_ammeter;
};

} // namespace Spiceplus

#endif // METERSELECTOR_H

// vim: ts=4 sw=4 et
