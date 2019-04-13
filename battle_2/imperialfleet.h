#ifndef IMPERIALFLEET_H
#define IMPERIALFLEET_H

#include <memory>
#include <vector>

using ShieldPoints = int;
using AttackPower = ShieldPoints;
using Speed = int;

class Shield {
public:
    virtual ~Shield() = default;
    virtual ShieldPoints getShield() const = 0;
    virtual void takeDamage(AttackPower damage) = 0;
    virtual size_t getAlive() const = 0;
};

class Weapon {
public:
    virtual ~Weapon() = default;
    virtual AttackPower getAttackPower() const = 0;
};

class Engine {
public:
    virtual ~Engine() = default;
    virtual Speed getSpeed() const = 0;
};

class BasicShield : public virtual Shield {
public:
    BasicShield(ShieldPoints power);
    ShieldPoints getShield() const override;
    void takeDamage(AttackPower damage) override;
    size_t getAlive() const override;
private:
    ShieldPoints shield;
};

class BasicWeapon : public virtual Weapon {
public:
    BasicWeapon(AttackPower power);
    AttackPower getAttackPower() const override;
private:
    AttackPower power;
};

class BasicEngine : public virtual Engine {
public:
    BasicEngine(Speed speed);
    Speed getSpeed() const override;
private:
    Speed speed;
};

class ImperialUnit : public virtual Shield, public virtual Weapon {
};

class ImperialStarship : public ImperialUnit, public BasicShield, public BasicWeapon {
public:
    ImperialStarship(ShieldPoints shield, AttackPower power);
};

class Squadron : public ImperialUnit {
public:
    Squadron(const std::vector<std::shared_ptr<ImperialUnit>> &src);
    Squadron(std::initializer_list<std::shared_ptr<ImperialUnit>> src);
    ShieldPoints getShield() const override;
    void takeDamage(AttackPower damage) override;
    AttackPower getAttackPower() const override;
    size_t getAlive() const override;
private:
    std::vector<std::shared_ptr<ImperialUnit>> ships;
};

class DeathStar : public ImperialStarship {
public:
    DeathStar(ShieldPoints shield, AttackPower power);
};

class ImperialDestroyer : public ImperialStarship {
public:
    ImperialDestroyer(ShieldPoints shield, AttackPower power);
};

class TIEFighter : public ImperialStarship {
public:
    TIEFighter(ShieldPoints shield, AttackPower power);
};

std::shared_ptr<ImperialUnit> createDeathStar(ShieldPoints shield, AttackPower power);
std::shared_ptr<ImperialUnit> createImperialDestroyer(ShieldPoints shield, AttackPower power);
std::shared_ptr<ImperialUnit> createTIEFighter(ShieldPoints shield, AttackPower power);
std::shared_ptr<ImperialUnit> createSquadron(const std::vector<std::shared_ptr<ImperialUnit>> &src);
std::shared_ptr<ImperialUnit> createSquadron(std::initializer_list<std::shared_ptr<ImperialUnit>> src);

#endif // IMPERIALFLEET_H
