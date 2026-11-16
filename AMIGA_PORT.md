Cytopia for GFOS
================

A port of Cytopia 0.4, the CytopiaTeam's free isometric city builder,
from https://github.com/CytopiaTeam/Cytopia at commit
c70bbce3ba8b0ba23b8dcce9ff8bc8df0af971f8, for GFOS: AmigaOS 3.2 with
GFOpenUp, on a 68040 or 68060 with an FPU, on GFOpenGPU's SDL 2.

Lay out roads, zone land for homes, shops and industry, run power lines
and watch the town grow, on a terrain made fresh from a seed each game.

Status: written for Cradle's os32-gcc16-open6 stove; not yet run in a lab.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp and its GFOpenGPU part, which installs
  LIBS:GFOpenGPU/SDL2.module and SDL2_image.module, and LIBS:z.library
  (GFOpenZlib 53.13 or later) for saving cities.
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen
  (the game's window is 800 x 600), about 48 MB of free memory and 14 MB
  on disk. Making a new map and drawing the city are heavy work: on a real
  68040 it is slow; on AmiCradle it runs at speed.
- Nothing else: the tiles, fonts and building data are in the drawer.


How to play
-----------

On GFOS: GFOpenUpTool INSTALL Parts/Cytopia puts the game in
SYS:Games/Cytopia. From the archive: run Install-Cytopia (or copy the
Cytopia drawer where you want it). Then double-click Cytopia, choose New
Game, and build with the menu along the bottom: roads, zones, power,
terrain tools. Drag with the right mouse button to move the view, the
mouse wheel zooms, Escape opens the pause menu (save, load, settings).

Your settings, saved cities and error.log are kept in the drawer's User
drawer (User/Cytopia/); the Installer keeps it on an update and GFOpenUp
never touches it.

There is no sound yet: upstream plays its music and effects only through
OpenAL, which GFOS has not got.


Building
--------

    make -f amiga/Makefile.amigaos -j4 STOVE=/path/to/os32-gcc16-open6

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package (package/build-amiga.sh), which also runs the host tests, makes
the GFOpenUp part (Parts/Cytopia) and packs Cytopia-VERSION-68040.lha.


What was changed
----------------

The sources of upstream's src/CMakeLists.txt as upstream builds them
without OpenAL or AngelScript, and Dear ImGui from external/imgui, built
by amiga/Makefile.amigaos. On top:

- amiga/noise: the five libnoise modules the terrain generator uses
  (Perlin, RidgedMulti, ScaleBias, Clamp, Blend), written for this port
  with libnoise's interface, so no libnoise is needed. A seed always gives
  the same map. tests/amiga/test_noise.cxx checks them on the host.
- amiga/zlib.h: the six zlib calls the saved cities use, on GFOS's
  z.library (amiga/zsdk is gfopensdk's z.library SDK, 79e8d48).
- amiga/amiga_c99.hxx: strtoll, strtoull, strtof and strtold, which libnix
  has not got and the JSON reader needs; tests/amiga/test_c99.cxx checks
  them against the C library on the host.
- amiga/amiga_start.cxx: a 1 MB stack, the $VER string, PROGDIR:User,
  z.library (53.13 or later), and a statvfs() stub std::filesystem needs
  to link.
- src/engine/common/Constants.hxx: settings and saves in PROGDIR:User.
- src/engine/common/platform.hxx: GFOS is its own platform (no xdg-open
  or dialog commands).
- src/util/Exception.cxx: no execinfo backtrace on GFOS.
- external/imgui/backends/imgui_impl_sdl.cpp: the GL drawable-size
  fallback is the window size on GFOS, which keeps GL.module out.
- The drawer gets data/resources without audio/ (no sound yet) and
  data/languages.
- amiga/icons: the drawer's GlowIcons, from The AmiCradle Team's own art
  (an isometric city block).


Licence
-------

Cytopia: GPL 3 (license.txt); its art, data and fonts as listed in
credits.txt and data/resources/fonts/Licenses. The Amiga files (amiga/,
package/, tools/, tests/test_part.sh, tests/amiga/): MIT, Copyright (c)
2026 AmiCradle Team, except where a file says it follows Cytopia's GPL 3.

Designed, tested and delivered by The AmiCradle Team.
