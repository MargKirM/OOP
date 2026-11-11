#ifndef LAB1_CELL_H_
#define LAB1_CELL_H_

namespace lab2 {

class Cell{
public:
    Cell(); // constructor

    bool IsPassable() const;
    void SetPassable(bool passable);

    int GetPassCost() const;
    void SetPassCost(int pass_cost);

private:
    bool is_passable_;
    int pass_cost_;
};

}

#endif