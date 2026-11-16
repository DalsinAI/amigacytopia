// The GFOS port's start: the stack, the version string, PROGDIR:User (the
// settings, the saved cities and error.log), z.library for the saved
// cities, and the one libstdc++ hole std::filesystem leaves on libnix.
// MIT, Copyright (c) 2026 AmiCradle Team.
#include <sys/stat.h>
#include <proto/exec.h>

extern "C" {
// A Shell gives 4 KB of stack; C++17, the JSON reader and SDL want more.
// libnix's startup sets this much up.
unsigned long __stack = 1024 * 1024;
struct Library *ZBase;
}

static const char version[] __attribute__((used)) =
    "$VER: Cytopia 0.4 (16.11.2026)";

static void __attribute__((constructor)) amiga_start(void)
{
  mkdir("PROGDIR:User", 0755);
  // GFOpenZlib 53.13 and on (the Aminet z.library table); 53.12 has
  // another table and is refused (gfopensdk Docs/LIBRARIES.md).
  ZBase = OpenLibrary((CONST_STRPTR) "z.library", 53);
  if (ZBase != NULL && ZBase->lib_Version == 53 && ZBase->lib_Revision == 12)
  {
    CloseLibrary(ZBase);
    ZBase = NULL;
  }
}

static void __attribute__((destructor)) amiga_stop(void)
{
  if (ZBase != NULL)
    CloseLibrary(ZBase);
}

// libstdc++'s std::filesystem::space() calls statvfs(), which libnix has
// not got; Cytopia never asks for free space, so it only has to link.
struct statvfs;
int statvfs(const char *, struct statvfs *) { return -1; }
