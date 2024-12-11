#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>


#include "ICalcDistribution.h"
#include "jthreads\CalcDistributionThreads.h"
#include "async\CalcDistributionAsync.h"
#include "coroutines\CalcDistributionCoro.h"
#include "several_coroutines\CalcDistributionCoro_N.h"
#include "coroutines_await\CalcDistributionCoroAwait.h"

int main()
{
	const char* fileName = "rand_data.log";
    
    std::shared_ptr<ICalcDistribution> iCalcDistribution = nullptr;

    iCalcDistribution.reset(new CalcDistributionCoroAwait(fileName));
    probability_distribution_t coroAwaitResults = iCalcDistribution->CalculateDistribution();

    iCalcDistribution.reset(new CalcDistributionThreads(fileName));
    probability_distribution_t threadResults = iCalcDistribution->CalculateDistribution();
 
    iCalcDistribution.reset(new CalcDistributionAsync(fileName));
    probability_distribution_t asyncResults = iCalcDistribution->CalculateDistribution();

    iCalcDistribution.reset(new CalcDistributionCoroGen(fileName));
    probability_distribution_t coroResults = iCalcDistribution->CalculateDistribution();

    iCalcDistribution.reset(new CalcDistributionCoroGen_N(fileName));
    probability_distribution_t coroResultsN = iCalcDistribution->CalculateDistribution();


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
