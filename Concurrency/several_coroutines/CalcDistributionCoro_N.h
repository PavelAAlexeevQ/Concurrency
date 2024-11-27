#pragma once
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "CalcDistributionStreamRead.h"
#include "../coroutines/GeneratorCoro.h"


class CalcDistributionCoroGen_N : public CalcDistributionStreamRead
{
public:
	CalcDistributionCoroGen_N(const char* fileName);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionCoroGen_N() override;
private:
	std::pair<probability_distribution_t, bool> CalculateDistributionPiece(void);
	Generator CoroutineFunction(void);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
