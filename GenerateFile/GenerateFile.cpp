// GenerateFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <cstdlib>
#include <fstream>
#include <limits>
#include <iostream>

int main()
{
    std::ofstream outFile("rand_data.log", std::ios_base::out | std::ios_base::binary | std::ios::trunc);
    constexpr int8_t div = std::numeric_limits<int8_t>::max();
    std::srand( std::time(nullptr) % std::numeric_limits<unsigned int>::max());
    const int count = 1'000'000'000;
    std::cout.setf(std::ios::fixed);
    std::cout.precision(1);
    for (int n = 0; n < 1'000'000; n++)
    {
        int8_t v = std::rand() % div;
        outFile.write(reinterpret_cast<const char *>(&v), sizeof(v));

        if (n % 1'000'000 == 0)
        {
            std::cout << n / (double)count * 100.0 << '%' << std::endl;
        }
    }
}
