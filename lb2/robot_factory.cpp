#include "robot_factory.h"

namespace lab2 {

namespace {
    constexpr int kMinPeriod = 8;
}

RobotFactory::RobotFactory(int x, int y, int period) : 
    x_(x), y_(y), period_(period < kMinPeriod ? kMinPeriod : period),
    counter_(0) {}


int RobotFactory::GetX() const {
    return x_;
}
int RobotFactory::GetY() const {
    return y_;
}
int RobotFactory::GetPeriod() const {
    return period_;
}
int RobotFactory::GetCounter() const {
    return counter_;
}

bool RobotFactory::ToMakeRobot() {
    ++counter_;
    if(counter_ >= period_) {
        counter_ = 0;
        return true;
    }
    return false;
}

bool RobotFactory::Occupies(int x, int y) const {
    return (x == x_ || x == x_ + 1) &&
            (y == y_ || y == y_ + 1);
}

void RobotFactory::ResetCounter() {
    counter_ = period_ - 1; // to try next turn
}

}