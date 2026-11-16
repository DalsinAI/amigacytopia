// Host test for amiga/amiga_c99.hxx: the replacement strtoll/strtoull give
// what the C library gives. MIT, Copyright (c) 2026 AmiCradle Team.
#include "amiga_c99.hxx"
#include <cstdio>
#include <cstring>
static int fails;
int main()
{
  const char *in[] = {"0", "42", "-42", "  +17x", "9223372036854775807", "-9223372036854775808",
                      "9223372036854775808", "18446744073709551615", "18446744073709551616",
                      "0x1F", "077", "", "abc", "-0", "123456789012"};
  int bases[] = {10, 0, 16};
  for (const char *s : in)
    for (int b : bases)
    {
      char *e1, *e2;
      errno = 0;
      long long a = amiga_strtoll(s, &e1, b);
      int ea = errno;
      errno = 0;
      long long c = ::strtoll(s, &e2, b);
      int ec = errno;
      if (a != c || e1 != e2 || ea != ec)
      {
        printf("FAIL strtoll(\"%s\", %d): %lld/%d vs %lld/%d\n", s, b, a, (int)(e1 - s), c, (int)(e2 - s));
        fails++;
      }
      errno = 0;
      unsigned long long ua = amiga_strtoull(s, &e1, b);
      ea = errno;
      errno = 0;
      unsigned long long uc = ::strtoull(s, &e2, b);
      ec = errno;
      if (ua != uc || e1 != e2 || ea != ec)
      {
        printf("FAIL strtoull(\"%s\", %d): %llu vs %llu\n", s, b, ua, uc);
        fails++;
      }
    }
  char *e;
  if (amiga_strtof("1.5e3x", &e) != 1500.0f || *e != 'x')
    fails++, printf("FAIL strtof\n");
  printf(fails ? "c99: %d checks failed\n" : "c99: all checks passed\n", fails);
  return fails != 0;
}
