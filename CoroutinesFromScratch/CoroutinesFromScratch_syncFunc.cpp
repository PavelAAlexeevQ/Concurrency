// CoroutinesFromScratch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

int SyncResult() {
	return 10;
}

int Function() // just sync function
{
	std::cout << "SyncFunction called\n";
	int stack_val = 0xDEADBEEF;
	auto res = SyncResult();
	return res;
}

int main()
{
	auto res = Function();
    std::cout <<"Result = " << res << "\n";
}
