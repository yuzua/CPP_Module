#include <iostream>
#include <string>

int main(void) {
	std::string const str = "HI THIS IS BRAIN";
	std::string const* stringPTR = &str;
	std::string const& stringREF = str;

	std::cout << "string address: " << &str << std::endl;
	std::cout << "stringPTR address: " << stringPTR << std::endl;
	std::cout << "stringREF address: " << &stringREF << std::endl;

	std::cout << "string value: " << str << std::endl;
	std::cout << "stringPTR value: " << *stringPTR << std::endl;
	std::cout << "stringREF value: " << stringREF << std::endl;

	return 0;
}
