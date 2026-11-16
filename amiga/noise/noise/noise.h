// Cytopia for GFOS: the five libnoise modules TerrainGenerator.cxx uses
// (Perlin, RidgedMulti, ScaleBias, Clamp, Blend), written for this port
// so the game needs no libnoise. Same names, setters and GetValue(x, y, z)
// as libnoise; the noise itself is gradient (Perlin) noise from a seeded
// permutation table, so a seed always gives the same map on any machine.
//
// MIT, Copyright (c) 2026 AmiCradle Team.
#ifndef AMIGA_NOISE_H
#define AMIGA_NOISE_H

#include <cmath>
#include <cstdint>
#include <vector>

namespace noise {
namespace module {

class Module {
public:
  virtual ~Module() {}
  virtual double GetValue(double x, double y, double z) const = 0;
  void SetSourceModule(int index, const Module &m) {
    if (index >= 0 && index < 3)
      m_source[index] = &m;
  }

protected:
  const Module *m_source[3] = {nullptr, nullptr, nullptr};
  double Source(int i, double x, double y, double z) const {
    return m_source[i] ? m_source[i]->GetValue(x, y, z) : 0.0;
  }
};

// Gradient noise in [-1, 1], from a 256-entry permutation shuffled by seed.
class GradientNoise {
public:
  explicit GradientNoise(int seed = 0) { Reseed(seed); }
  void Reseed(int seed);
  double Value(double x, double y, double z) const;

private:
  uint8_t m_perm[512];
};

class Perlin : public Module {
public:
  void SetSeed(int seed) { m_seed = seed; }
  void SetFrequency(double f) { m_frequency = f; }
  void SetLacunarity(double l) { m_lacunarity = l; }
  void SetPersistence(double p) { m_persistence = p; }
  void SetOctaveCount(int n) { m_octaves = n < 1 ? 1 : (n > 30 ? 30 : n); }
  double GetValue(double x, double y, double z) const override;

protected:
  // One permutation table per octave, made once per seed and octave count.
  const GradientNoise &Octave(int o) const;
  mutable std::vector<GradientNoise> m_tables;
  mutable int m_tableSeed = 0;
  int m_seed = 0;
  double m_frequency = 1.0;
  double m_lacunarity = 2.0;
  double m_persistence = 0.5;
  int m_octaves = 6;
};

// Ridged multifractal: sharp ridges for mountains.
class RidgedMulti : public Perlin {
public:
  double GetValue(double x, double y, double z) const override;
};

class ScaleBias : public Module {
public:
  void SetScale(double s) { m_scale = s; }
  void SetBias(double b) { m_bias = b; }
  double GetValue(double x, double y, double z) const override {
    return Source(0, x, y, z) * m_scale + m_bias;
  }

private:
  double m_scale = 1.0;
  double m_bias = 0.0;
};

class Clamp : public Module {
public:
  void SetBounds(double lo, double hi) { m_lo = lo; m_hi = hi; }
  double GetValue(double x, double y, double z) const override {
    double v = Source(0, x, y, z);
    return v < m_lo ? m_lo : (v > m_hi ? m_hi : v);
  }

private:
  double m_lo = -1.0;
  double m_hi = 1.0;
};

// Mixes source 0 and source 1 by the control module: -1 gives source 0,
// +1 gives source 1 (libnoise's convention).
class Blend : public Module {
public:
  void SetControlModule(const Module &m) { m_source[2] = &m; }
  double GetValue(double x, double y, double z) const override {
    double a = Source(0, x, y, z), b = Source(1, x, y, z);
    double t = (Source(2, x, y, z) + 1.0) / 2.0;
    return a + t * (b - a);
  }
};

} // namespace module
} // namespace noise

#endif
