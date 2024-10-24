#include "CalcDistributionAsync.h"

#include <algorithm>
#include <future>
#include <vector>

#include "iostream"


CalcDistributionAsync::CalcDistributionAsync(std::istream& s) : stream(s)
{
	threadsCount = std::thread::hardware_concurrency();
}

probability_distribution_t CalcDistributionAsync::CalculateDistribution()
{
	std::vector<std::future<probability_distribution_t>> partialResultsFutures(threadsCount);
	for (int c = 0; c < threadsCount; c++)
	{
		partialResultsFutures[c] = std::async(std::launch::async, [this]() {
			return this->CalculateDistributionPiece(); });
	}

	probability_distribution_t result(std::numeric_limits<uint8_t>::max());
	for (auto c = partialResultsFutures.begin(); c != partialResultsFutures.end(); c++)
	{
		probability_distribution_t partialResults = c->get();
		for (size_t i = 0; i < partialResults.size(); i++)
		{
			result[i] += partialResults[i];
		}
	}

	auto thCount = this->threadsCount;
	std::for_each(result.begin(), result.end(), [thCount](probability_distribution_t::value_type& v)
	{
		v /= thCount;
	});

	return result;
}

probability_distribution_t CalcDistributionAsync::CalculateDistributionPiece(void)
{
	probability_distribution_t result(0xFF);
	size_t pieceSzie = 100'000;
	std::vector<uint8_t> pieceOfData(pieceSzie);

	size_t processed = 0;
	while (true) {
		pieceOfData.resize(pieceSzie);
		std::streamsize read_len;
		lockStream.lock();
		if (stream)
		{
			stream.read(reinterpret_cast<char*>(&(pieceOfData[0])), pieceOfData.size());
			read_len = stream.gcount();
			lockStream.unlock();
		}
		else
		{
			lockStream.unlock(); 
			break;
		}
		if (read_len > 0) {
			processed += read_len;
			pieceOfData.resize(read_len);
			CalculateDistributionPiece(pieceOfData, result);
		}
		else
		{
			break;
		}
	}

	std::for_each(result.begin(), result.end(), [processed](probability_distribution_t::value_type& v)
		{
			v /= (double)processed;
		});

	return result;
}

void CalcDistributionAsync::CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result )
{
	std::for_each(data.begin(), data.end(), [&result](auto v)
		{
			result[v]++;
		});
}


CalcDistributionAsync::~CalcDistributionAsync()
{
}