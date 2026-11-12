#ifndef LAB2_ITEM_H_
#define LAB2_ITEM_H_

#include <string>

namespace lab2 {


enum class ItemKind {
    kHealthPotion,   // recovery health
    kEnergyPotion,   // recovery energy
    kAttackBoost     // temporary boost attack
};

class Item {
public:
    explicit Item(ItemKind kind) : kind_(kind) {}
    ItemKind GetKind() const { return kind_; }
    const std::string& GetName() const;

private:
    ItemKind kind_;
};

}

#endif