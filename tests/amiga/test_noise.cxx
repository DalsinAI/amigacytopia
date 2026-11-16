// Host test for amiga/noise: values stay in range, a seed always gives the
// same terrain, different seeds give different terrain, and the module
// chain TerrainGenerator.cxx builds yields heights inside its 0..255 clamp
// with both land and water. MIT, Copyright (c) 2026 AmiCradle Team.
#include "noise/noise.h"
#include <cstdio>

using namespace noise::module;

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main() {
  GradientNoise g(7);
  double lo = 9, hi = -9;
  for (int i = 0; i < 20000; i++) {
    double v = g.Value(i * 0.137, i * 0.071, i * 0.013);
    lo = v < lo ? v : lo;
    hi = v > hi ? v : hi;
  }
  CHECK(lo >= -1.0 && hi <= 1.0);
  CHECK(hi - lo > 1.0);
  CHECK(g.Value(3, 4, 5) == 0.0); // gradient noise is 0 on lattice points

  Perlin a, b, c;
  a.SetSeed(42); b.SetSeed(42); c.SetSeed(43);
  a.SetOctaveCount(16); b.SetOctaveCount(16); c.SetOctaveCount(16);
  a.SetFrequency(0.003 / 32); b.SetFrequency(0.003 / 32); c.SetFrequency(0.003 / 32);
  int same = 1, differ = 0;
  for (int x = 0; x < 64; x++) {
    double va = a.GetValue(x * 32 * 7.3, x * 32 * 3.1, 0.5);
    same &= va == b.GetValue(x * 32 * 7.3, x * 32 * 3.1, 0.5);
    differ += va != c.GetValue(x * 32 * 7.3, x * 32 * 3.1, 0.5);
  }
  CHECK(same);
  CHECK(differ > 32);

  // TerrainGenerator.cxx's chain, mountainAmplitude 20, seed 1.
  Perlin hp; hp.SetSeed(1); hp.SetFrequency(0.003 / 32); hp.SetLacunarity(1.5); hp.SetOctaveCount(16);
  ScaleBias hps; hps.SetSourceModule(0, hp); hps.SetScale(0.25); hps.SetBias(-0.5);
  RidgedMulti hf; hf.SetSeed(1); hf.SetFrequency(0.005 / 32); hf.SetLacunarity(2);
  ScaleBias hfs; hfs.SetSourceModule(0, hf); hfs.SetScale(20 * 0.025); hfs.SetBias(0.5);
  Perlin bp; bp.SetSeed(2); bp.SetFrequency(0.005 / 32);
  ScaleBias bs; bs.SetSourceModule(0, bp); bs.SetScale(2.0); bs.SetBias(-0.1 * 20);
  Clamp bc; bc.SetSourceModule(0, bs); bc.SetBounds(0, 1);
  Blend bl; bl.SetSourceModule(0, hps); bl.SetSourceModule(1, hfs); bl.SetControlModule(bc);
  ScaleBias sc; sc.SetSourceModule(0, bl); sc.SetScale(20.0); sc.SetBias(4.0);
  Clamp h; h.SetSourceModule(0, sc); h.SetBounds(0, 255);
  int water = 0, land = 0, outside = 0;
  for (int y = 0; y < 128; y++)
    for (int x = 0; x < 128; x++) {
      double v = h.GetValue(x * 32, y * 32, 0.5);
      outside += v < 0 || v > 255;
      if (v < 1) water++; else land++;
    }
  printf("128x128 map: %d land, %d water\n", land, water);
  CHECK(outside == 0);
  CHECK(land > 0);
  printf(fails ? "noise: %d checks failed\n" : "noise: all checks passed\n", fails);
  return fails != 0;
}
