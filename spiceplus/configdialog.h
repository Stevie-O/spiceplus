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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <kconfigdialog.h>

namespace Spiceplus {

class ConfigDialogPagePaths;
class SymbolDirsEdit;
class ModelDirsEdit;

class ConfigDialog : public KConfigDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget *parent, const char *name);

protected slots:
    void updateSettings();
    void updateWidgets();

protected:
    bool hasChanged();

private:
    ConfigDialogPagePaths *m_pathsPage;
};

class ConfigDialogPageSchematicEditor : public QWidget
{
    Q_OBJECT

public:
    ConfigDialogPageSchematicEditor();
};

class ConfigDialogPageAnalysis : public QWidget
{
    Q_OBJECT

public:
    ConfigDialogPageAnalysis();
};

class ConfigDialogPagePaths : public QWidget
{
    Q_OBJECT

public:
    ConfigDialogPagePaths();

    void updateSettings();
    void updateWidgets();

    bool hasChanged();

signals:
    void widgetModified();
    void settingsChanged();

private:
    SymbolDirsEdit *m_symbolDirsEdit;
    ModelDirsEdit *m_modelDirsEdit;
};

} // namespace Spiceplus

#endif // CONFIGDIALOG_H

// vim: ts=4 sw=4 et
