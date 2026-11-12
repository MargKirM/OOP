#ifndef LAB2_ABILITY_H_
#define LAB2_ABILITY_H_

#include <memory>
#include <string>

namespace lab2 {

class Robot;
class Game;

enum class AbilityKind {
    kAreaStrike, kRangedStrike, kHeal, kTeleport
};

class ISpecialAbility { // virtual class-interface
public:
    virtual const std::string& GetName() const = 0;
    virtual int GetEnergyCost() const = 0;
    virtual bool CanUse(const Robot& owner, const Game& game,  int tx, int ty) const = 0;
    virtual void Use(Robot& owner, Game& game, int tx, int ty) const = 0;
    virtual void Upgrade() = 0;
    virtual bool IsUpgradable() const = 0;
};

std::unique_ptr<ISpecialAbility> CreateAbility(AbilityKind kind);
const char* AbilityName(AbilityKind kind);
}

#endif