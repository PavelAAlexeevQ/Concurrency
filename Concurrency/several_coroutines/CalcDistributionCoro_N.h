#pragma once
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "ICalcDistribution.h"
#include "../coroutines/GeneratorCoro.h"


class CalcDistributionCoro_N : public ICalcDistribution
{
public:
	CalcDistributionCoro_N(std::istream& s);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionCoro_N() override;
private:
	std::istream& stream;

	std::pair<probability_distribution_t, bool> CalculateDistributionPiece(void);
	Generator CoroutineFunction(void);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
