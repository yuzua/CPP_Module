#include "PhoneBook.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <limits>

namespace {
	std::string _truncateField(const std::string& value) {
		const std::size_t width = 10;

		if (value.length() > width)
			return value.substr(0, width - 1) + ".";
		return value;
	}

	void _printField(const std::string& value) {
		std::cout << std::setw(10) << std::right << _truncateField(value);
	}

	int _getArrayIndex(int displayIndex, int contactCount, int _oldestContactIndex, int maxContacts) {
		if (contactCount < maxContacts)
			return displayIndex;
		return (_oldestContactIndex + displayIndex) % maxContacts;
	}
}

PhoneBook::PhoneBook(void) : _contactCount(0), _oldestContactIndex(0) {}

void PhoneBook::addContact(const Contact& contact) {
	this->_contacts[this->_oldestContactIndex] = contact;
	this->_oldestContactIndex = (this->_oldestContactIndex + 1) % MAX_CONTACTS;
	if (this->_contactCount < MAX_CONTACTS)
		this->_contactCount++;
}

void PhoneBook::searchContact(void) const {
	if (this->_contactCount == 0)
		return;

	_printField("index");
	std::cout << '|';
	_printField("first name");
	std::cout << '|';
	_printField("last name");
	std::cout << '|';
	_printField("nickname");
	std::cout << '|' << std::endl;

	for (int i = 0; i < this->_contactCount; ++i) {
		const Contact& contact =
			this->_contacts[_getArrayIndex(i, this->_contactCount, this->_oldestContactIndex, MAX_CONTACTS)];

		std::ostringstream indexStream;
		indexStream << i;

		_printField(indexStream.str());
		std::cout << '|';
		_printField(contact.getFirstName());
		std::cout << '|';
		_printField(contact.getLastName());
		std::cout << '|';
		_printField(contact.getNickname());
		std::cout << '|' << std::endl;
	}

	std::cout << "Index: ";
	int indexInput;
	if (!(std::cin >> indexInput))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return;
	}

	if (indexInput < 0 || this->_contactCount <= indexInput)
		return;

	const Contact& selected =
		this->_contacts[_getArrayIndex(indexInput, this->_contactCount, this->_oldestContactIndex, MAX_CONTACTS)];
	selected.printDetails();
}
