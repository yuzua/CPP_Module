#include "Animal.hpp"
#include "Brain.hpp"
#include "Cat.hpp"
#include "Dog.hpp"

#include <iostream>
#include <sstream>
#include <string>

static int g_checks   = 0;
static int g_failures = 0;

static void checkTrue(std::string const &label, bool ok) {
    ++g_checks;
    if (!ok)
        ++g_failures;
    std::cout << (ok ? "  [PASS] " : "  [FAIL] ") << label << std::endl;
}

static void checkText(std::string const &label, std::string const &actual,
                      std::string const &expected) {
    checkTrue(label, actual == expected);
}

static void section(std::string const &title) {
    std::cout << std::endl << "===== " << title << " =====" << std::endl;
}

class CoutCapture {
    public:
        CoutCapture(void) : previous_(std::cout.rdbuf(stream_.rdbuf())) {}
        ~CoutCapture(void) { std::cout.rdbuf(this->previous_); }
        std::string str(void) const { return this->stream_.str(); }

    private:
        CoutCapture(CoutCapture const &);
        CoutCapture &operator=(CoutCapture const &);

        std::ostringstream stream_;
        std::streambuf    *previous_;
};

static std::string soundOf(Animal const &animal) {
    CoutCapture capture;

    animal.makeSound();
    return capture.str();
}

static bool containsInOrder(std::string const &log, std::string const &first,
                            std::string const &second) {
    std::string::size_type const a = log.find(first);
    std::string::size_type const b = log.find(second);

    return a != std::string::npos && b != std::string::npos && a < b;
}

static int countOf(std::string const &log, std::string const &needle) {
    int                      count = 0;
    std::string::size_type   pos   = 0;

    while ((pos = log.find(needle, pos)) != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

// CI-10: 犬 1 匹の delete は、犬 → 脳 → 動物の順で破棄する。
static void testDeleteReleasesBrain(void) {
    section("CI-10 delete through Animal* destroys the Brain between Dog and Animal");
    std::string log;

    {
        CoutCapture capture;
        Animal     *dog = new Dog();
        Animal     *cat = new Cat();

        delete dog;
        delete cat;
        log = capture.str();
    }
    checkTrue("dog, then its brain, then Animal",
              containsInOrder(log, "Dog is destructed.", "Brain is destructed.") &&
                  containsInOrder(log, "Brain is destructed.", "Animal is destructed."));
    checkTrue("cat is also destructed", log.find("Cat is destructed.") != std::string::npos);
    checkTrue("two brains are destructed", countOf(log, "Brain is destructed.") == 2);
}

// CI-11: 配列の半分が犬、半分が猫。Animal* として消す。
static void testHalfAndHalfArray(void) {
    section("CI-11 half the animals are dogs and half are cats");
    int const count = 4;
    Animal   *animals[4];
    std::string log;

    for (int i = 0; i < count; ++i) {
        if (i < count / 2)
            animals[i] = new Dog();
        else
            animals[i] = new Cat();
    }
    {
        CoutCapture capture;

        for (int i = 0; i < count; ++i)
            delete animals[i];
        log = capture.str();
    }
    checkTrue("two dog destructors", countOf(log, "Dog is destructed.") == 2);
    checkTrue("two cat destructors", countOf(log, "Cat is destructed.") == 2);
    checkTrue("four brain destructors", countOf(log, "Brain is destructed.") == 4);
    checkTrue("four animal destructors", countOf(log, "Animal is destructed.") == 4);
}

// CI-12 / CI-13 / CI-14: コピーは深く、自己代入でアイデアを失わない。
static void testDogIdeasAreIndependent(void) {
    section("CI-12 copy, CI-13 assignment, CI-14 self-assignment");
    Dog original;

    original.setIdea(0, "chase");
    {
        Dog copy(original);

        checkText("copy starts equal", copy.getIdea(0), "chase");
        copy.setIdea(0, "sleep");
        checkText("origin unchanged by copy", original.getIdea(0), "chase");
        checkText("copy changed alone", copy.getIdea(0), "sleep");
    }
    {
        Dog assigned;

        assigned.setIdea(0, "old");
        assigned = original;
        assigned.setIdea(0, "eat");
        checkText("origin unchanged by assignment", original.getIdea(0), "chase");
        checkText("assignee changed alone", assigned.getIdea(0), "eat");
    }
    original = original;
    checkText("self-assignment keeps the idea", original.getIdea(0), "chase");
}

// CI-15: 範囲外の書き込みは、既存のアイデアを変えない。
static void testIdeaIndexIsRejected(void) {
    section("CI-15 an out-of-range idea does not change slot 0");
    Dog dog;

    dog.setIdea(0, "stay");
    dog.setIdea(-1, "nope");
    dog.setIdea(Brain::kIdeaCount, "nope");
    checkText("slot 0", dog.getIdea(0), "stay");
    checkText("negative read is empty", dog.getIdea(-1), "");
}

// CI-16: 猫も同じ深いコピー。
static void testCatIdeasAreIndependent(void) {
    section("CI-16 a copied cat does not share ideas");
    Cat origin;

    origin.setIdea(0, "nap");
    Cat copy(origin);
    copy.setIdea(0, "hunt");
    checkText("origin", origin.getIdea(0), "nap");
    checkText("copy", copy.getIdea(0), "hunt");
}

// ex02: Animal は抽象クラス。値も new Animal() も書かない。
// 犬と猫の窓口が Animal* のまま動くことだけを残す。
static void testPolymorphicSoundStillWorks(void) {
    section("CI-02 Dog and Cat still answer through Animal*");
    Dog  dog;
    Cat  cat;
    Animal const *asDog = &dog;
    Animal const *asCat = &cat;

    checkTrue("dog and cat sounds differ", soundOf(*asDog) != soundOf(*asCat));
    checkText("dog type", asDog->getType(), "Dog");
    checkText("cat type", asCat->getType(), "Cat");
}

int main(void) {
    testDeleteReleasesBrain();
    testHalfAndHalfArray();
    testDogIdeasAreIndependent();
    testIdeaIndexIsRejected();
    testCatIdeasAreIndependent();
    testPolymorphicSoundStillWorks();

    std::cout << std::endl
              << "===== summary =====" << std::endl
              << g_checks - g_failures << " / " << g_checks << " checks passed"
              << std::endl;
    return (g_failures == 0) ? 0 : 1;
}
