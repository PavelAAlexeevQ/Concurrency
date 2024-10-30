#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>


#include "ICalcDistribution.h"
#include "jthreads\CalcDistributionThreads.h"
#include "async\CalcDistributionAsync.h"
#include "coroutines\CalcDistributionCoro.h"
#include "several_coroutines/CalcDistributionCoro_N.h"

int main()
{
    std::ifstream dataFile("rand_data.log", std::ios::binary | std::ios::in);
    dataFile.seekg(std::ios::beg);
    
    std::shared_ptr<ICalcDistribution> iCalcDistribution = nullptr;

    dataFile.clear();
    dataFile.seekg(std::ios::beg);
    iCalcDistribution.reset(new CalcDistributionCoro_N(dataFile));
    probability_distribution_t coroResultsN = iCalcDistribution->CalculateDistribution();

    dataFile.clear();
    dataFile.seekg(std::ios::beg); 
    iCalcDistribution.reset(new CalcDistributionThreads(dataFile));
    probability_distribution_t threadResults = iCalcDistribution->CalculateDistribution();
 
    dataFile.clear();
    dataFile.seekg(std::ios::beg);
    iCalcDistribution.reset(new CalcDistributionAsync(dataFile));
    probability_distribution_t asyncResults = iCalcDistribution->CalculateDistribution();

    dataFile.clear();
    dataFile.seekg(std::ios::beg);
    iCalcDistribution.reset(new CalcDistributionCoro(dataFile));
    probability_distribution_t coroResults = iCalcDistribution->CalculateDistribution();

	std::cout << "char" << "\t" << "Threads" << "\t\t" << "Async" << "\t\t" << "Coro" << "\t\t" << "Coro N" << std::endl;
    
    for (size_t i = 0; i < threadResults.size(); i++)
    {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << i << "\t" 
            << std::dec << threadResults[i] << '\t' 
            << asyncResults[i] << '\t'
            << coroResults[i] << '\t'
            << coroResultsN[i] << std::endl;
    }

}
