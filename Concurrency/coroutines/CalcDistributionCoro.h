#pragma once
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "CalcDistributionStreamRead.h"
#include "GeneratorCoro.h"


class CalcDistributionCoroGen : public CalcDistributionStreamRead
{
public:
	CalcDistributionCoroGen(const char* fileName);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionCoroGen() override;
private:
	std::pair<probability_distribution_t, bool> CalculateDistributionPiece(void);
	Generator CoroutineFunction(void);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
