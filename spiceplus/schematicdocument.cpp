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

#include <qlayout.h>
#include <qwidgetstack.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kurl.h>
#include <kfiledialog.h>

#include "schematicdocument.h"
#include "devicewindow.h"
#include "schematicview.h"
#include "schematic.h"
#include "schematicdevice.h"
#include "schematictool.h"
#include "schematiccommandhistory.h"
#include "dcanalysispropertiesdialog.h"
#include "acanalysispropertiesdialog.h"

using namespace Spiceplus;

SchematicDocument::SchematicDocument(QWidgetStack *toolWindowStack, QWidget *parent, const char *name)
    : Document(toolWindowStack, parent, name)
{
    setIcon(SmallIcon("misc_doc"));

    QBoxLayout *layout = new QVBoxLayout(this);
    m_view = new SchematicView(this);
    layout->addWidget(m_view);

    Schematic *schem = new Schematic(m_view);
    // TODO: Make size dynamic
    schem->resize(4000, 4000);
    m_view->setSchematic(schem);

    connect(m_view->history(), SIGNAL(modified(bool)), SIGNAL(modified(bool)));
    connect(m_view->history(), SIGNAL(undoAvailable(bool)), SIGNAL(undoAvailable(bool)));
    connect(m_view->history(), SIGNAL(redoAvailable(bool)), SIGNAL(redoAvailable(bool)));
    connect(m_view, SIGNAL(zoomInAvailable(bool)), SIGNAL(zoomInAvailable(bool)));
    connect(m_view, SIGNAL(zoomOutAvailable(bool)), SIGNAL(zoomOutAvailable(bool)));
    connect(m_view, SIGNAL(zoomChanged(int)), SIGNAL(zoomChanged(int)));

    m_deviceWindow = new DeviceWindow(m_toolWindowStack);
    m_toolWindowStack->addWidget(m_deviceWindow);
    connect(m_deviceWindow, SIGNAL(deviceChanged()), SLOT(placeDevice()));

    toolSelect();
}

SchematicDocument::~SchematicDocument()
{
    m_toolWindowStack->removeWidget(m_deviceWindow);
    delete m_deviceWindow;
}

void SchematicDocument::openNew(const KURL &url)
{
    m_fileURL = url;
    setMDICaption(fileName());
}

bool SchematicDocument::open(const KURL &url)
{
    if (!m_view->schematic()->load(url))
    {
        KMessageBox::error(this, m_view->schematic()->errorString());
        return false;
    }

    m_fileURL = url;
    setMDICaption(fileName());

    return true;
}

bool SchematicDocument::save()
{
    m_view->resetTool();

    if (isBrandNew())
        return saveAs();

    if (m_view->schematic()->save(fileURL()))
        m_view->history()->resetModificationCounter();
    else
    {
        KMessageBox::error(this, m_view->schematic()->errorString());
        return false;
    }

    return true;
}

bool SchematicDocument::saveAs()
{
    m_view->resetTool();

    KURL url = KFileDialog::getSaveURL(isBrandNew() ? ":document" : fileURL().url(),
                                       "*.schematic|" + i18n("Schematic File"), this, i18n("Save Schematic"));

    if (url.isEmpty() || !isFileNewOrCanOverwrite(url))
        return false;

    if (m_view->schematic()->save(url))
    {
        m_fileURL = url;
        setMDICaption(fileName());
        m_isBrandNew = false;
        m_view->history()->resetModificationCounter();
    }
    else
    {
        KMessageBox::error(this, m_view->schematic()->errorString());
        return false;
    }

    return true;
}

bool SchematicDocument::isModified() const
{
    return m_view->history()->isModified();
}

bool SchematicDocument::isUndoAvailable() const
{
    return m_view->history()->isUndoAvailable();
}

bool SchematicDocument::isRedoAvailable() const
{
    return m_view->history()->isRedoAvailable();
}

bool SchematicDocument::isDeleteAvailable() const
{
    return m_view->tool()->isDeleteAvailable();
}

bool SchematicDocument::isSelectAllAvailable() const
{
    return m_view->tool()->isSelectAllAvailable();
}

bool SchematicDocument::isRotateAvailable() const
{
    return m_view->tool()->isRotateAvailable();
}

bool SchematicDocument::isFlipAvailable() const
{
    return m_view->tool()->isFlipAvailable();
}

bool SchematicDocument::isZoomInAvailable() const
{
    return m_view->isZoomInAvailable();
}

bool SchematicDocument::isZoomOutAvailable() const
{
    return m_view->isZoomOutAvailable();
}

