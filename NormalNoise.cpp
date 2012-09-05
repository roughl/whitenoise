#include "NormalNoise.h"

NormalNoise::NormalNoise(int seed)
	:m_dist(127.5, 50)
{
	m_rng.seed(seed);
}
		
Uint8 NormalNoise::getNoise(double x, double y, double z) {
	return m_dist(m_rng);
}

