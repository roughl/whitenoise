#ifndef WHITENOISE_H
#define WHITENOISE_H
#include "INoise.h"
#include <random>
class WhiteNoise : INoise {
	public:
		WhiteNoise(int seed);
		Uint8 getNoise(double x, double y, double z);
	private:
		std::mt19937 m_rng;
		std::uniform_int_distribution<uint8_t> m_dist;
};
#endif //WHITENOISE_H

