METASOURCES = AUTO
INCLUDES    = -I$(top_srcdir)/libspiceplus $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = spiceplus_device_inductor.la

# Srcs for the plugin
spiceplus_device_inductor_la_SOURCES = inductor.cpp

# Libs needed by the plugin
spiceplus_device_inductor_la_LIBADD = $(top_builddir)/libspiceplus/libspiceplus.la

# LD flags for the plugin
spiceplus_device_inductor_la_LDFLAGS = -module $(all_libraries)

icondir = $(kde_datadir)/spiceplus/icons
icon_ICON = spiceplus_inductor

devicedir = $(kde_datadir)/spiceplus/plugins/devices/Standard
device_DATA = Inductor.device

# i18n translation messages
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/spiceplus_device_inductor.pot
