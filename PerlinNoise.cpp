#include "PerlinNoise.h"
		
PerlinNoise::PerlinNoise(int seed)
{
	myModule.SetSeed(seed);
}

Uint8 PerlinNoise::getNoise(double x, double y, double z)
{
		double value = (myModule.GetValue (x, y, z)+1)*127.5;
		if(value > 255) value = 255;
		if( value < 0) value = 0;
		return (Uint8)value;
}

