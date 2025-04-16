#include "big_double.h"

void Test(const bool condition, const std::string &name, const std::string &error_text) {
	std::cout << name << ": "
				 << (condition ? ("PASS") : ("FAIL: \"" + error_text + "\""))
				 << std::endl;
}

int main() {
	{
		{
			BigDouble a("593");
			BigDouble b("395");
			Test(a + b == BigDouble("988"), "INTEGER ADDITION", "error in compare 593 + 395 == 988");
		}

		{
			BigDouble a("0.58273");
			BigDouble b("0.40021456");
			Test(a + b == BigDouble("0.98294456"), "FRACTIONAL ADDITION", "error in compare 0.58273 + 0.40021456 == 0.98294456");
		}

		{
			BigDouble a("9014.95821");
			BigDouble b("92.4029210491");
			Test(a + b == BigDouble("9107.3611310491"), "MIXED ADDITION", "error in compare 9014.95821 + 92.4029210491 == 9107.3611310491");
		}

		{
			BigDouble a("9014.95821");
			BigDouble b("92.4029210491");
			Test(a + -b == BigDouble("8922.5552889509"), "MIXED ADDITION WITH ONE NEGATIVE OPERAND", "error in compare 9014.95821 + -92.4029210491 == 8922.5552889509");
		}
	}

	{
		{
			BigDouble a("593");
			BigDouble b("395");
			Test(a - b == BigDouble("198"), "INTEGER SUBTRACTION", "error in compare 593 - 395 == 198");
		}

		{
			BigDouble a("0.58273");
			BigDouble b("0.40021456");
			Test(a - b == BigDouble("0.18251544"), "FRACTIONAL SUBTRACTION", "error in compare 0.58273 - 0.40021456 == 0.18251544");
		}

		{
			BigDouble a("9014.95821");
			BigDouble b("92.4029210491");
			Test(a - b == BigDouble("8922.5552889509"), "MIXED SUBTRACTION", "error in compare 9014.95821 - 92.4029210491 == 8922.5552889509");
		}
	}
}