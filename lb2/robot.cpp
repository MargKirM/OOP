#include "robot.h"

#include <algorithm>
#include <utility>

#include "ability.h"
#include "game.h"
#include "status.h"

namespace lab2 {

namespace {
    constexpr int kMaxRank = 10;
    constexpr int kHealthBonusPerRank = 20;
    constexpr int kAttackBonusPerRank = 3;
    constexpr int kEnergyBonusPerRank = 3;
    constexpr int kKillExperienceBase = 20;
    constexpr int kMinHealth = 1;
    constexpr int kMinEnergy = 0;
    constexpr int kMinExperienceToRankUp = 1;
    constexpr int kMinSpeed = 1;
    constexpr int kMinVisibilityRadius = 0;
    constexpr int kMinAttack = 0;
    constexpr int kMinExperience = 0;

    constexpr int kHealAmount = 10;


    bool SameAbility(const ISpecialAbility& a, AbilityKind kind) {
        auto probe = CreateAbility(kind);
        return probe && a.GetName() == probe->GetName();
    }
}

Robot::Robot(const std::string& name, int health, int max_health,
                int attack, int energy, int max_energy,
                int experience, int experience_to_rank_up, int speed,
                int visibility_radius)
    : name_(name),
        health_(0),
        max_health_(kMinHealth),
        attack_(0),
        energy_(0),
        max_energy_(kMinEnergy),
        experience_(0),
        experience_to_rank_up_(kMinExperienceToRankUp),
        rank_(1),
        speed_(kMinSpeed),
        visibility_radius_(kMinVisibilityRadius),
        x_(0), y_(0) {
    SetMaxHealth(max_health);
    SetMaxEnergy(max_energy);
    SetExperienceToRankUp(experience_to_rank_up);
    SetSpeed(speed);
    SetVisibilityRadius(visibility_radius);

    SetHealth(health);
    SetAttack(attack);
    SetEnergy(energy);
    SetExperience(experience);
}

const std::string& Robot::GetName() const { return name_; }
int Robot::GetHealth() const { return health_; }
int Robot::GetMaxHealth() const { return max_health_; }
int Robot::GetAttack() const { return attack_; }
int Robot::GetEnergy() const { return energy_; }
int Robot::GetMaxEnergy() const { return max_energy_; }
int Robot::GetExperience() const { return experience_; }
int Robot::GetExperienceToRankUp() const { return experience_to_rank_up_; }
int Robot::GetRank() const { return rank_; }
int Robot::GetSpeed() const { return speed_; }
int Robot::GetVisibilityRadius() const { return visibility_radius_; }

int Robot::GetX() const { return x_; }
int Robot::GetY() const { return y_; }

void Robot::SetPosition(int x, int y) {
    x_ = x;
    y_ = y;
}

bool Robot::IsAlive() const { return health_ > 0; }

void Robot::SetHealth(int health) {
    health_ = std::clamp(health, 0, max_health_);
}

void Robot::SetMaxHealth(int max_health) {
    max_health_ = std::max(max_health, kMinHealth);
    if (health_ > max_health_) {
        health_ = max_health_;
    }
}

void Robot::SetAttack(int attack) {
    attack_ = std::max(attack, kMinAttack);
}

void Robot::SetEnergy(int energy) {
    energy_ = std::clamp(energy, 0, max_energy_);
}

void Robot::SetMaxEnergy(int max_energy) {
    max_energy_ = std::max(max_energy, kMinEnergy);
    if (energy_ > max_energy_) {
        energy_ = max_energy_;
    }
}

void Robot::SetExperience(int experience) {
    experience_ = std::max(experience, kMinExperience);
}

void Robot::SetExperienceToRankUp(int experience_to_rank_up) {
    experience_to_rank_up_ = std::max(experience_to_rank_up, kMinExperienceToRankUp);
}

void Robot::SetSpeed(int speed) {
    speed_ = std::max(speed, kMinSpeed);
}

void Robot::SetVisibilityRadius(int radius) {
    visibility_radius_ = std::max(radius, kMinVisibilityRadius);
}

void Robot::Interact(Robot& other, bool is_enemy) {
    if (is_enemy) {
        if (!other.IsAlive()) return; // before attack

        other.TakeDamage(attack_);
        if (!other.IsAlive()) {
            AddExperience(other.GetExperience() / 2 + kKillExperienceBase); // + bonus
        }
    } else {
        other.Heal(kHealAmount);
    }
}

void Robot::RegenerateEnergy(int amount) {
    SetEnergy(energy_ + amount);
}

void Robot::TakeDamage(int damage) {
    if (damage <= 0) return;
    SetHealth(health_ - damage);
}

void Robot::Heal(int amount) {
    SetHealth(health_ + amount);
}

void Robot::AddExperience(int amount) {
    if (amount <= 0) return;
    SetExperience(experience_ + amount);
    TryRankUp();
}

void Robot::TryRankUp() {
    while (experience_ >= experience_to_rank_up_ && rank_ < kMaxRank) {
        experience_ -= experience_to_rank_up_;
        ++rank_;
        SetMaxHealth(max_health_ + kHealthBonusPerRank);
        SetHealth(max_health_);
        SetAttack(attack_ + kAttackBonusPerRank);
        SetMaxEnergy(max_energy_ + kEnergyBonusPerRank);
        SetEnergy(max_energy_);

        if (rank_up_callback_) rank_up_callback_(*this);
    }
}

// statuses
bool Robot::IsMovementBlocked() const {
    for (const auto& s : statuses_) if (s->BlocksMovement()) return true;
    return false;
}
bool Robot::AreAbilitiesBlocked() const {
    for (const auto& s : statuses_) if (s->BlocksAbilities()) return true;
    return false;
}

bool Robot::HasStatus(const std::string& name) const {
    for (const auto& s : statuses_) if (s->GetName() == name) return true;
    return false;
}

void Robot::AddStatus(std::unique_ptr<IStatus> status) {
    if (!status) return;
    if (status->GetName() == "Перегрузка") {
        for (auto& s : statuses_)
            if (s->GetName() == "Перегрузка") return;   // одна
    }
    status->OnApply(*this);
    statuses_.push_back(std::move(status));
}

void Robot::OnAbilityAttempted() {
    for (auto it = statuses_.begin(); it != statuses_.end(); ) {
        if ((*it)->OnAbilityUsed()) it = statuses_.erase(it);
        else ++it;
    }
}

void Robot::EndTurn() {
    for (auto it = statuses_.begin(); it != statuses_.end(); ) {
        if ((*it)->OnTurnEnd(*this)) it = statuses_.erase(it);
        else ++it;
    }
}

void Robot::ApplyDamage(int damage) {
    if (damage <= 0) return;
    int remaining = damage;
    for (auto it = statuses_.begin(); it != statuses_.end(); ) {
        remaining = (*it)->OnDamageTaken(remaining);
        if ((*it)->ShouldRemoveAfterDamage()) it = statuses_.erase(it);
        else ++it;
        if (remaining <= 0) break;
    }
    if (remaining > 0) TakeDamage(remaining);
}

// abilities
bool Robot::HasAbility(AbilityKind kind) const {
    for (const auto& a : abilities_) if (SameAbility(*a, kind)) return true;
    return false;
}

void Robot::AddAbility(AbilityKind kind) {
    if (!HasAbility(kind)) abilities_.push_back(CreateAbility(kind));
}

bool Robot::TryUseAbility(Game& game, AbilityKind kind, int tx, int ty) {
    if (!IsAlive()) return false;
    if (AreAbilitiesBlocked()) { OnAbilityAttempted(); return false; }

    for (auto& a : abilities_) {
        if (!SameAbility(*a, kind)) continue;
        if (!a->CanUse(*this, game, tx, ty)) return false;
        if (GetEnergy() < a->GetEnergyCost()) return false;
        SetEnergy(GetEnergy() - a->GetEnergyCost());
        a->Use(*this, game, tx, ty);
        return true;
    }
    return false;
}

void Robot::UpgradeAbility(AbilityKind kind) {
    for (auto& a : abilities_) {
        if (SameAbility(*a, kind) && a->IsUpgradable()) {
            a->Upgrade();
            return;
        }
    }
}

const std::vector<std::unique_ptr<ISpecialAbility>>& Robot::GetAbilities() const {
    return abilities_;
}

const std::vector<std::unique_ptr<IStatus>>& Robot::GetStatuses() const {
    return statuses_;
}

void Robot::SetRankUpCallback(std::function<void(Robot&)> cb) {
    rank_up_callback_ = std::move(cb);
}

}