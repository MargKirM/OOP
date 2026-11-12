#include "console_renderer.h"

#include <utility>
#include <iostream>
#include <limits>

namespace lab2 {

namespace {
    const char* kReset  = "\033[0m";
    const char* kRed    = "\033[31m";
    const char* kGray   = "\033[90m";
    const char* kYellow = "\033[33m";
}

ConsoleRenderer::ConsoleRenderer(const Game& game) : game_(game) {}

char ConsoleRenderer::GetCellSymbol(int x, int y) const {

    if (!game_.IsExplored(x, y)) return '?'; // not explored cell

    if (game_.GetPlayerPosition() == std::make_pair(x, y)) {
            return game_.GetPlayer().IsAlive() ? 'P' : 't';
    }

    if(!game_.IsVisible(x, y)) {
        for (const auto& factory : game_.GetFactories()) {
            if (factory->Occupies(x, y)) return '@';   // factory is visible
        }
        if (!game_.GetField().IsPassable(x, y)) { // not passable cell
            return '#';
        }
        return '?';
    }

    for(const auto& enemy : game_.GetEnemies()) {
        if (enemy->GetX() == x && enemy->GetY() == y) {
            return enemy->IsAlive() ? 'E' : 'x';
        }
    }

    for (const auto& friend_ : game_.GetFriends()) {
            if (friend_->GetX() == x && friend_->GetY() == y) {
                return 'F';
            }
    }

    for(const auto& factory : game_.GetFactories()) {
        if (factory->Occupies(x, y)) return '@';
    }

    if (!game_.GetField().IsPassable(x, y)) { // not passable cell
        return '#';
    }

    int cost = game_.GetField().GetPassCost(x, y);
    if (cost >= 3) return '~'; // swamp
    if (cost == 2) return '^'; // dirt

    return '.'; // usual cell
}


bool ConsoleRenderer::ShouldHighlight(int x, int y) const {
    switch (mode_) {
        case RenderMode::kNormal:
            return false;
        case RenderMode::kHighlightEnemies:
            if (!game_.IsVisible(x, y)) return false;
            for (const auto& e : game_.GetEnemies())
                if (e->IsAlive() && e->GetX() == x && e->GetY() == y) return true;
            return false;
        case RenderMode::kHighlightWalls:
            return game_.IsExplored(x, y) && !game_.GetField().IsPassable(x, y);
        case RenderMode::kHighlightStatusCells:
            return game_.IsExplored(x, y) && game_.GetCellStatusCount(x, y) > 0;
    }
    return false;
}

void ConsoleRenderer::Render() const {
    
    const GameField& field = game_.GetField();

    for(int y = 0; y < field.GetHeight(); y++) {  
        for(int x = 0; x < field.GetWidth(); x++) {
            char sym = GetCellSymbol(x, y);

            if (ShouldHighlight(x, y)) { //Highlighting
                const char* color = kRed;
                if (mode_ == RenderMode::kHighlightWalls)        color = kGray;
                if (mode_ == RenderMode::kHighlightStatusCells)  color = kYellow;
                std::cout << color << sym << kReset << ' ';
            } else {
                std::cout << sym << ' ';
            }
        }
        std::cout << '\n';
    }

    std::cout << "\nP — игрок, E — враг, F — друг, "
                 "@ — фабрика роботов, x — убитый враг, t — убитый игрок, "
                 "# — стена, ? — не исследована, "
                 "~ — болото (3), ^ — грязь (2)\n";

    const Robot& player = game_.GetPlayer();

    std::cout << "\nPlayer: Health " << player.GetHealth() << '/'
                << player.GetMaxHealth()
                << ", Attack " << player.GetAttack()
                << ", Energy " << player.GetEnergy() << '/'
                << player.GetMaxEnergy()
                << ", Experience " << player.GetExperience() << '/'
                << player.GetExperienceToRankUp()
                << ", Rank " << player.GetRank()
                << ", Speed " << player.GetSpeed() << '\n';

    std::cout << "Статусы: ";
    if (player.GetStatuses().empty()) {
        std::cout << "(нет)";
    } else {
        for (const auto& s : player.GetStatuses())
            std::cout << s->GetName() << "  ";
    }
    std::cout << '\n';

    std::cout << "\nAbilities: ";
    for (const auto& a : player.GetAbilities())
        std::cout << a->GetName() << " (" << a->GetEnergyCost() << ") ";
    std::cout << '\n';

    // only player
    std::cout << "Inventory: ";
    const auto& inv = game_.GetInventory();
    if (inv.empty()) std::cout << "(пусто)";
    else for (const auto& item : inv) std::cout << item.GetName() << "  ";
    std::cout << '\n';

    // Режим отображения
    const char* modeName = "Стандартный";
    switch (mode_) {
        case RenderMode::kNormal:              modeName = "Стандартный"; break;
        case RenderMode::kHighlightEnemies:    modeName = "Подсветка врагов"; break;
        case RenderMode::kHighlightWalls:      modeName = "Подсветка стен"; break;
        case RenderMode::kHighlightStatusCells:modeName = "Подсветка клеток со статусами"; break;
    }
    std::cout << "Режим: " << modeName << '\n';

    if(!player.IsAlive()) {
        std::cout << "Player is dead. Game over!\n";
    } else if(game_.IsLevelComplete()) {
        std::cout << "Level complete!\n";
    }
}

void ConsoleRenderer::ToggleMode() { //переключение по кругу
    switch (mode_) {
        case RenderMode::kNormal:               mode_ = RenderMode::kHighlightEnemies; break;
        case RenderMode::kHighlightEnemies:     mode_ = RenderMode::kHighlightWalls; break;
        case RenderMode::kHighlightWalls:       mode_ = RenderMode::kHighlightStatusCells; break;
        case RenderMode::kHighlightStatusCells: mode_ = RenderMode::kNormal; break;
    }
}

AbilityKind ConsoleRenderer::ChooseAbility(const Robot& robot, bool rank_up) const{
    while (true) {
        std::cout << (rank_up ? "\nПовышение ранга: выберите способность:\n"
                              : "\nВыберите начальную способность:\n");
        for (int i = 0; i < 4; ++i) {
            AbilityKind kind = static_cast<AbilityKind>(i);
            std::cout << i + 1 << " - " << AbilityName(kind);
            if (rank_up) std::cout << (robot.HasAbility(kind) ? " (улучшить)" : " (получить)");
            std::cout << '\n';
        }
        int choice;
        if (std::cin >> choice && choice >= 1 && choice <= 4) {
            AbilityKind kind = static_cast<AbilityKind>(choice - 1);
            if (!rank_up || !robot.HasAbility(kind)) return kind;
            for (const auto& ability : robot.GetAbilities()) {
                if (ability->GetName() == AbilityName(kind) && ability->IsUpgradable()) return kind;
            }
            std::cout << "Эта способность уже улучшена до предела.\n";
        } else {
            std::cout << "Введите число от 1 до 4.\n";
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

}