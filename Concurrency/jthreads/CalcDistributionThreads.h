#pragma once
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "ICalcDistribution.h"

class CalcDistributionThreads : public ICalcDistribution
{
public:
	CalcDistributionThreads(std::istream& s);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionThreads() override;
private:
	int threadsCount;
	std::istream& stream;
	std::mutex lockStream;

	void CalculateDistributionPiece(probability_distribution_t& result);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
