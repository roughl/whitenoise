#ifndef PERLINNOISE_H
#define PERLINNOISE_H
#include "INoise.h"
#include <noise/noise.h>
using namespace noise;
class PerlinNoise : INoise {
	public:
		PerlinNoise(int seed);
		Uint8 getNoise(double x, double y, double z);
	private:
		module::Perlin myModule;
};
#endif //PERLINNOISE_H

