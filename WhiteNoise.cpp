#include "WhiteNoise.h"

WhiteNoise::WhiteNoise(int seed)
{
	m_rng.seed(seed);
}
		
Uint8 WhiteNoise::getNoise(double x, double y, double z) {
	return m_dist(m_rng);
}
