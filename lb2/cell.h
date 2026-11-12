#ifndef LAB1_CELL_H_
#define LAB1_CELL_H_

#include <memory>
#include <vector>

#include "status.h"

namespace lab2 {

class Robot;
class Cell {
public:
    Cell(); // constructor

    bool IsPassable() const;
    void SetPassable(bool passable);

    int GetPassCost() const;
    void SetPassCost(int pass_cost);

    void AddStatus(std::unique_ptr<IStatus> status);
    void RemoveRandomStatus();
    void ApplyTo(Robot& robot) const;


    std::size_t GetStatusCount() const;

private:
    bool is_passable_;
    int pass_cost_;

    std::vector<std::unique_ptr<IStatus>> statuses_;
};

}

#endif