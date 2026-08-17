#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "Contact.hpp"

class PhoneBook {
private:
	static const int MAX_CONTACTS = 8;
	Contact _contacts[MAX_CONTACTS];
	int _contactCount;
	int _oldestContactIndex;

public:
	PhoneBook(void);
	void addContact(const Contact& contact);
	void searchContact(void) const;
};

#endif
