METASOURCES = AUTO
INCLUDES    = -I$(top_srcdir)/libspiceplus $(all_includes)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = spiceplus_device_independentvoltagesource.la \
                         spiceplus_device_independentcurrentsource.la

# Srcs for the plugin
spiceplus_device_independentvoltagesource_la_SOURCES = independentsource.cpp independentvoltagesource.cpp
spiceplus_device_independentcurrentsource_la_SOURCES = independentsource.cpp independentcurrentsource.cpp

# Libs needed by the plugin
spiceplus_device_independentvoltagesource_la_LIBADD = $(top_builddir)/libspiceplus/libspiceplus.la
spiceplus_device_independentcurrentsource_la_LIBADD = $(top_builddir)/libspiceplus/libspiceplus.la

# LD flags for the plugin
spiceplus_device_independentvoltagesource_la_LDFLAGS = -module $(all_libraries)
spiceplus_device_independentcurrentsource_la_LDFLAGS = -module $(all_libraries)

icondir = $(kde_datadir)/spiceplus/icons
icon_ICON = spiceplus_independentvoltagesource \
            spiceplus_independentcurrentsource

devicedir = $(kde_datadir)/spiceplus/plugins/devices/Standard
device_DATA = IndependentVoltageSource.device \
              IndependentCurrentSource.device

# i18n translation messages
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/spiceplus_device_independentvoltagesource.pot
