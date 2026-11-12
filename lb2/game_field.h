#ifndef LAB1_GAME_FIELD_H_
#define LAB1_GAME_FIELD_H_

#include <memory>
#include <vector>

#include "cell.h"
#include "status.h"

namespace lab2 {

class Robot;

class GameField {
public:
    GameField(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
    bool IsInside(int x, int y) const;
    bool IsPassable(int x, int y) const;
    void SetPassable(int x, int y, bool passable);

    int GetPassCost(int x, int y) const;
    void SetPassCost(int x, int y, int cost);

    void AddStatusToCell(int x, int y, std::unique_ptr<IStatus> status);
    void ApplyCellStatusesTo(Robot& robot, int x, int y) const;
    void RemoveRandomStatusFromCell(int x, int y);

    // for lighting cells with status
    std::size_t GetCellStatusCount(int x, int y) const;

private:
    int width_;
    int height_;
    std::vector<std::vector<Cell>> cells_; // cells_[y][x]
};

}

#endif