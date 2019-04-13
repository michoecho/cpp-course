#include "rebelfleet.h"
#include <cassert>

RebelStarship::RebelStarship(ShieldPoints shield, Speed speed)
    : BasicEngine(speed), BasicShield(shield) {}

void RebelStarship::fight(ImperialUnit &imp) {
    takeDamage(imp.getAttackPower());
}

ArmedRebelStarship::ArmedRebelStarship(ShieldPoints shield, Speed speed, AttackPower power)
    : RebelStarship(shield, speed), BasicWeapon(power) {}

void ArmedRebelStarship::fight(ImperialUnit &imp) {
    takeDamage(imp.getAttackPower());
    imp.takeDamage(getAttackPower());
}

Explorer::Explorer(ShieldPoints shield, Speed speed)
    : RebelStarship(shield, speed) {
    assert(299796 <= speed && speed <= 2997960);
}

XWing::XWing(ShieldPoints shield, Speed speed, AttackPower power)
    : ArmedRebelStarship(shield, speed, power) {
    assert(299796 <= speed && speed <= 2997960);
}

StarCruiser::StarCruiser(ShieldPoints shield, Speed speed, AttackPower power)
    : ArmedRebelStarship(shield, speed, power) {
    assert(99999 <= speed && speed <= 299795);
}

std::shared_ptr<Explorer> createExplorer(ShieldPoints shield, Speed speed) {
    return std::make_shared<Explorer>(shield, speed);
}

std::shared_ptr<XWing> createXWing(ShieldPoints shield, Speed speed, AttackPower power) {
    return std::make_shared<XWing>(shield, speed, power);
}

std::shared_ptr<StarCruiser> createStarCruiser(ShieldPoints shield, Speed speed, AttackPower power) {
    return std::make_shared<StarCruiser>(shield, speed, power);
}
