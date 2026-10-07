#ifndef BRAIN_HPP
#define BRAIN_HPP

#include <string>

// アイデアを 100 個持つ値。文字列自体が中身のメモリを管理するので、
// Brain のコピーは文字列のコピーであり、別の深いコピーは不要。
// 所有される側であり、自分で new しない。
class Brain {
    public:
        static int const kIdeaCount = 100;

        Brain(void);
        Brain(Brain const &other);
        ~Brain(void);
        Brain &operator=(Brain const &other);

        void               setIdea(int index, std::string const &idea);
        std::string const &getIdea(int index) const;

    private:
        std::string ideas[kIdeaCount];
};

#endif