void SchematicDocument::updateState()
{
    emit modified(isModified());

    emit undoAvailable(isUndoAvailable());
    emit redoAvailable(isRedoAvailable());
    emit deleteAvailable(isDeleteAvailable());
    emit selectAllAvailable(isSelectAllAvailable());
    emit rotateAvailable(isRotateAvailable());
    emit flipAvailable(isFlipAvailable());

    emit zoomInAvailable(isZoomInAvailable());
    emit zoomOutAvailable(isZoomOutAvailable());
    emit zoomAvailable(true);
    emit zoomChanged(m_view->zoomFactorIndex());

    emit analysisEnabled(true);
    emit toolsEnabled(true);

    if (m_view->tool()->rtti() != SchematicToolPlaceDevice::RTTI)
    {
        switch (m_view->tool()->rtti())
        {
        case SchematicToolSelect::RTTI:
            emit toolSelectChecked(true);
            break;
        case SchematicToolPlaceWire::RTTI:
            emit toolPlaceWireChecked(true);
            break;
        case SchematicToolSplitWire::RTTI:
            emit toolSplitWireChecked(true);
            break;
        case SchematicToolPlaceGround::RTTI:
            emit toolPlaceGroundChecked(true);
            break;
        case SchematicToolPlaceTestPoint::RTTI:
            emit toolPlaceTestPointChecked(true);
            break;
        case SchematicToolPlaceAmmeter::RTTI:
            emit toolPlaceAmmeterChecked(true);
            break;
        default:
            break;
        }
    }
    else
        emit toolsChecked(false);

    m_toolWindowStack->raiseWidget(m_deviceWindow);
}

void SchematicDocument::undo()
{
    m_view->undo();
}

void SchematicDocument::redo()
{
    m_view->redo();
}

void SchematicDocument::deleteSelected()
{
    m_view->tool()->deleteSelected();
}

void SchematicDocument::selectAll()
{
    m_view->tool()->selectAll();
}

void SchematicDocument::rotateCCW()
{
    m_view->tool()->rotateCCW();
}

void SchematicDocument::rotateCW()
{
    m_view->tool()->rotateCW();
}

void SchematicDocument::flipHorizontal()
{
    m_view->tool()->flipHorizontal();
}

void SchematicDocument::flipVertical()
{
    m_view->tool()->flipVertical();
}

void SchematicDocument::zoomIn()
{
    m_view->zoomIn();
}

void SchematicDocument::zoomOut()
{
    m_view->zoomOut();
}

void SchematicDocument::zoom(int zoomFactorIndex)
{
    m_view->zoom(zoomFactorIndex);
}

void SchematicDocument::analysisDC()
{
    if (!m_view->schematic()->canRunAnalysis())
    {
        KMessageBox::error(this, m_view->schematic()->errorString());
        return;
    }

    m_view->resetTool();

    DCAnalysisPropertiesDialog *dlg = new DCAnalysisPropertiesDialog(m_view, this);
    dlg->exec();
    delete dlg;
}

void SchematicDocument::analysisAC()
{
    if (!m_view->schematic()->canRunAnalysis())
    {
        KMessageBox::error(this, m_view->schematic()->errorString());
        return;
    }

    m_view->resetTool();

    ACAnalysisPropertiesDialog *dlg = new ACAnalysisPropertiesDialog(m_view, this);
    dlg->exec();
    delete dlg;
}

void SchematicDocument::toolSelect()
{
    new SchematicToolSelect(m_view);
    connectToolSignals();
    m_deviceWindow->unselectDevice();
}

void SchematicDocument::toolPlaceWire()
{
    new SchematicToolPlaceWire(m_view);
    m_deviceWindow->unselectDevice();
}

void SchematicDocument::toolSplitWire()
{
    new SchematicToolSplitWire(m_view);
    m_deviceWindow->unselectDevice();
}

void SchematicDocument::toolPlaceGround()
{
    new SchematicToolPlaceGround(m_view);
    connectToolSignals();
    m_deviceWindow->unselectDevice();
}

void SchematicDocument::toolPlaceTestPoint()
{
    new SchematicToolPlaceTestPoint(m_view);
    connectToolSignals();
    m_deviceWindow->unselectDevice();
}

void SchematicDocument::toolPlaceAmmeter()
{
    new SchematicToolPlaceAmmeter(m_view);
    connectToolSignals();
    m_deviceWindow->unselectDevice();
}

void SchematicDocument::placeDevice()
{
    if (m_deviceWindow->deviceSelector() && m_deviceWindow->deviceSelector()->isDeviceSelected())
    {
        new SchematicToolPlaceDevice(m_view, m_deviceWindow->deviceSelector());
        connectToolSignals();
        emit toolsChecked(false);
    }
    else
    {
        new SchematicToolSelect(m_view);
        connectToolSignals();
        emit toolSelectChecked(true);
    }
}

void SchematicDocument::connectToolSignals()
{
    connect(m_view->tool(), SIGNAL(deleteAvailable(bool)), SIGNAL(deleteAvailable(bool)));
    connect(m_view->tool(), SIGNAL(selectAllAvailable(bool)), SIGNAL(selectAllAvailable(bool)));
    connect(m_view->tool(), SIGNAL(rotateAvailable(bool)), SIGNAL(rotateAvailable(bool)));
    connect(m_view->tool(), SIGNAL(flipAvailable(bool)), SIGNAL(flipAvailable(bool)));
}

#include "schematicdocument.moc"

// vim: ts=4 sw=4 et
