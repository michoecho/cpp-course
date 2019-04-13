#ifndef REBELFLEET_H
#define REBELFLEET_H

#include "imperialfleet.h"

class RebelUnit : public virtual Shield, public virtual Engine {
public:
    virtual void fight(ImperialUnit &imp) = 0;
};

class RebelStarship : public RebelUnit, public BasicEngine, public BasicShield {
public:
    RebelStarship(ShieldPoints shield, Speed speed);
    void fight(ImperialUnit &imp) override;
};

class ArmedRebelStarship : public RebelStarship, public BasicWeapon {
public:
    ArmedRebelStarship(ShieldPoints shield, Speed speed, AttackPower power);
    void fight(ImperialUnit &imp) override;
};

class Explorer : public RebelStarship {
public:
    Explorer(ShieldPoints shield, Speed speed);
};

class XWing : public ArmedRebelStarship {
public:
    XWing(ShieldPoints shield, Speed speed, AttackPower power);
};

class StarCruiser : public ArmedRebelStarship {
public:
    StarCruiser(ShieldPoints shield, Speed speed, AttackPower power);
};

std::shared_ptr<Explorer> createExplorer(ShieldPoints shield, Speed speed);
std::shared_ptr<XWing> createXWing(ShieldPoints shield, Speed speed, AttackPower power);
std::shared_ptr<StarCruiser> createStarCruiser(ShieldPoints shield, Speed speed, AttackPower power);
#endif // REBELFLEET_H
