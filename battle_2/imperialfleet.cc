#include "imperialfleet.h"
#include <cassert>

BasicShield::BasicShield(ShieldPoints shield) : shield(shield) {
    assert(shield >= 0);
}

BasicWeapon::BasicWeapon(AttackPower power) : power(power) {
    assert(power >= 0);
}

BasicEngine::BasicEngine(Speed speed) : speed(speed) {}

ShieldPoints BasicShield::getShield() const {
    return shield;
}

void BasicShield::takeDamage(AttackPower damage) {
    shield = shield > damage ? shield - damage : 0;
}

size_t BasicShield::getAlive() const {
    return getShield() > 0 ? 1 : 0;
}

AttackPower BasicWeapon::getAttackPower() const {
    return power;
}

Speed BasicEngine::getSpeed() const {
    return speed;
}

ImperialStarship::ImperialStarship(ShieldPoints shield, AttackPower power)
    : BasicShield(shield), BasicWeapon(power) {}

DeathStar::DeathStar(ShieldPoints shield, AttackPower power)
    : ImperialStarship(shield, power) {}

TIEFighter::TIEFighter(ShieldPoints shield, AttackPower power)
    : ImperialStarship(shield, power) {}

ImperialDestroyer::ImperialDestroyer(ShieldPoints shield, AttackPower power)
    : ImperialStarship(shield, power) {}

Squadron::Squadron(const std::vector<std::shared_ptr<ImperialUnit>> &src)
    : ships(src) {}

Squadron::Squadron(std::initializer_list<std::shared_ptr<ImperialUnit>> src)
    : ships(src) {}

ShieldPoints Squadron::getShield() const {
    ShieldPoints ans = 0;
    for (auto &ship : ships) {
        ans += ship->getShield();
    }
    return ans;
}

void Squadron::takeDamage(AttackPower damage) {
    for (auto &ship : ships) {
        ship->takeDamage(damage);
    }
}

AttackPower Squadron::getAttackPower() const {
    AttackPower ans = 0;
    for (auto &ship : ships) {
        ans += ship->getAlive() > 0 ? ship->getAttackPower() : 0;
    }
    return ans;
}

size_t Squadron::getAlive() const {
    size_t ans = 0;
    for (auto &ship : ships) {
        ans += ship->getAlive();
    }
    return ans;
}

std::shared_ptr<ImperialUnit> createDeathStar(ShieldPoints shield, AttackPower power) {
    return std::make_shared<DeathStar>(shield, power);
}

std::shared_ptr<ImperialUnit>
createImperialDestroyer(ShieldPoints shield, AttackPower power) {
    return std::make_shared<ImperialDestroyer>(shield, power);
}

std::shared_ptr<ImperialUnit>
createTIEFighter(ShieldPoints shield, AttackPower power) {
    return std::make_shared<TIEFighter>(shield, power);
}

std::shared_ptr<ImperialUnit>
createSquadron(const std::vector<std::shared_ptr<ImperialUnit>> &src) {
    return std::make_shared<Squadron>(src);
}

std::shared_ptr<ImperialUnit>
createSquadron(std::initializer_list<std::shared_ptr<ImperialUnit>> src) {
    return std::make_shared<Squadron>(src);
}
