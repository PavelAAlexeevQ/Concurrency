#pragma once
#include <fstream>

#include "ICalcDistribution.h"

class CalcDistributionStreamRead : public ICalcDistribution
{
public:
	CalcDistributionStreamRead(const char* fileName);
protected:
	std::ifstream stream;
};
