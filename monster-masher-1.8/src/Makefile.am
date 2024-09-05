# pass info on to source
INCLUDES = \
-DMONSTER_MASHER_GLADE_DIR=\""$(gladedir)/"\" \
-DMONSTER_MASHER_PIXMAP_DIR=\""$(pixmapdir)/"\" \
-DMONSTER_MASHER_SOUND_DIR=\""$(sounddir)/"\" \
-DGNOMEICONDIR=\""$(icondir)"\"  \
-DGNOMELOCALEDIR=\""$(gnomelocaledir)"\"

bin_PROGRAMS = monster-masher

monster_masher_SOURCES = \
                      animation.cpp animation.hpp \
                      arena.cpp arena.hpp \
                      canvas-message.cpp canvas-message.hpp \
                      canvas.cpp canvas.hpp \
                      center-menu.cpp center-menu.hpp \
                      fading-decoration.cpp fading-decoration.hpp \
                      footsteps.cpp footsteps.hpp \
                      game.cpp game.hpp \
                      graphic.cpp graphic.hpp \
                      gui-helpers.hpp \
                      helpers.cpp helpers.hpp \
                      hero.cpp hero.hpp \
                      i18n.hpp \
                      image-cache.cpp image-cache.hpp \
                      intro.cpp intro.hpp \
                      main-window.cpp main-window.hpp \
                      main.cpp \
                      monsters.cpp monsters.hpp \
                      movement.cpp movement.hpp \
                      new-game-window.cpp new-game-window.hpp \
                      obstacles.cpp obstacles.hpp \
                      persistent.cpp persistent.hpp \
                      pixbuf-drawing.cpp pixbuf-drawing.hpp \
                      player.cpp player.hpp \
                      preferences-window.cpp preferences-window.hpp \
                      sound.cpp sound.hpp \
                      splat-helpers.cpp splat-helpers.hpp \
                      thing.cpp thing.hpp \
                      ucompose.hpp \
                      vector.hpp

if MAINTAINER_MODE
AM_CXXFLAGS = @CXXFLAGS@ $(DEPS_CFLAGS) -O0 -Wall
else
AM_CXXFLAGS = @CXXFLAGS@ $(DEPS_CFLAGS) -Wall -O3
endif

LDADD = $(DEPS_LIBS)

glade_DATA = ui.glade
gladedir = $(datadir)/$(PACKAGE)/glade

pixmapdir = $(datadir)/$(PACKAGE)/pixmaps
sounddir = $(datadir)/$(PACKAGE)/sounds
icondir = $(datadir)/pixmaps

gnomelocaledir = $(datadir)/locale

EXTRA_DIST = $(glade_DATA) README.compose
