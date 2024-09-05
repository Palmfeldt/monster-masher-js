SUBDIRS = pixmaps sounds src po #help

# desktop entry
desktopdir		= $(datadir)/applications
desktop_in_files	= $(PACKAGE).desktop.in
desktop_DATA 		= $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

# GConf schema
schema_in_files = monster-masher.schemas.in
schemadir = $(sysconfdir)/gconf/schemas
schema_DATA = $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

# icon
icondir = $(datadir)/pixmaps
icon_DATA = $(PACKAGE).png

pixmapdir = $(datadir)/$(PACKAGE)/pixmaps/

EXTRA_DIST = $(desktop_in_files) $(schema_in_files) \
             $(desktop_DATA) $(icon_DATA) \
             intltool-merge.in intltool-extract.in intltool-update.in \
             MAINTAINERS

install-data-local:
	if test -z "$(DESTDIR)" ; then \
	for p in $(schema_DATA) ; do \
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/$$p; \
	done \
        fi
