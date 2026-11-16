/* Cytopia for GFOS: zlib's names for the six calls Cytopia's saved games
 * use (src/engine/basics/compression.hxx), on GFOS's z.library
 * (GFOpenZlib, installed with the GFOpenGPU part). amiga/zsdk/ is
 * gfopensdk's z.library SDK (79e8d48). ZBase is opened in amiga/amiga_start.cxx;
 * without z.library every call fails with Z_STREAM_ERROR and Cytopia
 * reports that the game could not be saved or loaded.
 * MIT, Copyright (c) 2026 AmiCradle Team. */
#ifndef AMIGA_ZLIB_SHIM_H
#define AMIGA_ZLIB_SHIM_H

#include <proto/z.h>
/* exec's inline names would hit C++ code: z.library's calls need none. */

#ifndef Z_NULL
#define Z_NULL 0
#endif
typedef unsigned char Bytef;
typedef unsigned int uInt;
typedef unsigned long uLong;

#ifdef __cplusplus
extern "C" {
#endif
extern struct Library *ZBase;
#ifdef __cplusplus
}
#endif

static inline int amiga_deflateInit(z_stream *s, int level)
{ return ZBase ? (int)DeflateInit(s, level) : Z_STREAM_ERROR; }
static inline int amiga_deflate(z_stream *s, int flush)
{ return ZBase ? (int)Deflate(s, flush) : Z_STREAM_ERROR; }
static inline int amiga_deflateEnd(z_stream *s)
{ return ZBase ? (int)DeflateEnd(s) : Z_STREAM_ERROR; }
static inline int amiga_inflateInit(z_stream *s)
{ return ZBase ? (int)InflateInit(s) : Z_STREAM_ERROR; }
static inline int amiga_inflate(z_stream *s, int flush)
{ return ZBase ? (int)Inflate(s, flush) : Z_STREAM_ERROR; }
static inline int amiga_inflateEnd(z_stream *s)
{ return ZBase ? (int)InflateEnd(s) : Z_STREAM_ERROR; }

#define deflateInit amiga_deflateInit
#define deflate amiga_deflate
#define deflateEnd amiga_deflateEnd
#define inflateInit amiga_inflateInit
#define inflate amiga_inflate
#define inflateEnd amiga_inflateEnd

#endif
