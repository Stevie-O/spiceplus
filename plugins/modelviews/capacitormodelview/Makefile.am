METASOURCES = AUTO
INCLUDES    = -I$(top_srcdir)/libspiceplus $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = spiceplus_modelview_capacitor.la

# Srcs for the plugin
spiceplus_modelview_capacitor_la_SOURCES = capacitormodelview.cpp

# Libs needed by the plugin
spiceplus_modelview_capacitor_la_LIBADD = $(top_builddir)/libspiceplus/libspiceplus.la

# LD flags for the plugin
spiceplus_modelview_capacitor_la_LDFLAGS = -module $(all_libraries)

modelviewdir = $(kde_datadir)/spiceplus/plugins/modelviews
modelview_DATA = c.modelview

# i18n translation messages
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/spiceplus_modelview_capacitor.pot
