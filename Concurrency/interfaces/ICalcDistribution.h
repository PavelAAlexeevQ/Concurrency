#pragma once
#include <vector>

typedef std::vector<double> probaility_distribution_t;

class ICalcDistribution
{
public:
	//ICalcDistribution() = delete;

	virtual probaility_distribution_t CalculateDistribution() = 0;
	virtual ~ICalcDistribution() {};
};
