// Cytopia for GFOS: gradient noise for amiga/noise/noise/noise.h.
// MIT, Copyright (c) 2026 AmiCradle Team.
#include "noise/noise.h"

namespace noise {
namespace module {

void GradientNoise::Reseed(int seed) {
  uint8_t p[256];
  for (int i = 0; i < 256; i++)
    p[i] = (uint8_t)i;
  // xorshift32 from the seed, Fisher-Yates shuffle: the same table for the
  // same seed everywhere.
  uint32_t s = (uint32_t)seed * 2654435761u + 0x9e3779b9u;
  if (s == 0)
    s = 1;
  for (int i = 255; i > 0; i--) {
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    int j = (int)(s % (uint32_t)(i + 1));
    uint8_t t = p[i];
    p[i] = p[j];
    p[j] = t;
  }
  for (int i = 0; i < 512; i++)
    m_perm[i] = p[i & 255];
}

static inline double Fade(double t) { return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }
static inline double Lerp(double a, double b, double t) { return a + t * (b - a); }

static inline double Grad(int h, double x, double y, double z) {
  h &= 15;
  double u = h < 8 ? x : y;
  double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
  return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

double GradientNoise::Value(double x, double y, double z) const {
  double fx = std::floor(x), fy = std::floor(y), fz = std::floor(z);
  int X = (int)((long long)fx & 255), Y = (int)((long long)fy & 255), Z = (int)((long long)fz & 255);
  x -= fx;
  y -= fy;
  z -= fz;
  double u = Fade(x), v = Fade(y), w = Fade(z);
  const uint8_t *p = m_perm;
  int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
  int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
  double r = Lerp(Lerp(Lerp(Grad(p[AA], x, y, z), Grad(p[BA], x - 1, y, z), u),
                       Lerp(Grad(p[AB], x, y - 1, z), Grad(p[BB], x - 1, y - 1, z), u), v),
                  Lerp(Lerp(Grad(p[AA + 1], x, y, z - 1), Grad(p[BA + 1], x - 1, y, z - 1), u),
                       Lerp(Grad(p[AB + 1], x, y - 1, z - 1), Grad(p[BB + 1], x - 1, y - 1, z - 1), u), v),
                  w);
  // Improved noise peaks a little past 1: keep it in [-1, 1].
  return r > 1.0 ? 1.0 : (r < -1.0 ? -1.0 : r);
}

const GradientNoise &Perlin::Octave(int o) const {
  if ((int)m_tables.size() < m_octaves || m_tableSeed != m_seed) {
    m_tables.clear();
    for (int i = 0; i < m_octaves; i++)
      m_tables.emplace_back(m_seed + i);
    m_tableSeed = m_seed;
  }
  return m_tables[o];
}

double Perlin::GetValue(double x, double y, double z) const {
  double sum = 0.0, amp = 1.0;
  x *= m_frequency;
  y *= m_frequency;
  z *= m_frequency;
  for (int o = 0; o < m_octaves; o++) {
    const GradientNoise &n = Octave(o);
    sum += n.Value(x, y, z) * amp;
    x *= m_lacunarity;
    y *= m_lacunarity;
    z *= m_lacunarity;
    amp *= m_persistence;
  }
  return sum;
}

double RidgedMulti::GetValue(double x, double y, double z) const {
  double sum = 0.0, weight = 1.0, freqAmp = 1.0;
  x *= m_frequency;
  y *= m_frequency;
  z *= m_frequency;
  for (int o = 0; o < m_octaves; o++) {
    const GradientNoise &n = Octave(o);
    double s = 1.0 - std::fabs(n.Value(x, y, z));
    s *= s;
    s *= weight;
    weight = s * 2.0;
    if (weight > 1.0)
      weight = 1.0;
    if (weight < 0.0)
      weight = 0.0;
    sum += s * freqAmp;
    freqAmp *= std::pow(m_lacunarity, -1.0);
    x *= m_lacunarity;
    y *= m_lacunarity;
    z *= m_lacunarity;
  }
  return sum * 1.25 - 1.0;
}

} // namespace module
} // namespace noise
