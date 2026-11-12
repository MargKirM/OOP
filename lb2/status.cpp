#include "status.h"

#include "robot.h"

namespace lab2 {

namespace {
    constexpr int kBurningDefaultTurns  = 2;
    constexpr int kBurningDamagePerTurn = 5;

    class Slow : public IStatus {
    public:
        Slow() = default;
        explicit Slow(int turns) : turns_(turns > 0 ? turns : 1) {}
        
        const std::string& GetName() const override {
            static const std::string kName = "Замедление";
            return kName;
        }
        std::unique_ptr<IStatus> Clone() const override {
            return std::make_unique<Slow>(*this);
        }
        void OnApply(Robot&) override {}
        bool OnTurnEnd(Robot&) override {
            if (turns_ > 0) --turns_;
            return turns_ == 0;
        }
        bool BlocksMovement() const override { return turns_ > 0; }
        bool BlocksAbilities() const override { return false; }
        bool OnAbilityUsed() override { return false; }
        int OnDamageTaken(int damage) override { return damage; }
        bool ShouldRemoveAfterDamage() const override { return false; }

    private:
        int turns_ = 1; // one turn per step
    };

    class Overload : public IStatus {
    public:
        const std::string& GetName() const override {
            static const std::string kName = "Перегрузка";
            return kName;
        }
        std::unique_ptr<IStatus> Clone() const override {
            return std::make_unique<Overload>(*this);
        }
        void OnApply(Robot&) override {}
        bool OnTurnEnd(Robot&) override { return false; }
        bool BlocksMovement() const override { return false; }
        bool BlocksAbilities() const override { return true; }
        bool OnAbilityUsed() override { return true; }
        int OnDamageTaken(int damage) override { return damage; }
        bool ShouldRemoveAfterDamage() const override { return false; }
    };

    class Burning : public IStatus {
    public:
        explicit Burning(int turns, int damage)
            : turns_(turns), damage_(damage) {}

        const std::string& GetName() const override {
            static const std::string kName = "Горение";
            return kName;
        }
        std::unique_ptr<IStatus> Clone() const override {
            return std::make_unique<Burning>(*this);
        }
        void OnApply(Robot&) override {}

        
        
        bool OnTurnEnd(Robot& robot) override {
            robot.TakeDamage(damage_);
            if (turns_ > 0) --turns_;
            return turns_ == 0;
        }
        bool BlocksMovement() const override { return false; }
        bool BlocksAbilities() const override { return false; }
        bool OnAbilityUsed() override { return false; }
        int OnDamageTaken(int damage) override { return damage; }
        bool ShouldRemoveAfterDamage() const override { return false; }

    private:
        int turns_;
        int damage_;
    };

    class Shield : public IStatus {
    public:
        explicit Shield(int amount) : amount_(amount > 0 ? amount : 0) {}

        const std::string& GetName() const override {
            static const std::string kName = "Щит";
            return kName;
        }
        std::unique_ptr<IStatus> Clone() const override {
            return std::make_unique<Shield>(*this);
        }
        
        
        void OnApply(Robot&) override {}
        bool OnTurnEnd(Robot&) override { return false; }
        bool BlocksMovement() const override { return false; }
        bool BlocksAbilities() const override { return false; }
        bool OnAbilityUsed() override { return false; }
        int OnDamageTaken(int damage) override {
            if (damage <= amount_) {
                amount_ -= damage;
                return 0;
            }
            const int rest = damage - amount_;
            amount_ = 0;
            return rest;
        }
        bool ShouldRemoveAfterDamage() const override { return amount_ <= 0; }

    private:
        int amount_;
    };

    class ItemPickup : public IStatus { // only on player
    public:
        const std::string& GetName() const override {
            static const std::string kName = "Получение предмета";
            return kName;
        }
        std::unique_ptr<IStatus> Clone() const override {
            return std::make_unique<ItemPickup>(*this);
        }
        void OnApply(Robot&) override {}
        bool OnTurnEnd(Robot&) override { return true; }   // снимается сразу
        bool BlocksMovement() const override { return false; }
        bool BlocksAbilities() const override { return false; }
        bool OnAbilityUsed() override { return false; }
        int OnDamageTaken(int damage) override { return damage; }
        bool ShouldRemoveAfterDamage() const override { return false; }
    };

    class AttackBoost : public IStatus {
    public:
        AttackBoost(int bonus, int turns) : bonus_(bonus), turns_(turns) {}
        const std::string& GetName() const override {
            static const std::string kName = "Усиление атаки";
            return kName;
        }
        std::unique_ptr<IStatus> Clone() const override {
            return std::make_unique<AttackBoost>(*this);
        }
        void OnApply(Robot& robot) override {
            robot.SetAttack(robot.GetAttack() + bonus_);
        }
        bool OnTurnEnd(Robot& robot) override {
            if (--turns_ <= 0) {
                robot.SetAttack(robot.GetAttack() - bonus_);
                return true;
            }
            return false;
        }
        bool BlocksMovement() const override { return false; }
        bool BlocksAbilities() const override { return false; }
        bool OnAbilityUsed() override { return false; }
        int OnDamageTaken(int damage) override { return damage; }
        bool ShouldRemoveAfterDamage() const override { return false; }

    private:
        int bonus_;
        int turns_;
    };
}

std::unique_ptr<IStatus> CreateSlowStatus() {
    return std::make_unique<Slow>();
}
std::unique_ptr<IStatus> CreateOverloadStatus() {
    return std::make_unique<Overload>();
}
std::unique_ptr<IStatus> CreateBurningStatus(int turns, int damage) {
    if (turns <= 0) turns = kBurningDefaultTurns;
    if (damage <= 0) damage = kBurningDamagePerTurn;
    return std::make_unique<Burning>(turns, damage);
}
std::unique_ptr<IStatus> CreateShieldStatus(int amount) {
    return std::make_unique<Shield>(amount);
}
std::unique_ptr<IStatus> CreateItemPickupStatus() {
    return std::make_unique<ItemPickup>();
}
std::unique_ptr<IStatus> CreateAttackBoostStatus(int bonus, int turns) {
    return std::make_unique<AttackBoost>(bonus, turns);
}

}