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

#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <qgroupbox.h>
#include <qlayout.h>

#include <kdialog.h>

namespace Spiceplus {

class GroupBox : public QGroupBox
{
public:
    inline GroupBox(QWidget *parent = 0);
    inline GroupBox(const QString &title, QWidget *parent = 0);
    inline GroupBox(int strips, Orientation orientation, QWidget *parent = 0);
    inline GroupBox(int strips, Orientation orientation, const QString &title, QWidget *parent = 0);
};

inline GroupBox::GroupBox(QWidget *parent)
    : QGroupBox(parent)
{
    layout()->setMargin(KDialog::marginHint());
    layout()->setSpacing(KDialog::spacingHint());
}

inline GroupBox::GroupBox(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
{
    layout()->setMargin(KDialog::marginHint());
    layout()->setSpacing(KDialog::spacingHint());
}

inline GroupBox::GroupBox(int strips, Orientation orientation, QWidget *parent)
    : QGroupBox(strips, orientation, parent)
{
    layout()->setMargin(KDialog::marginHint());
    layout()->setSpacing(KDialog::spacingHint());
}

inline GroupBox::GroupBox(int strips, Orientation orientation, const QString &title, QWidget *parent)
    : QGroupBox(strips, orientation, title, parent)
{
    layout()->setMargin(KDialog::marginHint());
    layout()->setSpacing(KDialog::spacingHint());
}

} // namespace Spiceplus

#endif // GROUPBOX_H

// vim: ts=4 sw=4 et
