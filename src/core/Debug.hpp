#include <iostream>

class Debug 
{
public:
	static void Log(const char* message) {
		std::cout << "[DEBUG]: " << message << std::endl;
	}
};