#include "AMateria.hpp"
#include "Character.hpp"
#include "Cure.hpp"
#include "Ice.hpp"
#include "ICharacter.hpp"
#include "IMateriaSource.hpp"
#include "MateriaSource.hpp"

#include <iostream>
#include <sstream>
#include <string>

static int g_checks   = 0;
static int g_failures = 0;

static std::string const kIceAtBob = "* shoots an ice bolt at bob *\n";
static std::string const kHealBob  = "* heals bob's wounds *\n";

static void checkTrue(std::string const &label, bool ok) {
    ++g_checks;
    if (!ok)
        ++g_failures;
    std::cout << (ok ? "  [PASS] " : "  [FAIL] ") << label << std::endl;
}

static void checkText(std::string const &label, std::string const &actual,
                      std::string const &expected) {
    bool const ok = (actual == expected);

    checkTrue(label, ok);
    if (!ok) {
        std::cout << "    actual:   [" << actual << "]" << std::endl;
        std::cout << "    expected: [" << expected << "]" << std::endl;
    }
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

static std::string capturedUse(ICharacter &user, int idx, ICharacter &target) {
    CoutCapture capture;

    user.use(idx, target);
    return capture.str();
}

// CI-30: クラスは沈黙し、課題の main は 2 行だけを出す。
static void testSubjectMain(void) {
    section("CI-30 subject main");
    std::string log;

    {
        CoutCapture     capture;
        IMateriaSource *src = new MateriaSource();

        src->learnMateria(new Ice());
        src->learnMateria(new Cure());

        ICharacter *me = new Character("me");
        AMateria   *tmp;

        tmp = src->createMateria("ice");
        me->equip(tmp);
        tmp = src->createMateria("cure");
        me->equip(tmp);

        ICharacter *bob = new Character("bob");

        me->use(0, *bob);
        me->use(1, *bob);
        delete bob;
        delete me;
        delete src;
        log = capture.str();
    }
    checkText("exact output", log, kIceAtBob + kHealBob);
}

// CI-31: clone は同じ種類の別個体。
static void testCloneIsADistinctTwin(void) {
    section("CI-31 clone");
    Ice       ice;
    AMateria *twin = ice.clone();

    checkTrue("different address", twin != &ice);
    checkText("same type", twin->getType(), "ice");
    delete twin;
}

// CI-32: 基底参照の代入は type をコピーしない。
static void testAssignmentDoesNotCopyType(void) {
    section("CI-32 assignment keeps the materia type");
    Ice       ice;
    Cure      cure;
    AMateria &asIce  = ice;
    AMateria &asCure = cure;

    asIce = asCure;
    checkText("ice stays ice", ice.getType(), "ice");
    checkText("cure stays cure", cure.getType(), "cure");
}

// CI-33: 未知タイプは 0。既知タイプは見本とは別の個体を返す。
static void testCreateMateria(void) {
    section("CI-33 createMateria");
    MateriaSource src;

    src.learnMateria(new Ice());
    checkTrue("unknown type", src.createMateria("fire") == 0);
    checkTrue("wrong case", src.createMateria("Ice") == 0);

    AMateria *first  = src.createMateria("ice");
    AMateria *second = src.createMateria("ice");

    checkTrue("known type", first != 0 && second != 0);
    checkTrue("two creates are distinct", first != second);
    checkText("created type", first->getType(), "ice");
    delete first;
    delete second;
}

// CI-34: 5 個目は受け取らない。ポインタは呼び出し側がまだ delete できる。
static void testEquipRejectsTheFifth(void) {
    section("CI-34 full inventory");
    MateriaSource src;
    Character     hero("hero");
    Character     bob("bob");

    src.learnMateria(new Cure());
    src.learnMateria(new Ice());
    for (int i = 0; i < 4; ++i)
        hero.equip(src.createMateria("cure"));

    AMateria *extra = src.createMateria("ice");

    hero.equip(extra);
    checkText("rejected materia is still alive", extra->getType(), "ice");

    std::string uses;
    for (int i = 0; i < 4; ++i)
        uses += capturedUse(hero, i, bob);
    checkTrue("no slot became the rejected ice", uses.find("ice bolt") == std::string::npos);
    delete extra;
}

// CI-35: unequip は消さない。空いた先頭スロットに次の装備が入る。
static void testUnequipReturnsOwnership(void) {
    section("CI-35 unequip");
    MateriaSource src;
    Character     hero("hero");
    Character     bob("bob");

    src.learnMateria(new Ice());
    src.learnMateria(new Cure());

    AMateria *ice  = src.createMateria("ice");
    AMateria *cure = src.createMateria("cure");

    hero.equip(ice);
    hero.equip(cure);
    hero.unequip(0);
    checkText("slot 0 is silent", capturedUse(hero, 0, bob), "");
    checkText("cure remains in slot 1", capturedUse(hero, 1, bob), kHealBob);

    hero.equip(ice);
    checkText("hole 0 is filled again", capturedUse(hero, 0, bob), kIceAtBob);
    hero.unequip(0);
    delete ice;
    checkText("cure still usable after the ice was deleted",
              capturedUse(hero, 1, bob), kHealBob);
}

// CI-36: 範囲外は何もしない。
static void testInvalidIndex(void) {
    section("CI-36 invalid index");
    MateriaSource src;
    Character     hero("hero");
    Character     bob("bob");

    src.learnMateria(new Ice());
    hero.equip(src.createMateria("ice"));
    checkText("negative use", capturedUse(hero, -1, bob), "");
    checkText("past the end", capturedUse(hero, 4, bob), "");
    hero.unequip(-1);
    hero.unequip(4);
    checkText("slot 0 survived", capturedUse(hero, 0, bob), kIceAtBob);
}

// CI-37: コピーは深く、元を破棄したあとも使える。名前もコピーされる。
static void testDeepCopySurvivesSource(void) {
    section("CI-37 deep copy of Character");
    MateriaSource src;
    Character     copy("placeholder");
    Character     bob("bob");

    src.learnMateria(new Ice());
    {
        Character original("original");

        original.equip(src.createMateria("ice"));
        copy = original;
        checkText("name is copied", copy.getName(), "original");
    }
    checkText("copy still shoots after the source is gone",
              capturedUse(copy, 0, bob), kIceAtBob);
}

// CI-38: 自己代入で装備を失わない。
static void testSelfAssignment(void) {
    section("CI-38 self-assignment");
    MateriaSource src;
    Character     hero("hero");
    Character     bob("bob");

    src.learnMateria(new Ice());
    hero.equip(src.createMateria("ice"));
    hero = hero;
    checkText("still equipped", capturedUse(hero, 0, bob), kIceAtBob);
}

// CI-39: 5 回 learn しても、見本から ice を作れ、破棄が落ちない。
static void testLearnOverflow(void) {
    section("CI-39 learnMateria keeps four templates");
    MateriaSource src;

    for (int i = 0; i < 5; ++i)
        src.learnMateria(new Ice());

    AMateria *ice = src.createMateria("ice");

    checkTrue("template still creates ice", ice != 0);
    delete ice;
}

// CI-40: MateriaSource のコピーは、元を破棄したあとも見本を持つ。
static void testMateriaSourceDeepCopy(void) {
    section("CI-40 deep copy of MateriaSource");
    MateriaSource copy;

    {
        MateriaSource original;

        original.learnMateria(new Ice());
        copy = original;
    }

    AMateria *ice = copy.createMateria("ice");

    checkTrue("copy still knows ice", ice != 0);
    if (ice != 0)
        checkText("type", ice->getType(), "ice");
    delete ice;
}

// CI-41: ヌルの equip はスロットを埋めない。
static void testEquipNull(void) {
    section("CI-41 equip null");
    Character hero("hero");
    Character bob("bob");

    hero.equip(0);
    checkText("slot stays empty", capturedUse(hero, 0, bob), "");
}

// CI-42: 同じアドレスの 2 回目は入らない。破棄は 1 回で済む。
static void testDuplicateEquip(void) {
    section("CI-42 duplicate equip");
    MateriaSource src;
    Character     hero("hero");
    Character     bob("bob");

    src.learnMateria(new Ice());

    AMateria *ice = src.createMateria("ice");

    hero.equip(ice);
    hero.equip(ice);
    checkText("slot 0", capturedUse(hero, 0, bob), kIceAtBob);
    checkText("slot 1 stays empty", capturedUse(hero, 1, bob), "");
}

int main(void) {
    testSubjectMain();
    testCloneIsADistinctTwin();
    testAssignmentDoesNotCopyType();
    testCreateMateria();
    testEquipRejectsTheFifth();
    testUnequipReturnsOwnership();
    testInvalidIndex();
    testDeepCopySurvivesSource();
    testSelfAssignment();
    testLearnOverflow();
    testMateriaSourceDeepCopy();
    testEquipNull();
    testDuplicateEquip();

    std::cout << std::endl
              << "===== summary =====" << std::endl
              << g_checks - g_failures << " / " << g_checks << " checks passed"
              << std::endl;
    return (g_failures == 0) ? 0 : 1;
}
