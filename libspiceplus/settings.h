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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qobject.h>

#include <kconfigskeleton.h>

#include "symboldirs.h"
#include "modeldirs.h"

class QString;
class QSize;
class QFont;

namespace Spiceplus {

class Settings : public QObject, public KConfigSkeleton
{
    Q_OBJECT

private:
    Settings();

public:
    ~Settings();

    static Settings *self();

    void usrReadConfig();
    void usrWriteConfig();

    // [Schematic]

    QColor schematicBackgroundColor() const { return m_schematicBackgroundColor; }
    bool isGridVisible() const { return m_isGridVisible; }
    QColor gridColor() const { return m_gridColor; }
    int gridSize() const { return m_gridSize; }

    int deviceRotationAngle() const { return m_deviceRotationAngle; }
    int devicePinDiameter() const { return m_devicePinDiameter; }
    QColor deviceColor() const { return m_deviceColor; }
    QColor deviceColorHighlighted() const { return m_deviceColorHighlighted; }

    QColor wireColor() const { return m_wireColor; }
    QColor wireColorHighlighted() const { return m_wireColorHighlighted; }

    QColor selectionMarkColor() const { return m_selectionMarkColor; }

    QColor connectionMarkColor() const { return m_connectionMarkColor; }
    QColor connectionMarkColorRedundant() const { return m_connectionMarkColorRedundant; }

    QFont tinySymbolFont() const { return m_tinySymbolFont; }
    QFont smallSymbolFont() const { return m_smallSymbolFont; }
    QFont normalSymbolFont() const { return m_normalSymbolFont; }
    QFont largeSymbolFont() const { return m_largeSymbolFont; }
    QFont hugeSymbolFont() const { return m_hugeSymbolFont; }

    // [Analysis]

    int acAnalysisNumPointsPerDecade() const { return m_acAnalysisNumPointsPerDecade; }

    // [Paths]

    QString spiceExecutablePath() const { return m_spiceExecutablePath; }
    QString deviceDir() const { return m_deviceDir; }
    QString modelViewDir() const { return m_modelViewDir; }

    QString standardSymbolDir() const { return m_standardSymbolDir; }
    SymbolDirs userSymbolDirs() const { return m_userSymbolDirs; }
    void setUserSymbolDirs(const SymbolDirs &dirs) { m_userSymbolDirs = dirs; }
    KURL symbolPathToURL(const QString &symbolPath) const;

    QString standardModelDir() const { return m_standardModelDir; }
    ModelDirs userModelDirs() const { return m_userModelDirs; }
    void setUserModelDirs(const ModelDirs &dirs) { m_userModelDirs = dirs; }
    KURL modelPathToURL(const QString &modelPath) const;

    // [Project]

    QString projectsDir() const { return m_projectsDir; }
    void setProjectsDir(const QString &dir) { m_projectsDir = dir; }

public slots:
    void emitSettingsChanged() { emit settingsChanged(); }

signals:
    void settingsChanged();

private:
    static Settings *s_self;

    QColor m_schematicBackgroundColor;
    bool m_isGridVisible;
    QColor m_gridColor;
    int m_gridSize;

    int m_deviceRotationAngle;
    int m_devicePinDiameter;

    QColor m_deviceColor;
    QColor m_deviceColorHighlighted;

    QColor m_wireColor;
    QColor m_wireColorHighlighted;

    QColor m_selectionMarkColor;

    QColor m_connectionMarkColor;
    QColor m_connectionMarkColorRedundant;

    QFont m_tinySymbolFont;
    QFont m_smallSymbolFont;
    QFont m_normalSymbolFont;
    QFont m_largeSymbolFont;
    QFont m_hugeSymbolFont;

    int m_acAnalysisNumPointsPerDecade;

    QString m_spiceExecutablePath;
    QString m_deviceDir;
    QString m_modelViewDir;
    QString m_standardSymbolDir;
    SymbolDirs m_userSymbolDirs;
    QString m_standardModelDir;
    ModelDirs m_userModelDirs;

    QString m_projectsDir;
};

} // namespace Spiceplus

#endif // SETTINGS_H

// vim: ts=4 sw=4 et
