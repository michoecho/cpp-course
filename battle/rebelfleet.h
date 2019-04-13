#ifndef JNP_STAR_WARS_REBELFLEET_H
#define JNP_STAR_WARS_REBELFLEET_H

#include <cassert>
#include <utility>

template<typename U, bool armed, int minSpeed, int maxSpeed>
class RebelStarship {
    U shield;
    U speed;
    U attackPower;

public:
    using valueType = U;

    template<bool enabled = armed, typename = typename std::enable_if_t<enabled, int>>
    RebelStarship(U shield, U speed, U attackPower)
            : shield(shield), speed(speed), attackPower(attackPower) {
        assert(static_cast<U>(minSpeed) <= speed && speed <= static_cast<U>(maxSpeed));
    }

    template<bool enabled = !armed, typename = typename std::enable_if_t<enabled, int>>
    RebelStarship(U shield, U speed)
            : shield(shield), speed(speed), attackPower(0) {
        assert(static_cast<U>(minSpeed) <= speed && speed <= static_cast<U>(maxSpeed));
    }

    U getShield() const {
        return shield;
    }

    U getSpeed() const {
        return speed;
    }

    template<bool enabled = armed, typename = typename std::enable_if_t<enabled, int>>
    U getAttackPower() const {
        return attackPower;
    }

    void takeDamage(U damage) {
        shield = shield > damage ? shield - damage : 0;
    }
};

template<typename U>
using Explorer = RebelStarship<U, false, 299796, 2997960>;

template<typename U>
using StarCruiser = RebelStarship<U, true, 99999, 299795>;

template<typename U>
using XWing = RebelStarship<U, true, 299796, 2997960>;

namespace jnp_sw_ {
    template<typename>
    struct IsRebel : std::false_type {};

    template<typename U, bool armed, int x, int y>
    struct IsRebel<RebelStarship<U, armed, x, y>> : std::true_type {};

    template<typename>
    struct IsArmedRebel : std::false_type {};

    template<typename U, int x, int y>
    struct IsArmedRebel<RebelStarship<U, true, x, y>> : std::true_type {};
}

#endif //JNP_STAR_WARS_REBELFLEET_H
