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

	void CalculateDistributionPiece(probaility_distribution_t& result);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probaility_distribution_t& result);
};
