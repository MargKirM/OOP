#include "game.h"

#include <cstdlib>

namespace lab2 {

namespace {
    constexpr int kEnergyRegen = 10;
    constexpr int kMoveEnergyCost = 1;
    constexpr int kAttackEnergyCost = 2;
}

Game::Game(int width, int height) : field_(width, height),
        player_(std::make_unique<Robot>("Player", 100, 100, 10,
                                        40, 50, 0, 100, 
                                        3,  // speed
                                        2)) // visibility_radius
{
    player_->SetPosition(0, 0); // Устанавливаем позицию игрока

    explored_cells_.assign(field_.GetHeight(), 
                            std::vector<bool>(field_.GetWidth(), false));
    UpdateVisibility();
}



void Game::ConfigureDemo() {

    int fx = field_.GetWidth() / 2 - 1; // place of factory
    int fy = field_.GetHeight() / 2 - 1;
    
    for(int dy = 0; dy < 2; dy++) {
        for(int dx = 0; dx < 2; dx++) {
            field_.SetPassable(fx + dx, fy + dy, false);
        }
    }
    
    factories_.push_back(std::make_unique<RobotFactory>(fx, fy, 5));
    
    field_.SetPassable(1, 1, false); // walls
    field_.SetPassable(2, 2, false);
    field_.SetPassable(3, 1, false);
    field_.SetPassable(4, 4, false);
    field_.SetPassable(4, 5, false);
    field_.SetPassable(10, 3, false);

    field_.SetPassCost(5, 5, 3); // swamp (cost == 3)
    field_.SetPassCost(6, 5, 3);
    field_.SetPassCost(3, 10, 3);
    field_.SetPassCost(2, 10, 3);
    field_.SetPassCost(2, 11, 3);

    field_.SetPassCost(8, 8, 2); // dirt (cost == 2)
    field_.SetPassCost(9, 8, 2);
    field_.SetPassCost(8, 9, 2);

    GenerateEnemies(3);
    GenerateFriends(1); 
}

void Game::GenerateEnemies(int count) {
    enemies_.clear();

    int placed = 0;
    for (int y = 0; y < field_.GetHeight() && placed < count; ++y) {
        for (int x = 0; x < field_.GetWidth() && placed < count; ++x) {
            if (!field_.IsPassable(x, y)) continue;
            if (player_->GetX() == x && player_->GetY() == y) continue;
            if (IsOccupiedExcept(x, y, nullptr)) continue;

            auto enemy = std::make_unique<Robot>(
                "Enemy" + std::to_string(placed), 40, 40, 5,
                30, 30, 0, 100, 3, 1);
            enemy->SetPosition(x, y);
            enemies_.push_back(std::move(enemy));
            ++placed;
        }
    }
}

void Game::GenerateFriends(int count) {
    friends_.clear();

    int placed = 0;
    for (int y = 0; y < field_.GetHeight() && placed < count; ++y) {
        for (int x = 0; x < field_.GetWidth() && placed < count; ++x) {
            if (!field_.IsPassable(x, y)) continue;
            if (player_->GetX() == x && player_->GetY() == y) continue;
            if (IsOccupiedExcept(x, y, nullptr)) continue;

            auto friend_ = std::make_unique<Robot>(
                "Friend" + std::to_string(placed), 40, 40, 0,
                20, 20, 0, 100, 1, 1);
            friend_->SetPosition(x, y);
            friends_.push_back(std::move(friend_));
            ++placed;
        }
    }
}

bool Game::IsOccupiedExcept(int x, int y, const Robot* except) const {
    if (player_.get() != except && player_->GetX() == x && player_->GetY() == y) { //occupied by player
        return true;
    }

    for (const auto& enemy : enemies_) {
        if (enemy.get() == except) continue;
        if (!enemy->IsAlive()) continue;
        if (enemy->GetX() == x && enemy->GetY() == y) return true; //occupied by enemy
    }
    
    for (const auto& friend_ : friends_) {
        if (friend_.get() == except) continue;
        if (friend_->GetX() == x && friend_->GetY() == y) return true; //occupied by friend
    }

    return false;
}

bool Game::IsEnemyAt(int x, int y) const {
    for (const auto& enemy : enemies_) {
        if (!enemy->IsAlive()) continue;
        if (enemy->GetX() == x && enemy->GetY() == y) return true;
    }
    return false;
}

bool Game::IsFriend(const Robot& robot) const {
    for (const auto& f : friends_) {
        if (f.get() == &robot) return true; // is friend
    }
    return false;
}

void Game::ApplyInteraction(Robot& robot, int x, int y) {
    // friend
    if (IsFriend(robot)) {
        if (player_->GetX() == x && player_->GetY() == y) {
            robot.Interact(*player_, false); // heal
        }
        return;
    }

    // player attacks
    if (&robot == player_.get()) {
        for (const auto& enemy : enemies_) {
            if (!enemy->IsAlive()) continue;
            if (enemy->GetX() == x && enemy->GetY() == y) {
                robot.Interact(*enemy, true);
                return;
            }
        }
        return; // player don't attacks friend
    }
    
    for (const auto& enemy : enemies_) {
        if (&robot == enemy.get()) {
            if (player_->GetX() == x && player_->GetY() == y) {
                robot.Interact(*player_, true);
            }
            return; // enemy attacks only player (not other enemies, not friend)
        }
    }
}

bool Game::IsVisible(int x, int y) const {
    if (!field_.IsInside(x, y)) return false;

    int r = player_->GetVisibilityRadius();
    int dx = std::abs(x - player_->GetX());
    int dy = std::abs(y - player_->GetY());

    return dx <= r && dy <= r;
}

bool Game::IsExplored(int x, int y) const {
    if (!field_.IsInside(x, y)) return false;
    return explored_cells_[y][x];
}

void Game::UpdateVisibility() {
    int r = player_->GetVisibilityRadius();
    int px = player_->GetX();
    int py = player_->GetY();

    for (int y = py - r; y <= py + r; y++) {
        for (int x = px - r; x <= px + r; x++) {
            if (field_.IsInside(x, y)) {
                explored_cells_[y][x] = true;
            }
        }
    }
}


bool Game::TryMovePlayer(int x, int y) {
    if (!field_.IsPassable(x, y)) return false;

    if (IsEnemyAt(x, y)) { // enemy
        if(player_->GetEnergy() < kAttackEnergyCost) return false;
        player_->SetEnergy(player_->GetEnergy() - kAttackEnergyCost);
        ApplyInteraction(*player_, x, y);
        return false; // end of turn
    }

    if (IsOccupiedExcept(x, y, player_.get())) return false;

    if(player_->GetEnergy() < kMoveEnergyCost) return false;
    player_->SetEnergy(player_->GetEnergy() - kMoveEnergyCost);

    player_->SetPosition(x, y);
    return true;
}

bool Game::TryMoveEnemy(std::size_t index, int x, int y) {
    if (!field_.IsPassable(x, y)) return false;

    if (player_->GetX() == x && player_->GetY() == y) {
        if(!player_->IsAlive()) return false;
        if (enemies_[index]->GetEnergy() < kAttackEnergyCost) return false;
        enemies_[index]->SetEnergy(enemies_[index]->GetEnergy() - kAttackEnergyCost);
        ApplyInteraction(*enemies_[index], x, y);
        return false;
    }

    if (IsOccupiedExcept(x, y, enemies_[index].get())) return false;

    if (enemies_[index]->GetEnergy() < kMoveEnergyCost) return false;
    enemies_[index]->SetEnergy(enemies_[index]->GetEnergy() - kMoveEnergyCost);
    enemies_[index]->SetPosition(x, y);
    return true;
}

bool Game::TryMoveFriend(std::size_t index, int x, int y) {
    if (!field_.IsPassable(x, y)) return false;

    if (player_->GetX() == x && player_->GetY() == y) return false;
    if (IsOccupiedExcept(x, y, friends_[index].get())) return false;

    if (friends_[index]->GetEnergy() < kMoveEnergyCost) return false;
    friends_[index]->SetEnergy(friends_[index]->GetEnergy() - kMoveEnergyCost);
    friends_[index]->SetPosition(x, y);
    return true;
}

void Game::StepPlayer(Direction direction) {
    if (!player_->IsAlive()) return;

    int budget = player_->GetSpeed();

    while(budget > 0) {
        int x = player_->GetX();
        int y = player_->GetY();

        switch (direction) {
            case Direction::kUp:    --y; break;
            case Direction::kDown:  ++y; break;
            case Direction::kLeft:  --x; break;
            case Direction::kRight: ++x; break;
        }

        int cost = field_.GetPassCost(x, y);
        if (cost > budget) break;

        if(!TryMovePlayer(x, y)) break;

        budget -= cost;
    }
}

void Game::StepEnemies() {
    if (!player_->IsAlive()) return;

    for (std::size_t i = 0; i < enemies_.size(); ++i) {
        if (!enemies_[i]->IsAlive()) continue;

        int budget = enemies_[i]->GetSpeed();

        while(budget > 0) {
            int dx = player_->GetX() - enemies_[i]->GetX();
            int dy = player_->GetY() - enemies_[i]->GetY();
            int x = enemies_[i]->GetX();
            int y = enemies_[i]->GetY();

            if (std::abs(dx) > std::abs(dy)) {
                x += (dx > 0) ? 1 : -1;
            } else if (dy != 0) {
                y += (dy > 0) ? 1 : -1;
            } else break;

            int cost = field_.GetPassCost(x, y);
            if (cost > budget) break;

            if(!TryMoveEnemy(i, x, y)) break;

            budget -= cost;
        }
    }
}

void Game::StepFriends() {
    for (std::size_t i = 0; i < friends_.size(); ++i) {
        if (!friends_[i]->IsAlive()) continue;

        int budget = friends_[i]->GetSpeed();
        while (budget > 0) {
            int dx = player_->GetX() - friends_[i]->GetX();
            int dy = player_->GetY() - friends_[i]->GetY();

            if (std::abs(dx) + std::abs(dy) == 1) { // heals player
                friends_[i]->Interact(*player_, false);
                break;
            }

            int x = friends_[i]->GetX();
            int y = friends_[i]->GetY();
            if (std::abs(dx) > std::abs(dy)) x += (dx > 0) ? 1 : -1;
            else if (dy != 0)               y += (dy > 0) ? 1 : -1;
            else break;

            int cost = field_.GetPassCost(x, y);
            if (cost > budget) break;

            if (!TryMoveFriend(i, x, y)) break;
            budget -= cost;
        }
    }
}

void Game::RegenerateEnergy() {
    if(player_->IsAlive()) {
        player_->RegenerateEnergy(kEnergyRegen);
    }
    
    for (auto& enemy : enemies_) {
        if(enemy->IsAlive()) enemy->RegenerateEnergy(kEnergyRegen);
    }

    for (auto& friend_ : friends_) {
        friend_->RegenerateEnergy(kEnergyRegen);
    }
}

void Game::Step(Direction direction) {
    if(!player_->IsAlive() || IsLevelComplete()) return;
    StepPlayer(direction);
    StepEnemies();
    StepFriends();
    if(!IsLevelComplete()) StepFactories();
    RegenerateEnergy();
    UpdateVisibility();
}

bool Game::IsLevelComplete() const {
    for (const auto& enemy : enemies_) {
        if (enemy->IsAlive()) return false;
    }
    return true;
}

void Game::StepFactories() {
    for (auto& factory : factories_) {
        if (factory->ToMakeRobot()) {
            GeneratorEnemyFromFactory(*factory);
        }
    }
}

void Game::GeneratorEnemyFromFactory(RobotFactory& factory) {
    int fx = factory.GetX();
    int fy = factory.GetY();

    for (int dy = -1; dy <= 2; dy++) { //go around factory
        for (int dx = -1; dx <= 2; dx++) {
            int x = fx + dx;
            int y = fy + dy;

            if (!field_.IsInside(x, y)) continue;
            if (factory.Occupies(x, y)) continue;
            if (!field_.IsPassable(x, y)) continue; // walls
            if (IsOccupiedExcept(x, y, nullptr)) continue; //anyone is excepted

            std::string name = "Enemy_" + std::to_string(enemies_.size());

            auto new_enemy = std::make_unique<Robot>(name, 40, 40, 5, 30, 30, 0, 100, 3, 1);
            new_enemy->SetPosition(x, y);
            enemies_.push_back(std::move(new_enemy));
            return;
        }
    }
    factory.ResetCounter(); // place for generating is not found
}

}