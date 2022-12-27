#pragma once
#include <random>
#include <unordered_set>

namespace uniza_fri {

	class RNG
	{
	private:

		std::mt19937 generator;
		std::uniform_int_distribution<size_t> unifromDist;
		std::unordered_set<size_t> yetGenerated;

	public:

		RNG(unsigned long seed = 144);

		size_t nextSizeT();
		size_t nextSizeT(size_t min, size_t max);
		size_t nextUniqueSizeT(size_t min, size_t max);

	};

}