#include "game.h"

#include <cstdlib>

namespace lab2 {

namespace {
    constexpr int kEnergyRegen = 10;
    constexpr int kMoveEnergyCost = 1;
    constexpr int kAttackEnergyCost = 2;

    // Вероятности: чем больше статусов, тем меньше вероятность
    constexpr int kChanceTwoStatuses = 10;   // 10% — two
    constexpr int kChanceOneStatus   = 15;   // 15% — one (at all 25%)
    constexpr int kBurningCellTurns  = 3;
    constexpr int kBurningCellDamage = 4;
    constexpr int kShieldCellAmount  = 15;

    // inventory
    constexpr int kHealthPotionHeal = 30;
    constexpr int kEnergyPotionRegen = 20;
    constexpr int kAttackBoostBonus = 5;
    constexpr int kAttackBoostTurns = 3;
}

Game::Game(int width, int height) : field_(width, height),
        player_(std::make_unique<Robot>("Player", 100, 100, 10,
                                        40, 50, 0, 100,
                                        3,  // speed
                                        2)) // visibility_radius
{
    player_->SetPosition(0, 0); 

    explored_cells_.assign(field_.GetHeight(),
                            std::vector<bool>(field_.GetWidth(), false));
    UpdateVisibility();
}

void Game::ConfigureDemo(AbilityKind start_ability) {
    int fx = field_.GetWidth() / 2 - 1; // place of factory
    int fy = field_.GetHeight() / 2 - 1;

    for(int dy = 0; dy < 2; dy++) {
        for(int dx = 0; dx < 2; dx++) {
            field_.SetPassable(fx + dx, fy + dy, false);
        }
    }

    robotFactory_ = std::make_unique<RobotFactory>(fx, fy, 5);

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

    player_->AddAbility(start_ability);
    GenerateCellStatuses();
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
            // Heal or Teleport for enemy
            enemy->AddAbility((std::rand() % 2) ? AbilityKind::kHeal : AbilityKind::kTeleport);
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
    if (player_.get() != except && player_->GetX() == x && player_->GetY() == y) {
        return true;
    }

    for (const auto& enemy : enemies_) {
        if (enemy.get() == except) continue;
        if (!enemy->IsAlive()) continue;
        if (enemy->GetX() == x && enemy->GetY() == y) return true;
    }

    for (const auto& friend_ : friends_) {
        if (friend_.get() == except) continue;
        if (friend_->GetX() == x && friend_->GetY() == y) return true;
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
        return; // player doesn't attack friend
    }

    // enemy attacks only player
    if (player_->GetX() == x && player_->GetY() == y) {
        robot.Interact(*player_, true);
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

bool Game::TryMoveRobot(Robot& robot, int x, int y) {
    if (!field_.IsPassable(x, y)) return false;
    if (IsOccupiedExcept(x, y, &robot)) return false;
    if (robot.GetEnergy() < kMoveEnergyCost) return false;
    if (robot.IsMovementBlocked()) return false;   // slow

    robot.SetEnergy(robot.GetEnergy() - kMoveEnergyCost);
    robot.SetPosition(x, y);
    field_.ApplyRandomCellStatusTo(robot, x, y);
    return true;
}

bool Game::TryAttack(Robot& attacker, int x, int y) {
    if (attacker.GetEnergy() < kAttackEnergyCost) return false;
    attacker.SetEnergy(attacker.GetEnergy() - kAttackEnergyCost);
    ApplyInteraction(attacker, x, y);
    return false; // end of turn
}

bool Game::TryMovePlayer(int x, int y) {
    if (IsEnemyAt(x, y)) { // enemy
        return TryAttack(*player_, x, y);
    }
    return TryMoveRobot(*player_, x, y);
}

bool Game::TryMoveEnemy(Robot& enemy, int x, int y) {
    if (player_->GetX() == x && player_->GetY() == y) {
        if(!player_->IsAlive()) return false;
        return TryAttack(enemy, x, y);
    }
    return TryMoveRobot(enemy, x, y);
}

bool Game::TryMoveFriend(Robot& friendRobot, int x, int y) {
    if (player_->GetX() == x && player_->GetY() == y) return false;

    return TryMoveRobot(friendRobot, x, y);
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

    for (auto& enemy : enemies_) {
        if (!enemy->IsAlive()) continue;

        int budget = enemy->GetSpeed();
        int playerX = player_->GetX();
        int playerY = player_->GetY(); 

        while(budget > 0) {
            int x = enemy->GetX();
            int y = enemy->GetY();
            int dx = playerX - x;
            int dy = playerY - y;

            if (std::abs(dx) > std::abs(dy)) {
                x += (dx > 0) ? 1 : -1;
            } else if (dy != 0) {
                y += (dy > 0) ? 1 : -1;
            } else break;

            int cost = field_.GetPassCost(x, y);
            if (cost > budget) break;

            if (!TryMoveEnemy(*enemy, x, y)) break;

            budget -= cost;
        }
    }
}

void Game::StepFriends() {
    for (auto& friendRobot : friends_) {
        if (!friendRobot->IsAlive()) continue;

        int budget = friendRobot->GetSpeed();
        int playerX = player_->GetX();
        int playerY = player_->GetY(); 

        while (budget > 0) {
            int x = friendRobot->GetX();
            int y = friendRobot->GetY();

            int dx = playerX - x;
            int dy = playerY - y;

            if (std::abs(dx) + std::abs(dy) == 1) { // heals player
                friendRobot->Interact(*player_, false);
                break;
            }

            if (std::abs(dx) > std::abs(dy)) {
                x += (dx > 0) ? 1 : -1;
            } else if (dy != 0) {
                y += (dy > 0) ? 1 : -1;
            } else break;

            int cost = field_.GetPassCost(x, y);
            if (cost > budget) break;

            if (!TryMoveFriend(*friendRobot, x, y)) break;
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

void Game::EndPlayerTurn() {
    StepEnemies();
    StepFriends();
    if (!IsLevelComplete()) GenerateEnemyFromFactory();
    RegenerateEnergy();

    if (player_->HasStatus("Получение предмета")) {  // ItemPickup only for player
        AddItem(RandomItemKind());
    }
    // check status at the end of turn
    player_->EndTurn();
    for (auto& e : enemies_) if (e->IsAlive()) e->EndTurn();
    for (auto& f : friends_) f->EndTurn();

    UpdateVisibility();
}

void Game::Step(Direction direction) {
    if (!player_->IsAlive() || IsLevelComplete()) return;
    StepPlayer(direction);
    EndPlayerTurn();
}


bool Game::IsLevelComplete() const {
    for (const auto& enemy : enemies_) {
        if (enemy->IsAlive()) return false;
    }
    return true;
}

void Game::GenerateEnemyFromFactory() {
    if (robotFactory_ && robotFactory_->CanMakeRobot()) {
        int fx = robotFactory_->GetX();
        int fy = robotFactory_->GetY();
    
        for (int dy = -1; dy <= 2; dy++) { //go around factory
            for (int dx = -1; dx <= 2; dx++) {
                int x = fx + dx;
                int y = fy + dy;
    
                if (!field_.IsInside(x, y)) continue;
                if (robotFactory_->Occupies(x, y)) continue;
                if (!field_.IsPassable(x, y)) continue; // walls
                if (IsOccupiedExcept(x, y, nullptr)) continue; //anyone is excepted
    
                std::string name = "Enemy_" + std::to_string(enemies_.size());
    
                auto new_enemy = std::make_unique<Robot>(name, 40, 40, 5, 30, 30, 0, 100, 3, 1);
                new_enemy->SetPosition(x, y);
                new_enemy->AddAbility((std::rand() % 2) ? AbilityKind::kHeal : AbilityKind::kTeleport);
                enemies_.push_back(std::move(new_enemy));
                return;
            }
        }
        robotFactory_->ResetCounter(); // place for generating is not found
    }
}

// for abilities
std::vector<Robot*> Game::GetRobots() const {
    std::vector<Robot*> robots;
    if (player_->IsAlive()) robots.push_back(player_.get());
    for (const auto& e : enemies_) if (e->IsAlive()) robots.push_back(e.get());
    for (const auto& f : friends_) if (f->IsAlive()) robots.push_back(f.get());
    return robots;
}

void Game::DealDamage(Robot& target, int damage) {
    target.ApplyDamage(damage);   // from statuses (shield)
}

bool Game::TeleportRobot(Robot& robot, int x, int y) {
    if (!field_.IsPassable(x, y)) return false;
    if (IsOccupiedExcept(x, y, &robot)) return false;
    robot.SetPosition(x, y);
    field_.ApplyRandomCellStatusTo(robot, x, y);
    return true;
}

// for player
bool Game::PlayerUseAbility(AbilityKind kind, int tx, int ty) {
    return player_->TryUseAbility(*this, kind, tx, ty);
}

// inventory
void Game::AddItem(ItemKind kind) {
    inventory_.push_back(Item(kind));
}

bool Game::UseItem(std::size_t index) {
    if (index >= inventory_.size()) return false;
    ItemKind kind = inventory_[index].GetKind();
    switch (kind) {
        case ItemKind::kHealthPotion:
            player_->Heal(kHealthPotionHeal);
            break;
        case ItemKind::kEnergyPotion:
            player_->RegenerateEnergy(kEnergyPotionRegen);
            break;
        case ItemKind::kAttackBoost:
            player_->AddStatus(CreateAttackBoostStatus(kAttackBoostBonus, kAttackBoostTurns));
            break;
    }
    inventory_.erase(inventory_.begin() + index);
    return true;
}

ItemKind Game::RandomItemKind() {
    switch (std::rand() % 3) {
        case 0: return ItemKind::kHealthPotion;
        case 1: return ItemKind::kEnergyPotion;
        default: return ItemKind::kAttackBoost;
    }
}

// statuses
void Game::GenerateCellStatuses() {
    for (int y = 0; y < field_.GetHeight(); ++y) {
        for (int x = 0; x < field_.GetWidth(); ++x) {
            if (!field_.IsPassable(x, y)) continue;
            int roll = std::rand() % 100;
            if (roll < kChanceTwoStatuses) {
                // 10% — два статуса
                field_.AddStatusToCell(x, y, RandomStatus());
                field_.AddStatusToCell(x, y, RandomStatus());
            } else if (roll < kChanceTwoStatuses + kChanceOneStatus) {
                // 15% — один статус
                field_.AddStatusToCell(x, y, RandomStatus());
            }
            // остальные 75% — без статусов
        }
    }
}

std::unique_ptr<IStatus> Game::RandomStatus() {
    switch (std::rand() % 5) {
        case 0: return CreateSlowStatus();
        case 1: return CreateOverloadStatus();
        case 2: return CreateBurningStatus(kBurningCellTurns, kBurningCellDamage);
        case 3: return CreateShieldStatus(kShieldCellAmount);
        default: return CreateItemPickupStatus();   // 20% — предмет
    }
}
}
