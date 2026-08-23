#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <string>

class Zombie {
    private:
        std::string _name;
	public:
		Zombie();
		Zombie(const std::string& name);
		~Zombie();
		void setName(const std::string& name);
		void announce(void) const;
};

#endif
