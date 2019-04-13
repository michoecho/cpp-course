#ifndef JNP_STAR_WARS_IMPERIALFLEET_H
#define JNP_STAR_WARS_IMPERIALFLEET_H

#include <utility>
#include "rebelfleet.h"

template<typename U>
class ImperialStarship {
    U shield;
    U attackPower;

public:
    using valueType = U;

    ImperialStarship(U shield, U attackPower)
            : shield(shield), attackPower(attackPower) {}

    U getShield() const {
        return shield;
    }

    U getAttackPower() const {
        return attackPower;
    }

    void takeDamage(U damage) {
        shield = shield > damage ? shield - damage : 0;
    }
};

template<typename U>
using DeathStar = ImperialStarship<U>;

template<typename U>
using ImperialDestroyer = ImperialStarship<U>;

template<typename U>
using TIEFighter = ImperialStarship<U>;

namespace jnp_sw_ {
    template<typename>
    struct IsImperial : std::false_type {};

    template<typename U>
    struct IsImperial<ImperialStarship<U>> : std::true_type {};
}

template<typename I, typename R>
void attack(I& imperialShip, R& rebelShip) {
    static_assert(jnp_sw_::IsImperial<I>() && jnp_sw_::IsRebel<R>(),
                  "Invalid ship types for battle");

    rebelShip.takeDamage(imperialShip.getAttackPower());
    if constexpr (jnp_sw_::IsArmedRebel<R>()) {
        imperialShip.takeDamage(rebelShip.getAttackPower());
    }
}

#endif //JNP_STAR_WARS_IMPERIALFLEET_H
