METASOURCES = AUTO
INCLUDES= $(all_includes)

# The library containing the plugin base class
lib_LTLIBRARIES =  libspiceplus.la
libspiceplus_la_SOURCES = file.cpp \
                          devicesymbol.cpp \
                          model.cpp \
                          modelfile.cpp \
                          modelselector.cpp \
                          schematic.cpp \
                          schematicwire.cpp \
                          schematicdevice.cpp \
                          schematicstandarddevice.cpp \
                          schematicjunction.cpp \
                          schematicground.cpp \
                          schematictestpoint.cpp \
                          schematicammeter.cpp \
                          pluginmanager.cpp \
                          project.cpp \
                          symboldirs.cpp \
                          modeldirs.cpp \
                          settings.cpp \
                          parameterlineedit.cpp \
                          editlistview.cpp
libspiceplus_la_LDFLAGS = $(all_libraries) -version-info 0:0:0

# Headerfiles are installed in a subdirectory for convenience
spiceplusincludedir = $(includedir)/spiceplus
spiceplusinclude_HEADERS = types.h \
                           groupbox.h \
                           file.h \
                           devicesymbol.h \
                           model.h \
                           modelfile.h \
                           modelselector.h \
                           schematic.h \
                           schematicwire.h \
                           schematicdevice.h \
                           schematicstandarddevice.h \
                           schematicjunction.h \
                           schematicground.h \
                           schematictestpoint.h \
                           schematicammeter.h \
                           pluginmanager.h \
                           project.h \
                           symboldirs.h \
                           modeldirs.h \
                           settings.h \
                           parameterlineedit.h \
                           editlistview.h

icondir = $(kde_datadir)/spiceplus/icons
icon_ICON = spiceplus_device_symbol \
            spiceplus_device_model
