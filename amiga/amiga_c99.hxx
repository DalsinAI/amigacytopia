// Cytopia for GFOS: the C99 string-to-number calls libnix has not got,
// which nlohmann json (external/header_only/json.hxx) asks std:: for.
// Included before every file by amiga/Makefile.amigaos (-include).
// strtof and strtold go through strtod (doubles are what the 68881
// computes in anyway); strtoll and strtoull are written out here.
// MIT, Copyright (c) 2026 AmiCradle Team.
#ifndef AMIGA_C99_HXX
#define AMIGA_C99_HXX

#ifdef __cplusplus
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <climits>

inline unsigned long long amiga_strtoull_abs(const char *s, char **end, int base, bool *neg, bool *overflow)
{
  const char *p = s;
  while (std::isspace((unsigned char)*p))
    p++;
  *neg = false;
  if (*p == '+' || *p == '-')
    *neg = *p++ == '-';
  if ((base == 0 || base == 16) && p[0] == '0' && (p[1] == 'x' || p[1] == 'X') && std::isxdigit((unsigned char)p[2]))
  {
    p += 2;
    base = 16;
  }
  else if (base == 0)
    base = p[0] == '0' ? 8 : 10;
  unsigned long long v = 0;
  const char *digits = p;
  *overflow = false;
  for (;; p++)
  {
    int d;
    unsigned char c = (unsigned char)*p;
    if (c >= '0' && c <= '9')
      d = c - '0';
    else if (c >= 'a' && c <= 'z')
      d = c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
      d = c - 'A' + 10;
    else
      break;
    if (d >= base)
      break;
    if (v > (ULLONG_MAX - (unsigned)d) / (unsigned)base)
      *overflow = true;
    v = v * (unsigned)base + (unsigned)d;
  }
  if (end)
    *end = (char *)(p == digits ? s : p);
  return v;
}

inline unsigned long long amiga_strtoull(const char *s, char **end, int base)
{
  bool neg, ovf;
  unsigned long long v = amiga_strtoull_abs(s, end, base, &neg, &ovf);
  if (ovf)
  {
    errno = ERANGE;
    return ULLONG_MAX;
  }
  return neg ? (unsigned long long)(-(long long)v) : v;
}

inline long long amiga_strtoll(const char *s, char **end, int base)
{
  bool neg, ovf;
  unsigned long long v = amiga_strtoull_abs(s, end, base, &neg, &ovf);
  if (!neg && (ovf || v > (unsigned long long)LLONG_MAX))
  {
    errno = ERANGE;
    return LLONG_MAX;
  }
  if (neg && (ovf || v > (unsigned long long)LLONG_MAX + 1ULL))
  {
    errno = ERANGE;
    return LLONG_MIN;
  }
  return neg ? (long long)(0ULL - v) : (long long)v;
}

inline float amiga_strtof(const char *s, char **end) { return (float)std::strtod(s, end); }
inline long double amiga_strtold(const char *s, char **end) { return std::strtod(s, end); }

#ifdef __amigaos__
namespace std
{
inline long long strtoll(const char *s, char **e, int b) { return amiga_strtoll(s, e, b); }
inline unsigned long long strtoull(const char *s, char **e, int b) { return amiga_strtoull(s, e, b); }
inline float strtof(const char *s, char **e) { return amiga_strtof(s, e); }
inline long double strtold(const char *s, char **e) { return amiga_strtold(s, e); }
} // namespace std
#endif
#endif

#endif
