#include "big_double.h"

void Test(const bool condition, const std::string &error_text, const std::string &name) {
	std::cout << name << ": "
				 << (condition ? ("PASS") : ("FAIL: \"" + error_text + "\""))
				 << std::endl;
}

int main() {
	{
		BigDouble a("593");
		BigDouble b("395");
		Test(a + b == "988")
	}
}