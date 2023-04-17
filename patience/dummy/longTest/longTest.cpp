#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv)
{
	using namespace std::chrono_literals;

	int sleepDuration = 1000;
	if(argc >= 2)
	{
		try
		{
			sleepDuration = std::stoi(argv[1]);
		}
		catch(std::exception& e)
		{
			std::cerr << "Argument must be a valid number: " << argv[1] << std::endl;
		}
	}

	std::cout << "Start waiting for " << sleepDuration << " ms..." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
	std::cout << "Done waiting..." << std::endl;
	return 0;
}
