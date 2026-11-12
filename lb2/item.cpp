#include "item.h"

namespace lab2 {

const std::string& Item::GetName() const {
    static const std::string kHealth = "Зелье здоровья";
    static const std::string kEnergy = "Зелье энергии";
    static const std::string kAttack = "Эликсир атаки";
    switch (kind_) {
        case ItemKind::kHealthPotion: return kHealth;
        case ItemKind::kEnergyPotion: return kEnergy;
        case ItemKind::kAttackBoost:  return kAttack;
    }
    return kHealth;
}

}