#include "console_renderer.h"

#include <utility>
#include <iostream>

namespace lab1 {

ConsoleRenderer::ConsoleRenderer(const Game& game) : game_(game) {}

char ConsoleRenderer::GetCellSymbol(int x, int y) const {

    if (!game_.IsExplored(x, y)) return '?'; // not explored cell

    if (game_.GetPlayerPosition() == std::make_pair(x, y)) {  //explored cell
            return game_.GetPlayer().IsAlive() ? 'P' : 't';
    }

    if(!game_.IsVisible(x, y)) {
        for (const auto& factory : game_.GetFactories()) {
            if (factory->Occupies(x, y)) return '?';   // factory is unvisible
        }
        if (!game_.GetField().IsPassable(x, y)) { // not passable cell
            return '#';
        }
        return '?';
    }
    
    const auto& positions = game_.GetEnemyPositions();
    const auto& enemies = game_.GetEnemies();

    for(std::size_t i = 0; i < positions.size(); i++) {
        if (positions[i] == std::make_pair(x, y)) {
            return enemies[i]->IsAlive() ? 'E' : 'x';
        }
    }

    const auto& friend_positions = game_.GetFriendPositions();

    for (std::size_t i = 0; i < friend_positions.size(); i++) { 
            if (friend_positions[i] == std::make_pair(x, y)) {
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

void ConsoleRenderer::Render() const {
    const GameField& field = game_.GetField();

    for(int y = 0; y < field.GetHeight(); y++) {
        for(int x = 0; x < field.GetWidth(); x++) {
            std::cout << GetCellSymbol(x, y) << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "\nP — игрок, E — враг, F — друг, "
                 "@ — фабрика роботов, x — убитый враг, t — убитый игрок,"
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
    
    if(!player.IsAlive()) {
        std::cout << "Player is dead. Game over!\n";
    }else if(game_.IsLevelComplete()) {
        std::cout << "Level complete!\n";
    }
}

}