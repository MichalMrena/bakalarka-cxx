#pragma once
#include <chrono>

namespace uniza_fri {

	/*
		Jednoduch� stopky. Meraj� �as od vytvorenia in�tancie v milisekund�ch.
	 */
	class Stopwatch
	{
	private:

		std::chrono::time_point<std::chrono::steady_clock> timeStart;

	public:

		Stopwatch();
		~Stopwatch() = default;

		long long getTime() const;

	};

}