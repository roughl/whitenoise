#ifndef INOISE_H
#define INOISE_H
#include "SDL/SDL.h"

class INoise {
	public:
		virtual Uint8 getNoise(double x, double y, double z) = 0;
};

#endif //INOISE_H

