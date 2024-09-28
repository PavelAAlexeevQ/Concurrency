#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>


#include "ICalcDistribution.h"
#include "jthreads\CalcDistributionThreads.h"
#include "async\CalcDistributionAsync.h"

int main()
{
    std::ifstream dataFile("rand_data.log", std::ios::binary | std::ios::in);
    dataFile.seekg(std::ios::beg);
    
    dataFile.seekg(std::ios::beg);
    std::shared_ptr<ICalcDistribution> iCalcDistribution(new CalcDistributionThreads(dataFile));
    probaility_distribution_t threadResults = iCalcDistribution->CalculateDistribution();
 
    dataFile.clear();
    dataFile.seekg(std::ios::beg);
    iCalcDistribution.reset(new CalcDistributionAsync(dataFile));
    probaility_distribution_t asyncResults = iCalcDistribution->CalculateDistribution();

    for (size_t i = 0; i < threadResults.size(); i++)
    {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << i << "\t" 
            << std::dec << threadResults[i] << '\t' << asyncResults[i] << std::endl;
    }

}
