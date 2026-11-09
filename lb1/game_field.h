#ifndef LAB1_GAME_FIELD_H_
#define LAB1_GAME_FIELD_H_

#include <vector>

#include "cell.h"

namespace lab1 {

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

private:
    int width_;
    int height_;
    std::vector<std::vector<Cell>> cells_; // cells_[y][x]
};

}

#endif