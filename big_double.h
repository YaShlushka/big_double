#pragma once
#include <string>
#include <algorithm>
#include <iostream>
#include <optional>
#include <cstdint>
#include <regex>

constexpr std::string ACCEPTABLE_CHARACTERS = "-.,1234567890";

class BigDouble {
public:
	BigDouble() = default;
	BigDouble(const std::string &argument, uint64_t limit = 100) {
		std::string str = argument;
		std::pair<bool, std::optional<std::string>> check = CheckDigit(str);

		if(!check.first) {
			throw std::invalid_argument(check.second.value());
		}

		std::pair<bool, std::optional<std::string>> negative_check = GetNegative(str);
		if(!negative_check.first) {
			is_negative_ = false;
		} else {
			is_negative_ = true;
			str = negative_check.second.value();
		}

		// integer_ = GetInteger(str);
		// fractional_ = GetFractional(str);
		GetNumber(str, integer_, fractional_);
		if(str.size() < 100) {
			limit_ = limit;
		} else {
			limit_ = str.size();
		}

		FractionalReduction();
	}

	BigDouble(const char *num, uint64_t limit = 100) {
		std::string str = num;
		BigDouble big_double(str, limit);
		is_negative_ = big_double.is_negative_;
		limit_ = big_double.limit_;
		integer_ = big_double.integer_;
		fractional_ = big_double.fractional_;
	}

	BigDouble operator+(BigDouble v2) {
		BigDouble v1 = *this;

		std::string result_integer;
		std::string result_fractional;

		std::string &integer1 = v1.integer_;
		std::string &integer2 = v2.integer_;

		std::string &fractional1 = v1.fractional_;
		std::string &fractional2 = v2.fractional_;

		int max = std::max(fractional1.size(), fractional2.size());

		int temp = 0;
		for(int i = max - 1; i >= 0; --i) {
			int digit1 = (i < fractional1.size()) ? (ToInt(fractional1[i])) : 0;
			int digit2 = (i < fractional2.size()) ? (ToInt(fractional2[i])) : 0;
			int sum = digit1 + digit2 + temp;

			temp = sum / 10;

			result_fractional.push_back(sum % 10 + '0');
		}

		max = std::max(integer1.size(), integer2.size());
		for(int i = max - 1; i >= 0; --i) {
			int digit1 = (i < integer1.size()) ? (ToInt(integer1[i])) : 0;
			int digit2 = (i < integer2.size()) ? (ToInt(integer2[i])) : 0;
			int sum = digit1 + digit2 + temp;

			temp = sum / 10;

			result_integer.push_back(sum % 10 + '0');
		}

		std::reverse(result_integer.begin(), result_integer.end());
		std::reverse(result_fractional.begin(), result_fractional.end());
      
		std::string result = result_integer + "." + result_fractional;
		BigDouble res(result);
		res.FractionalReduction();
		return res;
	}

	BigDouble operator-(BigDouble v2) {
		BigDouble v1 = *this;

		std::string integer1 = v1.integer_;
		std::string integer2 = v2.integer_;

		std::string fractional1 = v1.fractional_;
		std::string fractional2 = v2.fractional_;

		std::string result_integer;
		std::string result_fractional;

		bool is_negative_result = false;

		if(integer1.size() < integer2.size() || integer1 < integer2 ||
		fractional1.size() < fractional2.size() || fractional1 < fractional2) {
			std::swap(integer1, integer2);
			std::swap(fractional1, fractional2);
			is_negative_result = true;
		} else {
			return BigDouble("0");
		}

		int min_length = std::min(fractional1.size(), fractional2.size());
		std::string* max_element = min_length == fractional1.size() ? &fractional2 : &fractional1;

		for(int i = max_element->size() - 1; i >= min_length; --i) {
			result_fractional.push_back(max_element->at(i));
		}

		for(int i = min_length - 1; i >= 0; --i) {
			int num1 = ToInt(fractional1[i]);
			int num2 = ToInt(fractional2[i]);

			if(num1 < num2) {
				int n = i - 1;
				bool is_find = false;

				while(!is_find) {
					if(n < 0) {
						n = integer1.size() - 1;

						while(!is_find) {
							if(integer1[n] != '0') {
								num1 += 10;
								--integer1[n];
								is_find = true;
							} else {
								integer1[n] = '9';
							}
						}
					}

					if(fractional1[n] != '0') {
						num1 += 10;
						--fractional1[n];
						is_find = true;
					} else {
						fractional1[n] = '9';
					}
				}
			}
			result_fractional.push_back(num1 - num2);
		}

		min_length = std::min(integer1.size(), integer2.size());
		max_element = min_length == integer1.size() ? &integer2 : &integer1;

		std::reverse(integer1.begin(), integer1.end());
		std::reverse(integer2.begin(), integer2.end());


	}

