#include "Harl.hpp"

#include <iostream>

Harl::Harl(void) {}

Harl::~Harl(void) {}

void Harl::debug(void) {
    std::cout << "[ DEBUG ]" << std::endl;
    std::cout
        << "I love having extra bacon for my "
           "7XL-double-cheese-triple-pickle-special-ketchup burger. I really do!"
        << std::endl;
}

void Harl::info(void) {
    std::cout << "[ INFO ]" << std::endl;
    std::cout << "I cannot believe adding extra bacon costs more money. "
                 "You didn't put enough bacon in my burger! If you did, I "
                 "wouldn't be asking for more!"
              << std::endl;
}

void Harl::warning(void) {
    std::cout << "[ WARNING ]" << std::endl;
    std::cout << "I think I deserve to have some extra bacon for free. I've "
                 "been coming for years whereas you started working here "
                 "since last month."
              << std::endl;
}

void Harl::error(void) {
    std::cout << "[ ERROR ]" << std::endl;
    std::cout << "This is unacceptable! I want to speak to the manager now."
              << std::endl;
}

void Harl::complain(std::string const &level) {
    const std::int N = 4;

    void (Harl::*funcs[N])(void) = {&Harl::debug, &Harl::info, &Harl::warning,
                                    &Harl::error};
    std::string const levels[N] = {"DEBUG", "INFO", "WARNING", "ERROR"};


    void (Harl::**it_funcs)(void) = funcs;
    std::string const *it_levels = levels;
    std::string const *it_end = it_levels + N;
    while (it_levels != it_end) {
        if (level == *it_levels) {
            (this->*(*it_funcs))();
            return;
        }
        ++it_funcs;
        ++it_levels;
    }
    std::cout << "Invalid level" << std::endl;
}
