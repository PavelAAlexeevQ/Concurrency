#pragma once
#include <vector>

typedef std::vector<double> probability_distribution_t;

class ICalcDistribution
{
public:
	virtual probability_distribution_t CalculateDistribution() = 0;
	virtual ~ICalcDistribution() {};
};
