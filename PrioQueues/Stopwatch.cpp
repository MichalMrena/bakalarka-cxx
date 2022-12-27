#include <chrono>

#include "Stopwatch.h"

namespace uniza_fri {

	Stopwatch::Stopwatch()
	{
		this->timeStart = std::chrono::high_resolution_clock::now();
	}

	long long Stopwatch::getTime() const
	{
		const auto timeEnd = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - this->timeStart).count();
	}

}