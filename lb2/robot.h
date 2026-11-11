#ifndef LAB1_ROBOT_H_
#define LAB1_ROBOT_H_

#include <string>

namespace lab2 {

class Robot {
public:
    Robot(const std::string& name, int health, int max_health, int attack,
        int energy, int max_energy,
        int experience, int experience_to_rank_up,
        int speed,
        int visibility_radius); // constructor

    Robot(const Robot&) = delete; // copy constructor is forbidden (unique_ptr)
    Robot& operator=(const Robot&) = delete; // copy operator is forbidden
    
    Robot(Robot&&) = delete; // move constructor is forbidden
    Robot& operator=(Robot&&) = delete; // move operator is forbidden

    ~Robot() = default; // destructor

    const std::string& GetName() const;
    int GetHealth() const;
    int GetMaxHealth() const;
    int GetAttack() const;
    int GetEnergy() const;
    int GetMaxEnergy() const;
    int GetExperience() const;
    int GetExperienceToRankUp() const;
    int GetRank() const;
    int GetSpeed() const;
    int GetVisibilityRadius() const;

    int GetX() const;
    int GetY() const;
    void SetPosition(int x, int y);

    bool IsAlive() const; // robot is alive

    void SetHealth(int health);
    void SetMaxHealth(int max_health);
    void SetAttack(int attack);
    void SetEnergy(int energy);
    void SetMaxEnergy(int max_energy);
    void SetExperienceToRankUp(int experience_to_rank_up);
    void SetSpeed(int speed);
    void SetExperience(int experience);
    void SetVisibilityRadius(int radius);

    void Interact(Robot& other, bool is_enemy); // interact with other robot
    void RegenerateEnergy(int amount); // amount (= count) of regenerating energy
    void TakeDamage(int damage);
    void Heal(int amount); // recover health
    void AddExperience(int amount);
    
private:
    void TryRankUp(); // if necessary experience is

    std::string name_;
    int health_;
    int max_health_;
    int attack_;
    int energy_;
    int max_energy_;
    int experience_;
    int experience_to_rank_up_;
    int rank_;
    int speed_;
    int visibility_radius_;

    // position
    int x_;
    int y_;
};

}

#endif