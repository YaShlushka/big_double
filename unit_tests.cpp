#include "big_double.h"

void Test(const bool condition, const std::string &error_text, const std::string &name) {
	std::cout << name << ": ";

	if(condition) {
		std::cout << "PASS" << std::endl;
		return;
	}
	
	std::cout << "fail : \"" << error_text << "\"" << std::endl;
}