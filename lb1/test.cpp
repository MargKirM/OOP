#include <cassert>
#include <iostream>

#include "cell.h"
#include "game_field.h"
#include "robot.h"
#include "robot_factory.h"
#include "game.h"
#include "console_renderer.h"

using namespace lab1;

// ============ Cell ============

void TestCell() {
    std::cout << "\n=== Cell ===\n";

    Cell c;
    assert(c.IsPassable() == true);
    assert(c.GetPassCost() == 1);
    std::cout << "default: passable, cost=1\n";

    c.SetPassable(false);
    assert(c.IsPassable() == false);
    std::cout << "SetPassable(false): ok\n";

    c.SetPassCost(3);
    assert(c.GetPassCost() == 3);
    std::cout << "SetPassCost(3): cost=3\n";

    c.SetPassCost(0);
    assert(c.GetPassCost() == 1);   // set to 1
    std::cout << "SetPassCost(0): clamped to 1\n";

    c.SetPassCost(-5);
    assert(c.GetPassCost() == 1);   // set to 1
    std::cout << "SetPassCost(-5): clamped to 1\n";
}

// ============ GameField ============

void TestGameField() {
    std::cout << "\n=== GameField ===\n";

    GameField f(10, 10);
    assert(f.GetWidth() == 10);
    assert(f.GetHeight() == 10);
    std::cout << "size 10x10: ok\n";

    assert(f.IsInside(5, 5) == true);
    assert(f.IsInside(10, 10) == false);
    assert(f.IsInside(-1, 0) == false);
    std::cout << "IsInside: ok\n";

    assert(f.IsPassable(5, 5) == true);
    f.SetPassable(5, 5, false);
    assert(f.IsPassable(5, 5) == false);
    std::cout << "SetPassable: ok\n";

    f.SetPassCost(6, 6, 3);
    assert(f.GetPassCost(6, 6) == 3);
    std::cout << "SetPassCost: ok\n";

    // control of size
    GameField small(2, 2);   // < kMinSize=5
    assert(small.GetWidth() == 5);
    assert(small.GetHeight() == 5);
    std::cout << "min size clamp: 2 -> 5\n";

    GameField big(100, 100); // > kMaxSize=50
    assert(big.GetWidth() == 50);
    assert(big.GetHeight() == 50);
    // Now: width = kMaxSize = 50
    std::cout << "max size clamp: 100 -> " << big.GetWidth() << "\n";
}

// ============ Robot ============

void TestRobot() {
    std::cout << "\n=== Robot ===\n";

    Robot r("Test", 100, 100, 10, 50, 50, 0, 100, 3, 2);
    assert(r.GetHealth() == 100);
    assert(r.GetMaxHealth() == 100);
    assert(r.GetAttack() == 10);
    assert(r.GetEnergy() == 50);
    assert(r.GetMaxEnergy() == 50);
    assert(r.GetRank() == 1);
    assert(r.GetSpeed() == 3);
    assert(r.GetVisibilityRadius() == 2);
    assert(r.IsAlive() == true);
    std::cout << "constructor: ok\n";

    // Attack
    Robot enemy("Enemy", 40, 40, 5, 30, 30, 0, 100, 3, 1);
    r.Interact(enemy, true);
    assert(enemy.GetHealth() == 30);   // 40 - 10
    std::cout << "attack: enemy HP 40 -> 30\n";

    // Heal
    Robot hurt("Hurt", 50, 100, 10, 50, 50, 0, 100, 3, 2);
    Robot healer("Healer", 100, 100, 0, 50, 50, 0, 100, 3, 2);
    healer.Interact(hurt, false);   // heal by kHealAmount = 10
    assert(hurt.GetHealth() == 60);
    std::cout << "heal: HP 50 -> 60\n";

    // Experience and rank
    Robot hero("Hero", 100, 100, 10, 50, 50, 0, 100, 3, 2);
    hero.AddExperience(100);
    assert(hero.GetRank() == 2);
    assert(hero.GetMaxHealth() == 120);   // +20
    assert(hero.GetAttack() == 13);       // +3
    assert(hero.GetMaxEnergy() == 53);    // +3
    std::cout << "rank up: rank=2, HP=120, ATK=13, EN=53\n";

    // Death
    Robot weak("Weak", 5, 10, 0, 0, 0, 0, 100, 1, 0);
    weak.TakeDamage(100);
    assert(weak.IsAlive() == false);
    assert(weak.GetHealth() == 0);
    std::cout << "death: HP -> 0, IsAlive=false\n";

    // Value clamping (control of values)
    Robot clamped("Clamped", 200, 100, -5, 200, 50, -10, 100, -1, -1);
    assert(clamped.GetHealth() == 100);    // clamp to max
    assert(clamped.GetAttack() == 0);      // max(0, ...)
    assert(clamped.GetEnergy() == 50);     // clamp to max
    assert(clamped.GetSpeed() == 1);       // max(1, ...)
    assert(clamped.GetVisibilityRadius() == 0);  // max(0, ...)
    assert(clamped.GetMaxHealth() == 100);
    assert(clamped.GetMaxEnergy() == 50);
    assert(clamped.GetExperience() == 0);
    assert(clamped.GetExperienceToRankUp() == 100);
    std::cout << "clamping: ok\n";
}

