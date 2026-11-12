#include "game_field.h"

#include <utility>

namespace lab2 {

namespace {
    constexpr int kMinSize = 5; // constexpr to define true constants(compile-time values)
    constexpr int kMaxSize = 50;
}

GameField::GameField(int width, int height) : width_(width), height_(height) {  // constructor
    if (width < kMinSize) width_ = kMinSize;
    else if (width > kMaxSize) width_ = kMaxSize;

    if (height < kMinSize) height_ = kMinSize;
    else if (height > kMaxSize) height_ = kMaxSize;

    cells_.resize(height_); // cells_[y][x]
    for (auto& row : cells_) row.resize(width_);
}

int GameField::GetWidth() const {
    return width_;
}
int GameField::GetHeight() const {
    return height_;
}

bool GameField::IsInside(int x, int y) const {
    return x >= 0 && x < width_ &&
            y >= 0 && y < height_;
}
bool GameField::IsPassable(int x, int y) const {
    return IsInside(x, y) && cells_[y][x].IsPassable();
}
void GameField::SetPassable(int x, int y, bool passable) {
    if (IsInside(x, y)) {
        cells_[y][x].SetPassable(passable);
    }
}

int GameField::GetPassCost(int x, int y) const {
    if (!IsInside(x, y)) return 1;
    return cells_[y][x].GetPassCost();
}

void GameField::SetPassCost(int x, int y, int cost) {
    if (IsInside(x, y)) {
        cells_[y][x].SetPassCost(cost);
    }
}

void GameField::AddStatusToCell(int x, int y, std::unique_ptr<IStatus> status) {
    if (IsInside(x, y)) cells_[y][x].AddStatus(std::move(status));
}

void GameField::ApplyCellStatusesTo(Robot& robot, int x, int y) const {
    if (IsInside(x, y)) cells_[y][x].ApplyTo(robot);
}

void GameField::RemoveRandomStatusFromCell(int x, int y) {
    if (IsInside(x, y)) cells_[y][x].RemoveRandomStatus();
}

std::size_t GameField::GetCellStatusCount(int x, int y) const {
    return IsInside(x, y) ? cells_[y][x].GetStatusCount() : 0;
}

}