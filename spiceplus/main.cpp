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

#include <config.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "mainwindow.h"
#include "settings.h"
#include "pluginmanager.h"

using namespace Spiceplus;

static const char description[] = I18N_NOOP("A circuit simulator based on SPICE");

static KCmdLineOptions options[] =
{
//  { "+[URL]", I18N_NOOP("Document to open."), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about(PACKAGE, "SPICE+", VERSION,
                     description,
                     KAboutData::License_GPL,
                     "(c) 2004, Andreas Unger", 0, "http://www.spiceplus.org", "unger@spiceplus.org");

    about.addAuthor("Andreas Unger", 0, "unger@spiceplus.org");

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;
    Settings::self()->readConfig();

    if (app.isRestored())
    {
    	RESTORE(MainWindow);
    }
    else
    {
    	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    	MainWindow *mainWin = new MainWindow;
    	mainWin->show();

        args->clear();
    }

    int ret = app.exec();
    Settings::self()->writeConfig();
    return ret;
}

// vim: ts=4 sw=4 et
