#include "CalcDistributionStreamRead.h"

#include <fstream>


CalcDistributionStreamRead::CalcDistributionStreamRead(const char* fileName)
{
	stream.open("rand_data.log", std::ios::binary | std::ios::in);
	stream.seekg(std::ios::beg);
}