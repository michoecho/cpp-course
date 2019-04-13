#include "battle.h"
#include <cassert>
#include <iostream>

bool BasicTimer::attackTime() const {
    return (time % 5 != 0) && (time % 2 == 0 || time % 3 == 0);
}

void BasicTimer::tick(Time delta) {
    time += delta;
    time %= maxTime + 1;
}

void BasicTimer::init(Time t0, Time t1) {
    assert(0 <= t0);
    assert(t0 <= t1);
    time = t0;
    maxTime = t1;
}

SpaceBattle::SpaceBattle(
    std::vector<std::shared_ptr<RebelUnit>> &&rebels,
    std::vector<std::shared_ptr<ImperialUnit>> &&imperials,
    std::unique_ptr<Timer> timer) :

    rebels(std::move(rebels)),
    imperials(std::move(imperials)),
    timer(std::move(timer)) {}

size_t
SpaceBattle::countImperialFleet() const {
    size_t ans = 0;
    for (const auto &unit : imperials) {
        ans += unit->getAlive();
    }
    return ans;
}

size_t
SpaceBattle::countRebelFleet() const {
    size_t ans = 0;
    for (const auto &unit : rebels) {
        ans += unit->getAlive();
    }
    return ans;
}

void
SpaceBattle::tick(Time timeStep) {
    size_t impCount = countImperialFleet();
    size_t rebCount = countRebelFleet();
    if (impCount == 0 && rebCount == 0) {
        std::cout << "DRAW\n";
    } else if (impCount == 0) {
        std::cout << "REBELLION WON\n";
    } else if (rebCount == 0) {
        std::cout << "IMPERIUM WON\n";
    } else if (timer->attackTime()) {
        for (auto &imp : imperials) {
            for (auto &reb : rebels) { if (reb->getAlive() > 0 && imp->getAlive() > 0) {
                    reb->fight(*imp);
                }
            }
        }
    }
    timer->tick(timeStep);
}

SpaceBattle::Builder&
SpaceBattle::Builder::ship(const std::shared_ptr<ImperialUnit> &unit) {
    imperials.emplace_back(unit);
    return *this;
}

SpaceBattle::Builder&
SpaceBattle::Builder::ship(const std::shared_ptr<RebelUnit> &unit) {
    rebels.emplace_back(unit);
    return *this;
}

SpaceBattle::Builder&
SpaceBattle::Builder::startTime(Time t) {
    t0 = t;
    return *this;
}

SpaceBattle::Builder&
SpaceBattle::Builder::maxTime(Time t) {
    t1 = t;
    return *this;
}

SpaceBattle::Builder&
SpaceBattle::Builder::timer(std::unique_ptr<Timer> &&timer) {
    clock = std::move(timer);
    return *this;
}

SpaceBattle
SpaceBattle::Builder::build() {
    if (clock == nullptr) {
        clock = std::make_unique<BasicTimer>();
    }
    clock->init(t0, t1);
    return SpaceBattle(std::move(rebels), std::move(imperials), std::move(clock));
}
