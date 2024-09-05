pixmapdir = $(datadir)/$(PACKAGE)/pixmaps
pixmap_DATA = $(generated_images) \
             footprint-right-leftmost.png footprint-right-down-leftmost.png \
             footprint-down-leftmost.png footprint-left-down-leftmost.png \
             footprint-left-leftmost.png footprint-left-up-leftmost.png \
             footprint-up-leftmost.png footprint-right-up-leftmost.png \
             footprint-right-rightmost.png footprint-right-down-rightmost.png \
             footprint-down-rightmost.png footprint-left-down-rightmost.png \
             footprint-left-rightmost.png footprint-left-up-rightmost.png \
             footprint-up-rightmost.png footprint-right-up-rightmost.png

EXTRA_DIST = $(pixmap_DATA) $(povsources) splash-foot.png

povsources = settings.inc monster-blob.inc gore-blob.inc hero.inc egg.inc \
             block.pov levitation.pov wall.pov monster-wall.pov \
             power-up-invisibility.pov power-up-freeze.pov power-up-explode.pov \
             power-up.inc \
             plain-monster.pov plain-gore.pov \
             hard-monster.pov hard-gore.pov \
             seeker-monster.pov seeker-gore.pov \
             egg-layer-monster.pov egg-layer-gore.pov \
             freeze-box.pov \
             plain-egg.pov hard-egg.pov seeker-egg.pov egg-layer-egg.pov \
             hero.pov invisible-hero.pov hero-gore.pov \
             splash.pov


CLEANFILES = $(temporary_images)
#MAINTAINERCLEANFILES = $(generated_images)

.INTERMEDIATE: $(temporary_images)

POVRAY = povray
POVFLAGS = "-p -d +ua +a +kc -gr -gs"

# make and automake does not support macros, so the actual rules for
# rendering the graphics are instead generated with m4 from Makefile.povrules.m4
Makefile.povrules: Makefile.povrules.m4 povray-helpers.m4
	m4 -P < $< > $@

generated_images = splash.jpg
temporary_images = splash.png

include $(srcdir)/Makefile.povrules

splash.png: splash.pov
	$(POVRAY) $(POVFLAGS) +w960 +h704 +o$@ $<

splash.jpg: splash.png
	convert  $< $@
