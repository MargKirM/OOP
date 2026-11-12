#include "ability.h"

#include <cstdlib>

#include "game.h"
#include "robot.h"

namespace lab2 {

namespace {
    constexpr int kAreaStrikeBaseRadius = 1;
    constexpr int kAreaStrikeMaxRadius  = 3;
    constexpr int kAreaStrikeEnergyCost = 5;
    constexpr int kAreaStrikeDamage     = 8;

    constexpr int kRangedStrikeRange      = 3;
    constexpr int kRangedStrikeEnergyCost = 4;
    constexpr int kRangedStrikeBaseDamage = 12;
    constexpr int kRangedStrikeDamageStep = 5;

    constexpr int kHealEnergyCost = 6;
    constexpr int kHealBaseAmount = 20;
    constexpr int kHealAmountStep = 10;

    constexpr int kTeleportRange          = 5;
    constexpr int kTeleportBaseEnergyCost = 8;
    constexpr int kTeleportMinEnergyCost  = 2;

    constexpr int kBurningAbilityTurns  = 3;
    constexpr int kBurningAbilityDamage = 5;

    class AreaStrike : public ISpecialAbility {
    public:
        const std::string& GetName() const override {
            static const std::string kName = "Удар вокруг";
            return kName;
        }
        int GetEnergyCost() const override { return kAreaStrikeEnergyCost; }

        bool CanUse(const Robot& owner, const Game&, int, int) const override {
            return owner.IsAlive();
        }

        void Use(Robot& owner, Game& game, int, int) const override {
            const int ox = owner.GetX();
            const int oy = owner.GetY();
            game.ForEachRobot([&](Robot& robot) {
                if (&robot == &owner) return;
                if (!robot.IsAlive()) return;
                if (game.IsFriend(robot)) return;
                if (std::abs(robot.GetX() - ox) <= radius_ &&
                    std::abs(robot.GetY() - oy) <= radius_) {
                    game.DealDamage(robot, kAreaStrikeDamage);
                    robot.AddStatus(CreateOverloadStatus());   // + Перегрузка
                }
            });
        }

        void Upgrade() override {
            if (radius_ < kAreaStrikeMaxRadius) ++radius_;
        }
        bool IsUpgradable() const override {
            return radius_ < kAreaStrikeMaxRadius;
        }

    private:
        int radius_ = kAreaStrikeBaseRadius;
    };

    class RangedStrike : public ISpecialAbility {
    public:
        const std::string& GetName() const override {
            static const std::string kName = "Дальний удар";
            return kName;
        }
        int GetEnergyCost() const override { return kRangedStrikeEnergyCost; }

        bool CanUse(const Robot& owner, const Game& game, int tx, int ty) const override {
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

        void Use(Robot& owner, Game& game, int tx, int ty) const override {
            Robot* target = nullptr;
            game.ForEachRobot([&](Robot& robot) {
                if (&robot == &owner) return;
                if (game.IsFriend(robot)) return;
                if (robot.IsAlive() &&
                    robot.GetX() == tx && robot.GetY() == ty) {
                    target = &robot;
                }
            });
            if (target) {
                game.DealDamage(*target, damage_);
                target->AddStatus(CreateBurningStatus(kBurningAbilityTurns, kBurningAbilityDamage));   // + Burning
            }
        }

        void Upgrade() override { damage_ += kRangedStrikeDamageStep; }
        bool IsUpgradable() const override { return true; }

    private:
        int damage_ = kRangedStrikeBaseDamage;
    };

    class Heal : public ISpecialAbility {
    public:
        const std::string& GetName() const override {
            static const std::string kName = "Лечение";
            return kName;
        }
        int GetEnergyCost() const override { return kHealEnergyCost; }

        bool CanUse(const Robot& owner, const Game&, int, int) const override {
            return owner.IsAlive();
        }

        void Use(Robot& owner, Game&, int, int) const override {
            const int before = owner.GetHealth();
            const int overflow = before + amount_ - owner.GetMaxHealth();
            owner.Heal(amount_);
            if (overflow > 0) {
                owner.AddStatus(CreateShieldStatus(overflow));   // Shield = overflow
            }
        }

        void Upgrade() override { amount_ += kHealAmountStep; }
        bool IsUpgradable() const override { return true; }

    private:
        int amount_ = kHealBaseAmount;
    };

    class Teleport : public ISpecialAbility {
    public:
        const std::string& GetName() const override {
            static const std::string kName = "Телепортация";
            return kName;
        }
        int GetEnergyCost() const override { return energy_cost_; }

        bool CanUse(const Robot& owner, const Game& game,
                    int tx, int ty) const override {
            if (!owner.IsAlive()) return false;
            if (std::abs(tx - owner.GetX()) > kTeleportRange) return false;
            if (std::abs(ty - owner.GetY()) > kTeleportRange) return false;
            if (!game.GetField().IsPassable(tx, ty)) return false;
            if (game.IsOccupiedExcept(tx, ty, &owner)) return false;
            return true;
        }

        void Use(Robot& owner, Game& game, int tx, int ty) const override {
            game.TeleportRobot(owner, tx, ty);
        }

        void Upgrade() override {
            if (energy_cost_ > kTeleportMinEnergyCost) --energy_cost_;
        }
        bool IsUpgradable() const override {
            return energy_cost_ > kTeleportMinEnergyCost;
        }

    private:
        int energy_cost_ = kTeleportBaseEnergyCost;
    };
}

std::unique_ptr<ISpecialAbility> CreateAbility(AbilityKind kind) {
    switch (kind) {
        case AbilityKind::kAreaStrike:   return std::make_unique<AreaStrike>();
        case AbilityKind::kRangedStrike: return std::make_unique<RangedStrike>();
        case AbilityKind::kHeal:         return std::make_unique<Heal>();
        case AbilityKind::kTeleport:     return std::make_unique<Teleport>();
    }
    return nullptr;
}

const char* AbilityName(AbilityKind kind) {
    switch (kind) {
        case AbilityKind::kAreaStrike:   return "Удар вокруг";
        case AbilityKind::kRangedStrike: return "Дальний удар";
        case AbilityKind::kHeal:         return "Лечение";
        case AbilityKind::kTeleport:     return "Телепортация";
    }
    return "Неизвестная способность";
}

}