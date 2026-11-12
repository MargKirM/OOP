#include "cell.h"

#include <cstdlib>
#include <utility>

#include "robot.h"

namespace lab2 {

namespace {
    constexpr std::size_t kMaxCellStatuses = 2;   // от 0 до 2 statuses
}

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

void Cell::AddStatus(std::unique_ptr<IStatus> status) {
    if (!status) return;
    if (statuses_.size() >= kMaxCellStatuses) return;
    statuses_.push_back(std::move(status));
}

void Cell::RemoveRandomStatus() {
    if (statuses_.empty()) return;
    const std::size_t idx =
        static_cast<std::size_t>(std::rand()) % statuses_.size();
    statuses_.erase(statuses_.begin() + idx);
}

void Cell::ApplyTo(Robot& robot) const {
    for (const auto& s : statuses_) {
        if (s) robot.AddStatus(s->Clone());
    }
}

std::size_t Cell::GetStatusCount() const {
    return statuses_.size();
}

}