#include "File.hpp"
#include <fstream>

void File::LoadFile(const char* filename, char*& output)
{
	std::ifstream stream(filename, std::ios::binary);

	if (stream.is_open())
	{
		stream.seekg(0, stream.end);
		int length = stream.tellg();
		stream.seekg(0, stream.beg);

		output = new char[length + 1];
		stream.read(output, length);

		stream.read(output, length);
		output[length] = '\0';

		stream.close();
	}
	else
	{
		output = NULL;
	}
}