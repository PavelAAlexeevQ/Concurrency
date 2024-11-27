#pragma once
#include "CalcDistributionStreamRead.h"

#include <mutex>

class CalcDistributionThreads : public CalcDistributionStreamRead
{
public:
	CalcDistributionThreads(const char *fileName);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionThreads() override;
private:
	int threadsCount;
	std::mutex lockStream;

	void CalculateDistributionPiece(probability_distribution_t& result);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
