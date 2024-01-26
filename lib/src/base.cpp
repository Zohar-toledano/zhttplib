#include "base.hpp"

char* intToStr(int num) {
	std::ostringstream ss;
	ss << num;
	std::string str = ss.str();
	// Allocate memory for the string + null terminator.
	char* cstr = new char[str.length() + 1];
	// Copy the string to the allocated memory.
	strcpy_s(cstr,str.size()+1, str.c_str());
	return cstr;
}
