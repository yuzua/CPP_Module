#include "ClapTrap_.hpp"

#include <iostream>
#include <limits>

// 契約テストの判定器。
// 「動いた」ではなく「契約を破った実装を識別できる」ことを目的にしている。
static int g_checks   = 0;
static int g_failures = 0;

static void check(std::string const &label, unsigned int actual, unsigned int expected) {
    bool const ok = (actual == expected);

    ++g_checks;
    if (!ok)
        ++g_failures;
    std::cout << (ok ? "  [PASS] " : "  [FAIL] ") << label << " = " << actual
              << " (expected " << expected << ")" << std::endl;
}

static void section(std::string const &title) {
    std::cout << std::endl << "===== " << title << " =====" << std::endl;
}

// CI-01: 構築直後の属性は 10 / 10 / 0 である。
static void testInitialState(void) {
    section("CI-01 initial state is 10 / 10 / 0");
    ClapTrap unit("Bob");
    check("hit points", unit.getHitPoints(), ClapTrap::kBaseHitPoints);
    check("energy points", unit.getEnergyPoints(), ClapTrap::kBaseEnergyPoints);
    check("attack damage", unit.getAttackDamage(), ClapTrap::kBaseAttackDamage);
}

// CI-02: 成功した行動はエナジーをちょうど 1 消費する。
static void testActionCostsExactlyOneEnergy(void) {
    section("CI-02 a successful action consumes exactly one energy point");
    ClapTrap unit("Worker");
    unit.attack("Target");
    check("energy after 1 attack", unit.getEnergyPoints(), 9);
    unit.beRepaired(1);
    check("energy after 1 repair", unit.getEnergyPoints(), 8);
}

// CI-03: 残量を超えるダメージでヒットポイントは 0 で止まる。
//        符号なし整数の折り返し（10 - 9999 = 4294956307）を起こしてはならない。
static void testOverkillClampsToZero(void) {
    section("CI-03 overkill damage clamps hit points to zero");
    ClapTrap unit("Fragile");
    unit.takeDamage(9999);
    check("hit points after overkill", unit.getHitPoints(), 0);
}

// CI-04: 破壊済みは吸収状態である。行動は拒否され、エナジーも消費されない。
static void testDestroyedIsAbsorbing(void) {
    section("CI-04 a destroyed unit cannot act and pays no energy");
    ClapTrap unit("Wreck");
    unit.takeDamage(ClapTrap::kBaseHitPoints);
    check("hit points", unit.getHitPoints(), 0);

    unsigned int const energyBefore = unit.getEnergyPoints();
    unit.attack("Target");
    unit.beRepaired(50);
    check("energy is unchanged", unit.getEnergyPoints(), energyBefore);
    check("hit points stay at zero", unit.getHitPoints(), 0);
}

// CI-05: エナジー切れでは行動が拒否され、ヒットポイントは変化しない。
static void testExhaustedCannotAct(void) {
    section("CI-05 an exhausted unit cannot act");
    ClapTrap unit("Drained");
    for (unsigned int i = 0; i < ClapTrap::kBaseEnergyPoints; ++i)
        unit.attack("Dummy");
    check("energy is depleted", unit.getEnergyPoints(), 0);

    unsigned int const hitPointsBefore = unit.getHitPoints();
    unit.attack("Dummy");
    unit.beRepaired(5);
    check("hit points are unchanged", unit.getHitPoints(), hitPointsBefore);
}

// CI-06: 回復はヒットポイントを増やす。
static void testRepairRestoresHitPoints(void) {
    section("CI-06 repair restores hit points");
    ClapTrap unit("Medic");
    unit.takeDamage(6);
    check("hit points after damage", unit.getHitPoints(), 4);
    unit.beRepaired(3);
    check("hit points after repair", unit.getHitPoints(), 7);
}

// CI-07: 回復量が巨大でも符号なし整数を折り返さない。
static void testRepairDoesNotOverflow(void) {
    section("CI-07 repair does not wrap around");
    ClapTrap unit("Immortal");
    unit.beRepaired(std::numeric_limits<unsigned int>::max());
    check("hit points saturate", unit.getHitPoints(),
          std::numeric_limits<unsigned int>::max());
}

// CI-08 / CI-09: コピーは状態を複製し、複製後は互いに独立している。
static void testCopySemantics(void) {
    section("CI-08 copy duplicates the state, CI-09 copies are independent");
    ClapTrap origin("Origin");
    origin.takeDamage(3);
    origin.attack("Target");

    ClapTrap clone(origin);
    check("clone hit points", clone.getHitPoints(), origin.getHitPoints());
    check("clone energy points", clone.getEnergyPoints(), origin.getEnergyPoints());

    clone.takeDamage(1);
    check("origin is unaffected", origin.getHitPoints(), 7);
    check("clone changed alone", clone.getHitPoints(), 6);

    ClapTrap assigned("Assigned");
    assigned = origin;
    check("assigned hit points", assigned.getHitPoints(), origin.getHitPoints());
    check("assigned name is replaced",
          static_cast<unsigned int>(assigned.getName() == origin.getName()), 1);
}

int main(void) {
    testInitialState();
    testActionCostsExactlyOneEnergy();
    testOverkillClampsToZero();
    testDestroyedIsAbsorbing();
    testExhaustedCannotAct();
    testRepairRestoresHitPoints();
    testRepairDoesNotOverflow();
    testCopySemantics();

    std::cout << std::endl
              << "===== summary =====" << std::endl
              << g_checks - g_failures << " / " << g_checks << " checks passed"
              << std::endl;
    return (g_failures == 0) ? 0 : 1;
}