// ============ RobotFactory ============

void TestRobotFactory() {
    std::cout << "\n=== RobotFactory ===\n";

    RobotFactory f(5, 5, 3);
    assert(f.GetX() == 5);
    assert(f.GetY() == 5);
    assert(f.GetPeriod() == 8);   // 3 < kMinPeriod=8, => 8
    std::cout << "constructor: period=8 (min)\n";

    RobotFactory f2(5, 5, 10);
    assert(f2.GetPeriod() == 10);
    std::cout << "period 10: ok\n";

    // 2x2
    assert(f.Occupies(5, 5) == true);
    assert(f.Occupies(6, 5) == true);
    assert(f.Occupies(5, 6) == true);
    assert(f.Occupies(6, 6) == true);
    assert(f.Occupies(7, 7) == false);
    assert(f.Occupies(4, 4) == false);
    std::cout << "Occupies 2x2: ok\n";

    // Period
    for (int i = 0; i < 7; ++i) {
        assert(f.ToMakeRobot() == false);
    }
    assert(f.ToMakeRobot() == true);   // 8th turn
    std::cout << "ToMakeRobot every 8 turns: ok\n";

    // Reset
    RobotFactory f3(0, 0, 8);
    f3.ResetCounter();
    assert(f3.ToMakeRobot() == true);   // immediately after Reset
    std::cout << "ResetCounter: ok\n";
}

// ============ Game ============

void TestGame() {
    std::cout << "\n=== Game ===\n";

    Game game(10, 10);
    game.ConfigureDemo();

    // player is alive
    assert(game.GetPlayer().IsAlive() == true);
    assert(game.GetPlayerPosition() == std::make_pair(0, 0));
    std::cout << "initial: player at (0,0)\n";

    // Enemies created
    assert(game.GetEnemies().size() == 3);
    assert(game.GetFriends().size() == 1);
    std::cout << "enemies: 3, friends: 1\n";

    // Factory created
    assert(game.GetFactories().size() == 1);
    std::cout << "factory: 1\n";

    // Level isn't complete
    assert(game.IsLevelComplete() == false);
    std::cout << "level not complete: ok\n";

    // step
    auto old_pos = game.GetPlayerPosition();
    game.Step(Direction::kDown);  // go down (if can)
    auto new_pos = game.GetPlayerPosition();
    std::cout << "step down (with demo): ("
              << old_pos.first << "," << old_pos.second << ") -> ("
              << new_pos.first << "," << new_pos.second << ")\n";

    // Visibility
    auto pos = game.GetPlayerPosition();
    assert(game.IsVisible(pos.first, pos.second) == true); // sees itself
    assert(game.IsExplored(pos.first, pos.second) == true); // cell is explored
    assert(game.IsVisible(50, 50) == false);   // out of field
    std::cout << "visibility: ok\n";
}

// ============ ConsoleRenderer ============

void TestConsoleRenderer() {
    std::cout << "\n=== ConsoleRenderer ===\n";

    Game game(10, 10);
    game.ConfigureDemo();

    ConsoleRenderer renderer(game);
    renderer.Render();

    std::cout << "render: ok\n";
}

// ============ main ============

int main() {
    TestCell();
    TestGameField();
    TestRobot();
    TestRobotFactory();
    TestGame();
    TestConsoleRenderer();

    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}