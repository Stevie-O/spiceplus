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

#ifndef CAPACITORMODELVIEW_H
#define CAPACITORMODELVIEW_H

#include "model.h"

class QWidget;
class QString;

namespace Spiceplus {

class ParameterLineEdit;

namespace Plugin {

class CapacitorModelViewFactory : public ModelViewFactory
{
    Q_OBJECT

public:
    ModelView *createModelView(Model &model, QWidget *parent = 0);
};

class CapacitorModelView : public ModelView
{
    Q_OBJECT

public:
    CapacitorModelView(Model &model, QWidget *parent = 0);

    void update();
    bool apply();

private slots:
    void slotTextChanged(const QString &);

private:
    Model &m_model;
    ParameterLineEdit *m_cj;
    ParameterLineEdit *m_cjsw;
    ParameterLineEdit *m_defw;
    ParameterLineEdit *m_narrow;
};

} // namespace Plugin
} // namespace Spiceplus

#endif // CAPACITORMODELVIEW_H

// vim: ts=4 sw=4 et