	friend std::ostream& operator<<(std::ostream& out, const BigDouble &num);
	friend std::istream& operator>>(std::istream& in, BigDouble &num);

	BigDouble& operator=(const std::string &num) {
		// integer_ = GetInteger(num);
		// fractional_ = GetFractional(num);
		BigDouble temp(num);
		is_negative_ = temp.is_negative_;
		integer_ = temp.integer_;
		fractional_ = temp.fractional_;
		return *this;
	}

private:
	std::pair<bool, std::optional<std::string>> CheckDigit(const std::string &str) {
		if(str.find_first_not_of(ACCEPTABLE_CHARACTERS) != std::string::npos) {
			return {false, "not allowed character in string"};
		}

		if(str.find('-', 1) != std::string::npos) {
			return {false, "minus in middle of digit"};
		}

		int count = std::count(str.begin(), str.end(), '.') + std::count(str.begin(), str.end(), ',');

		if(count > 1) {
			return {false, "more than 1 floating point"};
		}

		return {true, std::nullopt};
	}

	std::pair<bool, std::optional<std::string>> GetNegative(const std::string &str) {
		if(str[0] == '-') {
			return {true, str.substr(1)};
		}
		return {false, std::nullopt};
	}

	void GetNumber(const std::string &str, std::string &integer, std::string &fractional) {
		std::regex pattern(R"((\d+)(?:\.(\d+))?)");
		std::smatch matches;
		std::regex_match(str, matches, pattern);
		integer = matches[1].str();
		fractional = matches[2].str();
	}

	// std::string GetInteger(const std::string &str) {
	//    auto it = str.find('.');

	//    if(it == std::string::npos) {
	//       it = str.find(',');
	//    }

	//    if(it == std::string::npos) {
	//       return str;
	//    }

	//    return str.substr(0, it);
	// }

	// std::string GetFractional(const std::string &str) {
	//    auto it = str.find('.');

	//    if(it == std::string::npos) {
	//       it = str.find(',');
	//    }

	//    if(it == std::string::npos) {
	//       return "0";
	//    }

	//    return str.substr(it+1);
	// }

	void FractionalReduction() {
		while(fractional_.size() != 0 && fractional_[fractional_.size() - 1] == '0') {
			fractional_.pop_back();
		}

		if(fractional_.size() == 0) {
			fractional_ = "0";
		}
	}

	int ToInt(const char ch) {
		if(!std::isdigit(ch)) {
			throw std::invalid_argument("A character that is not a number cannot be converted to a digit.");
		}
		return ch - '0';
	}

	char ToChar(const int num) {
		return num + '0';
	}

private:
	bool is_negative_ = false;
	uint64_t limit_ = 100;
	std::string integer_ = "0";
	std::string fractional_ = "0";
};


inline std::ostream& operator<<(std::ostream& out, const BigDouble &num) {
	out << (num.is_negative_ ? "-" : "") << num.integer_ <<
		 ((num.fractional_.size() == 1 && num.fractional_[0] == '0') ? 
		 "" : 
		 ('.' + num.fractional_));
	return out;
}

inline std::istream& operator>>(std::istream& in, BigDouble &num) {
	std::string str;
	in >> str;
	num = BigDouble(str);
	return in;
}