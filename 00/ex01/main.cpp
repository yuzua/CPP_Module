#include "Contact.hpp"
#include "PhoneBook.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

static std::string readNonEmptyLine(const std::string& prompt) {
	std::string line;

	while (true) {
		std::cout << prompt;
		if (!std::getline(std::cin, line))
			exit(0);
		if (!line.empty())
			return line;
	}
}

static Contact promptContact(void) {
	const std::string firstName = readNonEmptyLine("Enter first name: ");
	const std::string lastName = readNonEmptyLine("Enter last name: ");
	const std::string nickname = readNonEmptyLine("Enter nickname: ");
	const std::string phoneNumber = readNonEmptyLine("Enter phone number: ");
	const std::string darkestSecret = readNonEmptyLine("Enter darkest secret: ");

	return Contact(firstName, lastName, nickname, phoneNumber, darkestSecret);
}

int main(void) {
	PhoneBook phoneBook;
	std::string command;

	while (true) {
		std::cout << "Enter a command: " << std::flush;
		if (!std::getline(std::cin, command))
			break;

		if (command == "ADD") {
			phoneBook.addContact(promptContact());
		} else if (command == "SEARCH") {
			phoneBook.searchContact();
		} else if (command == "EXIT") {
			break;
		}
	}

	return 0;
}
