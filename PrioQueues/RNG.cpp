#include "RNG.h"

#define ever ;;			

namespace uniza_fri {


	RNG::RNG(unsigned long seed) :
		generator(seed)
	{
	}

	size_t RNG::nextSizeT()
	{
		return this->unifromDist(this->generator);
	}

	size_t RNG::nextSizeT(size_t min, size_t max)
	{
		return this->nextSizeT() % (max - min + 1) + min;
	}

	size_t RNG::nextUniqueSizeT(size_t min, size_t max)
	{
		size_t num;

		for (ever)
		{
			num = this->nextSizeT(min, max);
			if (this->yetGenerated.find(num) == this->yetGenerated.end()) break;
		}

		this->yetGenerated.insert(num);

		return num;
	}

}