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

#include <qvalidator.h>
#include <qregexp.h>

#include "parameterlineedit.h"

using namespace Spiceplus;

ParameterLineEdit::ParameterLineEdit(QWidget *parent, Type type)
    : QLineEdit(parent), m_type(type)
{
    init();
}

ParameterLineEdit::ParameterLineEdit(const QString &contents, QWidget *parent, Type type)
    : QLineEdit(contents, parent), m_type(type)
{
    init();
}

ParameterLineEdit::ParameterLineEdit(const QString &contents, const QString &defaultValue, QWidget *parent, Type type)
    : QLineEdit(contents, parent), m_type(type), m_defaultValue(defaultValue)
{
    init();
}

QValidator *ParameterLineEdit::createValidator(Type type, QObject *parent)
{
    if (type == Number)
        return new QRegExpValidator(QRegExp("(-?\\d+(\\.\\d+)?([eE]-?\\d+)?([a-z]|[A-Z])*)|"), parent);
    else
        return new QRegExpValidator(QRegExp("(([a-z]|[A-Z])([a-z]|[A-Z]|[0-9])*)|"), parent);
}

void ParameterLineEdit::init()
{
    setValidator(createValidator(m_type, this));
    connect(this, SIGNAL(lostFocus()), SLOT(validateText()));
}

void ParameterLineEdit::validateText()
{
    blockSignals(true);
    setText(fixupText());
    blockSignals(false);
}

QString ParameterLineEdit::fixupText() const
{
    QString t = QLineEdit::text();
    int pos = 0;

    while (validator()->validate(t, pos) != QValidator::Acceptable)
        t.truncate(t.length() - 1);

    if (t.isEmpty() && !m_defaultValue.isNull())
        t = m_defaultValue;

    return t;
}

#include "parameterlineedit.moc"

// vim: ts=4 sw=4 et
