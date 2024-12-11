#pragma once
#include <string>

#include "AwaiterCoro.h"
#include "ICalcDistribution.h"

class CalcDistributionCoroAwait : public ICalcDistribution
{
public:
	CalcDistributionCoroAwait(const char* fileName);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionCoroAwait() override;
private:
	std::string fileName;
	Task ReadDataTask(void);
	RequestAwaitable ReadDataAsync(void);

	std::pair<probability_distribution_t, bool> CalculateDistributionPiece(void);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
