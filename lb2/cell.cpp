#include "cell.h"

namespace lab2 {

Cell::Cell() : is_passable_(true), pass_cost_(1) {} // constructor

bool Cell::IsPassable() const {
    return is_passable_;
}
void Cell::SetPassable(bool passable) {
    is_passable_ = passable;
}

int Cell::GetPassCost() const {
    return pass_cost_;
}
void Cell::SetPassCost(int cost) {
    pass_cost_ = (cost < 1) ? 1 : cost;
}

}