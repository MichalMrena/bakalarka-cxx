#pragma once
#include <iostream>
#include <cmath>

namespace uniza_fri {
	
	/*
		Trieda, ktorá zbiera hodnoty a postupne z nich vypoèítava aritmetický priemer.
		Taktiež sleduje, èi už je priemer stabilizovaný. Podmienka stability je,
		že za posledných StableCount hodnôt sa v priemere nezmenila posledá èíslica.

		Print -> ak je nastavený na true, priebežne vypisuje príjmané hodnoty.
	 */
	template<int StableCount, bool Print = false>
	class ValueStabilizer
	{
	private:

		size_t valueCount;
		int stableInRowCount;
		double lastAverage;

	public:

		ValueStabilizer();
		~ValueStabilizer() = default;

		void addValue(long long val);
		bool isStable();
		double getLastAvg() const;

	private:

		void checkStability(double newPriemer);
		bool areStableLastDigit(double newPriemer, double oldPriemer) const;
		bool areStableRelativeDiff(double newPriemer, double oldPriemer) const;
		static int getLastDigit(double val);

	};

	//
	// ValueStabilizer
	//
	template<int Length, bool Print>
	ValueStabilizer<Length, Print>::ValueStabilizer() :
		valueCount(0),
		stableInRowCount(0),
		lastAverage(-1)
	{
	}

	template<int Length, bool Print>
	void ValueStabilizer<Length, Print>::addValue(long long val)
	{
		double newPriemer;

		if (this->valueCount == 0)
		{
			newPriemer = static_cast<double>(val);
		}
		else
		{
			// newPriemer = ( n * oldPriemer + val) / (n + 1)
			newPriemer = (this->valueCount * this->lastAverage + val) / (this->valueCount + 1);
		}

		if (Print)
		{
			std::cout << newPriemer << std::endl;
		}

		++this->valueCount;
		this->checkStability(newPriemer);
	}

	template<int Length, bool Print>
	bool ValueStabilizer<Length, Print>::isStable()
	{
		return this->stableInRowCount >= Length;
	}

	template<int Length, bool Print>
	double ValueStabilizer<Length, Print>::getLastAvg() const
	{
		return this->lastAverage;
	}

	template<int Length, bool Print>
	int ValueStabilizer<Length, Print>::getLastDigit(double val)
	{
		return static_cast<int>(val) % 10;
	}

	template<int Length, bool Print>
	void ValueStabilizer<Length, Print>::checkStability(double newPriemer)
	{
		if (this->areStableLastDigit(newPriemer, this->lastAverage))
		{
			++this->stableInRowCount;
		}
		else
		{
			this->stableInRowCount = 0;
		}

		this->lastAverage = newPriemer;
	}

	template<int Length, bool Print>
	bool ValueStabilizer<Length, Print>::areStableLastDigit(const double newPriemer, const double oldPriemer) const
	{
		return this->getLastDigit(newPriemer) == this->getLastDigit(oldPriemer);
	}

	template<int Length, bool Print>
	bool ValueStabilizer<Length, Print>::areStableRelativeDiff(const double newPriemer, const double oldPriemer) const
	{
		const double threshold = 1.0 / 300.0;
		const double diff = abs(newPriemer - oldPriemer);
		const double relativeChange = diff / oldPriemer;

		return relativeChange < threshold;
	}

}

