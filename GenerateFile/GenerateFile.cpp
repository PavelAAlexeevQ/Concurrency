// GenerateFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <iostream>
#include <ppl.h>
#include <vector>

int main()
{
    std::ofstream outFile("rand_data.log", std::ios_base::out | std::ios_base::binary | std::ios::trunc);
    constexpr uint8_t div = std::numeric_limits<uint8_t>::max();
    std::srand( std::time(nullptr) % std::numeric_limits<unsigned int>::max());
    const int count = 1'000'000'000;
    std::cout.setf(std::ios::fixed);
    std::cout.precision(1);
    std::vector<uint8_t> buf(count);
    
    concurrency::parallel_for_each(buf.begin(), buf.end(), [div](uint8_t& v)
        {
            v = std::rand() % div;
        });
    
    outFile.write(reinterpret_cast<const char*>(buf.data()), buf.size());
}
