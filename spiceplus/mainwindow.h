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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kmdimainfrm.h>

class QWidgetStack;

namespace Spiceplus {

class Document;
class SignalMultiplexer;

class MainWindow : public KMdiMainFrm
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    static MainWindow *self() { return s_self; }
    Document *openDocument(const KURL &url);
    void activateDocument(Document *doc);
    bool saveAllDocuments();
    void closeAllWindows();

signals:
    void guiEnabled(bool enabled);

public slots:
    void childWindowCloseRequest(KMdiChildView *childView);

protected:
    bool queryClose();

private slots:
    void changeDocument(KMdiChildView *childView);
    void lastDocumentClosed();
    void updateCaption(bool modified);

    void fileNewSchematic();
    void fileNewModel();
    void fileOpen();

    void showToolWindow();
    void showSettings();

private:
    static MainWindow *s_self;

    bool saveDocument(Document *doc);
    void createToolAction(const QString &text, const QString &pix, const char *name, const char *slotActivated, const char *signalChecked) const;
    Document *findDocument(const KURL &url) const;

    QWidgetStack *m_toolWindowStack;
    SignalMultiplexer *m_mux;
};

} // namespace Spiceplus

#endif // MAINWINDOW_H

// vim: ts=4 sw=4 et
