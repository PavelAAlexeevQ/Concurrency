#include "CalcDistributionThreads.h"

#include <algorithm>
#include <thread>
#include <vector>

#include "iostream"


CalcDistributionThreads::CalcDistributionThreads(std::istream& s) : stream(s)
{
	threadsCount = 1;// std::thread::hardware_concurrency();
}

probaility_distribution_t CalcDistributionThreads::CalculateDistribution()
{
	std::vector<probaility_distribution_t> partialResults(threadsCount);
	for (int c = 0; c < threadsCount; c++)
	{
		partialResults[c] = CalculateDistributionPeice();
	}

	probaility_distribution_t result = *(partialResults.begin());
	for (auto c = partialResults.begin() + 1; c != partialResults.end(); c++)
	{
		std::for_each(c->begin(), c->end(), [&](probaility_distribution_t::value_type &v)
			{
				result[v.first] += v.second;
			});
	}

	auto thCount = this->threadsCount;
	std::for_each(result.begin(), result.end(), [thCount](probaility_distribution_t::value_type& v)
	{
		v.second /= thCount;
	});



	return result;
}

probaility_distribution_t CalcDistributionThreads::CalculateDistributionPeice()
{
	size_t pieceSzie = 750'000;
	std::vector<int8_t> pieceOfData(pieceSzie);

	probaility_distribution_t result;

	size_t processed = 0;
	while (stream) {
		pieceOfData.resize(pieceSzie);
		stream.read(reinterpret_cast<char*>( &(pieceOfData[0]) ), pieceOfData.size());
		auto read_len = stream.gcount();
		if (read_len > 0) {
			processed += read_len;
			pieceOfData.resize(read_len);
			CalculateDistributionPeice(pieceOfData, result);
		}
		else
		{
			break;
		}
	}


	std::for_each(result.begin(), result.end(), [processed](probaility_distribution_t::value_type& v)
		{
			v.second /= (double)processed;
		});

	return result;
}

void CalcDistributionThreads::CalculateDistributionPeice(const std::vector<int8_t>& data, probaility_distribution_t& result )
{
	std::for_each(data.begin(), data.end(), [&result](auto v)
		{
			result[v]++;
		});
}


CalcDistributionThreads::~CalcDistributionThreads()
{
}