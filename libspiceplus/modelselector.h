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

#ifndef MODELSELECTOR_H
#define MODELSELECTOR_H

#include <kfiletreebranch.h>

#include "schematicdevice.h"

namespace Spiceplus {

class ModelSelectorTreeBranch : public KFileTreeBranch
{
public:
    ModelSelectorTreeBranch(const KURL &modelDir,
                            const QString &modelDirID,
                            const QString &deviceType,
                            KFileTreeView *view,
                            const KURL &startURL,
                            const QString &name,
                            const QPixmap &pix);

    KURL modelDir() const { return m_modelDir; }
    QString modelDirID() const { return m_modelDirID; }

protected:
    bool matchesFilter(const KFileItem *item) const;
    KFileTreeViewItem *createTreeViewItem(KFileTreeViewItem *parent, KFileItem *fileItem);

private:
    KURL m_modelDir;
    QString m_modelDirID;
    QString m_deviceType;
};

class ModelSelector : public SchematicDeviceSelector
{
    Q_OBJECT

public:
    ModelSelector(const QString &deviceType, QWidget *parent = 0);
    virtual ~ModelSelector() {}

    bool isModelSelected() const;
    KURL modelURL() const;
    QString modelPath() const;

    QString errorString() const { return m_errorString; }

public slots:
    bool setupTree();

private slots:
    void slotSelectionChanged();

private:
    QString m_deviceType;
    KFileTreeView *m_tree;
    QString m_errorString;
};

} // namespace Spiceplus

#endif // MODELSELECTOR_H

// vim: ts=4 sw=4 et
