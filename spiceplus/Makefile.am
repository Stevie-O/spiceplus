#SUBDIRS =

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/libspiceplus $(all_includes)

# these are the headers for your project
noinst_HEADERS = mainwindow.h \
                 document.h \
                 schematicdocument.h \
                 modeldocument.h \
                 devicechooser.h \
                 devicewindow.h \
                 schematicview.h \
                 schematiccommand.h \
                 schematiccommandhistory.h \
                 schematictool.h \
                 devicepropertiesdialog.h \
                 signalmultiplexer.h \
                 modelviewselector.h \
                 meter.h \
                 meterselector.h \
                 plot.h \
                 spiceprocess.h \
                 symboldialog.h \
                 symboldirsedit.h \
                 modeldirsedit.h \
                 projectmanager.h \
                 projectdialognew.h \
                 configdialog.h \
                 analysisdialog.h \
                 dcanalysispropertiesdialog.h \
                 dcanalysisdialog.h \
                 acanalysispropertiesdialog.h \
                 acanalysisdialog.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/spiceplus.pot

KDE_ICON = spiceplus
icondir = $(kde_datadir)/spiceplus/icons
icon_ICON = spiceplus_select \
            spiceplus_place_wire \
            spiceplus_split_wire \
            spiceplus_place_ground \
            spiceplus_place_test_point \
            spiceplus_place_ammeter

# binary name
bin_PROGRAMS = spiceplus

# the application source, library search path, and link libraries
spiceplus_SOURCES = main.cpp \
                    mainwindow.cpp \
                    document.cpp \
                    schematicdocument.cpp \
                    modeldocument.cpp \
                    devicechooser.cpp \
                    devicewindow.cpp \
                    schematicview.cpp \
                    schematiccommand.cpp \
                    schematiccommandhistory.cpp \
                    schematictool.cpp \
                    devicepropertiesdialog.cpp \
                    signalmultiplexer.cpp \
                    modelviewselector.cpp \
                    meter.cpp \
                    meterselector.cpp \
                    plot.cpp \
                    spiceprocess.cpp \
                    symboldialog.cpp \
                    symboldirsedit.cpp \
                    modeldirsedit.cpp \
                    projectmanager.cpp \
                    projectdialognew.cpp \
                    configdialog.cpp \
                    analysisdialog.cpp \
                    dcanalysispropertiesdialog.cpp \
                    dcanalysisdialog.cpp \
                    acanalysispropertiesdialog.cpp \
                    acanalysisdialog.cpp

spiceplus_LDFLAGS = $(KDE_RPATH) $(all_libraries)
spiceplus_LDADD = $(LIB_KDEUI) $(LIB_KFILE) $(LIB_KMDI) $(LIB_QWT) $(top_builddir)/libspiceplus/libspiceplus.la

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = spiceplus.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/spiceplus
shellrc_DATA = spiceplusui.rc
