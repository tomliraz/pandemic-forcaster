#include "Math.h"

float Math::Random()
{
	std::mt19937 rng((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	return ((float)rng() / (float)rng.max());
}
