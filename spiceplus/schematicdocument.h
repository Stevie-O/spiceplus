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

#ifndef SCHEMATICDOCUMENT_H
#define SCHEMATICDOCUMENT_H

#include "document.h"

namespace Spiceplus {

class SchematicView;
class DeviceWindow;

class SchematicDocument : public Document
{
    Q_OBJECT

public:
    SchematicDocument(QWidgetStack *toolWindowStack, QWidget *parent = 0, const char *name = 0);
    ~SchematicDocument();

    QString type() const { return "schematic"; }

    void openNew(const KURL &url);
    bool open(const KURL &url);

    bool isModified() const;

    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    bool isDeleteAvailable() const;
    bool isSelectAllAvailable() const;
    bool isRotateAvailable() const;
    bool isFlipAvailable() const;

    bool isZoomInAvailable() const;
    bool isZoomOutAvailable() const;

    void updateState();

public slots:
    bool save(); 
    bool saveAs(); 

    void undo();
    void redo();
    void deleteSelected();
    void selectAll();
    void rotateCCW();
    void rotateCW();
    void flipHorizontal();
    void flipVertical();

    void zoomIn();
    void zoomOut();
    void zoom(int zoomFactorIndex);

    void analysisDC();
    void analysisAC();

    void toolSelect();
    void toolPlaceWire();
    void toolSplitWire();
    void toolPlaceGround();
    void toolPlaceTestPoint();
    void toolPlaceAmmeter();

private slots:
    void placeDevice();

private:
    void connectToolSignals();

    SchematicView *m_view;
    DeviceWindow *m_deviceWindow;
};

} // namespace Spiceplus

#endif // SCHEMATICDOCUMENT_H

// vim: ts=4 sw=4 et
