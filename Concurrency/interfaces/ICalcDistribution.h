#pragma once
#include "DataTypes.h"

class ICalcDistribution
{
public:
	virtual probability_distribution_t CalculateDistribution() = 0;
	virtual ~ICalcDistribution() {};
};
