#pragma once
#include <mutex>

#include "CalcDistributionStreamRead.h"

class CalcDistributionAsync : public CalcDistributionStreamRead
{
public:
	CalcDistributionAsync(const char* fileName);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionAsync() override;
private:
	int threadsCount;
	std::mutex lockStream;

	probability_distribution_t CalculateDistributionPiece(void);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
