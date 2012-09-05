#ifndef NORMALNOISE_H
#define NORMALNOISE_H
#include "INoise.h"
#include <random>
class NormalNoise : INoise {
	public:
		NormalNoise(int seed);
		Uint8 getNoise(double x, double y, double z);
	private:
		std::mt19937 m_rng;
		std::normal_distribution<double> m_dist;
};
#endif //NORMALNOISE_H

