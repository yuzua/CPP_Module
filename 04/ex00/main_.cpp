#include "Animal.hpp"
#include "Cat.hpp"
#include "Dog.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

#include <iostream>
#include <sstream>
#include <string>

// 契約テストの判定器。標準出力の文言は、どの関数が呼ばれたかの oracle。
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
    checkTrue(label + " (got \"" + actual + "\")", actual == expected);
}

static void section(std::string const &title) {
    std::cout << std::endl << "===== " << title << " =====" << std::endl;
}

// makeSound の出力だけを取り出す。構築ログと混ぜない。
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

static std::string soundOfWrong(WrongAnimal const &animal) {
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

// CI-01: 基底ポインタでも、種類名は実体が初期化子で渡した値。
static void testGetTypeThroughBase(void) {
    section("CI-01 getType through Animal* reads the real type");
    Animal const *meta = new Animal();
    Animal const *dog  = new Dog();
    Animal const *cat  = new Cat();

    checkText("plain animal", meta->getType(), "Animal");
    checkText("dog", dog->getType(), "Dog");
    checkText("cat", cat->getType(), "Cat");

    delete meta;
    delete dog;
    delete cat;
}

// CI-02: virtual なら、基底ポインタの鳴き声は実体の鳴き声。
static void testSoundFollowsDynamicType(void) {
    section("CI-02 makeSound through Animal* follows the dynamic type");
    Animal animal;
    Dog    dog;
    Cat    cat;
    Animal const *asDog = &dog;
    Animal const *asCat = &cat;

    std::string const animalSound = soundOf(animal);
    std::string const dogSound    = soundOf(dog);
    std::string const catSound    = soundOf(cat);

    checkTrue("dog pointer matches a Dog variable", soundOf(*asDog) == dogSound);
    checkTrue("cat pointer matches a Cat variable", soundOf(*asCat) == catSound);
    checkTrue("dog sound is not the Animal sound", dogSound != animalSound);
    checkTrue("cat sound is not the dog sound", catSound != dogSound);
}

// CI-03: virtual が無いと、基底ポインタの鳴き声は基底のまま。
static void testWrongSoundStaysStatic(void) {
    section("CI-03 WrongAnimal* calls WrongAnimal::makeSound");
    WrongAnimal base;
    WrongCat    cat;
    WrongAnimal const *asCat = &cat;

    std::string const baseSound   = soundOfWrong(base);
    std::string const directSound = soundOfWrong(cat);

    checkTrue("pointer uses the base sound", soundOfWrong(*asCat) == baseSound);
    checkTrue("a WrongCat variable has its own sound", directSound != baseSound);
}

// CI-04: delete は派生のデストラクタを先に呼ぶ。
static void testDeleteOrder(void) {
    section("CI-04 delete through Animal* runs the derived destructor first");
    std::string log;

    {
        CoutCapture capture;
        Animal     *dog = new Dog();

        delete dog;
        log = capture.str();
    }
    checkTrue("Dog destructor appears before Animal destructor",
              containsInOrder(log, "Dog is destructed.", "Animal is destructed."));
}

// CI-05: コピーは基底部分を含めて種類名を保つ。
static void testCopyKeepsType(void) {
    section("CI-05 copy keeps the derived type");
    Dog original;
    Dog copy(original);

    checkText("copy type", copy.getType(), "Dog");
    original = copy;
    checkText("assigned type", original.getType(), "Dog");
}

// CI-06: 値へのコピーは派生部分を切り捨てる。音は Animal のものになる。
static void testSlicingLosesDynamicType(void) {
    section("CI-06 slicing drops the derived sound");
    Animal animal;
    Dog    dog;
    Animal sliced = dog;

    checkTrue("sliced value sounds like Animal", soundOf(sliced) == soundOf(animal));
    checkTrue("sliced value does not sound like Dog", soundOf(sliced) != soundOf(dog));
}

// CI-07: 非 virtual でもデータは実体のもの。関数本体だけがポインタの型に従う。
static void testDataAndFunctionAreDifferent(void) {
    section("CI-07 type is data, makeSound is a function body");
    WrongAnimal const *wrong = new WrongCat();
    WrongAnimal        base;

    checkText("type is WrongCat", wrong->getType(), "WrongCat");
    checkTrue("sound is still WrongAnimal", soundOfWrong(*wrong) == soundOfWrong(base));
    delete wrong;
}

int main(void) {
    testGetTypeThroughBase();
    testSoundFollowsDynamicType();
    testWrongSoundStaysStatic();
    testDeleteOrder();
    testCopyKeepsType();
    testSlicingLosesDynamicType();
    testDataAndFunctionAreDifferent();

    std::cout << std::endl
              << "===== summary =====" << std::endl
              << g_checks - g_failures << " / " << g_checks << " checks passed"
              << std::endl;
    return (g_failures == 0) ? 0 : 1;
}
