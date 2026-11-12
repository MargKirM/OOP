#ifndef LAB1_GAME_H_
#define LAB1_GAME_H_

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "ability.h"
#include "direction.h"
#include "game_field.h"
#include "item.h"
#include "robot.h"
#include "robot_factory.h"
#include "status.h"

namespace lab2 {

class Game {
public:
    Game(int width, int height); // constructor

    Game(const Game&) = delete; // copy constructor is forbidden (unique_ptr)
    Game& operator=(const Game&) = delete; // copy operator is forbidden

    Game(Game&&) = delete; // move constructor is forbidden
    Game& operator=(Game&&) = delete; // move operator is forbidden

    ~Game() = default; // destructor

    void ConfigureDemo(AbilityKind start_ability = AbilityKind::kAreaStrike);
    void Step(Direction direction);
    bool IsLevelComplete() const;
    void EndPlayerTurn();

    const GameField& GetField() const {
        return field_;
    }
    const Robot& GetPlayer() const {
        return *player_;
    }

    const std::vector<std::unique_ptr<Robot>>& GetEnemies() const {
        return enemies_;
    }

    const std::vector<std::unique_ptr<Robot>>& GetFriends() const {
        return friends_;
    }

    const std::vector<std::unique_ptr<RobotFactory>>& GetFactories() const {
        return factories_;
    }

    std::pair<int, int> GetPlayerPosition() const {
        return {player_->GetX(), player_->GetY()};
    }

    bool IsFriend(const Robot& robot) const;

    bool IsVisible(int x, int y) const;
    bool IsExplored(int x, int y) const;

    // for lighting
    std::size_t GetCellStatusCount(int x, int y) const {
        return field_.GetCellStatusCount(x, y);
    }

    // for abilities
    void ForEachRobot(const std::function<void(Robot&)>& fn) const;
    void DealDamage(Robot& target, int damage);
    bool TeleportRobot(Robot& robot, int x, int y);

    bool IsOccupiedExcept(int x, int y, const Robot* except) const;

    // for player
    bool PlayerUseAbility(AbilityKind kind, int tx, int ty);

    // only player
    const std::vector<Item>& GetInventory() const { return inventory_; }
    void AddItem(ItemKind kind);
    bool UseItem(std::size_t index);

    // choose ability and rank up
    void SetRankUpChoice(std::function<void(Robot&)> cb);

private:
    void GenerateEnemies(int count);
    void GenerateFriends(int count);

    void StepPlayer(Direction direction);
    void StepEnemies();
    void StepFriends();
    void RegenerateEnergy();
    void ApplyInteraction(Robot& robot, int x, int y);

    bool TryAttack(Robot& attacker, int x, int y);
    bool TryMoveRobot(Robot& robot, int x, int y);
    bool TryMovePlayer(int x, int y);
    bool TryMoveEnemy(std::size_t index, int x, int y);
    bool TryMoveFriend(std::size_t index, int x, int y);
    void UpdateVisibility();

    bool IsEnemyAt(int x, int y) const;

    void StepFactories();
    void GeneratorEnemyFromFactory(RobotFactory& factory);

    void GenerateCellStatuses();
    std::unique_ptr<IStatus> RandomStatus();
    ItemKind RandomItemKind();

    GameField field_;
    std::vector<std::vector<bool>> explored_cells_;
    std::unique_ptr<Robot> player_;

    std::vector<std::unique_ptr<Robot>> enemies_;
    std::vector<std::unique_ptr<Robot>> friends_;

    std::vector<std::unique_ptr<RobotFactory>> factories_;

    std::vector<Item> inventory_;   // player

    std::function<void(Robot&)> rank_up_choice_;
};

}

#endif