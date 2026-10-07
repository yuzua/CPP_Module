#ifndef BRAIN_HPP
#define BRAIN_HPP

#include <string>
#include <iostream>

class Brain {
    private:
        static const int size = 100;
        std::string ideas_[size];
    public:
        Brain();
        Brain(const Brain &other);
        virtual ~Brain();
        Brain &operator=(const Brain &other);
        void setIdea(int index, std::string const &idea);
        std::string const &getIdea(int index) const;
};

#endif