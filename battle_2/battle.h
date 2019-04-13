#ifndef BATTLE_H
#define BATTLE_H

#include "imperialfleet.h"
#include "rebelfleet.h"

using Time = int;

class Timer {
public:
    virtual ~Timer() = default;
    virtual void init(Time t0, Time t1) = 0;
    virtual void tick(Time delta) = 0;
    virtual bool attackTime() const = 0;
};

class BasicTimer : public Timer {
public:
    void init(Time t0, Time t1) override;
    void tick(Time delta) override;
    bool attackTime() const override; 
private:
    Time time;
    Time maxTime;
};

class SpaceBattle {
public: 
    size_t countImperialFleet() const;
    size_t countRebelFleet() const;
    void tick(Time timeStep);

    friend class Builder;
    class Builder {
    public:
        Builder& ship(const std::shared_ptr<ImperialUnit> &unit);
        Builder& ship(const std::shared_ptr<RebelUnit> &unit);
        Builder& startTime(Time t);
        Builder& maxTime(Time t);
        Builder& timer(std::unique_ptr<Timer> &&timer);
        SpaceBattle build();
    private:
        std::vector<std::shared_ptr<RebelUnit>> rebels;
        std::vector<std::shared_ptr<ImperialUnit>> imperials;
        std::unique_ptr<Timer> clock;
        Time t0 = 0;
        Time t1 = 0;
    };

private:
    SpaceBattle(
        std::vector<std::shared_ptr<RebelUnit>> &&rebels,
        std::vector<std::shared_ptr<ImperialUnit>> &&imperials,
        std::unique_ptr<Timer> timer);
    std::vector<std::shared_ptr<RebelUnit>> rebels;
    std::vector<std::shared_ptr<ImperialUnit>> imperials;
    std::unique_ptr<Timer> timer;
};

#endif // BATTLE_H
