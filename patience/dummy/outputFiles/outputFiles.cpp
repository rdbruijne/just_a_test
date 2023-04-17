#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
	for(int i = 1; i < argc; i++)
	{
		const char* path = argv[i];
		std::cout << "Writing test output to " << path << std::endl;
		std::ofstream stream(path);
		if(!stream.good())
		{
			std::cerr << "Failed to open file" << std::endl;
			continue;
		}
		stream << "test output" << std::endl;
	}
	return 0;
}
