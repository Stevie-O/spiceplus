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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <kmdichildview.h>
#include <kurl.h>

class QWidgetStack;

namespace Spiceplus {

class Document : public KMdiChildView
{
    Q_OBJECT

public:
    Document(QWidgetStack *toolWindowStack, QWidget *parent = 0, const char *name = 0);

    virtual QString type() const = 0;

    QString fileName(bool withExtension = false);
    KURL fileURL() const { return m_fileURL; }

    bool isBrandNew() const { return m_isBrandNew; }
    void setIsBrandNew(bool isBrandNew) { m_isBrandNew = isBrandNew; }

    virtual bool isModified() const = 0;

    virtual bool isUndoAvailable() const { return false; }
    virtual bool isRedoAvailable() const { return false; }
    virtual bool isDeleteAvailable() const { return false; }
    virtual bool isSelectAllAvailable() const { return false; }
    virtual bool isRotateAvailable() const { return false; }
    virtual bool isFlipAvailable() const { return false; }

    virtual bool isZoomInAvailable() const { return false; }
    virtual bool isZoomOutAvailable() const { return false; }

    virtual void updateState() = 0;

public slots:
    virtual bool save() = 0;
    virtual bool saveAs() = 0; 

    virtual void undo() {}
    virtual void redo() {}
    virtual void deleteSelected() {}
    virtual void selectAll() {}
    virtual void rotateCCW() {}
    virtual void rotateCW() {}
    virtual void flipHorizontal() {}
    virtual void flipVertical() {}

    virtual void zoomIn() {}
    virtual void zoomOut() {}
    virtual void zoom(int) {}

    virtual void analysisDC() {}
    virtual void analysisAC() {}

    virtual void toolSelect() {}
    virtual void toolPlaceWire() {}
    virtual void toolSplitWire() {}
    virtual void toolPlaceGround() {}
    virtual void toolPlaceTestPoint() {}
    virtual void toolPlaceAmmeter() {}

signals:
    void modified(bool yes);

    void undoAvailable(bool yes);
    void redoAvailable(bool yes);
    void deleteAvailable(bool yes);
    void selectAllAvailable(bool yes);
    void rotateAvailable(bool yes);
    void flipAvailable(bool yes);

    void zoomInAvailable(bool yes);
    void zoomOutAvailable(bool yes);
    void zoomAvailable(bool yes);
    void zoomChanged(int zoomFactorIndex);

    void analysisEnabled(bool enabled);

    void toolsChecked(bool checked);
    void toolsEnabled(bool enabled);

    void toolSelectChecked(bool checked);
    void toolPlaceWireChecked(bool checked);
    void toolSplitWireChecked(bool checked);
    void toolPlaceGroundChecked(bool checked);
    void toolPlaceTestPointChecked(bool checked);
    void toolPlaceAmmeterChecked(bool checked);

protected:
    bool isFileNewOrCanOverwrite(const KURL &url);

    QWidgetStack *m_toolWindowStack;
    KURL m_fileURL;
    bool m_isBrandNew;
};

} // namespace Spiceplus

#endif // DOCUMENT_H

// vim: ts=4 sw=4 et
