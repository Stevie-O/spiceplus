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

#ifndef PARAMETERLINEEDIT_H
#define PARAMETERLINEEDIT_H

#include <qlineedit.h>
#include <qvalidator.h>

namespace Spiceplus {

class ParameterLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    enum Type { Number, String };

    ParameterLineEdit(QWidget *parent, Type type = Number);
    ParameterLineEdit(const QString &contents, QWidget *parent, Type type = Number);
    ParameterLineEdit(const QString &contents, const QString &defaultValue, QWidget *parent, Type type = Number);

    QString text() const { return fixupText(); }

    static QValidator *createValidator(Type type, QObject *parent);

private slots:
    void validateText();

private:
    void init();
    QString fixupText() const;

    Type m_type;
    QString m_defaultValue;
};

} // namespace Spiceplus

#endif // PARAMETERLINEEDIT_H

// vim: ts=4 sw=4 et
