#include "strings.h"

#include <iostream>

int main(int argc, char** argv)
{
	std::vector<int> numbers;
	numbers.reserve(argc);

	// parse arguments
	for(int i = 1; i < argc; i++)
	{
		try
		{
			const int v = std::stoi(argv[i]);
			numbers.push_back(v);
		}
		catch(std::exception e)
		{
			std::cerr << "Argument " << i <<  "\"" << argv[i] << "\" is not a valid integer." << std::endl;
		}
	}

	// check validity
	if(numbers.size() == 0)
	{
		std::cerr << "Arguments do not contain numbers" << std::endl;
		return -1;
	}

	// print result
	const int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
	std::cout << join(numbers, " + ") << " = " << sum << std::endl;
	return 0;
}
