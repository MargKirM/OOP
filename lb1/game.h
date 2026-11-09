#ifndef LAB1_GAME_H_
#define LAB1_GAME_H_

#include <memory>
#include <utility>
#include <vector>

#include "direction.h"
#include "game_field.h"
#include "robot.h"
#include "robot_factory.h"

namespace lab1 {

class Game {
public:
    Game(int width, int height); // constructor

    Game(const Game&) = delete; // copy constructor is forbidden (unique_ptr)
    Game& operator=(const Game&) = delete; // copy operator is forbidden

    Game(Game&&) = delete; // move constructor is forbidden
    Game& operator=(Game&&) = delete; // move operator is forbidden

    ~Game() = default; // destructor

    void ConfigureDemo();
    void Step(Direction direction);
    bool IsLevelComplete() const;

    const GameField& GetField() const {
        return field_;
    }
    const Robot& GetPlayer() const {
        return *player_;
    }
    
    const std::vector<std::unique_ptr<Robot>>& GetEnemies() const {
        return enemies_;
    }

    const std::vector<std::pair<int, int>>& GetEnemyPositions() const {
        return enemy_positions_;
    }

    
    std::pair<int, int> GetPlayerPosition() const {
        return player_position_;
    }

    const std::vector<std::unique_ptr<Robot>>& GetFriends() const {
        return friends_;
    }

    const std::vector<std::pair<int, int>>& GetFriendPositions() const {
        return friends_positions_;
    }

    const std::vector<std::unique_ptr<RobotFactory>>& GetFactories() const {
        return factories_;
    }

    bool IsVisible(int x, int y) const;
    bool IsExplored(int x, int y) const;

private:
    void GenerateEnemies(int count);
    void GenerateFriends(int count);

    void StepPlayer(Direction direction);
    void StepEnemies();
    void StepFriends();
    void RegenerateEnergy();
    void ApplyInteraction(Robot& robot, int x, int y);
    bool TryMovePlayer(int x, int y);
    bool TryMoveEnemy(std::size_t index, int x, int y);
    bool TryMoveFriend(std::size_t index, int x, int y);
    void UpdateVisibility();
    
    bool IsOccupiedExcept(int x, int y, const Robot* except) const;
   
    bool IsEnemyAt(int x, int y) const;
    bool IsFriend(const Robot& robot) const;

    void StepFactories();
    void GeneratorEnemyFromFactory(RobotFactory& factory);

    GameField field_;
    std::vector<std::vector<bool>> explored_cells_;
    std::unique_ptr<Robot> player_;
    std::pair<int, int> player_position_;
    std::vector<std::unique_ptr<Robot>> enemies_;
    std::vector<std::pair<int, int>> enemy_positions_;

    std::vector<std::unique_ptr<Robot>> friends_;
    std::vector<std::pair<int, int>> friends_positions_;

    std::vector<std::unique_ptr<RobotFactory>> factories_;
};

}

#endif