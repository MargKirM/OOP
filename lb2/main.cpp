#include <iostream>
#include <limits>
#include "console_renderer.h"
#include "game.h"


using namespace lab2;

int main() {
    using namespace lab2;
    Game game(15, 15);
    ConsoleRenderer renderer(game);
    game.ConfigureDemo(renderer.ChooseAbility(game.GetPlayer(), false));
    game.SetRankUpChoice([&renderer](Robot& robot) {
        AbilityKind kind = renderer.ChooseAbility(robot, true);
        if (robot.HasAbility(kind)) robot.UpgradeAbility(kind);
        else robot.AddAbility(kind);
    });
    
    
    while (game.GetPlayer().IsAlive() && !game.IsLevelComplete()) {
        renderer.Render();

        std::cout << "\nСпособности (нажмите цифру):\n";
    const auto& abilities = game.GetPlayer().GetAbilities();
    if (abilities.empty()) {
        std::cout << "  (нет способностей)\n";
    } else {
        for (const auto& a : abilities) {
            const char* key = "?";
            for (int i = 0; i < 4; ++i) {
                if (a->GetName() == AbilityName(static_cast<AbilityKind>(i))) {
                    key = (i == 0) ? "1" : (i == 1) ? "2" : (i == 2) ? "3" : "4";
                    break;
                }
            }
            std::cout << "  [" << key << "] " << a->GetName()
                      << " (энергия: " << a->GetEnergyCost() << ")\n";
        }
    }

        std::cout << "\nw/a/s/d - движение, 1..4 - способность, "
                 "i - инвентарь, v - режим отображения, e - закончить ход, q - выход\n";

        std::cout << "\nКоманда: ";
        char command;
        if (!(std::cin >> command) || command == 'q') return 0;
        if (command == 'e') { game.EndPlayerTurn(); continue; }
        if (command == 'v') { renderer.ToggleMode(); continue; }
        if (command == 'i') {
            const auto& inv = game.GetInventory();
            if (inv.empty()) { std::cout << "Инвентарь пуст.\n"; continue; }
            for (std::size_t i = 0; i < inv.size(); ++i)
                std::cout << i + 1 << " - " << inv[i].GetName() << '\n';
            std::cout << "Использовать (0 — отмена): ";
            int idx;
            if (!(std::cin >> idx)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if (idx >= 1) game.UseItem(static_cast<std::size_t>(idx - 1));
            continue;
        }
        if (command >= '1' && command <= '4') {
            AbilityKind kind = static_cast<AbilityKind>(command - '1');
            int x = 0, y = 0;

            if (kind == AbilityKind::kRangedStrike || kind == AbilityKind::kTeleport) {
                std::cout << "Направление (w/a/s/d): ";
                char dir;
                std::cin >> dir;

                int max_steps = (kind == AbilityKind::kRangedStrike) ? 3 : 5;
                std::cout << "Клеток (1-" << max_steps << "): ";
                int steps;
                if (!(std::cin >> steps) || steps < 1 || steps > max_steps) {
                    std::cout << "Неверное количество.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                int dx = 0, dy = 0;
                switch (dir) {
                    case 'w': dy = -steps; break;
                    case 's': dy =  steps; break;
                    case 'a': dx = -steps; break;
                    case 'd': dx =  steps; break;
                    default: std::cout << "Неверное направление.\n"; continue;
                }
                x = game.GetPlayer().GetX() + dx;
                y = game.GetPlayer().GetY() + dy;
            }

            if (!game.PlayerUseAbility(kind, x, y)) std::cout << "Способность не сработала.\n";
            continue; // несколько способностей за ход
        }
        switch (command) {
            case 'w': game.Step(Direction::kUp); break;
            case 's': game.Step(Direction::kDown); break;
            case 'a': game.Step(Direction::kLeft); break;
            case 'd': game.Step(Direction::kRight); break;
            default: std::cout << "Неизвестная команда.\n"; break;
        }
    }
    renderer.Render();
}