#include "CalcDistributionThreads.h"

#include <algorithm>
#include <thread>
#include <vector>

#include <fstream>
#include "iostream"


CalcDistributionThreads::CalcDistributionThreads(const char* fileName) : CalcDistributionStreamRead(fileName)
{
	threadsCount = std::thread::hardware_concurrency();
}

probability_distribution_t CalcDistributionThreads::CalculateDistribution()
{
	std::vector<probability_distribution_t> partialResults(threadsCount);
	{
		std::vector<std::jthread> threads(threadsCount);
		for (int c = 0; c < threadsCount; c++)
		{
			threads[c] = std::jthread([&, c]() {
				CalculateDistributionPiece(partialResults[c]);
				});
		}
	}

	probability_distribution_t result = *(partialResults.begin());
	for (auto partialResult = partialResults.begin() + 1; partialResult != partialResults.end(); partialResult++)
	{
		for (int i = 0; i < partialResult->size(); i++)
		{
			result[i] += (*partialResult)[i];
		}
	}

	auto thCount = this->threadsCount;
	std::for_each(result.begin(), result.end(), [thCount](probability_distribution_t::value_type& v)
	{
		v /= thCount;
	});

	return result;
}

void CalcDistributionThreads::CalculateDistributionPiece(probability_distribution_t& result)
{
	size_t pieceSzie = 1'000'000;
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
}

void CalcDistributionThreads::CalculateDistributionPiece(const std::vector<uint8_t>& data, probability_distribution_t& result )
{
	result.resize(0xFF);
	std::for_each(data.begin(), data.end(), [&result](auto v)
		{
			result[v]++;
		});
}


CalcDistributionThreads::~CalcDistributionThreads()
{
}