#include "CalcDistributionCoro.h"
#include "PromiseCoro.h"

#include <algorithm>
#include <coroutine>
#include <exception>
#include "iostream"
#include <future>
#include <vector>


CalcDistributionCoro::CalcDistributionCoro(std::istream& s) : stream(s)
{
}

Generator CalcDistributionCoro::CoroutineFunction(void) 
{
	while (true)
	{
		auto result = CalculateDistributionPiece();
		bool finished = result.second;
		if (!finished)
		{
			co_yield result.first;
		}
		else
		{
			break;
		}
	} 
}

probability_distribution_t CalcDistributionCoro::CalculateDistribution()
{
	Generator myCoroutineResult = CoroutineFunction();
	std::vector<probability_distribution_t> partialResults; 
	
	while (myCoroutineResult.next()) {
		probability_distribution_t partialResult = myCoroutineResult.value();
		partialResults.push_back(partialResult);
	}

	probability_distribution_t result = *(partialResults.begin());
	for (auto partialResult = partialResults.begin() + 1; partialResult != partialResults.end(); partialResult++)
	{
		for (size_t i = 0; i < partialResult->size(); i++)
		{
			result[i] += (*partialResult)[i];
		}
	}

	auto piecesCount = partialResults.size();
	std::for_each(result.begin(), result.end(), [piecesCount](probability_distribution_t::value_type& v)
	{
		v /= piecesCount;
	});

	return result;
}

std::pair<probability_distribution_t, bool> CalcDistributionCoro::CalculateDistributionPiece(void)
{
	probability_distribution_t result(std::numeric_limits<uint8_t>::max());
	size_t pieceSzie = 1'000'000;
	std::vector<uint8_t> pieceOfData(pieceSzie);
	bool finished = false;
	size_t processed = 0;

	pieceOfData.resize(pieceSzie);
	std::streamsize read_len;

	if (stream)
	{
		stream.read(reinterpret_cast<char*>(&(pieceOfData[0])), pieceOfData.size());
		read_len = stream.gcount();
	}
	else
	{
		finished = true;
	}
	if (read_len > 0) {
		processed += read_len;
		pieceOfData.resize(read_len);
		CalculateDistributionPiece(pieceOfData, result);
	}
	else
	{
		finished = true;
	}

	std::for_each(result.begin(), result.end(), [processed](probability_distribution_t::value_type& v)
		{
			v /= (double)processed;
		});

	return std::make_pair(result, finished);
}

void CalcDistributionCoro::CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result )
{
	std::for_each(data.begin(), data.end(), [&result](auto v)
		{
			result[v]++;
		});
}


CalcDistributionCoro::~CalcDistributionCoro()
{
}