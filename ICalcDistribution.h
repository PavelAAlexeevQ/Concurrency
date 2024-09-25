#pragma once
#include <unordered_map>

typedef std::unordered_map<int8_t, double> probaility_distribution_t;

class ICalcDistribution
{
public:
	//ICalcDistribution() = delete;

	virtual probaility_distribution_t CalculateDistribution() = 0;
	virtual ~ICalcDistribution() {};
};
