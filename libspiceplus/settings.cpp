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

#include <qstring.h>
#include <qsize.h>
#include <qfont.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kurl.h>

#include "settings.h"
#include "project.h"

using namespace Spiceplus;

Settings *Settings::s_self = 0;

Settings::Settings()
    : QObject(kapp)
{
    setCurrentGroup("Schematic");

    addItemColor("SchematicBackgroundColor", m_schematicBackgroundColor, Qt::white);
    addItemBool("IsGridVisible", m_isGridVisible, false);
    addItemColor("GridColor", m_gridColor, Qt::lightGray);
    addItemInt("GridSize", m_gridSize, 10);

    addItemInt("DeviceRotationAngle", m_deviceRotationAngle, 90);
    addItemInt("DevicePinDiameter", m_devicePinDiameter, 5);

    addItemColor("DeviceColor", m_deviceColor, Qt::black);
    addItemColor("DeviceColorHighlighted", m_deviceColorHighlighted, Qt::red);

    addItemColor("WireColor", m_wireColor, Qt::black);
    addItemColor("WireColorHighlighted", m_wireColorHighlighted, Qt::red);

    addItemColor("SelectionMarkColor", m_selectionMarkColor, Qt::blue);

    addItemColor("ConnectionMarkColor", m_connectionMarkColor, QColor(0, 0xa0, 0));
    addItemColor("ConnectionMarkColorRedundant", m_connectionMarkColorRedundant, Qt::red);

    QFont generalFont = KGlobalSettings::generalFont();
    generalFont.setPointSize(6);
    addItemFont("TinySymbolFont", m_tinySymbolFont, generalFont);
    generalFont.setPointSize(8);
    addItemFont("SmallSymbolFont", m_smallSymbolFont, generalFont);
    generalFont.setPointSize(10);
    addItemFont("NormalSymbolFont", m_normalSymbolFont, generalFont);
    generalFont.setPointSize(12);
    addItemFont("LargeSymbolFont", m_largeSymbolFont, generalFont);
    generalFont.setPointSize(14);
    addItemFont("HugeSymbolFont", m_hugeSymbolFont, generalFont);

    setCurrentGroup("Analysis");
    addItemInt("ACAnalysisNumPointsPerDecade", m_acAnalysisNumPointsPerDecade, 100);

    setCurrentGroup("Paths");
    addItemPath("SpiceExecutablePath", m_spiceExecutablePath, KStandardDirs::findExe("spice3"));

    QStringList dirs = KGlobal::dirs()->findDirs("data", "spiceplus/plugins/devices");
    addItemPath("DeviceDir", m_deviceDir, !dirs.isEmpty() ? dirs.first() : "");

    dirs = KGlobal::dirs()->findDirs("data", "spiceplus/plugins/modelviews");
    addItemPath("ModelViewDir", m_modelViewDir, !dirs.isEmpty() ? dirs.first() : "");

    dirs = KGlobal::dirs()->findDirs("data", "spiceplus/resources/symbols");
    addItemPath("StandardSymbolDir", m_standardSymbolDir, !dirs.isEmpty() ? dirs.first() : "");

    dirs = KGlobal::dirs()->findDirs("data", "spiceplus/resources/models");
    addItemPath("StandardModelDir", m_standardModelDir, !dirs.isEmpty() ? dirs.first() : "");

    setCurrentGroup("Project");
    addItemPath("ProjectsDir", m_projectsDir, KGlobalSettings::documentPath());
}

Settings::~Settings()
{
    s_self = 0;
}

Settings *Settings::self()
{
    if (!s_self)
        s_self = new Settings;
    return s_self;
}

void Settings::usrReadConfig()
{
    QString symbolDirsFile = KGlobal::dirs()->findResource("data", "spiceplus/symbol-dirs.xml");
    if (!symbolDirsFile.isNull())
    {
        if (!m_userSymbolDirs.load(symbolDirsFile))
        {
            kdError() << "Could not load user defined symbol directories from " << symbolDirsFile << endl;
            m_userSymbolDirs.clear();
        }
    }
    else
        m_userSymbolDirs.clear();

    QString modelDirsFile = KGlobal::dirs()->findResource("data", "spiceplus/model-dirs.xml");
    if (!modelDirsFile.isNull())
    {
        if (!m_userModelDirs.load(modelDirsFile))
        {
            kdError() << "Could not load user defined model directories from " << modelDirsFile << endl;
            m_userModelDirs.clear();
        }
    }
    else
        m_userModelDirs.clear();
}

void Settings::usrWriteConfig()
{
    QString symbolDirsFile = KGlobal::dirs()->findResource("data", "spiceplus/symbol-dirs.xml");
    if (symbolDirsFile.isNull())
    {
        symbolDirsFile = KGlobal::dirs()->saveLocation("data", "spiceplus/");
        if (!symbolDirsFile.isNull())
            symbolDirsFile += "symbol-dirs.xml";
    }

    if (symbolDirsFile.isNull() || !m_userSymbolDirs.save(symbolDirsFile))
        kdError() << "Could not save symbol directories" << endl;

    QString modelDirsFile = KGlobal::dirs()->findResource("data", "spiceplus/model-dirs.xml");
    if (modelDirsFile.isNull())
    {
        modelDirsFile = KGlobal::dirs()->saveLocation("data", "spiceplus/");
        if (!modelDirsFile.isNull())
            modelDirsFile += "model-dirs.xml";
    }

    if (modelDirsFile.isNull() || !m_userModelDirs.save(modelDirsFile))
        kdError() << "Could not save model directories" << endl;
}

KURL Settings::modelPathToURL(const QString &modelPath) const
{
    QString dirID = modelPath.section(':', 0, 0);
    QString relPath = modelPath.section(':', 1);
    KURL url;

    if (dirID == "standard")
        url = m_standardModelDir;
    else if (dirID == "project")
    {
        if (Project::self()->isOpen())
            url = Project::self()->dir();
    }
    else
        url = m_userModelDirs.dir(dirID).path;

    if (!url.isEmpty())
        url.addPath(relPath);

    return url;
}

KURL Settings::symbolPathToURL(const QString &symbolPath) const
{
    QString dirID = symbolPath.section(':', 0, 0);
    QString relPath = symbolPath.section(':', 1);
    KURL url;

    if (dirID == "standard")
        url = m_standardSymbolDir;
    else if (dirID == "project")
    {
        if (Project::self()->isOpen())
            url = Project::self()->dir();
    }
    else
        url = m_userSymbolDirs.dir(dirID).path;

    if (!url.isEmpty())
        url.addPath(relPath);

    return url;
}

#include "settings.moc"

// vim: ts=4 sw=4 et
