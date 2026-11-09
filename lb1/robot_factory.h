#ifndef LAB1_ROBOT_FACTORY_H_
#define LAB1_ROBOT_FACTORY_H_

namespace lab1 {

class RobotFactory {
public:
    RobotFactory(int x, int y, int period);

    int GetX() const; // upper left corner
    int GetY() const; // upper left corner
    int GetPeriod() const; // how many turns between creates
    int GetCounter() const; // count of passed turns

    bool ToMakeRobot();

    bool Occupies(int x, int y) const;

    void ResetCounter();

private:
    int x_;
    int y_;
    int period_;
    int counter_;
};

}

#endif