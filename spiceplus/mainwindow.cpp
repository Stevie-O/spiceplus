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

#include <qdir.h>
#include <qwidgetstack.h>

#include <kapplication.h>
#include <kaction.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstdaccel.h>
#include <kstatusbar.h>
#include <kdebug.h>
#include <kconfigdialog.h>

#include "mainwindow.h"
#include "schematicdocument.h"
#include "schematicview.h"
#include "modeldocument.h"
#include "signalmultiplexer.h"
#include "modelviewselector.h"
#include "modelfile.h"
#include "projectmanager.h"
#include "project.h"
#include "configdialog.h"

using namespace Spiceplus;

MainWindow *MainWindow::s_self = 0;

MainWindow::MainWindow()
    : KMdiMainFrm(0, 0, KMdi::TabPageMode)
{
    if (s_self)
        kdError() << "Multiple instances of MainWindow created." << endl;

    s_self = this;

    QRect geometry(0, 0, 800, 600);
    kapp->config()->setGroup("MainWindow");
    setGeometry(kapp->config()->readRectEntry("Main:Geometry", &geometry));

    connect(this, SIGNAL(viewActivated(KMdiChildView *)), SLOT(changeDocument(KMdiChildView *)));
    connect(this, SIGNAL(lastChildViewClosed()), SLOT(lastDocumentClosed()));

    m_mux = new SignalMultiplexer(this);
    m_mux->connect(SIGNAL(modified(bool)), this, SLOT(updateCaption(bool)));

    setXMLFile("spiceplusui.rc");

    KAction *action;

    action = new KAction(i18n("&New Schematic"), "filenew", KStdAccel::shortcut(KStdAccel::New), this, SLOT(fileNewSchematic()), actionCollection(), "file_new_schematic");
    action = new KAction(i18n("New &Model..."), "filenew", 0, this, SLOT(fileNewModel()), actionCollection(), "file_new_model");

    KStdAction::open(this, SLOT(fileOpen()), actionCollection());

    action = KStdAction::save(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(save()));
    m_mux->connect(SIGNAL(modified(bool)), action, SLOT(setEnabled(bool)));

    action = KStdAction::saveAs(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(saveAs()));

    action = KStdAction::close(this, SLOT(closeActiveView()), actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));

    KStdAction::quit(this, SLOT(close()), actionCollection());

    action = KStdAction::undo(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(undo()));
    m_mux->connect(SIGNAL(undoAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = KStdAction::redo(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(redo()));
    m_mux->connect(SIGNAL(redoAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("D&elete"), "editdelete", Qt::Key_Delete, 0, 0, actionCollection(), "edit_delete");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(deleteSelected()));
    m_mux->connect(SIGNAL(deleteAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = KStdAction::selectAll(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(selectAll()));
    m_mux->connect(SIGNAL(selectAllAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("Rotate Counter Clock Wise"), 0, Qt::Key_R, 0, 0, actionCollection(), "edit_rotate_ccw");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(rotateCCW()));
    m_mux->connect(SIGNAL(rotateAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("Rotate Clock Wise"), 0, Qt::SHIFT + Qt::Key_R, 0, 0, actionCollection(), "edit_rotate_cw");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(rotateCW()));
    m_mux->connect(SIGNAL(rotateAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("Flip Horizontal"), 0, Qt::Key_F, 0, 0, actionCollection(), "edit_flip_horizontal");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(flipHorizontal()));
    m_mux->connect(SIGNAL(flipAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("Flip Vertical"), 0, Qt::SHIFT + Qt::Key_F, 0, 0, actionCollection(), "edit_flip_vertical");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(flipVertical()));
    m_mux->connect(SIGNAL(flipAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = KStdAction::zoomIn(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(zoomIn()));
    m_mux->connect(SIGNAL(zoomInAvailable(bool)), action, SLOT(setEnabled(bool)));

    action = KStdAction::zoomOut(0, 0, actionCollection());
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(zoomOut()));
    m_mux->connect(SIGNAL(zoomOutAvailable(bool)), action, SLOT(setEnabled(bool)));

    KSelectAction *zoomAction = new KSelectAction(i18n("&Zoom"), 0, actionCollection(), "view_zoom");

    QStringList zoomFactors;
    for (int i = 0; SchematicView::zoomFactors()[i] != -1; ++i)
        zoomFactors << QString::number(SchematicView::zoomFactors()[i]) + "%";

    zoomAction->setItems(zoomFactors);
    zoomAction->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), zoomAction, SLOT(setEnabled(bool)));
    m_mux->connect(zoomAction, SIGNAL(activated(int)), SLOT(zoom(int)));
    m_mux->connect(SIGNAL(zoomAvailable(bool)), zoomAction, SLOT(setEnabled(bool)));
    m_mux->connect(SIGNAL(zoomChanged(int)), zoomAction, SLOT(setCurrentItem(int)));

    new KAction(i18n("&New Project..."), "window_new", 0, ProjectManager::self(), SLOT(newProject()), actionCollection(), "project_new");
    new KAction(i18n("&Open Project..."), "project_open", 0, ProjectManager::self(), SLOT(openProject()), actionCollection(), "project_open");
    action = new KAction(i18n("&Close Project"), "fileclose", 0, ProjectManager::self(), SLOT(closeProject()), actionCollection(), "project_close");
    action->setEnabled(false);
    connect(Project::self(), SIGNAL(opened(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("&DC Analysis"), 0, 0, 0, 0, actionCollection(), "analysis_dc");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(analysisDC()));
    m_mux->connect(SIGNAL(analysisEnabled(bool)), action, SLOT(setEnabled(bool)));

    action = new KAction(i18n("&AC Analysis"), 0, 0, 0, 0, actionCollection(), "analysis_ac");
    action->setEnabled(false);
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(action, SIGNAL(activated()), SLOT(analysisAC()));
    m_mux->connect(SIGNAL(analysisEnabled(bool)), action, SLOT(setEnabled(bool)));

    createToolAction(i18n("&Select"), "spiceplus_select", "tool_select",
                     SLOT(toolSelect()), SIGNAL(toolSelectChecked(bool)));

    createToolAction(i18n("Place &Wire"), "spiceplus_place_wire", "tool_place_wire",
                     SLOT(toolPlaceWire()), SIGNAL(toolPlaceWireChecked(bool)));

    createToolAction(i18n("S&plit Wire"), "spiceplus_split_wire", "tool_split_wire",
                     SLOT(toolSplitWire()), SIGNAL(toolSplitWireChecked(bool)));

    createToolAction(i18n("Place &Ground"), "spiceplus_place_ground", "tool_place_ground",
                     SLOT(toolPlaceGround()), SIGNAL(toolPlaceGroundChecked(bool)));

    createToolAction(i18n("Place &Test Point"), "spiceplus_place_test_point", "tool_place_test_point",
                     SLOT(toolPlaceTestPoint()), SIGNAL(toolPlaceTestPointChecked(bool)));

    createToolAction(i18n("Place &Ammeter"), "spiceplus_place_ammeter", "tool_place_ammeter",
                     SLOT(toolPlaceAmmeter()), SIGNAL(toolPlaceAmmeterChecked(bool)));

    action = new KAction(i18n("Show &Tool Window"), 0, 0, 0, 0, actionCollection(), "window_show_tool_window");
    connect(action, SIGNAL(activated()), SLOT(showToolWindow()));

    KStdAction::preferences(this, SLOT(showSettings()), actionCollection());

    setStandardToolBarMenuEnabled(true);
    createStandardStatusBarAction();
    statusBar()->insertItem("", 0, 1);
    statusBar()->setItemAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);

    createGUI(0);
    setAutoSaveSettings("MainWindow", false);
    m_pTaskBar->switchOn(false);
    m_toolWindowStack = new QWidgetStack(this, "Tool Window");
    addToolWindow(m_toolWindowStack, KDockWidget::DockLeft, getMainDockWidget(), 20, i18n("Tool Window"), i18n("Tool Window"));
    readDockConfig(kapp->config(), "MainWindow");
}

MainWindow::~MainWindow()
{
    if (Project::self()->isOpen())
        ProjectManager::self()->saveProject();

    closeAllWindows();

    writeDockConfig(kapp->config(), "MainWindow");
}

Document *MainWindow::openDocument(const KURL &url)
{
    Document *doc = findDocument(url);
    if (doc)
        activateDocument(doc);
    else
    {
        QString ext = url.fileName().section('.', -1);
        if (ext == "schematic")
        {
            SchematicDocument *sdoc = new SchematicDocument(m_toolWindowStack, this);
            if (sdoc->open(url))
            {
                addWindow(sdoc);
                activateView(sdoc);
                doc = sdoc;
            }
            else
                delete sdoc;
        }
        else if (ext == "model")
        {
            ModelFile model;
            if (model.load(url))
            {
                ModelViewFactory *factory = PluginManager::self()->modelViewFactory(model.deviceType());
                if (factory)
                {
                    ModelDocument *mdoc = new ModelDocument(factory, m_toolWindowStack, this);
                    mdoc->open(model, url);
                    addWindow(mdoc);
                    activateView(mdoc);
                    doc = mdoc;
                }
                else
                    KMessageBox::error(this, PluginManager::self()->errorString());
            }
            else
                KMessageBox::error(this, model.errorString());
        }
        else
            KMessageBox::sorry(this, i18n("Unknown file extension"));
    }

    return doc;
}

void MainWindow::activateDocument(Document *doc)
{
    doc->setFocus();
    changeDocument(doc);
}

bool MainWindow::saveAllDocuments()
{
    for (QPtrList<KMdiChildView>::Iterator it = m_pDocumentViews->begin(); it != m_pDocumentViews->end(); ++it)
        if (!saveDocument(static_cast<Document *>(*it)))
            return false;

    return true;
}

void MainWindow::closeAllWindows()
{
    while (m_pCurrentWindow)
        closeWindow(m_pCurrentWindow);
}

void MainWindow::childWindowCloseRequest(KMdiChildView *childView)
{
    if (saveDocument(static_cast<Document *>(childView)))
        KMdiMainFrm::childWindowCloseRequest(childView);
}

bool MainWindow::queryClose()
{
    return saveAllDocuments();
}

bool MainWindow::saveDocument(Document *doc)
{
    if (doc->isModified())
    {
        int code = KMessageBox::warningYesNoCancel(this,
                   i18n("The document \"%1\" has been modified.\nDo you want to save it?").arg(doc->fileName(true)),
                   i18n("Save Document?"), KStdGuiItem::save(), KStdGuiItem::discard());

        if (code == KMessageBox::Cancel || code == KMessageBox::Yes && !doc->save())
            return false;
    }

    return true;
}

void MainWindow::createToolAction(const QString &text, const QString &pix, const char *name, const char *slotActivated, const char *signalChecked) const
{
    KRadioAction *action = new KRadioAction(text, pix, 0, 0, 0, actionCollection(), name);
    action->setExclusiveGroup("tools");
    action->setChecked(false);
    action->setEnabled(false);

    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(guiEnabled(bool)), action, SLOT(setChecked(bool)));
    m_mux->connect(action, SIGNAL(activated()), slotActivated);
    m_mux->connect(signalChecked, action, SLOT(setChecked(bool)));
    m_mux->connect(SIGNAL(toolsEnabled(bool)), action, SLOT(setEnabled(bool)));
    m_mux->connect(SIGNAL(toolsChecked(bool)), action, SLOT(setChecked(bool)));
}

void MainWindow::changeDocument(KMdiChildView *childView)
{
    Document *doc = static_cast<Document *>(childView);

    setCaption(doc->fileName(), doc->isModified());
    m_mux->setCurrentObject(doc);

    action(KStdAction::name(KStdAction::Close))->setEnabled(true);
    action(KStdAction::name(KStdAction::SaveAs))->setEnabled(true);
}

void MainWindow::lastDocumentClosed()
{
    emit guiEnabled(false);
    setCaption("");
}

void MainWindow::updateCaption(bool modified)
{
    setCaption(static_cast<SchematicDocument *>(activeWindow())->fileName(), modified);
}

void MainWindow::fileNewSchematic()
{
    SchematicDocument *doc = new SchematicDocument(m_toolWindowStack, this);

    int i = 1;
    KURL url(QDir::homeDirPath());

    do
        url.addPath(i18n("Schematic%1.schematic").arg(i++));
    while (findDocument(url));

    doc->openNew(url);
    doc->setIsBrandNew(true);
    addWindow(doc);
    activateView(doc);
}

void MainWindow::fileNewModel()
{
    ModelViewSelector *dlg = new ModelViewSelector(this);

    if (dlg->exec() == QDialog::Accepted)
    {
        QString deviceType = dlg->deviceType();
        if (!deviceType.isNull())
        {
            ModelViewFactory *factory = PluginManager::self()->modelViewFactory(deviceType);
            if (factory)
            {
                ModelDocument *doc = new ModelDocument(factory, m_toolWindowStack, this);

                int i = 1;
                KURL url(QDir::homeDirPath());

                do
                    url.addPath(i18n("Model%1.model").arg(i++));
                while (findDocument(url));

                doc->openNew(url);
                doc->setIsBrandNew(true);
                addWindow(doc);
                activateView(doc);
            }
            else
                KMessageBox::error(this, PluginManager::self()->errorString());
        }
    }

    delete dlg;
}

void MainWindow::fileOpen()
{
    KURL url = KFileDialog::getOpenURL(":document",
                                       QString("*.schematic *.model|%1\n*.schematic|%2\n*.model|%3")
                                              .arg(i18n("All Supported Files"))
                                              .arg(i18n("Schematic File"))
                                              .arg(i18n("Model File")),
                                       this, i18n("Open File"));

    if (!url.isEmpty())
        openDocument(url);
}

Document *MainWindow::findDocument(const KURL &url) const
{
    for (QPtrList<KMdiChildView>::Iterator it = m_pDocumentViews->begin(); it != m_pDocumentViews->end(); ++it)
    {
        Document *doc = static_cast<Document *>(*it);
        if (doc->fileURL() == url)
            return doc;
    }

    return 0;
}

void MainWindow::showToolWindow()
{
    if (!m_toolWindowStack->isVisible() && m_pToolViews->contains(m_toolWindowStack))
        (*m_pToolViews)[m_toolWindowStack]->place(KDockWidget::DockLeft, getMainDockWidget(), 20);
}

void MainWindow::showSettings()
{
    if (KConfigDialog::showDialog("settings"))
        return;

    ConfigDialog *dlg = new ConfigDialog(this, "settings");
    dlg->show();
}

#include "mainwindow.moc"

// vim: ts=4 sw=4 et
