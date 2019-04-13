#ifndef JNP_STAR_WARS_BATTLE_H
#define JNP_STAR_WARS_BATTLE_H

#include <tuple>
#include <algorithm>
#include <iostream>

#include "rebelfleet.h"
#include "imperialfleet.h"

namespace jnp_sw_ {
    template<template<typename> class Predicate, typename... Ts>
    class TupleFilter {
        template<class T>
        static auto filterOne(T t) {
            if constexpr (Predicate<T>()) {
                return std::tuple<T>(t);
            } else {
                return std::tuple<>();
            }
        }

    public:
        static auto filter(Ts... t) {
            return std::tuple_cat(filterOne(t)...);
        }

        using ResultTupleType = decltype(filter(std::declval<Ts>()...));

        constexpr static size_t size() {
            return std::tuple_size<ResultTupleType>();
        }
    };
}

template<typename T, T t0, T t1, typename... S>
class SpaceBattle {
    using ImperialFilter = jnp_sw_::TupleFilter<jnp_sw_::IsImperial, S...>;
    using RebelFilter = jnp_sw_::TupleFilter<jnp_sw_::IsRebel, S...>;
    typename ImperialFilter::ResultTupleType imperialShips;
    typename RebelFilter::ResultTupleType rebelShips;
    T time;

    static_assert(sizeof...(S) == ImperialFilter::size() + RebelFilter::size(),
                  "Non-starship arguments for SpaceBattle");
    static_assert(t0 <= t1, "SpaceBattle: t0 > t1");
    static_assert(t0 >= 0, "SpaceBattle: t0 < 0");

    template<size_t i = 0, typename ...Ts>
    size_t countAlive(const std::tuple<Ts...> &shipList) const {
        if constexpr (i < sizeof...(Ts))
            return (std::get<i>(shipList).getShield() > 0) +
                   countAlive<i + 1, Ts...>(shipList);
        else
            return 0;
    }

    template<size_t i = 0>
    void attackAll() {
        if constexpr (i < ImperialFilter::size()) {
            if (std::get<i>(imperialShips).getShield() > 0)
                attackOne(std::get<i>(imperialShips));
            attackAll<i + 1>();
        }
    }

    template<size_t i = 0, typename I>
    void attackOne(I& imperialShip) {
        if constexpr (i < RebelFilter::size()) {
            if (std::get<i>(rebelShips).getShield() > 0)
                attack(imperialShip, std::get<i>(rebelShips));
            attackOne<i + 1>(imperialShip);
        }
    }

    static constexpr T sqrtT1() {
        T i = 1;
        while (i <= t1 / i)
            ++i;
        return i - 1;
    }

    static constexpr auto squaresArray() {
        constexpr T root = sqrtT1();
        std::array<T, root + 1> ans{0};
        for (T i = 0; i <= root; ++i) {
            ans[i] = i * i;
        }
        return ans;
    }

    static constexpr auto squares = squaresArray();

public:
    SpaceBattle(S... ships) :
            imperialShips(ImperialFilter::filter(ships...)),
            rebelShips(RebelFilter::filter(ships...)),
            time(t0) {}

    size_t countRebelFleet() const {
        return countAlive(rebelShips);
    }

    size_t countImperialFleet() const {
        return countAlive(imperialShips);
    }

    void tick(T timeStep) {
        size_t rebelCount = countRebelFleet();
        size_t imperialCount = countImperialFleet();
        if (rebelCount == 0 && imperialCount == 0) {
            std::cout << "DRAW" << std::endl;
            return;
        } else if (rebelCount == 0) {
            std::cout << "IMPERIUM WON" << std::endl;
            return;
        } else if (imperialCount == 0) {
            std::cout << "REBELLION WON" << std::endl;
            return;
        }

        if (std::binary_search(squares.begin(), squares.end(), time))
            attackAll();

        time += timeStep;
        time %= (t1 + 1);
    }
};

#endif //JNP_STAR_WARS_BATTLE_H
