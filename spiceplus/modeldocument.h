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

#ifndef MODELDOCUMENT_H
#define MODELDOCUMENT_H

#include "document.h"
#include "modelfile.h"

class KLineEdit;

namespace Spiceplus {

class ModelViewPlugin;
class ModelView;

class ModelDocument : public Document
{
    Q_OBJECT

public:
    ModelDocument(ModelViewFactory *factory, QWidgetStack *toolWindowStack, QWidget *parent = 0, const char *name = 0);
    ~ModelDocument();

    QString type() const { return "model"; }

    void openNew(const KURL &url);
    void open(const ModelFile &model, const KURL &url);

    bool isModified() const;
    void updateState();

public slots:
    bool save(); 
    bool saveAs();

private slots:
    void openSymbolDialog();
    void slotTextChanged(const QString &) { setIsModified(true); }
    void setIsModified(bool isModified);

private:
    QWidget *m_toolWindow;
    ModelFile m_model;
    ModelView *m_view;
    KLineEdit *m_alias;
    KLineEdit *m_symbolPath;
    bool m_isModified;
};

} // namespace Spiceplus

#endif // MODELDOCUMENT_H

// vim: ts=4 sw=4 et
