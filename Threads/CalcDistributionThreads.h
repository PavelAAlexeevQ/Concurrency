#pragma once
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "ICalcDistribution.h"

class CalcDistributionThreads : public ICalcDistribution
{
public:
	CalcDistributionThreads(std::istream& s);
	virtual probaility_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionThreads() override;
private:
	int threadsCount;
	std::istream& stream;
	std::mutex lockStream;

	void CalculateDistributionPeice(probaility_distribution_t& result);
	void CalculateDistributionPeice(const std::vector<int8_t>& data, probaility_distribution_t& result);
};
