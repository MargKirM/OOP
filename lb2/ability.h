#ifndef LAB2_ABILITY_H_
#define LAB2_ABILITY_H_

#include <memory>
#include <string>
#include "robot.h"
#include "game.h"

namespace lab2 {


class IAbility { // virtual class-interface
public:
    virtual const std::string& GetName() const = 0;
    virtual void apply(Robot& robot) = 0;
    virtual int energyCost() = 0;
    virtual void modify(int value) = 0;
};

class RoundStrike : public IAbility {
public:
    RoundStrike(Game& game);

    const std::string& GetName() const override;
    void apply(Robot& robot) override;
    int energyCost() override;
    void modify(int value) override;

private:
    Game& game_;
    int radius_;
};
class FarStrike : public IAbility {
public:
    FarStrike(Game& game): game_(game) {}

    const std::string& GetName() const override;
    void apply(Robot& robot) override;
    int energyCost() override;
    void modify(int value) override;

private:
    Game& game_;
};
class Heal : public IAbility {
public:
    Heal() = default;

    const std::string& GetName() const override;
    void apply(Robot& robot) override;
    int energyCost() override;
    void modify(int value) override;
};
class Teleport : public IAbility {
public:
    Teleport(Game& game): game_(game) {}

    const std::string& GetName() const override;
    void apply(Robot& robot) override;
    void modify(int value) override;

private:
    Game& game_;
};
}

#endif