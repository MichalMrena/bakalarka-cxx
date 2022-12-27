#pragma once
#include <chrono>

namespace uniza_fri {

	/*
		Jednoduché stopky. Merajú èas od vytvorenia inštancie v milisekundách.
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