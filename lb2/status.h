#ifndef LAB2_STATUS_H_
#define LAB2_STATUS_H_

#include <memory>
#include <string>

namespace lab2 {

class Robot;


class IStatus {
public:
    virtual ~IStatus() = 0;

    virtual const std::string& GetName() const = 0;
    virtual std::unique_ptr<IStatus> Clone() const = 0;

    virtual void OnApply(Robot& robot) = 0;
    virtual bool OnTurnEnd(Robot& robot) = 0;

    virtual bool BlocksMovement() const = 0;
    virtual bool BlocksAbilities() const = 0;
    virtual bool OnAbilityUsed() = 0;

    virtual int OnDamageTaken(int damage) = 0;
    virtual bool ShouldRemoveAfterDamage() const = 0;
};
inline IStatus::~IStatus() = default;


std::unique_ptr<IStatus> CreateSlowStatus();
std::unique_ptr<IStatus> CreateOverloadStatus();
std::unique_ptr<IStatus> CreateBurningStatus(int turns, int damage);
std::unique_ptr<IStatus> CreateShieldStatus(int amount);
std::unique_ptr<IStatus> CreateItemPickupStatus();               // предмет с клетки
std::unique_ptr<IStatus> CreateAttackBoostStatus(int bonus, int turns);  // временный буст атаки

}

#endif