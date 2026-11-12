#include "ability.h"

#include <cstdlib>

#include "game.h"
#include "robot.h"

namespace lab2 {

constexpr int kHealEnergyCost = 6;
constexpr int kHealBaseAmount = 20;
constexpr int kHealAmountStep = 10;

constexpr int kTeleportRange          = 5;
constexpr int kTeleportBaseEnergyCost = 8;
constexpr int kTeleportMinEnergyCost  = 2;

constexpr int kBurningAbilityTurns  = 3;
constexpr int kBurningAbilityDamage = 5;

// -------------------------------------------------------------------------------
constexpr int kAreaStrikeBaseRadius = 1;
constexpr int kAreaStrikeMaxRadius  = 3;
constexpr int kAreaStrikeEnergyCost = 5;
constexpr int kAreaStrikeDamage     = 8;


RoundStrike::RoundStrike(Game& game): game_(game), radius_(kAreaStrikeBaseRadius) {}

const std::string& RoundStrike::GetName()const {
    static std::string name = "RoundStrike";
    return name;
}

void RoundStrike::apply(Robot& owner) {
    const int ox = owner.GetX();
    const int oy = owner.GetY();
    game_.ForEachRobot([&](Robot& robot) {
        if (&robot == &owner) return;
        if (!robot.IsAlive()) return;
        if (game_.IsFriend(robot)) return;
        if (std::abs(robot.GetX() - ox) <= radius_ &&
            std::abs(robot.GetY() - oy) <= radius_) {
            robot.ApplyDamage(kAreaStrikeDamage);
            robot.AddStatus(CreateOverloadStatus());   // + Перегрузка
        }
    });
}

int RoundStrike::energyCost() {
    return kAreaStrikeEnergyCost;
}

void RoundStrike::modify(int value) {
    radius_ += value;
    if (radius_ > kAreaStrikeMaxRadius) {
        radius_ = kAreaStrikeMaxRadius;
    }
}

// -------------------------------------------------------------------------------
constexpr int kFarStrikeRange      = 3;
constexpr int kFarStrikeEnergyCost = 4;
constexpr int kFarStrikeBaseDamage = 12;
constexpr int kFarStrikeDamageStep = 5;


FarStrike::FarStrike(Game& game): game_(game), radius_(kAreaStrikeBaseRadius) {}

const std::string& FarStrike::GetName()const {
    static std::string name = "FarStrike";
    return name;
}

void FarStrike::apply(Robot& owner) {
    if (!owner.IsAlive()) return false;
    if (std::abs(tx - owner.GetX()) > kRangedStrikeRange) return false;
    if (std::abs(ty - owner.GetY()) > kRangedStrikeRange) return false;

    bool found = false;
    game.ForEachRobot([&](Robot& robot) {
        if (&robot == &owner) return;
        if (game.IsFriend(robot)) return;
        if (robot.IsAlive() &&
            robot.GetX() == tx && robot.GetY() == ty) {
            found = true;
        }
    });
    return found;
}

int FarStrike::energyCost() {
    return kFarStrikeEnergyCost;
}

void FarStrike::modify(int value) {
    radius_ += value;
    if (radius_ > kAreaStrikeMaxRadius) {
        radius_ = kAreaStrikeMaxRadius;
    }
}

//     class RangedStrike : public ISpecialAbility {

//         bool CanUse(const Robot& owner, const Game& game, int tx, int ty) const override {
//             if (!owner.IsAlive()) return false;
//             if (std::abs(tx - owner.GetX()) > kRangedStrikeRange) return false;
//             if (std::abs(ty - owner.GetY()) > kRangedStrikeRange) return false;

//             bool found = false;
//             game.ForEachRobot([&](Robot& robot) {
//                 if (&robot == &owner) return;
//                 if (game.IsFriend(robot)) return;
//                 if (robot.IsAlive() &&
//                     robot.GetX() == tx && robot.GetY() == ty) {
//                     found = true;
//                 }
//             });
//             return found;
//         }

//         void Use(Robot& owner, Game& game, int tx, int ty) const override {
//             Robot* target = nullptr;
//             game.ForEachRobot([&](Robot& robot) {
//                 if (&robot == &owner) return;
//                 if (game.IsFriend(robot)) return;
//                 if (robot.IsAlive() &&
//                     robot.GetX() == tx && robot.GetY() == ty) {
//                     target = &robot;
//                 }
//             });
//             if (target) {
//                 game.DealDamage(*target, damage_);
//                 target->AddStatus(CreateBurningStatus(kBurningAbilityTurns, kBurningAbilityDamage));   // + Burning
//             }
//         }

//         void Upgrade() override { damage_ += kRangedStrikeDamageStep; }
//         bool IsUpgradable() const override { return true; }

//     private:
//         int damage_ = kRangedStrikeBaseDamage;
//     };

//     class Heal : public ISpecialAbility {
//     public:
//         const std::string& GetName() const override {
//             static const std::string kName = "Лечение";
//             return kName;
//         }
//         int GetEnergyCost() const override { return kHealEnergyCost; }

//         bool CanUse(const Robot& owner, const Game&, int, int) const override {
//             return owner.IsAlive();
//         }

//         void Use(Robot& owner, Game&, int, int) const override {
//             const int before = owner.GetHealth();
//             const int overflow = before + amount_ - owner.GetMaxHealth();
//             owner.Heal(amount_);
//             if (overflow > 0) {
//                 owner.AddStatus(CreateShieldStatus(overflow));   // Shield = overflow
//             }
//         }

//         void Upgrade() override { amount_ += kHealAmountStep; }
//         bool IsUpgradable() const override { return true; }

//     private:
//         int amount_ = kHealBaseAmount;
//     };

//     class Teleport : public ISpecialAbility {
//     public:
//         const std::string& GetName() const override {
//             static const std::string kName = "Телепортация";
//             return kName;
//         }
//         int GetEnergyCost() const override { return energy_cost_; }

//         bool CanUse(const Robot& owner, const Game& game,
//                     int tx, int ty) const override {
//             if (!owner.IsAlive()) return false;
//             if (std::abs(tx - owner.GetX()) > kTeleportRange) return false;
//             if (std::abs(ty - owner.GetY()) > kTeleportRange) return false;
//             if (!game.GetField().IsPassable(tx, ty)) return false;
//             if (game.IsOccupiedExcept(tx, ty, &owner)) return false;
//             return true;
//         }

//         void Use(Robot& owner, Game& game, int tx, int ty) const override {
//             game.TeleportRobot(owner, tx, ty);
//         }

//         void Upgrade() override {
//             if (energy_cost_ > kTeleportMinEnergyCost) --energy_cost_;
//         }
//         bool IsUpgradable() const override {
//             return energy_cost_ > kTeleportMinEnergyCost;
//         }

//     private:
//         int energy_cost_ = kTeleportBaseEnergyCost;
//     };
// }

// std::unique_ptr<ISpecialAbility> CreateAbility(AbilityKind kind) {
//     switch (kind) {
//         case AbilityKind::kAreaStrike:   return std::make_unique<AreaStrike>();
//         case AbilityKind::kRangedStrike: return std::make_unique<RangedStrike>();
//         case AbilityKind::kHeal:         return std::make_unique<Heal>();
//         case AbilityKind::kTeleport:     return std::make_unique<Teleport>();
//     }
//     return nullptr;
// }

// const char* AbilityName(AbilityKind kind) {
//     switch (kind) {
//         case AbilityKind::kAreaStrike:   return "Удар вокруг";
//         case AbilityKind::kRangedStrike: return "Дальний удар";
//         case AbilityKind::kHeal:         return "Лечение";
//         case AbilityKind::kTeleport:     return "Телепортация";
//     }
//     return "Неизвестная способность";
// }

}