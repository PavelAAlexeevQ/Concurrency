#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>


#include "ICalcDistribution.h"
#include "CalcDistributionThreads.h"

int main()
{
    std::ifstream dataFile("rand_data.log", std::ios::binary | std::ios::in);
    dataFile.seekg(std::ios::beg);
    
    CalcDistributionThreads cdf(dataFile);
    probaility_distribution_t unorderderResults = cdf.CalculateDistribution();
    
    std::vector<std::pair<int8_t, double>> ordertedResults(unorderderResults.size());
    std::copy(unorderderResults.begin(), unorderderResults.end(), ordertedResults.begin());
    std::sort(ordertedResults.begin(), ordertedResults.end(), [](const auto& v1, const auto& v2) {
        return v1.first < v2.first;
        });

    std::for_each(ordertedResults.begin(), ordertedResults.end(), [](const auto& v) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << int(v.first) << "\t" 
            << std::dec << v.second << std::endl;
        });

}
