#pragma once
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "ICalcDistribution.h"

class CalcDistributionAsync : public ICalcDistribution
{
public:
	CalcDistributionAsync(std::istream& s);
	virtual probability_distribution_t CalculateDistribution() override;
	virtual ~CalcDistributionAsync() override;
private:
	int threadsCount;
	std::istream& stream;
	std::mutex lockStream;

	probability_distribution_t CalculateDistributionPiece(void);
	void CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result);
};
