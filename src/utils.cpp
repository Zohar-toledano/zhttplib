#include <utils.hpp>
#include <sstream>
char *intToStr(int num)
{
	// TODO: fix this fucking function
	std::ostringstream ss;
	ss << num;
	std::string str = ss.str();
	// Allocate memory for the string + null terminator.
	char *cstr = new char[str.length() + 1];
	// Copy the string to the allocated memory.
	strcpy_s(cstr, str.size() + 1, str.c_str());
	return cstr;
}
int getCorrectArgc(int argc, char *argv[])
{
	int i = 1;
	while (i < argc)
	{
		if (std::string(argv[i]) == "worker")
			break;
		i++;
	}
	return i;
}

void fillBufferWithRandomChars(char *buffer, size_t size)
{
	std::random_device rd;
	std::mt19937 gen(rd());							// Mersenne Twister engine
	std::uniform_int_distribution<int> dist(0, 61); // 26 letters + 10 digits

	for (size_t i = 0; i < size; ++i)
	{
		int randomIndex = dist(gen);
		char randomChar;

		if (randomIndex < 26)
		{
			randomChar = 'A' + randomIndex; // Uppercase letters
		}
		else if (randomIndex < 52)
		{
			randomChar = 'a' + (randomIndex - 26); // Lowercase letters
		}
		else
		{
			randomChar = '0' + (randomIndex - 52); // Digits
		}

		buffer[i] = randomChar;
	}
	// null terminator
	buffer[size - 1] = '\0';
}
